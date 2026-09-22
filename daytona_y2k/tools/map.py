#!/usr/bin/env python3
"""Map of the recompiled program: functions, sizes, callers and callees.

    map.py <romdir> [trace.txt ...] [--calls ADDR] [--dis ADDR]

With no trace files it uses all of traces/.
"""
import sys, collections
import i960dec as D, i960rec as R

romdir = sys.argv[1]
rom, mem, tr, funcs = R.program(romdir, [a for a in sys.argv[2:] if a.endswith('.txt')])

callers = collections.defaultdict(set)
callees = collections.defaultdict(set)
for e, f in funcs.items():
    for a, i in f.insns.items():
        ts = set()
        if i.name in ('call', 'bal'):
            ts.add(i.target)
        elif i.name in ('callx', 'balx'):
            t = R.static_ea(i)
            if t is not None:
                ts.add(t)
            ts |= tr['C' if i.name == 'callx' else 'B'].get(a, set())
        for t in ts:
            callers[t].add(e)
            callees[e].add(t)

args = sys.argv[2:]
if '--dis' in args:
    e = int(args[args.index('--dis') + 1], 16)
    f = funcs[e]
    for a in sorted(f.insns):
        print('%06x %08x  %s' % (a, f.insns[a].raw, D.disasm(f.insns[a])))
    syms = R.load_symbols()
    print('callers:', ' '.join('%06x%s' % (c, '(' + syms[c][0] + ')' if c in syms else '')
                               for c in sorted(callers[e])))
    sys.exit()

syms = R.load_symbols()
print('%-8s %6s %7s %7s %7s  %s' % ('entry', 'insns', 'callers', 'callees', 'traced', 'name'))
for e in sorted(funcs, key=lambda e: -len(callers[e])):
    f = funcs[e]
    hit = sum(1 for a in f.insns if a in tr['X'])
    print('%08x %6d %7d %7d %6d%%  %s' % (e, len(f.insns), len(callers[e]), len(callees[e]),
                                           100 * hit // max(1, len(f.insns)),
                                           syms.get(e, ('', ''))[0]))
