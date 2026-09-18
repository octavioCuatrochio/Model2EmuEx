#!/usr/bin/env python3
"""
Differential test of the Model 2/2A copro driver (the i960 side of the TGP):
random sequences of port, control and upload accesses go to the original
handlers (EMULATOR.EXE 0x4c5620, 0x4c5700, 0x4c5730, 0x4c5790, 0x4c57f0,
0x4c5310, 0x4c5450, 0x4c79c0) and to the tgp_m2_* functions. The TGP runs a
program that copies its input FIFO to its output FIFO, so every run stalls.

usage: boardtest.py EMULATOR.EXE libtgptest.so [sequences] [seed]
"""
import ctypes
import random
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
EXE, LIB = sys.argv[1], sys.argv[2]
SEQS = int(sys.argv[3]) if len(sys.argv) > 3 else 300
SEED = int(sys.argv[4]) if len(sys.argv) > 4 else 1

# reuse the machine setup of difftest.py
sys.argv = ["difftest.py", EXE, LIB]
src = (HERE / "difftest.py").read_text().split("if __name__")[0]
g = {"__file__": str(HERE / "difftest.py"), "__name__": "difftest"}
exec(compile(src, "difftest.py", "exec"), g)
mu, lib, port = g["mu"], g["lib"], g["port"]
BLK, BLK_SIZE, PROG, EXT_RAM, EXT_RAM_SIZE = g["BLK"], g["BLK_SIZE"], g["PROG"], g["EXT_RAM"], g["EXT_RAM_SIZE"]
FIFO_IN, FIFO_OUT, STUB, STACK = g["FIFO_IN"], g["FIFO_OUT"], g["STUB"], g["STACK"]
p_prog, p_ext = g["p_prog"], g["p_ext"]
from unicorn.x86_const import UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_EAX, UC_X86_REG_ESP, UC_X86_REG_EIP

DATA_BUF, DATA_WORDS = 0x35000000, 0x4000
mu.mem_map(DATA_BUF, DATA_WORDS * 4)
mu.mem_write(0x5aa87c, struct.pack("<I", PROG))
mu.mem_write(0x5aa8b0, struct.pack("<I", DATA_BUF))
mu.mem_write(0x5aa8c0, struct.pack("<I", 0x574b88))       # current copro driver: Model 2/2A
mu.mem_write(0x4a81d1, struct.pack("<I", 1000))
mu.ctl_remove_cache(0x4a81c0, 0x4a81e0)
g["port_setup"](False)
mu.mem_write(0x5aa8dc, struct.pack("<I", 0))

p_data = (ctypes.c_uint32 * DATA_WORDS)()


class Board(ctypes.Structure):   # the board registers at the end of struct tgp
    _fields_ = [(n, ctypes.c_uint32) for n in (
        "prog_upload_ctrl", "prog_upload_off", "data_upload_ctrl", "data_upload_off",
        "reg_98000c", "ext_ptr", "reg_803008", "reg_801008_read")]


lib.t_board.restype = ctypes.c_void_p
lib.t_board.argtypes = [ctypes.c_void_p]
lib.t_set_data.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint32]
lib.t_call.restype = ctypes.c_uint32
lib.t_call.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_uint32, ctypes.c_uint32]
lib.t_set_data(port, ctypes.addressof(p_data), DATA_WORDS)
lib.t_set_ptr_values.argtypes = [ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32]
lib.t_set_ptr_values(port, g["TABLE"], PROG)
board = Board.from_address(lib.t_board(port))

ORIG_GLOBALS = [("prog_upload_ctrl", 0x10ed8cc), ("prog_upload_off", 0x10ed8d0),
                ("data_upload_ctrl", 0x10ed8d4), ("data_upload_off", 0x10ed8d8),
                ("reg_98000c", 0x10ed8e0), ("ext_ptr", 0x5aa758), ("reg_803008", 0x5aa870),
                ("reg_801008_read", 0x5aa720)]
# which original handler implements each port call (index = t_call op)
HANDLERS = {0: 0x4c5620, 1: 0x4c5700, 2: 0x4c5730, 3: 0x4c5790, 4: 0x4c57f0,
            5: 0x4c5310, 6: 0x4c5450, 7: 0x4c79c0, 8: 0x4c5400}
g["ALLOWED"].update({a for _, a in ORIG_GLOBALS} | {0x5aa87c, 0x5aa8b0, 0x5aa8c0, 0x573320})
# the upload control ends in a virtual call through the driver table
g["ALLOWED"].update({0x574b88 + 0x3c, 0x574b88 + 0x40})


def orig_call(op, addr, val):
    code = b"\xe8" + struct.pack("<i", HANDLERS[op] - (STUB + 0x400 + 5))
    mu.mem_write(STUB + 0x400, code)
    mu.ctl_remove_cache(STUB + 0x400, STUB + 0x410)
    mu.reg_write(UC_X86_REG_ESP, STACK + 0xff00)
    mu.reg_write(UC_X86_REG_ECX, addr)
    mu.reg_write(UC_X86_REG_EDX, val)
    g["stray"].clear()
    try:
        mu.emu_start(STUB + 0x400, STUB + 0x405, count=50_000_000)
    except Exception as e:
        raise RuntimeError(f"original faulted at eip={mu.reg_read(UC_X86_REG_EIP):#x} "
                           f"op={op} addr={addr:#x} val={val:#x}: {e}")
    ret = mu.reg_read(UC_X86_REG_EAX)
    return {2: ret, 3: ret & 0xffff, 4: ret & 0xff, 6: ret}.get(op, 0)


# TGP program: move FIFO in -> FIFO out (with an ALU op), branch back
COPY = [0x1c000000 | (0x13 << 21) | (0x1f << 16) | (0x22 << 9) | 0x21,
        (0x2f << 26) | (0x16 << 20) | 0x0000]


def snapshot_orig():
    vals = {n: struct.unpack("<I", mu.mem_read(a, 4))[0] for n, a in ORIG_GLOBALS}
    return (bytes(mu.mem_read(BLK, BLK_SIZE)), bytes(mu.mem_read(FIFO_IN, 0x8c)),
            bytes(mu.mem_read(FIFO_OUT, 0x8c)), bytes(mu.mem_read(EXT_RAM, 0x10000)),
            bytes(mu.mem_read(PROG, 0x400)), bytes(mu.mem_read(DATA_BUF, 0x400)), vals)


def snapshot_port():
    vals = {n: getattr(board, n) for n, _ in ORIG_GLOBALS}
    return (ctypes.string_at(g["P_BLK"], BLK_SIZE), ctypes.string_at(g["P_IN"], 0x8c),
            ctypes.string_at(g["P_OUT"], 0x8c), bytes(p_ext)[:0x10000],
            ctypes.string_at(ctypes.addressof(p_prog), 0x400),
            ctypes.string_at(ctypes.addressof(p_data), 0x400), vals)


def reset_both(rng):
    prog = struct.pack("<2I", *COPY) + bytes(0x400 - 8)
    mu.mem_write(PROG, prog)
    ctypes.memmove(ctypes.addressof(p_prog), prog, len(prog))
    mu.mem_write(DATA_BUF, bytes(0x400))
    ctypes.memset(ctypes.addressof(p_data), 0, 0x400)
    ext = bytes(0x10000)
    mu.mem_write(EXT_RAM, ext)
    ctypes.memmove(p_ext, ext, len(ext))
    fifo = struct.pack("<32I", *[0] * 32) + struct.pack("<IIi", 0, 0, 32)
    mu.mem_write(FIFO_IN, fifo)
    mu.mem_write(FIFO_OUT, fifo)
    ctypes.memmove(g["P_IN"], fifo, 0x8c)
    ctypes.memmove(g["P_OUT"], fifo, 0x8c)
    # state: program copied in and TGP reset through the upload interface
    blk = bytearray(BLK_SIZE)
    struct.pack_into("<I", blk, 0x8000, g["TABLE"])
    struct.pack_into("<I", blk, 0x8075, PROG)
    blk[0x801e] = 1
    mu.mem_write(BLK, bytes(blk))
    ctypes.memmove(g["P_BLK"], bytes(blk), BLK_SIZE)
    for n, a in ORIG_GLOBALS:
        mu.mem_write(a, struct.pack("<I", 0))
        setattr(board, n, 0)


def rnd_access(rng, uploading):
    k = rng.random()
    if k < 0.25:
        return 0, 0x880000 + rng.randrange(0x400) * 0x10 + rng.choice([0, 0, 4]), rng.getrandbits(32)
    if k < 0.45:
        return 0, 0x884000 + rng.randrange(0x1000) * 4, rng.getrandbits(32)
    if k < 0.5:
        return 1, rng.choice([0x884000, 0x880010, 0x884100]), rng.getrandbits(16)
    if k < 0.65:
        return rng.choice([2, 3, 4]), rng.choice([0x884000, 0x884004, 0x880000]), 0
    if k < 0.8:
        a = rng.choice([0x801008, 0x803008, 0x804000 + rng.randrange(0x400) * 4,
                        0x800000 + rng.randrange(0x3ff) * 0x10, 0x804000 + rng.randrange(0x1000) * 4,
                        0x807ffc, 0x800004])
        v = rng.getrandbits(32)
        if a == 0x801008:
            v = rng.randrange(0, 0xf000, 4)
        return 5, a, v
    if k < 0.85:
        return 6, rng.choice([0x801008, 0x802008, 0x803008, 0x800000]), 0
    if k < 0.9:
        return 8, rng.choice([0x804000, 0x804004]), rng.getrandbits(16)
    a = rng.choice([0x980000, 0x980008, 0x98000c])
    v = rng.choice([0x80000000, 0, 0x80000001, rng.getrandbits(32)])
    if a == 0x980000:
        # never start a program upload here: a random program would run off
        # the end of the original's tables (the upload path is exercised by
        # the fixed prologue of every sequence)
        v &= 0x7fffffff
    return 7, a, v


def main():
    rng = random.Random(SEED)
    fails = 0
    ops = 0
    for seq in range(SEQS):
        reset_both(rng)
        # upload the copy program through the interface, which also resets the TGP
        script = [(7, 0x980000, 0x80000000)] + [(0, 0x884000, w) for w in COPY] + [(7, 0x980000, 0)]
        script += [rnd_access(rng, False) for _ in range(rng.randrange(5, 40))]
        for op, addr, val in script:
            ro = orig_call(op, addr, val)
            rp = lib.t_call(port, op, addr, val)
            ops += 1
            o, p = snapshot_orig(), snapshot_port()
            if g["stray"]:
                print(f"seq {seq}: original touched {hex(g['stray'][0])} (not modelled)")
            if ro != rp or o != p:
                fails += 1
                what = [n for n, a, b in zip(("blk", "fifo in", "fifo out", "ext", "prog", "data", "regs"), o, p) if a != b]
                print(f"FAIL seq {seq} op {op} addr {addr:#x} val {val:#x}: ret {ro:#x}/{rp:#x} {what}"
                      + (f" {o[6]} vs {p[6]}" if "regs" in what else ""))
                break
    print(f"\n{SEQS} sequences, {ops} driver calls compared, {fails} failing sequences")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main())
