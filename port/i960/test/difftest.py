#!/usr/bin/env python3
"""
Differential test of the i960 port against the original machine code.

The original core is run inside EMULATOR.EXE under Unicorn (x86-32), with a
hand-built CPU state block laid out like the original (see i960.h). The port
is driven through test/shim.c. Each case sets the same random state on both,
executes one instruction (or one interrupt entry) and compares everything:
registers, flags, FP registers, frame cache, literals, RAM, I/O writes and the
cycle count returned by execute.

usage: difftest.py EMULATOR.EXE libi960test.so [cases] [seed]
"""
import ctypes
import math
import random
import re
import struct
import sys
from pathlib import Path

from unicorn import Uc, UcError, UC_ARCH_X86, UC_MODE_32, UC_HOOK_INTR
from unicorn.x86_const import UC_X86_REG_EIP, UC_X86_REG_ESI, UC_X86_REG_ESP

EXE, LIB = sys.argv[1], sys.argv[2]
CASES = int(sys.argv[3]) if len(sys.argv) > 3 else 20000
SEED = int(sys.argv[4]) if len(sys.argv) > 4 else 1

RAM_SIZE = 0x10000          # guest 0x0000-0xffff is plain memory, the rest is I/O
RAM_HOST = 0x30000000
CTX = 0x20000000
CTX_SIZE = 0x204000
CACHE = 0x40000000
STUB = 0x70000000
STACK = 0x71000000
LOG = 0x72000000

# original CPU state layout
O_REGION = 0x200000
O_IP = 0x200040
O_R = 0x200044
O_FP = 0x2000c8
O_CC, O_AC, O_PC, O_SAT, O_PRCB, O_ICON = 0x2000e8, 0x2000ec, 0x2000f0, 0x2000f4, 0x2000f8, 0x2000fc
O_IAC = 0x200100
O_MCOUNT, O_MADDR = 0x200140, 0x200144
O_FCI, O_FCC, O_FCA, O_FCR = 0x200148, 0x20014c, 0x200150, 0x200350
O_CYC, O_STOP, O_INIRQ = 0x202350, 0x202354, 0x202358

G_CTX, G_PCPTR, G_LIT, G_LN2 = 0x5aa8f4, 0x5aa8f0, 0x5aa8f8, 0x5aeaf8
F_EXECUTE, F_IRQ, F_BUILD_OPTABLE = 0x4c4b00, 0x4c4c60, 0x4c2740


class Snap(ctypes.Structure):
    _fields_ = [
        ("ip", ctypes.c_uint32), ("r", ctypes.c_uint32 * 32),
        ("cc", ctypes.c_uint32), ("ac", ctypes.c_uint32), ("pc", ctypes.c_uint32),
        ("sat", ctypes.c_uint32), ("prcb", ctypes.c_uint32), ("icon", ctypes.c_uint32),
        ("fp", ctypes.c_double * 4),
        ("fc_index", ctypes.c_int32), ("fc_count", ctypes.c_int32),
        ("fc_addr", ctypes.c_uint32 * 128), ("fc_regs", (ctypes.c_uint32 * 16) * 128),
        ("in_interrupt", ctypes.c_uint32), ("stop", ctypes.c_uint32),
        ("multi_count", ctypes.c_uint32), ("multi_addr", ctypes.c_uint32),
        ("iac", ctypes.c_uint32 * 4), ("lit", (ctypes.c_uint32 * 4) * 32),
    ]


# ------------------------------------------------------------------ port side

lib = ctypes.CDLL(LIB)
lib.t_new.restype = ctypes.c_void_p
lib.t_new.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
lib.t_load.argtypes = [ctypes.c_void_p, ctypes.POINTER(Snap)]
lib.t_save.argtypes = [ctypes.c_void_p, ctypes.POINTER(Snap)]
lib.t_run.argtypes = [ctypes.c_void_p, ctypes.c_int]
lib.t_irq.argtypes = [ctypes.c_void_p, ctypes.c_int]
lib.t_clear.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
lib.t_iolog.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
lib.t_iolog.restype = ctypes.c_uint32
assert lib.t_snap_size() == ctypes.sizeof(Snap), "Snap layout mismatch"

port_ram = (ctypes.c_uint8 * RAM_SIZE)()
port = lib.t_new(ctypes.addressof(port_ram), RAM_SIZE)


def port_run(snap, ram, irq_line=None):
    ctypes.memmove(port_ram, ram, RAM_SIZE)
    lib.t_load(port, ctypes.byref(snap))
    lib.t_clear(port, snap.ip)
    ret = lib.t_irq(port, irq_line) if irq_line is not None else lib.t_run(port, 1)
    out = Snap()
    lib.t_save(port, ctypes.byref(out))
    buf = (ctypes.c_uint32 * (3 * 4096))()
    n = lib.t_iolog(buf, 4096)
    log = [tuple(buf[i * 3:i * 3 + 3]) for i in range(n)]
    return ret if irq_line is None else None, out, bytes(port_ram), log


# -------------------------------------------------------------- original side

def asm_stubs():
    """Runners and memory callbacks for the original, as raw x86."""
    code = bytearray(0x400)

    def put(off, b):
        code[off:off + len(b)] = b

    def rel32(frm, to):
        return struct.pack("<i", to - (frm + 5))

    cw = STUB + 0x300
    # execute runner: fldcw; push 1; call i960_execute; mov [res], eax
    b = b"\xd9\x2d" + struct.pack("<I", cw) + b"\x6a\x01"
    b += b"\xe8" + rel32(STUB + len(b), F_EXECUTE)
    b += b"\xa3" + struct.pack("<I", STUB + 0x304)
    put(0x000, b)
    run_end = STUB + len(b)
    # irq runner: fldcw; push line; push ctx; call irq
    b = b"\xd9\x2d" + struct.pack("<I", cw) + b"\xff\x35" + struct.pack("<I", STUB + 0x308)
    b += b"\x68" + struct.pack("<I", CTX)
    b += b"\xe8" + rel32(STUB + 0x80 + len(b), F_IRQ)
    put(0x080, b)
    irq_end = STUB + 0x80 + len(b)
    # optable builder
    b = b"\xe8" + rel32(STUB + 0x100, F_BUILD_OPTABLE)
    put(0x100, b)
    opt_end = STUB + 0x100 + len(b)
    # read callbacks (fastcall, ecx = address): eax = ecx ^ 0xa5a5a5a5
    put(0x180, b"\x89\xc8\x35\xa5\xa5\xa5\xa5\xc3")
    # write callbacks (ecx = address, edx = value): append (size, addr, value)
    for off, size in ((0x1a0, 1), (0x1c0, 2), (0x1e0, 4)):
        b = b"\xa1" + struct.pack("<I", STUB + 0x30c)            # mov eax,[logp]
        b += b"\xc7\x00" + struct.pack("<I", size)                # mov [eax],size
        b += b"\x89\x48\x04\x89\x50\x08"                          # mov [eax+4],ecx; mov [eax+8],edx
        b += b"\x83\x05" + struct.pack("<I", STUB + 0x30c) + b"\x0c"  # add [logp],12
        b += b"\xc3"
        put(off, b)
    return bytes(code), run_end, irq_end, opt_end


def load_exe(mu):
    d = Path(EXE).read_bytes()
    pe = struct.unpack_from("<I", d, 0x3c)[0]
    nsec = struct.unpack_from("<H", d, pe + 6)[0]
    optsz = struct.unpack_from("<H", d, pe + 20)[0]
    base = struct.unpack_from("<I", d, pe + 24 + 28)[0]
    size = struct.unpack_from("<I", d, pe + 24 + 56)[0]
    mu.mem_map(base, (size + 0xfff) & ~0xfff)
    for i in range(nsec):
        _, vsize, va, rawsize, rawptr = struct.unpack_from("<8sIIII", d, pe + 24 + optsz + i * 40)
        mu.mem_write(base + va, d[rawptr:rawptr + rawsize])


mu = Uc(UC_ARCH_X86, UC_MODE_32)
load_exe(mu)
for addr, size in ((CTX, CTX_SIZE), (CACHE, 0x100000), (RAM_HOST, RAM_SIZE),
                   (STUB, 0x1000), (STACK, 0x10000), (LOG, 0x40000)):
    mu.mem_map(addr, size)
stubs, RUN_END, IRQ_END, OPT_END = asm_stubs()
mu.mem_write(STUB, stubs)
mu.mem_write(STUB + 0x300, struct.pack("<H", 0x27f))   # x87: 53-bit precision, MSVC default

R_CB, W8_CB, W16_CB, W32_CB = STUB + 0x180, STUB + 0x1a0, STUB + 0x1c0, STUB + 0x1e0
rtab = bytearray()
wtab = bytearray()
for pg in range(0x10000):
    host = RAM_HOST if pg < RAM_SIZE >> 16 else 0
    rtab += struct.pack("<4I", host, R_CB, R_CB, R_CB)
    wtab += struct.pack("<4I", host, W8_CB, W16_CB, W32_CB)
mu.mem_write(CTX, bytes(rtab))
mu.mem_write(CTX + 0x100000, bytes(wtab))
# code regions: region 0 covers RAM, regions 1 and 2 start at 0xfff00000
reg = struct.pack("<IIIII", CACHE, RAM_HOST, 1, RAM_SIZE, 0)
reg += struct.pack("<IIIII", 0, 0, 1, 0, 0xfff00000)
reg += struct.pack("<IIIII", 0, 0, 1, 0, 0xfff00000)
reg += struct.pack("<I", CTX + O_REGION)
mu.mem_write(CTX + O_REGION, reg)
mu.mem_write(G_CTX, struct.pack("<I", CTX))
mu.mem_write(G_PCPTR, struct.pack("<I", CTX + O_IP))
mu.mem_write(G_LN2, struct.pack("<d", math.log(2.0)))
# CRT startup sets these on any SSE2 CPU: _ftol2, floor, ceil and pow take
# their SSE2 paths (_ftol2 then returns 0x80000000 on overflow).
mu.mem_write(0x10f1fec, struct.pack("<I", 1))
mu.mem_write(0x10f1fe4, struct.pack("<I", 1))

orig_fault = []
mu.hook_add(UC_HOOK_INTR, lambda uc, intno, data: (orig_fault.append(intno), uc.emu_stop()))

mu.reg_write(UC_X86_REG_ESP, STACK + 0xff00)
mu.emu_start(STUB + 0x100, OPT_END)


def orig_load(snap, ram):
    mu.mem_write(RAM_HOST, ram)
    w = struct.pack
    blk = w("<I", snap.ip) + w("<32I", *snap.r) + w("<I", 0) + w("<4d", *snap.fp)
    blk += w("<6I", snap.cc, snap.ac, snap.pc, snap.sat, snap.prcb, snap.icon)
    blk += w("<4I", *snap.iac)
    mu.mem_write(CTX + O_IP, blk)
    mu.mem_write(CTX + O_MCOUNT, w("<BxxxI", snap.multi_count, snap.multi_addr))
    fca = w("<ii", snap.fc_index, snap.fc_count) + w("<128I", *snap.fc_addr)
    fcr = b"".join(w("<16I", *snap.fc_regs[k]) for k in range(128))
    mu.mem_write(CTX + O_FCI, fca + fcr)
    mu.mem_write(CTX + O_STOP, w("<I", snap.stop) + w("<B", snap.in_interrupt))
    mu.mem_write(G_LIT, b"".join(w("<4I", *snap.lit[n]) for n in range(32)))
    # prev-ip history and the cache entry for ip
    mu.mem_write(0x10f1fd0, w("<I", snap.ip))
    mu.mem_write(CACHE + (snap.ip >> 2) * 0x30, bytes(0x30))
    mu.mem_write(STUB + 0x30c, w("<I", LOG))
    mu.mem_write(CTX + O_REGION + 0x3c, w("<I", CTX + O_REGION))   # current region = 0


def orig_save():
    out = Snap()
    rd = lambda a, n: mu.mem_read(a, n)
    blk = rd(CTX + O_IP, 0xd0)
    out.ip = struct.unpack_from("<I", blk, 0)[0]
    for k, v in enumerate(struct.unpack_from("<32I", blk, 4)):
        out.r[k] = v
    for k, v in enumerate(struct.unpack_from("<4d", blk, 0x88)):
        out.fp[k] = v
    (out.cc, out.ac, out.pc, out.sat, out.prcb, out.icon) = struct.unpack_from("<6I", blk, 0xa8)
    for k, v in enumerate(struct.unpack_from("<4I", blk, 0xc0)):
        out.iac[k] = v
    out.multi_count, out.multi_addr = struct.unpack("<BxxxI", rd(CTX + O_MCOUNT, 8))
    fc = rd(CTX + O_FCI, 8 + 128 * 4 + 128 * 64)
    out.fc_index, out.fc_count = struct.unpack_from("<ii", fc, 0)
    for k, v in enumerate(struct.unpack_from("<128I", fc, 8)):
        out.fc_addr[k] = v
    for k in range(128):
        for j, v in enumerate(struct.unpack_from("<16I", fc, 8 + 512 + k * 64)):
            out.fc_regs[k][j] = v
    out.stop = struct.unpack("<I", rd(CTX + O_STOP, 4))[0]
    out.in_interrupt = rd(CTX + O_INIRQ, 1)[0]
    lit = rd(G_LIT, 512)
    for n in range(32):
        for k, v in enumerate(struct.unpack_from("<4I", lit, n * 16)):
            out.lit[n][k] = v
    logp = struct.unpack("<I", rd(STUB + 0x30c, 4))[0]
    raw = rd(LOG, logp - LOG)
    log = []
    for i in range(0, len(raw), 12):
        size, a, v = struct.unpack_from("<3I", raw, i)
        log.append((size, a, v & (0xff, 0xffff, 0, 0xffffffff)[size - 1]))
    return out, bytes(rd(RAM_HOST, RAM_SIZE)), log


def orig_run(snap, ram, irq_line=None):
    orig_load(snap, ram)
    orig_fault.clear()
    mu.reg_write(UC_X86_REG_ESP, STACK + 0xff00)
    mu.reg_write(UC_X86_REG_ESI, CTX)
    try:
        if irq_line is None:
            mu.emu_start(STUB, RUN_END, count=200000)
        else:
            mu.mem_write(STUB + 0x308, struct.pack("<I", irq_line))
            mu.emu_start(STUB + 0x80, IRQ_END, count=200000)
    except UcError as e:
        return ("error", str(e))
    end = RUN_END if irq_line is None else IRQ_END
    if orig_fault or mu.reg_read(UC_X86_REG_EIP) != end:
        # a divide fault can end emulation without the interrupt hook firing
        return ("fault", orig_fault[0] if orig_fault else "stopped early")
    ret = struct.unpack("<i", mu.mem_read(STUB + 0x304, 4))[0] if irq_line is None else None
    out, ram_out, log = orig_save()
    return ret, out, ram_out, log


# ------------------------------------------------------------ case generator

OPS = []
OPTABLE = Path(__file__).resolve().parent.parent / "i960_optable.inc"
for m in re.finditer(r"OP\(op_(\w+)\s*,\s*(\d), 0x([0-9a-f]+),\s*(\d+), (\d), \d, \d, 0x([0-9a-f]+)\)",
                     OPTABLE.read_text()):
    OPS.append(dict(name=m[1], fmt=int(m[2]), key=int(m[3], 16), cyc=int(m[4]),
                    fp=int(m[5]), ops=int(m[6], 16)))

MULTI = {"movl": 2, "movt": 4, "movq": 4, "ldl": 2, "stl": 2, "ldt": 4, "stt": 4,
         "ldq": 4, "stq": 4, "emul": 2, "ediv": 2}
LONGREAL = re.compile(r".*rl$|cvtilr|cvtzril|cpysre|cpyrsre|movre")


def rnd_u32(rng):
    k = rng.random()
    if k < 0.3:
        return rng.getrandbits(32)
    if k < 0.55:
        return rng.randrange(0, 40)
    if k < 0.7:
        return (-rng.randrange(1, 40)) & 0xffffffff
    if k < 0.85:
        return rng.choice([0, 1, 0x7fffffff, 0x80000000, 0xffffffff, 0x30, 0x39, 0x3a, 0x2f])
    return struct.unpack("<I", struct.pack("<f", rng.uniform(-100, 100)))[0]


def rnd_double(rng):
    k = rng.random()
    if k < 0.6:
        return rng.uniform(-1000, 1000)
    if k < 0.75:
        return rng.uniform(0.001, 4.0)
    if k < 0.9:
        return float(rng.randrange(-20, 20))
    return rng.choice([0.0, -0.0, 0.5, -0.5, 1.5, 2.5, -2.5, 1e10, -1e10])


def rnd_state(rng):
    s = Snap()
    s.ip = rng.randrange(0x1000, 0xe000, 4)
    for k in range(32):
        s.r[k] = rnd_u32(rng)
    s.r[31] = rng.randrange(0x8000, 0xa000, 0x40)     # fp
    s.r[1] = s.r[31] + 0x40 + rng.randrange(0, 0x100, 4)  # sp
    for k in range(4):
        s.fp[k] = rnd_double(rng)
    s.cc = rng.randrange(8)
    s.ac = rng.getrandbits(32) & 0xc000ff07 if rng.random() < 0.5 else rng.randrange(8)
    s.pc = rng.getrandbits(32) & 0x001f2002
    s.sat = rng.getrandbits(32)
    s.prcb = rng.randrange(0xc000, 0xd000, 4)
    s.icon = rng.getrandbits(32)
    s.fc_count = rng.choice([0, 0, 1, 2, 5, rng.randrange(129), 127, 128])
    s.fc_index = rng.randrange(128)
    for k in range(128):
        s.fc_addr[k] = rng.randrange(0x2000, 0x7fc0, 0x40)
        for j in range(16):
            s.fc_regs[k][j] = rng.getrandbits(32)
    s.in_interrupt = rng.randrange(2)
    s.multi_count = 0
    s.multi_addr = 0
    for k in range(4):
        s.iac[k] = rng.getrandbits(32)
    if rng.random() < 0.5:
        s.iac[0] = 0x93000000 | (s.iac[0] & 0xffffff)
    for n in range(32):
        for k in range(4):
            s.lit[n][k] = n
    return s


def put_fp_in_gprs(rng, s, regs, double):
    for r in regs:
        if double and r % 2 == 0 and r < 31:
            lo, hi = struct.unpack("<II", struct.pack("<d", rnd_double(rng)))
            s.r[r], s.r[r + 1] = lo, hi
        else:
            s.r[r] = struct.unpack("<I", struct.pack("<f", rnd_double(rng)))[0]


def make_positive(s, gpr, dbl, fpreg):
    if gpr is not None:
        if dbl and gpr < 31:
            d = struct.unpack("<d", struct.pack("<II", s.r[gpr], s.r[gpr + 1]))[0]
            lo, hi = struct.unpack("<II", struct.pack("<d", abs(d) + 0.25))
            s.r[gpr], s.r[gpr + 1] = lo, hi
        f = struct.unpack("<f", struct.pack("<I", s.r[gpr]))[0]
        if not dbl:
            s.r[gpr] = struct.unpack("<I", struct.pack("<f", abs(f) + 0.25))[0]
    if fpreg is not False and fpreg is not None:
        s.fp[fpreg] = abs(s.fp[fpreg]) + 0.25


def gen_case(rng):
    s = rnd_state(rng)
    ram = bytearray(rng.getrandbits(8) for _ in range(256)) * (RAM_SIZE // 256)
    op = rng.choice(OPS)
    name, fmt = op["name"], op["fmt"]
    align = MULTI.get(name, 1)
    dbl = bool(LONGREAL.match(name))

    def reg(al=1):
        return rng.randrange(0, 32, al) if rng.random() < 0.5 else rng.randrange(4, 16, al)

    words = []
    if fmt == 0:   # REG
        key = op["key"]
        src1, src2 = reg(2 if dbl else 1), reg(2 if dbl else 1)
        dst = reg(max(align, 2 if dbl else 1))
        m1 = m2 = m3 = 0
        if op["fp"]:
            m1, m2, m3 = (rng.random() < 0.4 for _ in range(3))
            if m1:
                src1 = rng.choice([0, 1, 2, 3, 0x10, 0x16, 5])
            if m2:
                src2 = rng.choice([0, 1, 2, 3, 0x10, 0x16, 7])
            if m3:
                dst = rng.choice([0, 1, 2, 3])
            put_fp_in_gprs(rng, s, [src1, src2], dbl or rng.random() < 0.5)
            if name.startswith("log"):
                # Unicorn's fyl2x leaves x <= 0 on the stack instead of giving
                # -inf/NaN like a real x87, so only positive arguments are
                # comparable.
                make_positive(s, src1 if not m1 else None, dbl, m1 and src1 < 4 and src1)
                if m1 and src1 in (0x10, 5):
                    return gen_case(rng)
        else:
            m1 = rng.random() < 0.3
            m2 = rng.random() < 0.15
            m3 = rng.random() < 0.03
        raw = ((key >> 4) << 24) | (dst << 19) | (src2 << 14) | (m3 << 13) | (m2 << 12) \
            | (m1 << 11) | ((key & 0xf) << 7) | src1
        words.append(raw)
        if name in ("synmov", "synmovq"):
            s.r[src1] = rng.choice([rng.randrange(0x100, 0xf000, 4), 0xff000004, 0xff000010,
                                    0x00900000 + rng.randrange(0, 0x1000, 4)])
            s.r[src2] = rng.choice([rng.randrange(0x100, 0xf000, 4), 0x00900040])
    elif fmt == 1:  # MEM
        sd = reg(align)
        abase = rng.randrange(32)
        index = rng.randrange(32)
        s.r[abase] = rng.choice([rng.randrange(0x100, 0xe000),
                                 0x00900000 + rng.randrange(0x1000)]) & ~(3 if rng.random() < 0.8 else 0)
        if index != abase:
            s.r[index] = rng.randrange(0, 0x80)
        if rng.random() < 0.3:
            offset = rng.randrange(0x1000)
            abmode = rng.randrange(2)
            raw = (op["key"] << 24) | (sd << 19) | (abase << 14) | (abmode << 13) | offset
            if not abmode:
                s.r[abase] = s.r[abase]
            words.append(raw)
        else:
            mode = rng.choice([4, 5, 7, 0xc, 0xd, 0xe, 0xf])
            scale = rng.randrange(5)
            raw = (op["key"] << 24) | (sd << 19) | (abase << 14) | (mode << 10) | (scale << 7) | index
            words.append(raw)
            if mode in (5, 0xc, 0xd, 0xe, 0xf):
                if mode == 0xc:
                    disp = rng.choice([rng.randrange(0x100, 0xe000), 0x00900000 + rng.randrange(0x1000)])
                elif mode == 5:
                    disp = rng.randrange(-0x800, 0x800)
                else:
                    disp = rng.randrange(-0x80, 0x800)
                words.append(disp & 0xffffffff)
        if name in ("ldl", "ldt", "ldq", "stl", "stt", "stq"):
            pass
    elif fmt == 2:  # COBR
        src1, src2 = rng.randrange(32), rng.randrange(32)
        m1 = rng.random() < 0.4
        disp = rng.randrange(-0x400, 0x400) * 4 & 0x1ffc
        raw = (op["key"] << 24) | (src1 << 19) | (src2 << 14) | (m1 << 13) | disp
        if rng.random() < 0.3 and not m1:
            s.r[src2] = s.r[src1]
        words.append(raw)
    else:           # CTRL
        disp = rng.randrange(-0x100000, 0x100000) * 4 & 0xfffffc
        words.append((op["key"] << 24) | disp)
        if name == "ret":
            k = rng.random()
            if k < 0.4 and s.fc_count:
                slot = (s.fc_index - 1 - rng.randrange(s.fc_count)) % 128
                s.r[0] = s.fc_addr[slot] | rng.choice([0, 7])
            elif k < 0.7:
                s.r[0] = rng.randrange(0x2000, 0x7fc0, 0x40) | rng.choice([0, 7, 1, 2])
    for i, w in enumerate(words):
        struct.pack_into("<I", ram, s.ip + 4 * i, w)
    # interrupt table and PRCB for irq cases
    struct.pack_into("<I", ram, s.prcb + 0x14, 0xb000)
    struct.pack_into("<I", ram, s.prcb + 0x18, 0xa800)
    return name, s, bytes(ram)


# ---------------------------------------------------------------- comparison

def same_double(a, b, rel):
    if math.isnan(a) and math.isnan(b):
        return True
    if a == b:
        return math.copysign(1, a) == math.copysign(1, b) or rel > 0
    if rel and math.isfinite(a) and math.isfinite(b):
        return abs(a - b) <= rel * max(abs(a), abs(b))
    return False


FLOAT_TOL = re.compile(r"^(sin|cos|tan|atan|log|exp|scale|rem)")


def expected_rem(s, word, name):
    """remr/remrl computed exactly (x87 fprem and C fmod are exact; Unicorn's
    fprem is not), or None when an operand is not a plain register."""
    src1, src2, dst = word & 0x1f, (word >> 14) & 0x1f, (word >> 19) & 0x1f
    if word & 0x3800:
        return None
    if name == "remrl":
        if src1 > 30 or src2 > 30 or dst > 30:
            return None
        get = lambda r: struct.unpack("<d", struct.pack("<II", s.r[r], s.r[r + 1]))[0]
        return ("d", dst, math.fmod(get(src2), get(src1)) if get(src1) != 0 else None)
    get = lambda r: struct.unpack("<f", struct.pack("<I", s.r[r]))[0]
    a, b = get(src2), get(src1)
    if b == 0 or math.isinf(a) or math.isnan(a) or math.isnan(b):
        return None
    return ("f", dst, math.fmod(a, b))


def unreproducible(name, s, word):
    """Cases QUIRKS.md lists as not reproducible (they read x87 stack
    leftovers or bytes past a constant in the original's data section)."""
    m1, src1 = (word >> 11) & 1, word & 0x1f
    if name == "movre" and m1 and src1 in (0x10, 0x16):
        return True
    if name == "tanr":
        v = s.fp[src1] if m1 and src1 < 4 else struct.unpack("<f", struct.pack("<I", s.r[src1]))[0]
        return not abs(v) < 2.0 ** 63
    if name == "tanrl":
        return True if src1 > 30 else not abs(struct.unpack("<d", struct.pack("<II", s.r[src1], s.r[src1 + 1]))[0]) < 2.0 ** 63
    return False


def compare(name, o, p):
    diffs = []
    ro, so, ramo, logo = o
    rp, sp, ramp, logp = p
    if ro != rp:
        diffs.append(f"cycles {ro} != {rp}")
    rel = 1e-12 if FLOAT_TOL.match(name) else 0.0
    for f in ("ip", "cc", "ac", "pc", "sat", "prcb", "icon", "fc_index", "fc_count",
              "in_interrupt", "stop", "multi_count", "multi_addr"):
        if getattr(so, f) != getattr(sp, f):
            diffs.append(f"{f} {getattr(so, f):#x} != {getattr(sp, f):#x}")
    for k in range(32):
        a, b = so.r[k], sp.r[k]
        if a != b:
            if rel:   # FP results stored in GPRs
                fa, fb = struct.unpack("<f", struct.pack("<I", a))[0], struct.unpack("<f", struct.pack("<I", b))[0]
                if same_double(fa, fb, 1e-6):
                    continue
                if k < 31:
                    da = struct.unpack("<d", struct.pack("<II", a, so.r[k + 1] if k + 1 < 32 else 0))[0]
                    db = struct.unpack("<d", struct.pack("<II", b, sp.r[k + 1] if k + 1 < 32 else 0))[0]
                    if same_double(da, db, rel):
                        continue
                if k > 0:
                    da = struct.unpack("<d", struct.pack("<II", so.r[k - 1], a))[0]
                    db = struct.unpack("<d", struct.pack("<II", sp.r[k - 1], b))[0]
                    if same_double(da, db, rel):
                        continue
            diffs.append(f"r{k} {a:#010x} != {b:#010x}")
    for k in range(4):
        if not same_double(so.fp[k], sp.fp[k], rel) and \
                struct.pack("<d", so.fp[k]) != struct.pack("<d", sp.fp[k]):
            diffs.append(f"fp{k} {so.fp[k]!r} != {sp.fp[k]!r}")
    for k in range(128):
        if so.fc_addr[k] != sp.fc_addr[k] or list(so.fc_regs[k]) != list(sp.fc_regs[k]):
            diffs.append(f"frame cache slot {k}")
            break
    for n in range(32):
        if list(so.lit[n]) != list(sp.lit[n]):
            diffs.append(f"literal {n} {list(so.lit[n])} != {list(sp.lit[n])}")
    if ramo != ramp:
        first = next(i for i in range(RAM_SIZE) if ramo[i] != ramp[i])
        diffs.append(f"ram differs at {first:#x}")
    if logo != logp:
        diffs.append(f"io writes {logo[:4]} != {logp[:4]}")
    return diffs


def main():
    rng = random.Random(SEED)
    stats = {}
    failures = 0
    skipped = {}
    for case in range(CASES):
        irq = None
        if rng.random() < 0.03:
            name, s, ram = gen_case(rng)
            name, irq = "(interrupt)", rng.randrange(4)
        else:
            name, s, ram = gen_case(rng)
        o = orig_run(s, ram, irq)
        if o[0] in ("error", "fault"):
            skipped[name] = skipped.get(name, 0) + 1
            continue
        word = struct.unpack_from("<I", ram, s.ip)[0]
        if irq is None and unreproducible(name, s, word):
            skipped[name + " (unreproducible)"] = skipped.get(name + " (unreproducible)", 0) + 1
            continue
        p = port_run(s, ram, irq)
        if name in ("remr", "remrl"):
            exp = expected_rem(s, word, name)
            if exp is None or exp[2] is None:
                skipped[name + " (no exact oracle)"] = skipped.get(name + " (no exact oracle)", 0) + 1
                continue
            kind, dst, val = exp
            if kind == "d":
                got = struct.unpack("<d", struct.pack("<II", p[1].r[dst], p[1].r[dst + 1]))[0]
            else:
                got = struct.unpack("<f", struct.pack("<I", p[1].r[dst]))[0]
                val = struct.unpack("<f", struct.pack("<f", val))[0]
            d = [] if got == val or (math.isnan(got) and math.isnan(val)) else [f"fmod {got!r} != exact {val!r}"]
        else:
            d = compare(name, o, p)
        st = stats.setdefault(name, [0, 0])
        st[0] += 1
        if d:
            st[1] += 1
            failures += 1
            if st[1] <= 3:
                print(f"FAIL case {case} {name} ip={s.ip:#x} word={struct.unpack_from('<I', ram, s.ip)[0]:#010x}: "
                      + "; ".join(d[:6]))
    tested = sum(v[0] for v in stats.values())
    print(f"\n{tested} cases compared over {len(stats)} kinds, {failures} mismatches")
    bad = {k: v for k, v in stats.items() if v[1]}
    if bad:
        print("mismatching kinds:", ", ".join(f"{k} {v[1]}/{v[0]}" for k, v in sorted(bad.items())))
    if skipped:
        print("skipped (original faulted, e.g. divide by zero):",
              ", ".join(f"{k} {v}" for k, v in sorted(skipped.items())))
    missing = sorted({o["name"] for o in OPS} - set(stats))
    if missing:
        print("never compared:", ", ".join(missing))
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
