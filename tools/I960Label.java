import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.io.*;
import java.util.*;

public class I960Label extends GhidraScript {
    Address a(long v) { return toAddr(v); }

    void fn(long addr, String name) throws Exception {
        Address ad = a(addr);
        if (getInstructionAt(ad) == null) disassemble(ad);
        Function f = getFunctionAt(ad);
        if (f == null) {
            Function c = getFunctionContaining(ad);
            if (c != null && !c.getEntryPoint().equals(ad)) {
                println("splitting " + c.getName() + " at " + ad);
                // shrink containing function so the handler becomes its own function
                c.setBody(new AddressSet(c.getEntryPoint(), ad.subtract(1)));
            }
            f = createFunction(ad, name);
        }
        if (f != null) f.setName(name, SourceType.USER_DEFINED);
        else println("could not create " + name + " at " + ad);
    }

    void lbl(long addr, String name) throws Exception {
        createLabel(a(addr), name, true, SourceType.USER_DEFINED);
    }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        String tsv = args[0], out = args[1];

        List<long[]> handlers = new ArrayList<>();
        try (BufferedReader r = new BufferedReader(new FileReader(tsv))) {
            r.readLine();
            String l;
            while ((l = r.readLine()) != null) {
                String[] p = l.split("\t");
                long h = Long.parseLong(p[3], 16);
                fn(h, "i960_op" + p[1] + "_" + p[6]);
            }
        }
        fn(0x4c2530L, "i960_invalid_opcode");
        fn(0x4c4b00L, "i960_execute");
        fn(0x4c4750L, "i960_decode_insn");
        fn(0x4c48f0L, "i960_predecode_region");
        fn(0x4c2740L, "i960_build_optable");
        fn(0x4c2790L, "i960_opcode_key");
        fn(0x4c2720L, "i960_get_byte_200140");
        fn(0x4c2730L, "i960_get_pc");

        lbl(0x5aa8f0L, "g_i960_pc_ptr");
        lbl(0x5aa8f4L, "g_i960_ctx");
        lbl(0x5aa8f8L, "g_i960_literals");
        lbl(0x5aaaf8L, "g_i960_optable");
        lbl(0x5aeb00L, "g_i960_cur_insn");
        lbl(0x5735e0L, "g_i960_opdefs");
        lbl(0x5733c4L, "g_i960_invalid_opdef");
        lbl(0x10f1fd0L, "g_i960_pc_hist0");
        lbl(0x10f1fd4L, "g_i960_pc_hist1");
        lbl(0x10f1fd8L, "g_i960_pc_hist2");
        lbl(0x10f1fccL, "g_i960_pc_hist3");

        // decompile everything in the i960 core region
        DecompInterface d = new DecompInterface();
        d.toggleCCode(true);
        d.openProgram(currentProgram);
        AddressSet core = new AddressSet(a(0x4c0000L), a(0x4c5900L));
        try (PrintWriter w = new PrintWriter(new FileWriter(out))) {
            int n = 0;
            for (Function f : currentProgram.getFunctionManager().getFunctions(core, true)) {
                DecompileResults r = d.decompileFunction(f, 120, monitor);
                w.println("// ===== " + f.getName() + " @ " + f.getEntryPoint() + " (" + f.getBody().getNumAddresses() + " bytes) =====");
                w.println(r.decompileCompleted() ? r.getDecompiledFunction().getC()
                        : "/* DECOMPILE FAILED: " + r.getErrorMessage() + " */\n");
                n++;
            }
            println("I960 DONE functions=" + n);
        }
    }
}
