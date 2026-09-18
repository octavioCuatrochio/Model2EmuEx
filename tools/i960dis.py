#!/usr/bin/env python3
"""Minimal i960 KB/SB disassembler for debugging the Model 2 port.
usage: i960dis.py file base start [count]   (base = address of file offset 0)"""
import sys, struct
mn={
0x08:'b',0x09:'call',0x0a:'ret',0x0b:'bal',0x10:'bno',0x11:'bg',0x12:'be',0x13:'bge',0x14:'bl',0x15:'bne',0x16:'ble',0x17:'bo',
0x18:'faultno',0x19:'faultg',0x1a:'faulte',0x1b:'faultge',0x1c:'faultl',0x1d:'faultne',0x1e:'faultle',0x1f:'faulto',
0x20:'testno',0x21:'testg',0x22:'teste',0x23:'testge',0x24:'testl',0x25:'testne',0x26:'testle',0x27:'testo',
0x30:'bbc',0x31:'cmpobg',0x32:'cmpobe',0x33:'cmpobge',0x34:'cmpobl',0x35:'cmpobne',0x36:'cmpoble',0x37:'bbs',
0x38:'cmpibno',0x39:'cmpibg',0x3a:'cmpibe',0x3b:'cmpibge',0x3c:'cmpibl',0x3d:'cmpibne',0x3e:'cmpible',0x3f:'cmpibo',
0x80:'ldob',0x82:'stob',0x84:'bx',0x85:'balx',0x86:'callx',0x88:'ldos',0x8a:'stos',0x8c:'lda',0x90:'ld',0x92:'st',
0x98:'ldl',0x9a:'stl',0xa0:'ldt',0xa2:'stt',0xb0:'ldq',0xb2:'stq',0xc0:'ldib',0xc2:'stib',0xc8:'ldis',0xca:'stis',
0x580:'notbit',0x581:'and',0x582:'andnot',0x583:'setbit',0x584:'notand',0x586:'xor',0x587:'or',0x588:'nor',0x589:'xnor',
0x58a:'not',0x58b:'ornot',0x58c:'clrbit',0x58d:'notor',0x58e:'nand',0x58f:'alterbit',
0x590:'addo',0x591:'addi',0x592:'subo',0x593:'subi',0x598:'shro',0x59a:'shrdi',0x59b:'shri',0x59c:'shlo',0x59d:'rotate',0x59e:'shli',
0x5a0:'cmpo',0x5a1:'cmpi',0x5a2:'concmpo',0x5a3:'concmpi',0x5a4:'cmpinco',0x5a5:'cmpinci',0x5a6:'cmpdeco',0x5a7:'cmpdeci',
0x5ac:'scanbyte',0x5ae:'chkbit',0x5b0:'addc',0x5b2:'subc',0x5cc:'mov',0x5d8:'eshro',0x5dc:'movl',0x5ec:'movt',0x5fc:'movq',
0x600:'synmov',0x601:'synmovl',0x602:'synmovq',0x610:'atmod',0x612:'atadd',0x615:'synld',
0x640:'spanbit',0x641:'scanbit',0x642:'daddc',0x643:'dsubc',0x644:'dmovt',0x645:'modac',0x650:'modify',0x651:'extract',
0x654:'modtc',0x655:'modpc',0x660:'calls',0x66b:'mark',0x66c:'fmark',0x66d:'flushreg',0x66f:'syncf',0x670:'emul',0x671:'ediv',
0x674:'cvtir',0x675:'cvtilr',0x676:'scalerl',0x677:'scaler',
0x680:'atanr',0x681:'logepr',0x682:'logr',0x683:'remr',0x684:'cmpor',0x685:'cmpr',0x688:'sqrtr',0x689:'expr',0x68a:'logbnr',
0x68b:'roundr',0x68c:'sinr',0x68d:'cosr',0x68e:'tanr',0x68f:'classr',
0x690:'atanrl',0x691:'logeprl',0x692:'logrl',0x693:'remrl',0x694:'cmporl',0x695:'cmprl',0x698:'sqrtrl',0x699:'exprl',
0x69a:'logbnrl',0x69b:'roundrl',0x69c:'sinrl',0x69d:'cosrl',0x69e:'tanrl',0x69f:'classrl',
0x6c0:'cvtri',0x6c1:'cvtril',0x6c2:'cvtzri',0x6c3:'cvtzril',0x6c9:'movr',0x6d9:'movrl',0x6e1:'movre',0x6e2:'cpysre',0x6e3:'cpyrsre',
0x701:'mulo',0x708:'remo',0x70b:'divo',0x741:'muli',0x748:'remi',0x749:'modi',0x74b:'divi',
0x78b:'divr',0x78c:'mulr',0x78d:'subr',0x78f:'addr',0x79b:'divrl',0x79c:'mulrl',0x79d:'subrl',0x79f:'addrl'}

def reg(n, lit=False, fp=False):
    if lit: return str(n)
    if n == 31: return 'fp'
    if n < 16: return ['pfp','sp','rip'][n] if n < 3 else 'r%d' % n
    return 'g%d' % (n - 16)

def dis(mem, base, addr):
    w = struct.unpack_from('<I', mem, addr - base)[0]
    op = w >> 24
    if op < 0x20:
        d = w & 0xfffffc
        if d & 0x800000: d -= 0x1000000
        return 4, '%s 0x%x' % (mn.get(op, 'op%02x' % op), addr + d) if op < 0x18 or op >= 0x10 else ''
    if op < 0x40:
        s1 = (w >> 19) & 31; s2 = (w >> 14) & 31; m1 = (w >> 13) & 1
        d = w & 0x1ffc
        if d & 0x1000: d -= 0x2000
        if op < 0x30:
            return 4, '%s %s' % (mn.get(op, 'op%02x' % op), reg(s1))
        return 4, '%s %s,%s,0x%x' % (mn.get(op, 'op%02x' % op), reg(s1, m1), reg(s2), addr + d)
    if op < 0x80:
        full = (op << 4) | ((w >> 7) & 15)
        d = (w >> 19) & 31; s2 = (w >> 14) & 31; s1 = w & 31
        m3 = (w >> 13) & 1; m2 = (w >> 12) & 1; m1 = (w >> 11) & 1
        return 4, '%s %s,%s,%s' % (mn.get(full, 'op%03x' % full), reg(s1, m1), reg(s2, m2), reg(d, m3) if not m3 else 'lit%d' % d)
    sd = (w >> 19) & 31; ab = (w >> 14) & 31
    name = mn.get(op & 0xfe, 'op%02x' % op)
    if not (w >> 12) & 1:
        off = w & 0xfff
        ea = '0x%x(%s)' % (off, reg(ab)) if (w >> 13) & 1 else '0x%x' % off
        size = 4
    else:
        mode = (w >> 10) & 15; sc = 1 << ((w >> 7) & 7); ix = w & 31
        size = 4
        disp = None
        if mode >= 12 or mode == 5:
            disp = struct.unpack_from('<I', mem, addr - base + 4)[0]; size = 8
        if mode == 4: ea = '(%s)' % reg(ab)
        elif mode == 5: ea = '0x%x' % ((addr + 8 + disp) & 0xffffffff)
        elif mode == 7: ea = '(%s)[%s*%d]' % (reg(ab), reg(ix), sc)
        elif mode == 12: ea = '0x%x' % disp
        elif mode == 13: ea = '0x%x(%s)' % (disp, reg(ab))
        elif mode == 14: ea = '0x%x[%s*%d]' % (disp, reg(ix), sc)
        elif mode == 15: ea = '0x%x(%s)[%s*%d]' % (disp, reg(ab), reg(ix), sc)
        else: ea = '?mode%d' % mode
    if name.startswith('st'):
        return size, '%s %s,%s' % (name, reg(sd), ea)
    if name in ('b', 'bx', 'balx', 'callx'):
        return size, '%s %s' % (name, ea) + (',%s' % reg(sd) if name == 'balx' else '')
    return size, '%s %s,%s' % (name, ea, reg(sd))

if __name__ == '__main__':
    mem = open(sys.argv[1], 'rb').read()
    base = int(sys.argv[2], 0); a = int(sys.argv[3], 0)
    n = int(sys.argv[4], 0) if len(sys.argv) > 4 else 40
    for _ in range(n):
        size, t = dis(mem, base, a)
        print('%08x: %-20s %s' % (a, ' '.join('%08x' % struct.unpack_from('<I', mem, a - base + k)[0] for k in range(0, size, 4)), t))
        a += size
