import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.data.*;
import ghidra.program.model.symbol.*;
import ghidra.program.util.DefinedStringIterator;
import java.io.*;
import java.util.*;

public class ExportDecomp extends GhidraScript {
    @Override
    public void run() throws Exception {
        String out = getScriptArgs()[0];
        new File(out).mkdirs();
        DecompInterface d = new DecompInterface();
        DecompileOptions o = new DecompileOptions();
        d.setOptions(o);
        d.toggleCCode(true);
        d.toggleSyntaxTree(false);
        d.openProgram(currentProgram);

        // types header
        try (PrintWriter h = new PrintWriter(new FileWriter(out + "/types.h"))) {
            DataTypeWriter w = new DataTypeWriter(currentProgram.getDataTypeManager(), h);
            List<DataType> all = new ArrayList<>();
            currentProgram.getDataTypeManager().getAllDataTypes(all);
            w.write(all, monitor);
        } catch (Exception e) { println("types.h: " + e); }

        // prototypes + all functions in one file, plus per-4k-address-bucket split
        PrintWriter all = new PrintWriter(new FileWriter(out + "/all_functions.c"));
        PrintWriter protos = new PrintWriter(new FileWriter(out + "/prototypes.h"));
        PrintWriter idx = new PrintWriter(new FileWriter(out + "/functions.tsv"));
        idx.println("address\tname\tsize\tthunk\tlibrary_guess");
        all.println("#include \"types.h\"\n#include \"prototypes.h\"\n");
        int n = 0, fail = 0;
        FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
        while (it.hasNext() && !monitor.isCancelled()) {
            Function f = it.next();
            idx.println(f.getEntryPoint() + "\t" + f.getName() + "\t" + f.getBody().getNumAddresses() + "\t" + f.isThunk() + "\t" + (f.isExternal() ? "ext" : ""));
            if (f.isExternal() || f.isThunk()) continue;
            protos.println(f.getPrototypeString(false, false) + ";");
            DecompileResults r = d.decompileFunction(f, 120, monitor);
            all.println("// ===== " + f.getName() + " @ " + f.getEntryPoint() + " =====");
            if (r != null && r.decompileCompleted()) {
                all.println(r.getDecompiledFunction().getC());
            } else {
                fail++;
                all.println("/* DECOMPILE FAILED: " + (r == null ? "null" : r.getErrorMessage()) + " */\n");
            }
            if (++n % 500 == 0) println("decompiled " + n);
        }
        all.close(); protos.close(); idx.close();

        // strings
        try (PrintWriter s = new PrintWriter(new FileWriter(out + "/strings.tsv"))) {
            for (Data dt : DefinedStringIterator.forProgram(currentProgram)) {
                s.println(dt.getAddress() + "\t" + String.valueOf(dt.getValue()).replace("\n","\\n").replace("\r","\\r"));
            }
        }
        println("DONE functions=" + n + " failed=" + fail);
    }
}
