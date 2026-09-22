#!/usr/bin/env python3
"""What the game puts in the display list, and who writes it.

    dlmap.py <romdir> <out.mvg>        [then: magick -size WxH mvg:out.mvg out.png]

One row per display-list opcode (../notes/geometry.md and
../../port/m2/m2geo.c), with the functions that write it. Green ones are
rewritten by hand in src/; grey ones are still the recompiler's output.
"""
import os, subprocess, sys, collections

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import i960rec as R

# what the renderer does with each opcode (port/m2/m2geo.c, the switch on
# (op >> 23) & 0x1f)
MEANING = {
    0x1: ('an object', 'where its textures and polygons are'),
    0x2: ('polygons in the list', 'the vertices come straight after'),
    0x3: ('a window', 'the area drawn into, and its four centres'),
    0x4: ('texture data', 'an address, a count, then the words'),
    0x5: ('polygon data', 'the same, into polygon RAM'),
    0x6: ('texture parameters', 'diffuse, ambient and specular, per texture'),
    0x7: ('the drawing mode', ''),
    0x8: ('the z adjust', 'the exponent depth is measured against'),
    0x9: ('the projection', 'how far the eye is from the screen'),
    0xa: ('the light', 'the direction it comes from'),
    0xb: ('a matrix', 'places everything that follows'),
    0xc: ('a translation', 'the matrix\'s last three numbers only'),
    0x10: ('a skipped word', ''),
    0x16: ('the level of detail', ''),
    0xf: ('the end of the list', ''),
}


def dl_object(i):
    """st r,0x10(g10)[rN*1]: an object, with the choice of window (the two
    high bits of the address) coming from the object itself."""
    return (i.name.startswith('st') and i.ea and i.ea[0] == 'abase_index_off'
            and i.ea[1] == R.DL_BASE and i.ea[4] == 0x10)


def main():
    romdir, out = sys.argv[1], sys.argv[2]
    _, mem, traces, funcs = R.program(romdir)
    syms = R.load_symbols()
    byhand = R.hand_written(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'src'))

    writers = collections.defaultdict(dict)     # exponent -> {entry: count}
    for e, f in funcs.items():
        for a, i in sorted(f.insns.items()):
            note = R.dl_note(i)
            if note.startswith(' display list, exponent'):
                x = int(note.split()[-1]) & 0xf
            elif dl_object(i):
                x = 1          # an object: which window it uses is worked
            else:              # out as it runs, so the address is not static
                continue
            writers[x][e] = writers[x].get(e, 0) + 1

    rows = sorted(writers)
    name = lambda a: syms.get(a, ('f_%06x' % a,))[0]
    # layout
    lead, boxw, boxh, gap, lineh = 330, 250, 34, 12, 46
    per_row = 4
    margin = 30
    W = lead + per_row * (boxw + gap) - gap + margin
    y = 104
    place = {}
    for x in rows:
        ws = sorted(writers[x], key=lambda a: (a not in byhand, name(a)))
        lines = (len(ws) - 1) // per_row + 1
        place[x] = (y, ws, lines)
        y += lines * lineh + 26
    H = y + 30

    font = subprocess.run(['fc-match', '-f', '%{file}', 'DejaVu Sans'],
                          capture_output=True, text=True).stdout or ''
    o = ['font "%s"' % font, 'fill #0b0e12', 'rectangle 0 0 %d %d' % (W, H)]
    hand_n = sum(1 for x in rows for a in writers[x] if a in byhand)
    all_n = sum(len(writers[x]) for x in rows)
    o += ['fill white', 'font-size 20',
          'text 30,36 "What Daytona USA puts in the display list, and which C writes it"',
          'font-size 13', 'fill #93a2ae',
          'text 30,60 "one row per opcode; the renderer reads them in order '
          '(port/m2/m2geo.c). green: rewritten by hand (%d of %d writers)"' % (hand_n, all_n),
          'text 30,80 "grey: still the recompiler\'s output. the number is how many places in '
          'that function write this opcode."']
    for x in rows:
        yy, ws, lines = place[x]
        what, more = MEANING.get(x, ('?', ''))
        o += ['fill #e8f0f0', 'font-size 15',
              'text %d,%d "%x   %s"' % (margin, yy + 20, x, what)]
        if more:
            o += ['fill #5d6b78', 'font-size 11',
                  'text %d,%d "%s"' % (margin + 26, yy + 38, more[:46])]
        o += ['stroke #1c242e', 'stroke-width 1',
              'line %d,%.0f %d,%.0f' % (margin, yy + lines * lineh + 14,
                                        W - margin, yy + lines * lineh + 14), 'stroke none']
        for k, a in enumerate(ws):
            line, col = divmod(k, per_row)
            bx, by = lead + col * (boxw + gap), yy + line * lineh
            hand = a in byhand
            o += ['stroke #7fd4a0' if hand else 'stroke #3a4756', 'stroke-width 1.2',
                  'fill #1d3b2c' if hand else 'fill #151a20',
                  'roundrectangle %.0f,%.0f %.0f,%.0f 5,5' % (bx, by, bx + boxw, by + boxh),
                  'stroke none', 'font-size 12', 'fill #e8f0f0' if hand else 'fill #93a2ae',
                  'text %.0f,%.0f "%s"' % (bx + 9, by + 21, name(a)[:26]),
                  'fill #7fd4a0' if hand else 'fill #5d6b78', 'font-size 11',
                  'text %.0f,%.0f "%d"' % (bx + boxw - 22, by + 21, writers[x][a])]
    open(out, 'w').write('\n'.join(o) + '\n')
    print('%dx%d' % (W, H))


if __name__ == '__main__':
    main()
