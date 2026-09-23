/* m2emu's Android activity: SDL2's, with the command line and environment
   taken from the intent, so a run can be started from adb:
     am start -n org.m2emu/.M2Activity --es args '--no-gui daytona' \
         --es env 'M2EMU_KEYS=200:06:5,300:02:5;M2EMU_BENCH=3600'
   args: m2emu's command line, split at spaces; env: NAME=value pairs split
   at ';' (m2emu reads its debugging switches from the environment).
   surface: WxH to draw at that size and have the display hardware scale it
   up (cheaper for the GPU, but some TV boxes, like the Allwinner H3 one,
   show it unscaled in a corner); by default the screen's own size, where
   the game's frame is drawn at its own resolution and scaled by the GPU. */
package org.m2emu;

import android.os.Bundle;
import android.system.Os;
import android.util.Log;

import org.libsdl.app.SDLActivity;

public class M2Activity extends SDLActivity {
    @Override
    protected void onCreate(Bundle state) {
        String env = getIntent().getStringExtra("env");
        if (env != null)
            for (String kv : env.split(";")) {
                int eq = kv.indexOf('=');
                if (eq <= 0) continue;
                try {
                    Os.setenv(kv.substring(0, eq).trim(), kv.substring(eq + 1), true);
                } catch (Exception e) {
                    Log.w("m2emu", "setenv " + kv + ": " + e);
                }
            }
        super.onCreate(state);

        String surface = getIntent().getStringExtra("surface");
        if (mSurface == null || surface == null || !surface.matches("\\d+x\\d+"))
            return;
        String[] wh = surface.split("x");
        int w = Integer.parseInt(wh[0]), h = Integer.parseInt(wh[1]);
        Log.v("m2emu", "surface " + w + "x" + h);
        mSurface.getHolder().setFixedSize(w, h);
    }

    @Override
    protected String[] getArguments() {
        String args = getIntent().getStringExtra("args");
        if (args == null || args.trim().isEmpty())
            return new String[0];
        return args.trim().split("\\s+");
    }

    /* the emulator keeps state in globals: start the next run in a new process */
    @Override
    protected void onDestroy() {
        super.onDestroy();
        System.exit(0);
    }
}
