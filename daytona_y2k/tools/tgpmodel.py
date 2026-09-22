#!/usr/bin/env python3
"""A model of Daytona USA's geometry-processor commands, checked against a
raw command log (tools/trace.c: TRACE_TGP=1 TRACE_TGP_RAW=file ...).

    tgpmodel.py raw.txt            check every command with results
    tgpmodel.py raw.txt --search   find the matrix conventions that fit

See notes/geometry.md for what the commands are.
"""
import itertools, math, struct, sys


def f32(w):
    return struct.unpack('<f', struct.pack('<I', w & 0xffffffff))[0]


def s16(w):
    w &= 0xffff
    return w - 0x10000 if w & 0x8000 else w


def ang(w):
    """16-bit angle (65536 = one turn) -> radians"""
    return s16(w) * 2 * math.pi / 65536


def to_ang(rad):
    return int(round(rad * 65536 / (2 * math.pi))) & 0xffff


def load(path, limit=None):
    out = []
    for line in open(path):
        p = line.split()
        if len(p) < 2 or p[0] != 'cmd':
            continue
        ins = [int(w, 16) for w in p[3:] if not w.startswith('->')]
        outs = [int(w[2:], 16) for w in p[3:] if w.startswith('->')]
        out.append((int(p[1], 16), int(p[2][1:-1], 16), ins, outs))
        if limit and len(out) >= limit:
            break
    return out


def close(a, b, rel=2e-3, ab=2e-3):
    return abs(a - b) <= ab + rel * abs(b)

# ---------------------------------------------------------------- matrices

def mat_mul(a, b):
    return [[sum(a[i][k] * b[k][j] for k in range(3)) for j in range(3)] for i in range(3)]


def rot(axis, t):
    c, s = math.cos(t), math.sin(t)
    if axis == 0:
        return [[1, 0, 0], [0, c, -s], [0, s, c]]
    if axis == 1:
        return [[c, 0, s], [0, 1, 0], [-s, 0, c]]
    return [[c, -s, 0], [s, c, 0], [0, 0, 1]]


class Matrices:
    """The current matrix (R, T) and its stack, with the conventions as
    parameters (for the search)."""
    def __init__(self, conv):
        self.c = conv
        self.R = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
        self.T = [0.0, 0.0, 0.0]
        self.stack = []

    def op(self, cmd, ins):
        c = self.c
        if cmd == 0x05:
            self.stack.append(([r[:] for r in self.R], self.T[:]))
        elif cmd == 0x06:
            if self.stack:
                self.R, self.T = self.stack.pop()
        elif cmd == 0x10:
            self.R = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]
            self.T = [0.0, 0.0, 0.0]
        elif cmd == 0x12:
            v = [f32(w) for w in ins]
            if c['trans_local']:
                self.T = [self.T[i] + sum(self.R[i][k] * v[k] for k in range(3)) for i in range(3)]
            else:
                self.T = [self.T[i] + v[i] for i in range(3)]
        elif cmd == 0x13:
            v = [f32(w) for w in ins]
            if c['scale_cols']:
                self.R = [[self.R[i][j] * v[j] for j in range(3)] for i in range(3)]
            else:
                self.R = [[self.R[i][j] * v[i] for j in range(3)] for i in range(3)]
        elif cmd in (0x14, 0x15, 0x16):
            k = cmd - 0x14
            m = rot(c['axis'][k], c['sign'][k] * ang(ins[0]))
            self.R = mat_mul(self.R, m) if c['post'] else mat_mul(m, self.R)

    def words(self):
        """what 0x11 returns: 9 rotation words then the translation"""
        R = self.R if self.c['rows'] else [list(r) for r in zip(*self.R)]
        return [x for r in R for x in r] + self.T

    def xform(self, v):
        R = self.R if not self.c['xform_t'] else [list(r) for r in zip(*self.R)]
        return [sum(R[i][k] * v[k] for k in range(3)) + self.T[i] for i in range(3)]

# ---------------------------------------------------------- the calculator

def scalar(cmd, ins, st):
    """the commands whose results don't depend on the matrices; st keeps
    the accumulator. Returns the expected outputs (floats, or ('ang', a))."""
    F = [f32(w) for w in ins]
    if cmd == 0x02: return [F[0] * F[1]]
    if cmd == 0x03: return [F[0] / F[1] if F[1] else None]
    if cmd == 0x1b: return [math.sin(ang(ins[0]))]
    if cmd == 0x1c: return [math.cos(ang(ins[0]))]
    if cmd == 0x1d: return [F[1] * math.sin(ang(ins[0]))]
    if cmd == 0x1e: return [F[1] * math.cos(ang(ins[0]))]
    if cmd == 0x0a: return [('ang', math.atan2(F[1], F[0]))]
    if cmd == 0x0f: return [('ang', math.atan2(F[1] - F[3], F[0] - F[2]))]
    if cmd == 0x43: return [math.hypot(F[2] - F[0], F[3] - F[1])]
    if cmd == 0x44:
        a = ang(ins[0])
        return [F[1] * math.sin(a), F[1] * math.cos(a)]
    if cmd == 0x47:
        a = ang(ins[0])
        dx, dz = F[1] * math.sin(a), F[1] * math.cos(a)
        return [dx, dz, F[2] + dx, F[3] + dz]
    if cmd == 0x31: st['quad'] = F; return []
    if cmd == 0x32:                      # is (x, z) inside the quad? -1 if so
        q = st.get('quad', [0.0] * 12)
        pts = [(q[k], q[k + 2]) for k in range(0, 12, 3)]
        x, z = F[0], F[2]
        sides = [(bx - ax) * (z - az) - (bz - az) * (x - ax)
                 for (ax, az), (bx, bz) in zip(pts, pts[1:] + pts[:1])]
        inside = all(v >= 0 for v in sides) or all(v <= 0 for v in sides)
        return [('int', -1 if inside else 0)]
    if cmd == 0x40: st['circle'] = F; return []
    if cmd == 0x41:
        x0, z0, r = st.get('circle', (0.0, 0.0, 0.0))
        return [math.hypot(F[0] - x0, F[1] - z0) - r]
    if cmd == 0x24: st['acc'] = F[0]; return []
    if cmd == 0x26: st['acc'] += F[0]; return []
    if cmd == 0x27: st['acc'] -= F[0]; return []
    if cmd == 0x28: st['acc'] *= F[0]; return []
    if cmd == 0x29: st['acc'] = st['acc'] / F[0] if F[0] else float('inf'); return []
    if cmd == 0x25: return [st['acc']]
    return None


def match(exp, got):
    if exp is None or len(exp) != len(got):
        return False
    for e, g in zip(exp, got):
        if e is None:
            continue
        if isinstance(e, tuple) and e[0] == 'int':
            if (e[1] & 0xffffffff) != g:
                return False
        elif isinstance(e, tuple):
            d = (to_ang(e[1]) - (g & 0xffff)) & 0xffff
            if min(d, 0x10000 - d) > 8:
                return False
        elif not close(e, f32(g)):
            return False
    return True

# ------------------------------------------------------------------- main

MATRIX = (0x05, 0x06, 0x10, 0x12, 0x13, 0x14, 0x15, 0x16)

# what the search found (1465 of 1465 results in a race): rotations about
# x, y, z by minus the angle, post-multiplied; translate in the local frame;
# scale the columns; 0x11 gives the 3x3 part column by column
CONV = dict(axis=(0, 1, 2), sign=(-1, -1, -1), post=True, trans_local=True,
            scale_cols=True, rows=False, xform_t=False)


def score(packets, conv):
    m = Matrices(conv)
    ok = n = 0
    for cmd, site, ins, outs in packets:
        if cmd in MATRIX:
            m.op(cmd, ins)
        elif cmd == 0x11 and len(outs) == 12:
            n += 1
            ok += all(close(e, f32(g), 1e-3, 1e-6) for e, g in zip(m.words(), outs))
        elif cmd == 0x1a and len(outs) == 3:
            n += 1
            ok += all(close(e, f32(g)) for e, g in zip(m.xform([f32(w) for w in ins]), outs))
    return ok, n


def search(packets):
    best = []
    for axes in itertools.permutations(range(3)):
        for signs in itertools.product((1, -1), repeat=3):
            for post, tl, sc, rows, xt in itertools.product((True, False), repeat=5):
                conv = dict(axis=axes, sign=signs, post=post, trans_local=tl, scale_cols=sc,
                            rows=rows, xform_t=xt)
                ok, n = score(packets, conv)
                best.append((ok, n, conv))
    best.sort(key=lambda b: -b[0])
    for ok, n, conv in best[:5]:
        print('%d/%d  %s' % (ok, n, conv))


def check(packets):
    st = {'acc': 0.0}
    stats = {}
    m = Matrices(CONV)
    for cmd, site, ins, outs in packets:
        if cmd in MATRIX:
            m.op(cmd, ins)
            continue
        if cmd == 0x11:
            exp = m.words()
        elif cmd == 0x1a:
            exp = m.xform([f32(w) for w in ins])
        else:
            exp = scalar(cmd, ins, st)
        if exp is None or not outs:
            continue
        s = stats.setdefault(cmd, [0, 0, None])
        s[1] += 1
        if match(exp, outs):
            s[0] += 1
        elif s[2] is None:
            s[2] = (ins, outs, exp)
    for cmd in sorted(stats):
        ok, n, bad = stats[cmd]
        print('  %02x  %6d/%-6d match%s' % (cmd, ok, n, '' if ok == n else
              '   first miss: in %s out %s expected %s' % (
                  [hex(w) for w in bad[0]], [hex(w) for w in bad[1]], bad[2])))


if __name__ == '__main__':
    pk = load(sys.argv[1], 60000 if '--search' in sys.argv else None)
    if '--search' in sys.argv:
        search(pk)
    else:
        check(pk)
