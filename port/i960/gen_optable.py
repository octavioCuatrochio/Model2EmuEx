import struct,sys,csv
exe,tsv,out=sys.argv[1:4]
d=open(exe,'rb').read()
def off(va): return 0x125c00+va-0x528000
names={}
for r in csv.DictReader(open(tsv),delimiter='\t'):
    names[int(r['desc_va'],16)]=r['mnemonic']
va=0x5735e0; rows=[]
while True:
    w=struct.unpack_from('<9I',d,off(va))
    if w[0]==0: break
    rows.append((va,w,names[va])); va+=0x24
with open(out,'w') as f:
    f.write("/* Generated from EMULATOR.EXE (Model 2 Emulator 1.1a) opcode descriptor table at 0x5735e0.\n"
            "   Do not edit by hand: regenerate with gen_optable.py.\n"
            "   Columns: handler, format, opcode key, cycles, fp operands, flags18, flags1c, operand mask */\n")
    for va,w,n in rows:
        f.write(f"OP(op_{n:<9}, {w[2]}, 0x{w[3]:03x}, {w[4]:3d}, {w[5]}, {w[6]}, {w[7]}, 0x{w[8]:x}) /* {va:08x} orig handler {w[0]:08x} */\n")
print(len(rows),"rows")
# sanity: fields 1,5 constant?
print("name ptrs:",{w[1] for _,w,_ in rows}, "fp flag ops:",[n for _,w,n in rows if w[5]])
