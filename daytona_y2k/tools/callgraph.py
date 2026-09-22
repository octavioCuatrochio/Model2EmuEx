#!/usr/bin/env python3
"""A picture of the call tree below a function: which parts are already
written by hand, and what they reach.

    callgraph.py <romdir> <entry> <out.mvg> [depth] -- traces...

Functions rewritten by hand (src/) are filled in; the rest are outlines.
Names come from symbols.txt. Render with:  magick -size WxH mvg:out.mvg out.png
"""
import os, subprocess, sys, collections

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import i960dec as D, i960rec as R


def call_graph(mem, funcs, traces):
    edges = collections.defaultdict(list)
    for e, f in funcs.items():
        for a, i in sorted(f.insns.items()):
            ts = []
            if i.name in ('call', 'bal'):
                ts = [i.target]
            elif i.name in ('callx', 'balx'):
                t = R.static_ea(i)
                ts = ([t] if t is not None else []) + sorted(
                    traces['C' if i.name == 'callx' else 'B'].get(a, ()))
            for t in ts:
                if t not in edges[e]:
                    edges[e].append(t)
    return edges


def main():
    romdir, entry, out = sys.argv[1], int(sys.argv[2], 16), sys.argv[3]
    depth = int(sys.argv[4]) if len(sys.argv) > 4 and not sys.argv[4].endswith('.txt') else 3
    traces_files = [a for a in sys.argv[4:] if a.endswith('.txt')]

    rom = D.load_program(romdir)
    mem = D.code_reader(rom)
    traces = R.load_traces(traces_files)
    roots = {D.struct.unpack_from('<I', rom, 0xc)[0]} | R.load_itab_handlers(romdir, rom) | traces['E']
    for d in (traces['C'], traces['B']):
        for ts in d.values():
            roots |= ts
    funcs = R.discover(mem, roots, traces)
    edges = call_graph(mem, funcs, traces)
    syms = R.load_symbols()
    byhand = R.hand_written(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'src'))

    # levels by breadth-first walk from the entry
    level, seen, order = {entry: 0}, {entry}, [entry]
    q = [entry]
    while q:
        a = q.pop(0)
        if level[a] >= depth:
            continue
        for t in edges.get(a, []):
            if t not in seen:
                seen.add(t); level[t] = level[a] + 1; order.append(t); q.append(t)

    rows = collections.defaultdict(list)
    for a in order:
        rows[level[a]].append(a)
    per_row = int(os.environ.get('CG_PER_ROW', 6))
    cap = int(os.environ.get('CG_CAP', 18))       # at most this many per level
    left_out = {lv: max(0, len(v) - cap) for lv, v in rows.items()}
    for lv in rows:
        rows[lv] = rows[lv][:cap]

    rowh, boxw, boxh, gap, margin, lineh = 120, 250, 46, 24, 30, 60
    W = int(margin * 2 + min(per_row, max(len(v) for v in rows.values())) * (boxw + gap) - gap)
    pos = {}
    y = 90
    level_y = {}
    for lv in sorted(rows):
        level_y[lv] = y
        addrs = rows[lv]
        for k, a in enumerate(addrs):
            line, col = divmod(k, per_row)
            in_line = min(per_row, len(addrs) - line * per_row)
            span = in_line * (boxw + gap) - gap
            pos[a] = ((W - span) / 2 + col * (boxw + gap), y + line * lineh)
        y += ((len(addrs) - 1) // per_row + 1) * lineh + rowh - lineh
    H = int(y + 40)
    font = subprocess.run(['fc-match', '-f', '%{file}', 'DejaVu Sans'],
                          capture_output=True, text=True).stdout or ''
    o = ['font "%s"' % font, 'fill #0b0e12', 'rectangle 0 0 %d %d' % (W, H), 'font-size 13']
    for a in order:                                   # the calls
        x, y = pos[a]
        for t in edges.get(a, []):
            if t in pos and level.get(t, 0) == level[a] + 1:
                x2, y2 = pos[t]
                o += ['stroke #2c3b4a', 'stroke-width 1',
                      'line %.0f,%.0f %.0f,%.0f' % (x + boxw / 2, y + boxh, x2 + boxw / 2, y2)]
    for a in order:
        x, y = pos[a]
        hand = a in byhand
        o += ['stroke #7fd4a0' if hand else 'stroke #4a5a6a', 'stroke-width 1.4',
              'fill #1d3b2c' if hand else 'fill #151a20',
              'roundrectangle %.0f,%.0f %.0f,%.0f 6,6' % (x, y, x + boxw, y + boxh),
              'stroke none', 'fill #e8f0f0' if hand else 'fill #93a2ae',
              'text %.0f,%.0f "%s"' % (x + 12, y + 19, syms.get(a, ('f_%06x' % a,))[0]),
              'fill #7fd4a0' if hand else 'fill #5d6b78',
              'text %.0f,%.0f "%06x%s"' % (x + 12, y + 36, a, '  (C)' if hand else '')]
    for lv, n in left_out.items():
        if n:
            o += ['fill #5d6b78', 'font-size 12',
                  'text %d,%d "... and %d more at this level"' % (margin, level_y[lv] - 8, n)]
    o += ['fill white', 'font-size 19',
          'text 30,34 "The drawing call tree below %s"' % syms.get(entry, ('%06x' % entry,))[0],
          'font-size 13', 'fill #93a2ae',
          'text 30,58 "green: rewritten as readable C (%d of the %d shown); grey: still the recompiler\'s output"'
          % (sum(1 for a in order if a in byhand), len(order))]
    open(out, 'w').write('\n'.join(o) + '\n')
    print('%dx%d' % (W, H))


if __name__ == '__main__':
    main()
