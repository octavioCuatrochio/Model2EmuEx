import struct,sys
d=open(sys.argv[1],'rb').read()
secs=[(0x508000,0x106600,0x1f5a2),(0x528000,0x125c00,0x6cc00)]
def off(va):
    for v,f,s in secs:
        if v<=va<v+s: return f+va-v
def u32(va): return struct.unpack_from('<I',d,off(va))[0]
def cstr(va):
    o=off(va)
    if o is None: return None
    e=d.index(b'\0',o); s=d[o:e]
    return s.decode('latin1') if 0<len(s)<40 and all(32<=c<127 for c in s) else None
def rec(va):
    w=[u32(va+4*i) for i in range(9)]
    return w
print("default:", ' '.join(f"{x:08x}" for x in rec(0x5733c4)))
va=0x5735e0; n=0
while u32(va):
    w=rec(va); names=[cstr(x) for x in w]
    print(f"{va:08x} op={w[3]:03x} fmt={w[2]} h={w[0]:06x} cyc={w[4]} flags={w[8]:x} raw="+' '.join(f"{x:08x}" for x in w)+"  "+' '.join(n for n in names if n))
    va+=0x24; n+=1
print("count",n)
