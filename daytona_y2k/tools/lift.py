#!/usr/bin/env python3
"""A first draft of a function for src/, in the style of src/decomp.h.

    lift.py <romdir> <address> [trace.txt ...] > src/new.c

What comes out is correct but plain: the machine code's steps, one C line
each, with the cursor markers (UPTO/BRANCH/CALL/RET) already right, guest
memory through rd/wr, and the geometry processor and display list through
their macros. Rename things, give the loops their shape, and keep
`make check` passing as you go.
"""
import os, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import i960dec as D, i960rec as R


def reg(n):
    """the i960's registers as decomp.h names them"""
    return 'G(%d)' % (n - 16) if 16 <= n < 32 else 's->r[%d]' % n


def src(n, lit):
    return '0x%xu' % n if lit else reg(n)


def ea(i):
    """the address a memory instruction works on"""
    k = i.ea[0]
    if k == 'off':
        return '0x%xu' % i.ea[1]
    if k == 'abase_off':
        return '%s + 0x%xu' % (reg(i.ea[1]), i.ea[2]) if i.ea[2] else reg(i.ea[1])
    if k == 'abase':
        return reg(i.ea[1])
    if k == 'abase_index':
        return '%s + (%s << %d)' % (reg(i.ea[1]), reg(i.ea[2]), i.ea[3])
    if k == 'index_off':
        return '0x%xu + (%s << %d)' % (i.ea[3], reg(i.ea[1]), i.ea[2])
    if k == 'abase_index_off':
        return '0x%xu + %s + (%s << %d)' % (i.ea[4], reg(i.ea[1]), reg(i.ea[2]), i.ea[3])
    return '/* reserved */ 0'


LOADS = {'ldob': 'rd8(s, %s)', 'ldib': '(uint32_t)(int32_t)(int8_t)rd8(s, %s)',
         'ldos': 'rd16(s, %s)', 'ldis': '(uint32_t)(int32_t)(int16_t)rd16(s, %s)',
         'ld': 'rd32(s, %s)'}
STORES = {'stob': 'wr8(s, %s, (uint8_t)%s)', 'stib': 'wr8(s, %s, (uint8_t)%s)',
          'stos': 'wr16(s, %s, (uint16_t)%s)', 'stis': 'wr16(s, %s, (uint16_t)%s)',
          'st': 'wr32(s, %s, %s)'}
BIN = {'and': '%s & %s', 'andnot': '%s & ~%s', 'notand': '~%s & %s', 'xor': '%s ^ %s',
       'or': '%s | %s', 'nor': '~%s & ~%s', 'xnor': '~(%s ^ %s)', 'ornot': '%s | ~%s',
       'notor': '~%s | %s', 'nand': '~%s | ~%s',
       'addo': '%s + %s', 'addi': '%s + %s', 'subo': '%s - %s', 'subi': '%s - %s',
       'mulo': '%s * %s', 'muli': '%s * %s', 'divo': '%s / %s', 'divi': 'rt_divi(%s, %s)',
       'remo': '%s %% %s', 'remi': 'rt_remi(%s, %s)', 'modi': 'rt_modi(%s, %s)',
       'shlo': '%s << (%s & 0x1f)', 'shli': '%s << (%s & 0x1f)', 'shro': '%s >> (%s & 0x1f)',
       'shri': '(uint32_t)((int32_t)%s >> (%s & 0x1f))',
       'setbit': '%s | (1u << (%s & 0x1f))', 'clrbit': '%s & ~(1u << (%s & 0x1f))',
       'notbit': '%s ^ (1u << (%s & 0x1f))'}
FP = {'addr': '+', 'subr': '-', 'mulr': '*', 'divr': '/'}
COND = {'bno': '(s->cc & 7) == 0', 'bg': 's->cc & 1', 'be': 's->cc & 2', 'bge': 's->cc & 3',
        'bl': 's->cc & 4', 'bne': 's->cc & 5', 'ble': 's->cc & 6', 'bo': 's->cc & 7'}
CMPB = {'g': '>', 'e': '==', 'ge': '>=', 'l': '<', 'ne': '!=', 'le': '<='}


def body(f, cmds, data):
    out = []
    addrs = sorted(f.insns)
    targets = set(f.labels)
    for a in addrs:
        i = f.insns[a]
        n, nx = i.name, i.next
        note = R.tgp_note(i, cmds) or R.dl_note(i)
        if a in targets:
            out.append('L_%x:' % a)
        c = []                                     # what the instruction does
        if n in LOADS:
            c.append('%s = %s;' % (reg(i.dst), LOADS[n] % ea(i)))
        elif n in STORES:
            if note.startswith(' TGP param'):
                c.append('TGP_PARAM(%s);' % reg(i.dst))
            elif note.startswith(' TGP '):
                c.append('TGP_CMD(0x%02x);   /* %s */' % (i.ea[2] >> 4, note.split()[-1]))
            elif note.startswith(' display list, word'):
                c.append('DL_WORD(%s);' % reg(i.dst))
            elif note.startswith(' display list, exponent'):
                c.append('DL_EXP(%s, %s);' % (note.split()[-1], reg(i.dst)))
            else:
                c.append(STORES[n] % (ea(i), reg(i.dst)) + ';')
        elif n == 'lda':
            c.append('%s = %s;' % (reg(i.dst), ea(i)))
        elif n == 'mov':
            c.append('%s = %s;' % (reg(i.dst), src(i.src1, i.m1)))
        elif n == 'not':
            c.append('%s = ~%s;' % (reg(i.dst), src(i.src1, i.m1)))
        elif n in BIN:
            c.append('%s = %s;' % (reg(i.dst), BIN[n] % (src(i.src2, i.m2), src(i.src1, i.m1))))
        elif n in FP:
            c.append('wrF(&%s, rdF(&%s) %s rdF(&%s));'
                     % (reg(i.dst), reg(i.src2), FP[n], reg(i.src1)))
        elif n == 'cvtri':
            c.append('wrI(&%s, rt_round_ac(s, (double)rdF(&%s)));' % (reg(i.dst), reg(i.src1)))
        elif n == 'cvtzri':
            c.append('wrI(&%s, rt_cvtzri(s, (double)rdF(&%s)));' % (reg(i.dst), reg(i.src1)))
        elif n == 'cvtir':
            c.append('wrF(&%s, (float)(int32_t)%s);' % (reg(i.dst), src(i.src1, i.m1)))
        elif n == 'movr':
            c.append('wrF(&%s, rdF(&%s));' % (reg(i.dst), reg(i.src1)))
        elif n == 'cmpr':
            c.append('rt_fp_compare(s, (double)rdF(&%s), (double)rdF(&%s));'
                     % (reg(i.src1), reg(i.src2)))
        elif n in ('movl', 'movt', 'movq'):
            for k in range({'movl': 2, 'movt': 3, 'movq': 4}[n]):
                c.append('%s = %s;' % (reg(i.dst + k), src(i.src1 + k, i.m1)))
        elif n in ('ldl', 'ldt', 'ldq', 'stl', 'stt', 'stq'):
            k = {'ldl': 2, 'ldt': 3, 'ldq': 4, 'stl': 2, 'stt': 3, 'stq': 4}[n]
            c.append('{ uint32_t a_ = %s;' % ea(i))
            for w in range(k):
                c.append('  %s' % (('%s = rd32(s, a_ + %d);' % (reg(i.dst + w), 4 * w))
                                   if n[0] == 'l' else
                                   ('wr32(s, a_ + %d, %s);' % (4 * w, reg(i.dst + w)))))
            c.append('}')
        elif n == 'emul':
            c.append('{ uint64_t p_ = (uint64_t)%s * %s; %s = (uint32_t)p_; %s = (uint32_t)(p_ >> 32); }'
                     % (src(i.src2, i.m2), src(i.src1, i.m1), reg(i.dst), reg(i.dst + 1)))
        elif n == 'rotate':
            c.append('%s = rt_rotate(%s, %s);' % (reg(i.dst), src(i.src1, i.m1), src(i.src2, i.m2)))
        elif n == 'shrdi':
            c.append('%s = rt_shrdi(%s, %s);' % (reg(i.dst), src(i.src1, i.m1), src(i.src2, i.m2)))
        elif n in ('scanbit', 'spanbit'):
            c.append('%s = rt_%s(s, %s);' % (reg(i.dst), n, src(i.src1, i.m1)))
        elif n == 'alterbit':
            c.append('%s = (s->cc & 2) ? (%s | (1u << (%s & 0x1f))) : (%s & ~(1u << (%s & 0x1f)));'
                     % (reg(i.dst), src(i.src2, i.m2), src(i.src1, i.m1),
                        src(i.src2, i.m2), src(i.src1, i.m1)))
        elif n == 'concmpi':
            c.append('if (!(s->cc & 4)) s->cc = (int32_t)%s <= (int32_t)%s ? 2 : 1;'
                     % (src(i.src1, i.m1), src(i.src2, i.m2)))
        elif n in ('cmpo', 'cmpi'):
            c.append('rt_cc_%s(s, %s, %s);' % ('u' if n == 'cmpo' else 'i',
                                               src(i.src1, i.m1), src(i.src2, i.m2)))
        elif n == 'chkbit':
            c.append('s->cc = (%s & (1u << (%s & 0x1f))) ? 2 : 0;'
                     % (reg(i.src2), src(i.src1, i.m1)))
        elif n in ('cmpdeco', 'cmpdeci', 'cmpinco', 'cmpinci'):
            c.append('rt_cc_%s(s, %s, %s); %s = %s %s 1;'
                     % ('u' if n[-1] == 'o' else 'i', src(i.src1, i.m1), reg(i.src2),
                        reg(i.dst), reg(i.src2), '-' if 'dec' in n else '+'))
        elif n.startswith('cmpib') or n.startswith('cmpob'):
            op = CMPB[n[5:]]
            c.append('rt_cc_%s(s, %s, %s);' % ('u' if n[3] == 'o' else 'i',
                                               src(i.src1, i.m1), reg(i.src2)))
            c.append('if (%s %s %s) {' % (src(i.src1, i.m1), op, reg(i.src2)))
            c.append('    BRANCH(0x%xu, 0x%xu);' % (nx, i.target))
            c.append('    goto L_%x;' % i.target)
            c.append('}')
            targets.add(i.target)
        elif n in COND:
            c.append('if (%s) {' % COND[n])
            c.append('    BRANCH(0x%xu, 0x%xu);' % (nx, i.target))
            c.append('    goto L_%x;' % i.target)
            c.append('}')
            targets.add(i.target)
        elif n in ('bbc', 'bbs'):
            c.append('if (%s(%s & (1u << (%s & 0x1f)))) {'
                     % ('!' if n == 'bbc' else '', reg(i.src2), src(i.src1, i.m1)))
            c.append('    s->cc = 2;')
            c.append('    BRANCH(0x%xu, 0x%xu);' % (nx, i.target))
            c.append('    goto L_%x;' % i.target)
            c.append('}')
            c.append('s->cc = 0;')
            targets.add(i.target)
        elif n == 'b':
            c.append('BRANCH(0x%xu, 0x%xu);' % (nx, i.target))
            c.append('goto L_%x;' % i.target)
            targets.add(i.target)
        elif n in ('call', 'bal'):
            c.append('%s(f_%08x, 0x%xu);' % ('CALL' if n == 'call' else 'BAL', i.target, i.target))
        elif n in ('callx', 'balx'):
            t = R.static_ea(i)
            c.append('CALLX(%s);' % ('0x%xu' % t if t is not None else ea(i)))
        elif n == 'ret':
            c.append('RET();')
        elif n == 'bx':
            c.append('/* bx: %s */ RET_BX(%s);' % (D.disasm(i), ea(i)))
        else:
            c.append('/* TODO: %s */' % D.disasm(i))
        # the cursor, unless the instruction moved it itself
        moved = n in ('b', 'ret', 'bx', 'call', 'bal', 'callx', 'balx')
        pre = '    /* %06x  %s%s */' % (a, D.disasm(i), note)
        out.append(pre)
        out += ['    ' + x for x in c]
        if not moved:
            out.append('    UPTO(0x%xu);' % nx)
    # labels that were added late
    text = '\n'.join(out)
    for t in targets:
        if 'L_%x:' % t in text or t not in f.insns:
            continue
        text = text.replace('    /* %06x ' % t, 'L_%x:\n    /* %06x ' % (t, t), 1)
    return text


def main():
    romdir, addr = sys.argv[1], int(sys.argv[2], 16)
    rom, mem, traces, funcs = R.program(romdir, [a for a in sys.argv[3:] if a.endswith('.txt')])
    if addr not in funcs:                       # a function starts where it is called
        inside = R.containing(funcs, addr)
        sys.exit('%06x is %s; lift %s instead'
                 % (addr, 'inside %06x' % inside if inside else 'not code any trace reaches',
                    '%06x' % inside if inside else 'a function entry (tools/map.py lists them)'))
    f = funcs[addr]
    syms = R.load_symbols()
    name = syms.get(addr, ('', ''))[0]
    print('/* %s: a first draft from tools/lift.py; tidy it up. */' % (name or '%06x' % addr))
    print('#include "decomp.h"\n')
    print('/* 0x%x%s */' % (addr, (': ' + name) if name else ''))
    print('RT_FN(f_%08x)' % addr)
    print('{')
    print('    ENTRY(0x%xu);\n' % addr)
    print(body(f, R.load_tgp_commands(), R.load_symbols(data=True)))
    print('}')


if __name__ == '__main__':
    main()
