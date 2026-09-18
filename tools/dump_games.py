#!/usr/bin/env python3
"""Run EMULATOR.EXE's C++ static initializers under Unicorn (with a fake
heap) and dump the game definitions they build. usage: dump_games.py EXE out.json"""
import json, struct, sys
from pathlib import Path
from unicorn import Uc, UcError, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import *

EXE, OUT = sys.argv[1], sys.argv[2]
d = Path(EXE).read_bytes()
pe = struct.unpack_from("<I", d, 0x3c)[0]
nsec = struct.unpack_from("<H", d, pe + 6)[0]
optsz = struct.unpack_from("<H", d, pe + 20)[0]
base = struct.unpack_from("<I", d, pe + 24 + 28)[0]
size = struct.unpack_from("<I", d, pe + 24 + 56)[0]
mu = Uc(UC_ARCH_X86, UC_MODE_32)
mu.mem_map(base, (size + 0xfff) & ~0xfff)
for i in range(nsec):
    _, vs, va, rs, rp = struct.unpack_from("<8sIIII", d, pe + 24 + optsz + i * 40)
    mu.mem_write(base + va, d[rp:rp + rs])
HEAP, STACK, STOP = 0x20000000, 0x30000000, 0x31000000
mu.mem_map(HEAP, 0x4000000); mu.mem_map(STACK, 0x100000); mu.mem_map(STOP, 0x1000)
heap = [HEAP]
TEB = 0x32000000
mu.mem_map(TEB, 0x1000)
mu.mem_write(TEB, struct.pack("<I", 0xffffffff))   # empty SEH chain
# 32-bit mode needs a GDT entry for FS
GDT = 0x33000000
mu.mem_map(GDT, 0x1000)
def desc(base, limit, access, flags):
    return struct.pack("<HHBBBB", limit & 0xffff, base & 0xffff, (base >> 16) & 0xff, access,
                       ((limit >> 16) & 0xf) | (flags << 4), (base >> 24) & 0xff)
# flat code (1), data (2), stack (3) and FS=TEB (4); all ring 0
mu.mem_write(GDT, bytes(8) + desc(0, 0xfffff, 0x9a, 0xc) + desc(0, 0xfffff, 0x92, 0xc)
             + desc(0, 0xfffff, 0x92, 0xc) + desc(TEB, 0xfff, 0x92, 0x4))
mu.reg_write(UC_X86_REG_GDTR, (0, GDT, 5 * 8 - 1, 0))
mu.reg_write(UC_X86_REG_SS, 3 << 3)
mu.reg_write(UC_X86_REG_DS, 2 << 3)
mu.reg_write(UC_X86_REG_ES, 2 << 3)
mu.reg_write(UC_X86_REG_FS, 4 << 3)

def ret_with(uc, val, pop=0):
    esp = uc.reg_read(UC_X86_REG_ESP)
    ra = struct.unpack("<I", uc.mem_read(esp, 4))[0]
    uc.reg_write(UC_X86_REG_EAX, val)
    uc.reg_write(UC_X86_REG_ESP, esp + 4 + pop)
    uc.reg_write(UC_X86_REG_EIP, ra)

def alloc(uc):
    esp = uc.reg_read(UC_X86_REG_ESP)
    n = struct.unpack("<I", uc.mem_read(esp + 4, 4))[0]
    p = heap[0]; heap[0] += (n + 15) & ~15
    ret_with(uc, p)

STUBS = {0x48d1ce: alloc, 0x48d12f: alloc,                       # operator new, malloc
         0x48d1c3: lambda uc: ret_with(uc, 0), 0x48d0f5: lambda uc: ret_with(uc, 0),  # delete, free
         0x48d4db: lambda uc: ret_with(uc, 0)}                                    # _atexit
def hook(uc, addr, sz, _):
    f = STUBS.get(addr)
    if f: f(uc)
mu.hook_add(UC_HOOK_CODE, hook, begin=0x48d0f0, end=0x48d4f0)

table = [struct.unpack_from("<I", d, 0x106600 + 0x508320 - 0x508000 + 4 * i)[0] for i in range(95)]
errors = []
mu.mem_write(STOP, b"\xf4")
for t in table:
    mu.reg_write(UC_X86_REG_ESP, STACK + 0xff000)
    mu.mem_write(STACK + 0xff000, struct.pack("<I", STOP))
    try:
        mu.emu_start(t, STOP, count=2_000_000)
    except UcError as e:
        errors.append((hex(t), hex(mu.reg_read(UC_X86_REG_EIP)), str(e)))
print("initializers run:", len(table), "errors:", len(errors))
for e in errors[:3]: print("  ", e)
Path(OUT + ".mem").write_bytes(bytes(mu.mem_read(0x528000, 0x10f4000 - 0x528000)) )
Path(OUT + ".heap").write_bytes(bytes(mu.mem_read(HEAP, heap[0] - HEAP)))
print("heap used", hex(heap[0] - HEAP))
