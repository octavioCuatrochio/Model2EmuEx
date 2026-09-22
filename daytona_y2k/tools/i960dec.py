"""i960 instruction decoding, following the port's interpreter
(port/i960/i960.c: decode, decode_mem, opcode_key) so that the recompiler
sees every instruction exactly as the interpreter executes it."""
import re, struct, zlib, zipfile, os

HERE = os.path.dirname(os.path.abspath(__file__))
OPTABLE = os.path.join(HERE, '..', '..', 'port', 'i960', 'i960_optable.inc')


class OpDef:
    def __init__(self, name, fmt, key, cycles, fp, operands):
        self.name, self.fmt, self.key, self.cycles, self.fp, self.operands = name, fmt, key, cycles, fp, operands


def load_optable(path=OPTABLE):
    ops = {}
    for line in open(path):
        m = re.match(r'OP\(op_(\w+)\s*,\s*(\d+),\s*0x([0-9a-f]+),\s*(\d+),\s*(\d+),\s*\d+,\s*\d+,\s*0x([0-9a-f]+)\)', line)
        if m:
            d = OpDef(m.group(1), int(m.group(2)), int(m.group(3), 16), int(m.group(4)), int(m.group(5)), int(m.group(6), 16))
            ops[d.key] = d
    return ops


OPS = load_optable()
INVALID = OpDef('invalid', 0, -1, 0, 0, 0)


def opcode_key(raw):
    key = raw >> 24
    if 0 <= key - 0x58 < 0x22:
        key = (key << 4) | ((raw >> 7) & 0xf)
    return key & 0xfff


def sext(v, bits):
    return v - (1 << bits) if v & (1 << (bits - 1)) else v


class Insn:
    """One decoded instruction. Operand fields follow the interpreter:
    REG: src1/m1, src2/m2, dst/m3; MEM: dst (reg), ea (mode tuple);
    COBR: src1/m1, src2, target; CTRL: target."""
    def __init__(self, addr, raw, op):
        self.addr, self.raw, self.op = addr, raw, op
        self.len = 4
        self.target = None
        self.ea = None
        self.disp_word = None

    @property
    def name(self):
        return self.op.name

    @property
    def next(self):
        return (self.addr + self.len) & 0xffffffff


def decode(mem32, addr):
    raw = mem32(addr)
    if raw is None:
        return None
    op = OPS.get(opcode_key(raw), INVALID)
    i = Insn(addr, raw, op)
    m = op.operands
    if op.fmt == 0:
        i.src1, i.m1 = raw & 0x1f, bool(raw & 0x800)
        i.src2, i.m2 = (raw >> 14) & 0x1f, bool(raw & 0x1000)
        i.dst, i.m3 = (raw >> 19) & 0x1f, bool(raw & 0x2000)
    elif op.fmt == 1:
        i.dst = (raw >> 19) & 0x1f
        if m & 8:
            abase = (raw >> 14) & 0x1f
            if not raw & 0x1000:
                off = raw & 0x1fff
                i.ea = ('abase_off', abase, off) if raw & 0x2000 else ('off', off)
            else:
                index, scale, mode = raw & 0x1f, (raw >> 7) & 7, (raw >> 10) & 0xf
                if mode in (5, 0xc, 0xd, 0xe, 0xf):
                    d = mem32(addr + 4)
                    if d is None:
                        return None
                    i.disp_word = d
                    i.len = 8
                if mode == 4:
                    i.ea = ('abase', abase)
                elif mode == 5:
                    i.ea = ('off', (d + addr + 4 + 8) & 0xffffffff)
                elif mode == 7:
                    i.ea = ('abase_index', abase, index, scale)
                elif mode == 0xc:
                    i.ea = ('off', d)
                elif mode == 0xd:
                    i.ea = ('abase_off', abase, d)
                elif mode == 0xe:
                    i.ea = ('index_off', index, scale, d)
                elif mode == 0xf:
                    i.ea = ('abase_index_off', abase, index, scale, d)
                else:
                    i.ea = ('reserved',)
    elif op.fmt == 2:
        i.src1, i.m1 = (raw >> 19) & 0x1f, bool(raw & 0x2000)
        i.src2 = (raw >> 14) & 0x1f
        if m & 8:
            d = sext(raw & 0x1fff, 13)
            i.target = (addr + d) & 0xffffffff
    elif op.fmt == 3:
        if m & 8:
            d = sext(raw & 0xffffff, 24)
            i.target = (addr + d) & 0xffffffff
    return i


def regname(n):
    if n < 16:
        return ('pfp', 'sp', 'rip')[n] if n < 3 else 'r%d' % n
    return 'fp' if n == 31 else 'g%d' % (n - 16)


def ea_text(ea):
    k = ea[0]
    if k == 'off': return '0x%x' % ea[1]
    if k == 'abase_off': return '0x%x(%s)' % (ea[2], regname(ea[1]))
    if k == 'abase': return '(%s)' % regname(ea[1])
    if k == 'abase_index': return '(%s)[%s*%d]' % (regname(ea[1]), regname(ea[2]), 1 << ea[3])
    if k == 'index_off': return '0x%x[%s*%d]' % (ea[3], regname(ea[1]), 1 << ea[2])
    if k == 'abase_index_off': return '0x%x(%s)[%s*%d]' % (ea[4], regname(ea[1]), regname(ea[2]), 1 << ea[3])
    return '?'


def disasm(i):
    op = i.op
    if op is INVALID:
        return '.word 0x%08x' % i.raw
    n = op.name
    if op.fmt == 0:
        ops = []
        lit = lambda r, m: str(r) if m else regname(r)
        if op.operands & 1: ops.append(lit(i.src1, i.m1))
        if op.operands & 2: ops.append(lit(i.src2, i.m2))
        if op.operands & 4: ops.append(lit(i.dst, i.m3))
        return '%s %s' % (n, ','.join(ops))
    if op.fmt == 1:
        if n in ('bx', 'callx'):
            return '%s %s' % (n, ea_text(i.ea))
        if n.startswith('st'):
            return '%s %s,%s' % (n, regname(i.dst), ea_text(i.ea))
        return '%s %s,%s' % (n, ea_text(i.ea), regname(i.dst))
    if op.fmt == 2:
        s1 = str(i.src1) if i.m1 else regname(i.src1)
        if n.startswith('test'):
            return '%s %s' % (n, s1)
        return '%s %s,%s,0x%x' % (n, s1, regname(i.src2), i.target)
    if i.target is not None:
        return '%s 0x%x' % (n, i.target)
    return n


# ------------------------------------------------------------- the ROM

PROGRAM = [('epr-16722a.12', 0x48b94318), ('epr-16723a.13', 0x8af8b32d)]
PROGRAM_SIZE = 0x40000   # the rest of the 2 MB region is empty


def load_program(romdir):
    """Daytona USA program ROM (region 0, 2 MB): two 128 KB EPROMs,
    16-bit interleaved, as port/m2/gamedb.inc loads them."""
    z = zipfile.ZipFile(os.path.join(romdir, 'daytona.zip'))
    rom = bytearray(0x200000)
    for k, (name, crc) in enumerate(PROGRAM):
        data = z.read(name)
        if zlib.crc32(data) & 0xffffffff != crc:
            raise SystemExit('%s: bad CRC (need the parent daytona set)' % name)
        for j in range(0, len(data), 2):
            rom[j * 2 + k * 2: j * 2 + k * 2 + 2] = data[j:j + 2]
    return bytes(rom)


def code_reader(rom):
    """Reads instruction words as the board maps them: program ROM at 0,
    and its upper half again at 0x220000 (Model 2 memory map)."""
    def mem32(a):
        if a < PROGRAM_SIZE:
            o = a
        elif 0x220000 <= a < 0x240000:
            o = a - 0x200000
        else:
            return None
        if a & 3:
            return None
        return struct.unpack_from('<I', rom, o)[0]
    return mem32
