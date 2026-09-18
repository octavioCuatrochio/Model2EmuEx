import struct,sys
d=open(sys.argv[1],'rb').read()
# sections: (va, fileoff, size)
secs={'.rdata':(0x508000,0x106600,0x1f5a2),'.data':(0x528000,0x125c00,0x6cc00)}
T0,T1=0x401000,0x401000+0x10612b
for name,(va,fo,sz) in secs.items():
    run=[];start=None
    for off in range(0,sz-3,4):
        v=struct.unpack_from('<I',d,fo+off)[0]
        if T0<=v<T1:
            if start is None: start=off; run=[]
            run.append(v)
        else:
            if start is not None and len(run)>=16:
                u=len(set(run))
                print(f"{name} {va+start:08x} entries={len(run):5d} unique={u:5d} targets {min(run):06x}-{max(run):06x}")
            start=None
