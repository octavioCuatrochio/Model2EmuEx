#!/usr/bin/env python3
"""Static recompiler: i960 machine code of Daytona USA -> C.

    i960rec.py <romdir> <outdir> [trace.txt ...]

Functions rewritten by hand in src/ (RT_FN(f_<address>)) are left out of
the generated code: theirs is the one that runs.

Every function (a call, bal or interrupt entry, and all the code reachable
from it without following calls) becomes one C function that does exactly
what the port's interpreter (port/i960/i960.c) would do, instruction by
instruction, cycle count included: the guest state stays in i960_state, and
a slice ends at the same instruction as on the interpreter. What the
recompiler can't follow (an untraced jump-table target, a call into code it
never saw) goes to the interpreter until the function returns, so the
output always runs. See ../README.md.

Traces (tools/trace.c) add the entries and jump-table targets that static
analysis can't find.
"""
import glob, os, re, struct, sys, zipfile, zlib
from collections import defaultdict

import i960dec as D

# ------------------------------------------------------------------ input

DATA_HI = [('epr-16724a.6', 0x469f10fd), ('epr-16725a.7', 0xba0df8db)]   # data ROM 0x800000
IDLE_HOOK = 0x1394          # gamedb.inc: M2_HOOK_IDLE_COUNT at 0x1394
REGION_BOUNDARY = 0xffe00000  # the interpreter re-selects its code region when these bits change


def load_itab_handlers(romdir, rom):
    """Interrupt handlers: PRCB+0x14 points at the interrupt table, which
    Daytona keeps in data ROM (0x2802040 -> data ROM 0x802040)."""
    prcb = struct.unpack_from('<I', rom, 4)[0]
    itab = struct.unpack_from('<I', rom, prcb + 0x14)[0]
    if not 0x2800000 <= itab < 0x2900000:
        return set()
    z = zipfile.ZipFile(os.path.join(romdir, 'daytona.zip'))
    parts = [z.read(n) for n, _ in DATA_HI]
    hi = bytearray(len(parts[0]) * 2)
    for k, data in enumerate(parts):
        for j in range(0, len(data), 2):
            hi[j * 2 + k * 2: j * 2 + k * 2 + 2] = data[j:j + 2]
    o = itab - 0x2800000
    out = set()
    for vec in range(8, 256):   # vectors 0-7 are not usable
        h = struct.unpack_from('<I', hi, o + 4 + vec * 4)[0]
        if 0x100 <= h < D.PROGRAM_SIZE or 0x220000 <= h < 0x240000:
            out.add(h)
    return out


def load_symbols(path=None, data=False):
    """symbols.txt: address -> (name, what it does), for the comments in
    the generated C. Names are worked out by hand; nothing depends on them.
    Lines starting with `d` name memory instead of a function."""
    path = path or os.path.join(HERE, '..', 'symbols.txt')
    out = {}
    if not os.path.exists(path):
        return out
    for line in open(path):
        line = line.split('#')[0].strip()
        if not line:
            continue
        parts = line.split(None, 3)
        if parts[0] == 't':              # geometry commands: load_tgp_commands
            continue
        is_data = parts[0] == 'd'
        if is_data:
            parts = parts[1:]
        if is_data == data and len(parts) >= 2:
            out[int(parts[0], 16)] = (parts[1], parts[2] if len(parts) > 2 else '')
    return out


def load_tgp_commands(path=None):
    """symbols.txt `t` lines: geometry processor command -> name"""
    path = path or os.path.join(HERE, '..', 'symbols.txt')
    out = {}
    if os.path.exists(path):
        for line in open(path):
            p = line.split('#')[0].split()
            if len(p) >= 3 and p[0] == 't':
                out[int(p[1], 16)] = p[2]
    return out


TGP_BASE = 27   # g11 holds 0x880000, the command page (boot_main)


TGP_PARAM = 28   # g12 holds 0x4000, from the command page to the parameters


DL_BASE = 26     # g10 holds 0x800000, the copro control page


def dl_note(i):
    """a write into the display list through g10: the copro takes the
    float's exponent from the address (port/tgp/tgp.c, tgp_m2_ctrl_write32)"""
    if i.ea is None or not i.name.startswith('st'):
        return ''
    if i.ea[0] == 'abase_index' and i.ea[1] == DL_BASE and i.ea[2] == TGP_PARAM:
        return ' display list, word'
    if i.ea[0] == 'abase_off' and i.ea[1] == DL_BASE and i.ea[2] < 0x4000 and not i.ea[2] & 0xf:
        return ' display list, exponent %d' % ((i.ea[2] >> 4) & 0x3f)
    return ''


def tgp_note(i, cmds):
    """what a store to or load from the geometry processor through g11 is"""
    if i.ea is None:
        return ''
    if i.ea[0] == 'abase_index' and i.ea[1] == TGP_BASE and i.ea[2] == TGP_PARAM:
        return ' TGP param' if i.name.startswith('st') else ' TGP result'
    if i.ea[0] != 'abase_off' or i.ea[1] != TGP_BASE:
        return ''
    off = i.ea[2]
    if i.name.startswith('st') and off < 0x4000 and not off & 0xf:
        c = off >> 4
        return ' TGP %s' % cmds.get(c, 'cmd %02x' % c)
    if 0x4000 <= off < 0x8000:
        return ' TGP param' if i.name.startswith('st') else ' TGP result'
    return ''


def load_traces(paths):
    t = {'X': set(), 'C': defaultdict(set), 'B': defaultdict(set), 'J': defaultdict(set),
         'N': defaultdict(set), 'E': set()}
    for p in paths:
        for line in open(p):
            k, a, b = line.split()
            a, b = int(a, 16), int(b, 16)
            if k == 'X':
                t['X'].update(range(a, b, 4))
            elif k == 'E':
                t['E'].add(b)
            elif k in 'CBJN':
                t[k][a].add(b)
    return t

# --------------------------------------------------------------- analysis

class Func:
    def __init__(self, entry):
        self.entry = entry
        self.insns = {}          # addr -> Insn
        self.labels = set()      # addresses jumped to
        self.jtab = {}           # bx addr -> targets
        self.moved = {}          # stepped insn addr -> traced ip changes (IAC)


def is_bal_return(i):
    """bx (g14) or bx 0(g14): the return of a bal procedure."""
    return i.name == 'bx' and i.ea[0] in ('abase', 'abase_off') and i.ea[1] == 30 and (i.ea[0] == 'abase' or i.ea[2] == 0)


def static_ea(i):
    return i.ea[1] if i.ea and i.ea[0] == 'off' else None


def after_bal(mem32, t):
    """t is the return address of a bal/balx: a bx there is a return."""
    for back in (4, 8):
        p = D.decode(mem32, t - back)
        if p is not None and p.next == t and p.name in ('bal', 'balx'):
            return True
    return False


def static_jump_table(mem32, i):
    """ld table[rX*4],rY; bx (rY): the table's entries (code addresses)."""
    if i.ea[0] != 'abase':
        return []
    for back in (4, 8):
        p = D.decode(mem32, i.addr - back)
        if (p is not None and p.next == i.addr and p.name == 'ld' and p.dst == i.ea[1]
                and p.ea[0] == 'index_off' and p.ea[2] == 2):
            out, a = [], p.ea[3]
            while len(out) < 256:
                t = mem32(a)
                if t is None or t & 3 or D.decode(mem32, t) is None or D.decode(mem32, t).op is D.INVALID:
                    break
                out.append(t)
                a += 4
            return out
    return []


def explore(mem32, entry, traces, entries=frozenset()):
    """The code of the function at `entry`: everything reachable without
    following calls, stopping at other functions' entries (tail jumps)."""
    f = Func(entry)
    work = [entry]
    calls = set()
    while work:
        a = work.pop()
        if a in f.insns or (a != entry and a in entries):
            continue
        i = D.decode(mem32, a)
        if i is None:
            continue   # outside the program: the generated code bails out there
        f.insns[a] = i
        n = i.name
        if i.op is D.INVALID:
            succ = []                 # data, not code: the interpreter takes over if it ever runs
        elif n == 'b':
            succ = [i.target]
        elif n == 'ret':
            # return types 1-6 fall through instead of returning (QUIRKS.md
            # #8), but this program only uses 0 and 7, and what follows a ret
            # belongs to the next function. If an odd type ever turned up, the
            # generated code hands the guest to the interpreter there.
            succ = []
        elif n == 'bx':
            t = static_ea(i)
            if t is not None:
                succ = [t]
            elif is_bal_return(i):
                succ = []
            else:
                ts = set(traces['J'].get(a, ())) | set(static_jump_table(mem32, i))
                ts = sorted(t for t in ts if not after_bal(mem32, t))
                f.jtab[a] = ts
                succ = ts
        elif n in ('call', 'bal'):
            calls.add(i.target)
            succ = [i.next]
        elif n in ('callx', 'balx'):
            t = static_ea(i)
            if t is not None:
                calls.add(t)
            calls.update(traces['C' if n == 'callx' else 'B'].get(a, ()))
            succ = [i.next]
        elif i.target is not None:    # conditional branches
            succ = [i.target, i.next]
        else:
            succ = [i.next]
            moved = traces['N'].get(a)
            if moved:
                f.moved[a] = sorted(moved)
                succ += f.moved[a]
        if n != 'bx' and i.target is not None:
            f.labels.add(i.target)
        elif n == 'bx':
            f.labels.update(succ)
        f.labels.update(f.moved.get(a, ()))
        work.extend(succ)
    return f, calls


def program(romdir, trace_paths=None):
    """Everything the tools start from: the ROM, a reader for it, the traces
    and the functions. With no trace files, all of traces/ is used."""
    if not trace_paths:
        trace_paths = sorted(glob.glob(os.path.join(HERE, '..', 'traces', '*.txt')))
    traces = load_traces(trace_paths)
    rom = D.load_program(romdir)
    mem = D.code_reader(rom)
    roots = {struct.unpack_from('<I', rom, 0xc)[0]} | load_itab_handlers(romdir, rom) | traces['E']
    for d in (traces['C'], traces['B']):
        for ts in d.values():
            roots |= ts
    return rom, mem, traces, discover(mem, roots, traces)


def containing(funcs, addr):
    """The function an address belongs to, or None. Useful when an address
    turns out to be in the middle of one (or not reached at all)."""
    for e, f in sorted(funcs.items()):
        if addr in f.insns:
            return e
    return None


def discover(mem32, roots, traces):
    """Pass 1 finds every function entry; pass 2 cuts each function at the
    others' entries, so shared code isn't duplicated."""
    entries = set()
    work = sorted(roots)
    while work:
        e = work.pop()
        if e in entries or D.decode(mem32, e) is None:
            continue
        entries.add(e)
        _, calls = explore(mem32, e, traces, frozenset(entries))
        work.extend(c for c in calls if c not in entries)
    frozen = frozenset(entries)
    return {e: explore(mem32, e, traces, frozen)[0] for e in sorted(entries)}

# ------------------------------------------------------------ C emission

def R(n):
    """A register by number; the interpreter's multi-word operands can run
    past g15 into the rest of the register file (i960.h)."""
    return 's->r[%d]' % n if n < 32 else 'RX(%d)' % n


def src(n, lit):
    return '%du' % n if lit else R(n)


def ea_c(ea):
    k = ea[0]
    if k == 'off': return '0x%xu' % ea[1]
    if k == 'abase_off': return '%s + 0x%xu' % (R(ea[1]), ea[2])
    if k == 'abase': return R(ea[1])
    if k == 'abase_index': return '(%s << %d) + %s' % (R(ea[2]), ea[3], R(ea[1]))
    if k == 'index_off': return '(%s << %d) + 0x%xu' % (R(ea[1]), ea[2], ea[3])
    if k == 'abase_index_off': return '(%s << %d) + %s + 0x%xu' % (R(ea[2]), ea[3], R(ea[1]), ea[4])
    raise ValueError(k)


COND = {'bno': '(s->cc & 7) == 0', 'bg': 's->cc & 1', 'be': 's->cc & 2', 'bge': 's->cc & 3',
        'bl': 's->cc & 4', 'bne': 's->cc & 5', 'ble': 's->cc & 6', 'bo': 's->cc & 7'}
CMPB_MASK = {'g': 1, 'e': 2, 'ge': 3, 'l': 4, 'ne': 5, 'le': 6}

LOADS = {'ldob': 'rd8(s, a)', 'ldib': '(uint32_t)(int32_t)(int8_t)rd8(s, a)', 'ldos': 'rd16(s, a)',
         'ldis': '(uint32_t)(int32_t)(int16_t)rd16(s, a)', 'ld': 'rd32(s, a)'}
STORES = {'stob': 'wr8(s, a, (uint8_t)%s)', 'stib': 'wr8(s, a, (uint8_t)%s)', 'stos': 'wr16(s, a, (uint16_t)%s)',
          'stis': 'wr16(s, a, (uint16_t)%s)', 'st': 'wr32(s, a, %s)'}
MULTI = {'ldl': 2, 'ldt': 3, 'ldq': 4, 'stl': 2, 'stt': 3, 'stq': 4}

# REG ops with two sources and a destination: C expression on a (src1), b (src2)
BINOP = {
    'and': 'b & a', 'andnot': 'b & ~a', 'notand': '~b & a', 'xor': 'b ^ a', 'or': 'b | a',
    'nor': '~b & ~a', 'xnor': '~(b ^ a)', 'ornot': 'b | ~a', 'notor': '~b | a', 'nand': '~b | ~a',
    'notbit': 'b ^ (1u << (a & 0x1f))', 'setbit': 'b | (1u << (a & 0x1f))', 'clrbit': 'b & ~(1u << (a & 0x1f))',
    'shlo': 'b << (a & 0x1f)', 'shli': 'b << (a & 0x1f)', 'shro': 'b >> (a & 0x1f)',
    'shri': '(uint32_t)((int32_t)b >> (a & 0x1f))', 'shrdi': 'rt_shrdi(a, b)', 'rotate': 'rt_rotate(a, b)',
    'addo': 'b + a', 'addi': 'b + a', 'subo': 'b - a', 'subi': 'b - a', 'mulo': 'b * a', 'muli': 'b * a',
    'divo': 'a ? b / a : 0', 'remo': 'a ? b % a : 0',
    'divi': 'rt_divi(a, b)', 'remi': 'rt_remi(a, b)', 'modi': 'rt_modi(a, b)',
    'alterbit': '(s->cc & 2) ? b | (1u << (a & 0x1f)) : b & ~(1u << (a & 0x1f))',
    'addc': 'rt_addc(s, a, b)', 'subc': 'rt_subc(s, a, b)',
}
CMPOPS = {'cmpo': 'rt_cc_u(s, a, b)', 'cmpi': 'rt_cc_i(s, a, b)',
          'concmpo': 'if (!(s->cc & 4)) s->cc = a <= b ? 2 : 1',
          'concmpi': 'if (!(s->cc & 4)) s->cc = (int32_t)a <= (int32_t)b ? 2 : 1',
          'chkbit': 's->cc = (b & (1u << (a & 0x1f))) ? 2 : 0'}
CMPX = {'cmpinco': ('rt_cc_u', '+'), 'cmpdeco': ('rt_cc_u', '-'), 'cmpinci': ('rt_cc_i', '+'), 'cmpdeci': ('rt_cc_i', '-')}
TESTS = {'testno': 's->cc == 0', 'testg': 's->cc & 1', 'teste': '(s->cc >> 1) & 1', 'testge': '(s->cc & 3) != 0',
         'testl': '(s->cc >> 2) & 1', 'testne': '(s->cc & 5) != 0', 'testle': '(s->cc & 6) != 0'}

# FP: operand pointers exactly as the interpreter's reg_src/reg_dst
def fp_src_ptr(n, lit):
    if not lit:
        return '&s->r[%d]' % n
    if n < 4:
        return '&s->fp[%d]' % n
    return {0x10: '&rt_fp_zero', 0x16: '&rt_fp_one'}.get(n, 's->lit[0]')


def fp_dst_ptr(n, lit):
    if not lit:
        return '&s->r[%d]' % n
    return '&s->fp[%d]' % n if n < 4 else None   # literal-table writes: interpreter


FP_R = {'addr': 'b + a', 'subr': 'b - a', 'mulr': 'b * a', 'divr': 'b / a'}
FP_RL = {'addrl': 'b + a', 'subrl': 'b - a', 'mulrl': 'b * a', 'divrl': 'b / a'}


class Emitter:
    def __init__(self, funcs, fnames, symbols=None, data=None):
        self.funcs = funcs
        self.fnames = fnames      # entry -> C name
        self.symbols = symbols or {}
        self.data = data or {}
        self.tgp = load_tgp_commands()
        self.stepped = defaultdict(int)

    def sym(self, addr, prefix=' '):
        """the name of the function at `addr`, when it has one"""
        return prefix + self.symbols[addr][0] if addr in self.symbols else ''

    def call_fn(self, target, ret_ip, ret_fp):
        if target in self.fnames:
            return 'rt_call(%s, s, 0x%xu, %s);' % (self.fnames[target], ret_ip, ret_fp)
        return 'rt_run(s, 0x%xu, 0x%xu, %s);' % (target, ret_ip, ret_fp)

    def goto(self, f, t, L):
        """continue at static address t: in this function, another one (a
        tail jump), or wherever rt_resume finds"""
        if t in f.insns:
            return 'goto %s;' % L(t)
        if t in self.fnames:
            return 'return RT_TAIL(%s);' % self.fnames[t]
        return 's->ip = 0x%xu; goto resync;' % t

    def emit_insn(self, f, i, L):
        """C for one instruction, cycles included; L(addr) is the label of a
        jump target in this function."""
        a, n, op, nx = i.addr, i.name, i.op, i.next
        # M2_HOOK_IDLE_COUNT: the op runs, then the board's hook; the patched
        # entry costs 0 cycles (i960.c hook_opdef)
        c = 0 if a == IDLE_HOOK else op.cycles
        o = []
        w = o.append
        charged = False

        def charge(t, cost=c):
            """cycles of an instruction that goes on to static address t; the
            interpreter re-selects its code region when bits 21-31 change"""
            if a == IDLE_HOOK:
                w('if (s->hook) s->hook(s->user);')
            if (t ^ a) & REGION_BOUNDARY:
                w('s->ip = 0x%xu; CY(i960_region_switch(s, %d), 0x%xu);' % (t, cost, t))
            else:
                w('CY(%d, 0x%xu);' % (cost, t))

        def jump(t):
            charge(t)
            w(self.goto(f, t, L))

        def step():
            """the interpreter runs this one (it charges the cycles)"""
            self.stepped[n] += 1
            moved = f.moved.get(a)
            if not moved:
                w('s->ip = 0x%xu; s->prev_ip = 0x%xu; STEP(0x%xu);' % (a, a, nx))
                return True
            w('s->ip = 0x%xu; s->prev_ip = 0x%xu; if (rt_step(s)) goto resync;' % (a, a))
            w('switch (s->ip) {')
            for t in moved:
                w('case 0x%xu: %s' % (t, self.goto(f, t, L)))
            w('}')
            w('if (s->ip != 0x%xu) goto resync;' % nx)
            return True

        if op is D.INVALID or (op.fmt == 1 and i.ea and i.ea[0] == 'reserved'):
            charged = step()
        elif n in LOADS:
            w('{ uint32_t a = %s; %s = %s; }' % (ea_c(i.ea), R(i.dst), LOADS[n]))
        elif n in STORES:
            w('{ uint32_t a = %s; %s; }' % (ea_c(i.ea), STORES[n] % R(i.dst)))
        elif n in MULTI:
            k = MULTI[n]
            body = ['uint32_t a = %s;' % ea_c(i.ea), 's->multi_addr = a; s->multi_count = %d;' % k]
            for j in range(k):
                if n.startswith('ld'):
                    body.append('%s = rd32(s, a + %d);' % (R(i.dst + j), 4 * j))
                else:
                    body.append('wr32(s, a + %d, %s);' % (4 * j, R(i.dst + j)))
            body.append('s->multi_count = 0;')
            w('{ %s }' % ' '.join(body))
        elif n == 'lda':
            w('%s = %s;' % (R(i.dst), ea_c(i.ea)))
        elif op.fmt == 0 and op.operands & 4 and i.m3 and not op.fp:
            charged = step()     # result written into the literal table (QUIRKS.md #9)
        elif n in BINOP:
            w('{ uint32_t a = %s, b = %s; %s = %s; }' % (src(i.src1, i.m1), src(i.src2, i.m2), R(i.dst), BINOP[n]))
        elif n in ('not', 'mov'):
            w('%s = %s%s;' % (R(i.dst), '~' if n == 'not' else '', src(i.src1, i.m1)))
        elif n in ('movl', 'movt', 'movq'):
            k = {'movl': 2, 'movt': 3, 'movq': 4}[n]
            parts = ['uint32_t w0 = %s;' % src(i.src1, i.m1), '%s = w0;' % R(i.dst)]
            for j in range(1, k):
                parts.append('%s = %s;' % (R(i.dst + j), 'w0' if i.m1 else R(i.src1 + j)))
            w('{ %s }' % ' '.join(parts))
        elif n in CMPOPS:
            w('{ uint32_t a = %s, b = %s; %s; }' % (src(i.src1, i.m1), src(i.src2, i.m2), CMPOPS[n]))
        elif n in CMPX:
            fn, sign = CMPX[n]
            w('{ uint32_t a = %s, b = %s; %s(s, a, b); %s = b %s 1; }' % (src(i.src1, i.m1), src(i.src2, i.m2), fn, R(i.dst), sign))
        elif n in ('scanbit', 'spanbit'):
            w('%s = rt_%s(s, %s);' % (R(i.dst), n, src(i.src1, i.m1)))
        elif n == 'emul':
            w('{ uint64_t r = (uint64_t)%s * %s; %s = (uint32_t)r; %s = (uint32_t)(r >> 32); }'
              % (src(i.src2, i.m2), src(i.src1, i.m1), R(i.dst), R(i.dst + 1)))
        elif n == 'ediv':
            lo = src(i.src2, i.m2)
            hi = lo if i.m2 else R(i.src2 + 1)   # a literal pair repeats the literal
            w('{ uint64_t num = (uint64_t)%s << 32 | %s; uint32_t d = %s; '
              'if (!d) { %s = 0; %s = 0; } else { %s = (uint32_t)(num %% d); %s = (uint32_t)(num / d); } }'
              % (hi, lo, src(i.src1, i.m1), R(i.dst), R(i.dst + 1), R(i.dst), R(i.dst + 1)))
        elif n == 'modac':
            w('%s = rt_modac(s, %s, %s);' % (R(i.dst), src(i.src1, i.m1), src(i.src2, i.m2)))
        elif n == 'modpc':   # the old value is not returned (QUIRKS.md #5)
            w('{ uint32_t mask = %s; s->pc = (%s & mask) | (s->pc & ~mask); }' % (src(i.src2, i.m2), R(i.dst)))
        elif n == 'flushreg':
            w('i960_flushreg(s);')
        elif n in TESTS:
            if i.m1:
                charged = step()
            else:
                w('%s = %s;' % (R(i.src1), TESTS[n]))
        elif n in FP_R or n in ('cmpr', 'movr', 'cvtir', 'cvtri', 'cvtzri', 'scaler', 'sqrtr'):
            charged = self.emit_fp(i, w, step)
        # ------------------------------------------------ control flow
        elif n == 'b':
            jump(i.target)
            return o
        elif n in COND:
            w('if (%s) {' % COND[n])
            jump(i.target)
            w('}')
        elif n in ('bbc', 'bbs'):   # cc = 2 when taken, for both (QUIRKS.md #7)
            w('if (%s(%s & (1u << (%s & 0x1f)))) {' % ('!' if n == 'bbc' else '', R(i.src2), src(i.src1, i.m1)))
            w('s->cc = 2;')
            jump(i.target)
            w('}')
            w('s->cc = 0;')
        elif n.startswith('cmpob') or n.startswith('cmpib'):
            w('rt_cc_%s(s, %s, %s);' % ('u' if n[3] == 'o' else 'i', src(i.src1, i.m1), R(i.src2)))
            w('if (s->cc & %d) {' % CMPB_MASK[n[5:]])
            jump(i.target)
            w('}')
        elif n in ('call', 'callx', 'bal', 'balx'):
            # the callee returns to nx with the frame pointer it was called with
            t = i.target if n in ('call', 'bal') else static_ea(i)
            if n in ('call', 'callx'):
                pre = 'uint32_t fpc = s->r[31] & ~0x3fu; s->ip = 0x%xu; i960_call(s, %s);'
            else:
                link = R(30 if n == 'bal' else i.dst)
                pre = 'uint32_t fpc = s->r[31]; ' + link + ' = 0x%xu; s->ip = %s;'
            if t is not None:
                w('{ ' + pre % (nx, '0x%xu' % t))
                charge(t)
                w(self.call_fn(t, nx, 'fpc'))
            else:
                w('{ uint32_t t = %s; ' % ea_c(i.ea) + pre % (nx, 't'))
                w('CYD(0x%xu, %d, t);' % (a, c))
                w('rt_run(s, t, 0x%xu, fpc);' % nx)
            w('if (s->ip != 0x%xu || s->r[31] != fpc) goto resync; }' % nx)
            charged = True
        elif n == 'bx':
            t = static_ea(i)
            if t is not None:
                jump(t)
                return o
            w('{ uint32_t t = %s; s->ip = t;' % ea_c(i.ea))
            ts = f.jtab.get(a, [])
            if ts:
                w('switch (t) {')
                for t2 in ts:
                    w('case 0x%xu:' % t2)
                    jump(t2)
                w('}')
            # anything else (the return of a bal procedure): resync
            w('CYD(0x%xu, %d, t); goto resync; }' % (a, c))
            return o
        elif n == 'ret':
            # return types 1-6 do nothing and fall through (QUIRKS.md #8)
            w('s->ip = 0x%xu; if (i960_ret(s)) { CYD(0x%xu, %d, s->ip); return RT_DONE; }' % (nx, a, c))
        else:
            charged = step()

        if not charged:
            charge(nx)
        return o

    def emit_fp(self, i, w, step):
        n = i.name

        def fsrc(reg, lit, m):
            p = fp_src_ptr(reg, lit)
            return ('rdD(%s)' if m else '(double)rdF(%s)') % p

        if n in FP_R or n == 'scaler':
            dp = fp_dst_ptr(i.dst, i.m3)
            if dp is None:
                return step()
            a = fsrc(i.src1, i.m1, i.m1)
            b = fsrc(i.src2, i.m2, i.m2)
            if n == 'scaler':
                expr = 'pow(2.0, (double)(int32_t)%s) * b' % src(i.src1, i.m1) if not i.m1 else None
                if expr is None:
                    return step()
                w('{ double b = %s; wr%s(%s, %s); }' % (b, 'D' if i.m3 else 'F', dp,
                                                          expr if i.m3 else '(float)(%s)' % expr))
                return False
            expr = 'FPR(%s)' % FP_R[n]
            w('{ double a = %s, b = %s; %s; }' % (a, b, ('wrD(%s, %s)' if i.m3 else 'wrF(%s, (float)%s)') % (dp, expr)))
        elif n == 'cmpr':
            w('rt_fp_compare(s, %s, %s);' % (fsrc(i.src1, i.m1, i.m1), fsrc(i.src2, i.m2, i.m2)))
        elif n == 'movr' or n == 'sqrtr':
            dp = fp_dst_ptr(i.dst, i.m3)
            if dp is None:
                return step()
            v = fsrc(i.src1, i.m1, i.m1)
            if n == 'sqrtr':
                v = 'FPR(sqrt(%s))' % v
            w(('wrD(%s, %s);' if i.m3 else 'wrF(%s, (float)(%s));') % (dp, v))
        elif n == 'cvtir':
            dp = fp_dst_ptr(i.dst, i.m3)
            if dp is None:
                return step()
            v = '(double)(int32_t)rdU(%s)' % fp_src_ptr(i.src1, i.m1)
            w(('wrD(%s, %s);' if i.m3 else 'wrF(%s, (float)(%s));') % (dp, v))
        elif n in ('cvtri', 'cvtzri'):
            # integer result through the dst pointer (a register unless m3)
            dp = fp_dst_ptr(i.dst, i.m3)
            if dp is None:
                return step()
            v = fsrc(i.src1, i.m1, i.m1)
            fn = 'rt_round_ac' if n == 'cvtri' else 'rt_cvtzri'
            w('wrI(%s, %s(s, %s));' % (dp, fn, v))
        else:
            return step()
        return False

    def emit_func(self, f):
        name = self.fnames[f.entry]
        addrs = sorted(f.insns)
        labels = set(f.labels) | {f.entry}
        following = dict(zip(addrs, addrs[1:] + [None]))
        for a in addrs:   # fall-throughs that aren't emitted next need a label
            i = f.insns[a]
            if i.name not in ('b', 'bx') and i.next != following[a] and i.next in f.insns:
                labels.add(i.next)
        L = lambda t: 'L_%x' % t
        head = '/* %08x: %d instructions */' % (f.entry, len(addrs))
        if f.entry in self.symbols:
            sym, what = self.symbols[f.entry]
            head = '/* %08x %s: %d instructions%s */' % (f.entry, sym, len(addrs),
                                                         '\n   ' + what if what else '')
        out = [head, 'RT_FN(%s)' % name, '{']
        if addrs[0] != f.entry:
            out.append('    goto %s;' % L(f.entry))
        for a in addrs:
            i = f.insns[a]
            if a in labels:
                out.append('%s:' % L(a))
            note = ''
            if i.name in ('call', 'bal') and i.target is not None:
                note = self.sym(i.target, ' -> ')
            elif i.ea is not None:
                base = i.ea[1] if i.ea[0] == 'off' else i.ea[3] if i.ea[0] == 'index_off' else None
                if base in self.data:
                    note = ' ' + self.data[base][0]
                else:
                    note = tgp_note(i, self.tgp) or dl_note(i)
            out.append('    /* %08x  %s%s */' % (a, D.disasm(i), note))
            out.extend('    ' + line for line in self.emit_insn(f, i, L))
            if i.name not in ('b', 'bx') and i.next != following[a] and i.op is not D.INVALID:
                out.append('    ' + self.goto(f, i.next, L))
            elif i.op is D.INVALID and i.next != following[a]:
                out.append('    ' + self.goto(f, i.next, L))
        out += ['resync:',
                '    return rt_resume(s, ret_ip, ret_fp);',
                '}']
        return '\n'.join(out)


HERE = os.path.dirname(os.path.abspath(__file__))


def hand_written(srcdir):
    """Addresses of the functions rewritten by hand in src/."""
    out = set()
    for path in sorted(glob.glob(os.path.join(srcdir, '*.c'))):
        for m in re.finditer(r'RT_FN\(\s*f_([0-9a-fA-F]{8})\s*\)', open(path).read()):
            out.add(int(m.group(1), 16))
    return out


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        sys.exit(1)
    romdir, outdir = sys.argv[1], sys.argv[2]
    rom = D.load_program(romdir)
    mem32 = D.code_reader(rom)
    traces = load_traces(sys.argv[3:])

    reset_ip = struct.unpack_from('<I', rom, 0xc)[0]
    entries = {reset_ip} | load_itab_handlers(romdir, rom) | traces['E']
    for d in (traces['C'], traces['B']):
        for ts in d.values():
            entries |= ts
    funcs = discover(mem32, entries, traces)
    fnames = {e: 'f_%08x' % e for e in funcs}
    byhand = hand_written(os.path.join(HERE, '..', 'src'))
    em = Emitter(funcs, fnames, load_symbols(), load_symbols(data=True))

    os.makedirs(outdir, exist_ok=True)
    order = sorted(funcs)
    per_file = 150
    files = []
    for k in range(0, len(order), per_file):
        path = os.path.join(outdir, 'daytona_%02d.c' % (k // per_file))
        with open(path, 'w') as fo:
            fo.write('/* Generated by tools/i960rec.py from the Daytona USA program ROM. Do not edit. */\n')
            fo.write('#include "rt.h"\n#include "funcs.h"\n\n')
            for e in order[k:k + per_file]:
                if e in byhand:
                    continue
                fo.write(em.emit_func(funcs[e]))
                fo.write('\n\n')
        files.append(path)
    with open(os.path.join(outdir, 'funcs.h'), 'w') as fo:
        fo.write('/* Generated by tools/i960rec.py. Do not edit. */\n#ifndef FUNCS_H\n#define FUNCS_H\n#include "rt.h"\n')
        syms = load_symbols()
        for e in order:
            fo.write('RT_FN(%s);%s\n' % (fnames[e], '   /* %s */' % syms[e][0] if e in syms else ''))
        fo.write('#endif\n')
    # cost of the instruction starting at each word of the program, so that
    # hand-written functions can ask for the cycles of a run of machine code
    # instruction by instruction (rt_pay)
    costs = {}
    for f in funcs.values():
        for a, i in f.insns.items():
            w = a if a < D.PROGRAM_SIZE else a - 0x200000
            costs[w >> 2] = (0 if a == IDLE_HOOK else i.op.cycles) | (0x8000 if i.len == 8 else 0)
    with open(os.path.join(outdir, 'cycles.c'), 'w') as fo:
        fo.write('/* Generated by tools/i960rec.py. Do not edit.\n'
                 '   The cycles each instruction of the program costs, by word; bit 15\n'
                 '   marks an instruction with a displacement word (8 bytes). */\n'
                 '#include "rt.h"\n\nconst uint16_t rt_cycle_word[%d] = {\n' % (D.PROGRAM_SIZE // 4))
        row = []
        for w in range(D.PROGRAM_SIZE // 4):
            row.append('%d' % costs.get(w, 0))
            if len(row) == 24:
                fo.write('    ' + ','.join(row) + ',\n')
                row = []
        if row:
            fo.write('    ' + ','.join(row) + ',\n')
        fo.write('};\n')
    with open(os.path.join(outdir, 'table.c'), 'w') as fo:
        fo.write('/* Generated by tools/i960rec.py. Do not edit. */\n#include "funcs.h"\n\n')
        fo.write('const rt_entry rt_funcs[] = {\n')
        for e in order:
            fo.write('    { 0x%08xu, %s },\n' % (e, fnames[e]))
        fo.write('};\nconst int rt_nfuncs = %d;\n' % len(order))
        fo.write('const uint32_t rt_program_crc = 0x%08xu;\n' % (zlib.crc32(rom[:0x40000]) & 0xffffffff))
    ninsn = sum(len(f.insns) for f in funcs.values())
    uniq = len(set(a for f in funcs.values() for a in f.insns))
    covered = len(traces['X'] & set(a for f in funcs.values() for a in f.insns))
    print('%d functions, %d instructions (%d distinct), %d of %d traced words covered; %d files'
          % (len(funcs), ninsn, uniq, covered, len(traces['X']), len(files)))
    if byhand:
        print('%d functions come from src/: %s' % (len(byhand), ' '.join('%08x' % a for a in sorted(byhand))))
    if em.stepped:
        print('left to the interpreter:', dict(em.stepped))


if __name__ == '__main__':
    main()
