#!/usr/bin/env python3
"""
Differential test of the TGP port against the original machine code.

The original interpreter (EMULATOR.EXE 0x4a81c0) runs under Unicorn on its
real global state block; the port runs through test/shim.c. Both get the same
random state block, program, FIFOs, table ROM and external memory; after
running one instruction (or a burst of up to 1000) everything is compared:
the whole 0x90c0-byte state block, both FIFOs and external RAM.

Cases where the original touches memory outside the state block and the
buffers the port models (other emulator globals) are skipped.

Build the library with -DTGP_TEST_UNICORN_FLD: Unicorn's x87 does not quiet
signalling NaNs on fld like real hardware does, and the flag makes the port
do the same so that results can be compared exactly.

usage: difftest.py EMULATOR.EXE libtgptest.so [cases] [seed]
"""
import ctypes
import math
import random
import struct
import sys
from pathlib import Path

from unicorn import (Uc, UcError, UC_ARCH_X86, UC_MODE_32, UC_HOOK_MEM_READ,
                     UC_HOOK_MEM_WRITE, UC_HOOK_INTR)
from unicorn.x86_const import UC_X86_REG_EIP, UC_X86_REG_ESP

EXE, LIB = sys.argv[1], sys.argv[2]
CASES = int(sys.argv[3]) if len(sys.argv) > 3 else 5000
SEED = int(sys.argv[4]) if len(sys.argv) > 4 else 1
ONLY_STEP = len(sys.argv) > 5 and sys.argv[5] == "step"

BLK = 0x10e3ff0
BLK_SIZE = 0x90c0
O_TABLEPTR, O_A, O_B, O_C, O_D, O_P = 0x8000, 0x8008, 0x800c, 0x8010, 0x8014, 0x8018
O_PC, O_STOP, O_REPEAT, O_STACK, O_SP = 0x801c, 0x801e, 0x801f, 0x8021, 0x8029
O_FLAGS2, O_BANK, O_SHIFT, O_R34, O_SR, O_R = 0x802a, 0x802b, 0x802f, 0x8033, 0x8034, 0x8035
O_PROGPTR, O_RAM, O_MODE, O_TREG = 0x8075, 0x8079, 0x9079, 0x9000

FIFO_IN, FIFO_OUT = 0x70010000, 0x70010100
EXT_RAM, EXT_RAM_SIZE = 0x30000000, 0x100000
EXT_ROM, EXT_ROM_SIZE = 0x31000000, 0x1000000
TABLE, TABLE_SIZE = 0x33000000, 0x40000
PROG, PROG_WORDS = 0x34000000, 0x10000
STUB, STACK = 0x70000000, 0x71000000
LOOP_IMM = 0x4a81d1            # imm32 of "mov [esp+0x14], 1000"

ALLOWED = {0x5aa700, 0x5aa724, 0x5aa738, 0x5aa8dc, 0x10f1fe4, 0x10f1fec}

# ------------------------------------------------------------------ shared data

rng0 = random.Random(1234)
table_rom = bytearray(TABLE_SIZE)
for i in range(0, TABLE_SIZE, 4):      # plausible floats and shorts
    struct.pack_into("<f", table_rom, i, math.sin(i / 7.0) * 0.999 + 0.0001 * (i % 13))
ext_rom = bytes((i * 2654435761 >> 13) & 0xff for i in range(0x10000)) * (EXT_ROM_SIZE // 0x10000)
ext_ram_init = bytes(rng0.getrandbits(8) for _ in range(0x10000)) * (EXT_RAM_SIZE // 0x10000)

# ------------------------------------------------------------------ port side

lib = ctypes.CDLL(LIB)
lib.t_new.restype = ctypes.c_void_p
for f in ("t_blk", "t_in", "t_out"):
    getattr(lib, f).restype = ctypes.c_void_p
    getattr(lib, f).argtypes = [ctypes.c_void_p]
lib.t_setup.argtypes = [ctypes.c_void_p] + [ctypes.c_void_p, ctypes.c_uint32] * 4
lib.t_run.argtypes = [ctypes.c_void_p, ctypes.c_int]

port = lib.t_new()
p_prog = (ctypes.c_uint32 * PROG_WORDS)()
p_table = (ctypes.c_uint8 * TABLE_SIZE).from_buffer_copy(table_rom)
p_ext = (ctypes.c_uint8 * EXT_RAM_SIZE)()
p_rom = (ctypes.c_uint8 * EXT_ROM_SIZE).from_buffer_copy(ext_rom)
P_BLK, P_IN, P_OUT = lib.t_blk(port), lib.t_in(port), lib.t_out(port)


def port_setup(with_rom):
    lib.t_setup(port, ctypes.addressof(p_prog), PROG_WORDS, ctypes.addressof(p_table), TABLE_SIZE,
                ctypes.addressof(p_ext), EXT_RAM_SIZE // 4,
                ctypes.addressof(p_rom) if with_rom else None, EXT_ROM_SIZE // 4)


# -------------------------------------------------------------- original side

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
for a, n in ((EXT_RAM, EXT_RAM_SIZE), (EXT_ROM, EXT_ROM_SIZE), (TABLE, TABLE_SIZE),
             (PROG, PROG_WORDS * 4), (STUB, 0x20000), (STACK, 0x10000)):
    mu.mem_map(a, n)
mu.mem_write(TABLE, bytes(table_rom))
mu.mem_write(EXT_ROM, ext_rom)
mu.mem_write(0x10f1fec, struct.pack("<I", 1))   # CRT: SSE2 present
mu.mem_write(0x10f1fe4, struct.pack("<I", 1))
mu.mem_write(0x5aa724, struct.pack("<I", FIFO_IN))
mu.mem_write(0x5aa738, struct.pack("<I", FIFO_OUT))
mu.mem_write(0x5aa700, struct.pack("<I", EXT_RAM))
CW = STUB + 0x300
mu.mem_write(CW, struct.pack("<H", 0x27f))
# runner: fldcw [cw]; call tgp_run
code = b"\xd9\x2d" + struct.pack("<I", CW)
code += b"\xe8" + struct.pack("<i", 0x4a81c0 - (STUB + len(code) + 5))
mu.mem_write(STUB, code)
RUN_END = STUB + len(code)

stray = []


def on_mem(uc, access, addr, size, value, data):
    if (addr & ~3) not in ALLOWED:
        stray.append(addr)


mu.hook_add(UC_HOOK_MEM_READ | UC_HOOK_MEM_WRITE, on_mem, begin=0x528000, end=BLK - 1)
IMAGE_END = 0x10f4000 + 0x435b0     # end of .rsrc
mu.hook_add(UC_HOOK_MEM_READ | UC_HOOK_MEM_WRITE, on_mem, begin=BLK + BLK_SIZE, end=IMAGE_END)
faults = []
mu.hook_add(UC_HOOK_INTR, lambda uc, n, d: (faults.append(n), uc.emu_stop()))


def orig_run(blk, fin, fout, prog_window, n):
    mu.mem_write(BLK, bytes(blk))
    mu.mem_write(FIFO_IN, fin)
    mu.mem_write(FIFO_OUT, fout)
    mu.mem_write(LOOP_IMM, struct.pack("<I", n))
    mu.ctl_remove_cache(0x4a81c0, 0x4a81e0)   # drop the translated copy of the patched code
    stray.clear()
    faults.clear()
    mu.reg_write(UC_X86_REG_ESP, STACK + 0xff00)
    try:
        mu.emu_start(STUB, RUN_END, count=5_000_000)
    except UcError as e:
        return ("error", str(e))
    if faults or mu.reg_read(UC_X86_REG_EIP) != RUN_END:
        return ("error", "fault/stopped")
    if stray:
        return ("stray", hex(stray[0]))
    return (bytes(mu.mem_read(BLK, BLK_SIZE)), bytes(mu.mem_read(FIFO_IN, 0x8c)),
            bytes(mu.mem_read(FIFO_OUT, 0x8c)), bytes(mu.mem_read(EXT_RAM, EXT_RAM_SIZE)))


def port_run(blk, fin, fout, n):
    ctypes.memmove(P_BLK, bytes(blk), BLK_SIZE)
    ctypes.memmove(P_IN, fin, 0x8c)
    ctypes.memmove(P_OUT, fout, 0x8c)
    lib.t_run(port, n)
    return (ctypes.string_at(P_BLK, BLK_SIZE), ctypes.string_at(P_IN, 0x8c),
            ctypes.string_at(P_OUT, 0x8c), bytes(p_ext))


# ------------------------------------------------------------ case generator

SUB0 = [1, 4, 0xc, 0xd, 0xf, 0x10, 0x11, 0, 2, 5]
SUB7 = [3, 4, 7, 8, 0xb, 0xc, 0xd, 0xf, 0x10, 0x13, 0x14, 0x17, 0x1c, 0x1d, 0x1e, 0x1f, 5, 0]


def rnd_float_bits(rng):
    k = rng.random()
    if k < 0.55:
        return struct.unpack("<I", struct.pack("<f", rng.uniform(-100, 100)))[0]
    if k < 0.7:
        return struct.unpack("<I", struct.pack("<f", rng.choice([0.0, -0.0, 1.0, -1.0, 0.5, 2.5, -2.5, 1e30, -1e-30])))[0]
    if k < 0.8:
        return rng.randrange(-40, 40) & 0xffffffff
    if k < 0.85:
        return rng.choice([0x7f800000, 0xff800000, 0x7fc00000, 0xffc00000, 0x7fc00001, 0x7fa00000,
                           0xff800001, 0x00000001, 0x80400000])
    return rng.getrandbits(32)


def mem_code(rng):
    """9-bit memory operand whose address stays inside RAM."""
    mode = rng.choice([0, 1, 2, 3, 6, 7, 4, 5])
    return (mode << 6) | rng.getrandbits(6)


def rnd_insn(rng, pc):
    k = rng.random()
    alu = rng.randrange(0x20)
    if k < 0.3:
        return (alu << 21) | (rng.choice(SUB0) << 16) | (rng.getrandbits(7) << 9) | mem_code(rng)
    if k < 0.6:
        sub = rng.choice(SUB7)
        d = rng.getrandbits(7)
        if sub in (0x1c, 0x1d, 0x1e, 0x1f) and rng.random() < 0.7:
            d = rng.choice(list(range(0x10, 0x20)) + [0x21, 0x22, 0x23, 0x34, 4, 5, 8, 12, 13]) | (rng.getrandbits(1) << 6)
        s = mem_code(rng)
        if sub in (0x1f,) and rng.random() < 0.6:
            s = rng.choice(list(range(0x10, 0x20)) + [0x21, 0x23, 0x34, 5, 9, 12])
        return 0x1c000000 | (alu << 21) | (sub << 16) | (d << 9) | s
    if k < 0.75:
        op = rng.choice([0x2f, 0x3f])
        cond = rng.choice([0, 1, 2, 0xa, 0x10, 0x11, 0x16, 3])
        sub = rng.choice([0, 2, 4, 6, 0xa, 0xc, 1])
        if sub in (0, 4):
            tgt = (pc + rng.randrange(-8, 9)) & 0xffff
        elif sub == 6:
            tgt = rng.choice([rng.randrange(0x400), 0x4000 | rng.choice([5, 0x10, 0x19, 12])])
        elif sub == 2:
            tgt = rng.choice([5, 6, 0x10, 0x19, 12])
        else:
            tgt = rng.getrandbits(16) & 0xfdff
        return (op << 26) | (cond << 20) | (sub << 16) | tgt
    if k < 0.8:
        sub = rng.choice([4, 4, 2, 6, 0, 1])
        low = rng.choice([4, 8, 0x10, rng.getrandbits(12)])
        if sub == 4:
            low = rng.choice([0x000 | rng.randrange(1, 5), 0x8000 | rng.choice([12, 13, 5]), 0])
        return (0xf << 26) | (rng.randrange(0x20) << 20) | (sub << 16) | low
    op = rng.choice([0xd, 0xe, 0x10, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x1d, 0x05, 0x20])
    w = (op << 26) | rng.getrandbits(26)
    if op == 0xd and rng.random() < 0.7:
        w = (0xd << 26) | 0xa0000 | rng.getrandbits(8)
    if op in (0x17, 0x18) and rng.random() < 0.7:
        w |= 0x3000000
        if op == 0x17:
            w = (w & ~0xffffff) | rng.randrange(40)
        else:
            w = (w & ~0xffffff) | rng.choice([0, 0x400000 | rng.randrange(4) << 16, 0x800000 | rng.randrange(0x40) << 16, 0x100000])
    if op == 0x10:
        w = (w & ~0xffff) | rng.randrange(0x40)
    if op == 0x13:
        w = (w & ~0xffffff) | rng.randrange(8)
    return w


def gen_case(rng):
    blk = bytearray(BLK_SIZE)
    pc = rng.randrange(0x10, 0xfc0)
    window = [rnd_insn(rng, pc + i) for i in range(48)]
    for i in range(48):
        if rng.random() < 0.08:
            struct.pack_into("<I", blk, (pc + i) * 8, rng.choice([1, 2, 3, 0x10]))
    struct.pack_into("<I", blk, O_TABLEPTR, TABLE)
    struct.pack_into("<I", blk, O_PROGPTR, PROG)
    for off in (O_A, O_B, O_C, O_D, O_P):
        struct.pack_into("<I", blk, off, rnd_float_bits(rng))
    struct.pack_into("<H", blk, O_PC, pc)
    struct.pack_into("<H", blk, O_REPEAT, rng.choice([0] * 8 + [1, 2, 5]))
    blk[O_SP] = rng.choice([0, 0, 1, 2, 3])
    for i in range(4):
        struct.pack_into("<H", blk, O_STACK + 2 * i, (pc + rng.randrange(-20, 20)) & 0xffff)
    blk[O_FLAGS2] = rng.getrandbits(8)
    struct.pack_into("<I", blk, O_BANK, rng.choice([0, 0, 0x400000 | rng.randrange(4) << 16,
                                                    0x800000 | rng.randrange(0x40) << 16, 0x100000]))
    struct.pack_into("<I", blk, O_SHIFT, rng.randrange(40))
    blk[O_R34] = rng.getrandbits(8)
    blk[O_SR] = rng.getrandbits(3)
    for n in range(16):
        v = rng.randrange(0x40) if n < 4 else (rng.randrange(1, 6) if n in (12, 13) else rnd_float_bits(rng))
        struct.pack_into("<I", blk, O_R + 4 * n, v)
    for i in range(0x400):
        struct.pack_into("<I", blk, O_RAM + 4 * i, rnd_float_bits(rng) if rng.random() < 0.9 else rng.randrange(0x400))
    blk[O_MODE] = rng.getrandbits(8)
    for n in range(0x20, 0x30):
        v = rng.choice([rng.getrandbits(16), rnd_float_bits(rng), rng.getrandbits(32)])
        struct.pack_into("<I", blk, O_TREG + 4 * n, v)
    # table registers the table unit reads need plausible exponents
    struct.pack_into("<I", blk, O_TREG + 4 * 0x27, rng.choice([0, rng.getrandbits(32), (0x70 + rng.randrange(16)) << 23 | rng.getrandbits(23)]))
    blk[O_STOP] = 0
    fifos = []
    for _ in range(2):
        free = rng.choice([0, 1, 16, 31, 32, rng.randrange(33)])
        rd = rng.randrange(32)
        wr = (rd + 32 - free) & 31
        fifos.append(struct.pack("<32I", *[rnd_float_bits(rng) for _ in range(32)]) + struct.pack("<IIi", rd, wr, free))
    n = 1 if (rng.random() < 0.7 or ONLY_STEP) else rng.choice([2, 5, 20, 1000])
    return pc, window, blk, fifos[0], fifos[1], n


# ----------------------------------------------------------------------- main

FIELDS = [("pflags", 0, 0x8000), ("A", O_A, 4), ("B", O_B, 4), ("C", O_C, 4), ("D", O_D, 4),
          ("P", O_P, 4), ("PC", O_PC, 2), ("stop", O_STOP, 1), ("repeat", O_REPEAT, 2),
          ("stack", O_STACK, 8), ("SP", O_SP, 1), ("flags2", O_FLAGS2, 1), ("bank", O_BANK, 4),
          ("shift", O_SHIFT, 4), ("r34", O_R34, 1), ("SR", O_SR, 1), ("r", O_R, 64),
          ("RAM", O_RAM, 0x1000), ("mode", O_MODE, 1), ("treg", O_TREG + 0x80, 0x40)]


def describe(bo, bp):
    out = []
    for name, off, n in FIELDS:
        if bo[off:off + n] != bp[off:off + n]:
            if n <= 8:
                out.append(f"{name} {bo[off:off+n].hex()} != {bp[off:off+n].hex()}")
            else:
                i = next(j for j in range(n) if bo[off + j] != bp[off + j]) & ~3
                out.append(f"{name}[+{i:#x}] {bo[off+i:off+i+4].hex()} != {bp[off+i:off+i+4].hex()}")
    if not out:
        i = next(j for j in range(BLK_SIZE) if bo[j] != bp[j])
        out.append(f"blk[{i:#x}]")
    return out


def snan_only(o, p):
    """True if the results differ only by the quiet bit of signalling NaNs
    that Unicorn's fld failed to quiet (a real x87 quiets them)."""
    if o[1:] != p[1:]:
        return False
    a, b = o[0], p[0]
    for i in range(0, BLK_SIZE - 3):
        if a[i] != b[i]:
            break
    else:
        return True
    for off in range(0, BLK_SIZE - 3):
        x = struct.unpack_from("<I", a, off)[0]
        y = struct.unpack_from("<I", b, off)[0]
        if x != y and not ((x & 0x7fc00000) == 0x7f800000 and x & 0x3fffff and y == x | 0x400000):
            if all(a[k] == b[k] for k in range(off, off + 4)):
                continue
            # tolerate only aligned-to-field words; any other difference fails
            if (x ^ y) & ~0x400000:
                return False
    return True


def main():
    rng = random.Random(SEED)
    stats = {"compared": 0, "mismatch": 0, "skipped": {}}
    last_window = None
    shown = 0
    for case in range(CASES):
        pc, window, blk, fin, fout, n = gen_case(rng)
        with_rom = rng.random() < 0.5
        mu.mem_write(0x5aa8dc, struct.pack("<I", EXT_ROM if with_rom else 0))
        port_setup(with_rom)
        # program window: clear the previous one, write the new one
        if last_window is not None:
            mu.mem_write(PROG + last_window * 4, bytes(4 * 48))
            ctypes.memset(ctypes.addressof(p_prog) + last_window * 4, 0, 4 * 48)
        progbytes = struct.pack("<48I", *window)
        mu.mem_write(PROG + pc * 4, progbytes)
        ctypes.memmove(ctypes.addressof(p_prog) + pc * 4, progbytes, len(progbytes))
        last_window = pc
        mu.mem_write(EXT_RAM, ext_ram_init)
        ctypes.memmove(p_ext, ext_ram_init, EXT_RAM_SIZE)

        o = orig_run(blk, fin, fout, window, n)
        if isinstance(o[0], str):
            key = o[0] + (" " + o[1] if o[0] == "error" else "")
            stats["skipped"][key] = stats["skipped"].get(key, 0) + 1
            continue
        if o[0][O_TABLEPTR:O_TABLEPTR + 4] != blk[O_TABLEPTR:O_TABLEPTR + 4] or \
                o[0][O_PROGPTR:O_PROGPTR + 4] != blk[O_PROGPTR:O_PROGPTR + 4]:
            # the program overwrote one of the original's host pointers
            stats["skipped"]["pointer slot overwritten"] = stats["skipped"].get("pointer slot overwritten", 0) + 1
            continue
        p = port_run(blk, fin, fout, n)
        stats["compared"] += 1
        if o != p:
            stats["mismatch"] += 1
            if shown < 12:
                shown += 1
                d = describe(o[0], p[0]) if o[0] != p[0] else []
                if o[1] != p[1]: d.append("fifo in")
                if o[2] != p[2]: d.append("fifo out")
                if o[3] != p[3]:
                    i = next(j for j in range(EXT_RAM_SIZE) if o[3][j] != p[3][j])
                    d.append(f"ext ram at {i:#x}")
                print(f"FAIL case {case} n={n} pc={pc:#x} word={window[0]:#010x}: " + "; ".join(d[:5]))
    print(f"\n{stats['compared']} cases compared, {stats['mismatch']} mismatches")
    if stats["skipped"]:
        print("skipped:", ", ".join(f"{k}: {v}" for k, v in stats["skipped"].items()))
    return 1 if stats["mismatch"] else 0


if __name__ == "__main__":
    sys.exit(main())
