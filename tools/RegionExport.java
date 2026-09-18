// Creates functions in an address range and exports their decompiles.
// Args: <out.c> <start hex> <end hex> [extra entry hex ...]
// Function starts: explicit extras, plus 16-byte-aligned addresses that
// follow int3/ret padding and aren't inside an existing function (MSVC
// pads between functions with 0xcc). Needed because much of the code is only
// reached through pointer tables, which auto-analysis doesn't follow.
import ghidra.app.script.GhidraScript;
import ghidra.app.cmd.disassemble.DisassembleCommand;
import ghidra.app.decompiler.*;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.*;
import java.io.*;
import java.util.*;

public class RegionExport extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] a = getScriptArgs();
        String out = a[0];
        long start = Long.parseLong(a[1], 16), end = Long.parseLong(a[2], 16);
        AddressSpace sp = currentProgram.getAddressFactory().getDefaultAddressSpace();
        Memory mem = currentProgram.getMemory();
        Listing lst = currentProgram.getListing();
        FunctionManager fm = currentProgram.getFunctionManager();

        List<Long> cands = new ArrayList<>();
        for (int i = 3; i < a.length; i++)
            cands.add(Long.parseLong(a[i], 16));
        for (long x = start; x < end; x += 16) {
            byte prev = mem.getByte(sp.getAddress(x - 1));
            byte cur = mem.getByte(sp.getAddress(x));
            if ((prev == (byte) 0xcc || prev == (byte) 0xc3) && cur != (byte) 0xcc && cur != 0)
                cands.add(x);
        }
        int made = 0;
        for (long x : cands) {
            Address ad = sp.getAddress(x);
            Function in = fm.getFunctionContaining(ad);
            if (in != null && !in.getEntryPoint().equals(ad) && !isExtra(a, x))
                continue;   // inside an existing function's body
            if (lst.getInstructionAt(ad) == null) {
                DisassembleCommand c = new DisassembleCommand(ad, null, true);
                c.applyTo(currentProgram, monitor);
            }
            if (fm.getFunctionAt(ad) == null && createFunction(ad, null) != null)
                made++;
        }
        println("created " + made + " functions");

        DecompInterface d = new DecompInterface();
        d.setOptions(new DecompileOptions());
        d.toggleCCode(true);
        d.openProgram(currentProgram);
        try (PrintWriter w = new PrintWriter(new FileWriter(out))) {
            FunctionIterator it = fm.getFunctions(sp.getAddress(start), true);
            while (it.hasNext()) {
                Function f = it.next();
                long e = f.getEntryPoint().getOffset();
                if (e >= end) break;
                w.println("// ===== " + f.getName() + " @ " + f.getEntryPoint() + " (" + f.getBody().getNumAddresses() + " bytes) =====\n");
                DecompileResults r = d.decompileFunction(f, 180, monitor);
                if (r != null && r.decompileCompleted())
                    w.println(r.getDecompiledFunction().getC());
                else
                    w.println("/* DECOMPILE FAILED */\n");
            }
        }
        println("REGION DONE");
    }

    private static boolean isExtra(String[] a, long x) {
        for (int i = 3; i < a.length; i++)
            if (Long.parseLong(a[i], 16) == x) return true;
        return false;
    }
}
