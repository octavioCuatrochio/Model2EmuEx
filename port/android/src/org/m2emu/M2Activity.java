/* m2emu's Android activity: SDL2's, with the command line and environment
   taken from the intent, so a run can be started from adb:
     am start -n org.m2emu/.M2Activity --es args '--no-gui daytona' \
         --es env 'M2EMU_KEYS=200:06:5,300:02:5;M2EMU_BENCH=3600'
   args: m2emu's command line, split at spaces; env: NAME=value pairs split
   at ';' (m2emu reads its debugging switches from the environment).
   surface: the size m2emu draws at, WxH, or "full" for the screen's own;
   default, for a game started with --no-gui, the game's 384 lines at the
   screen's shape (683x384 on 16:9); the launcher gets the screen's size.
   The display hardware scales the surface to the screen (1080p) for free,
   where the GPU of a TV box (Mali-400) can't fill 1080p every frame. */
package org.m2emu;

import android.graphics.Point;
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
        String args = getIntent().getStringExtra("args");
        if (mSurface == null || "full".equals(surface) ||
            (surface == null && (args == null || !args.contains("--no-gui"))))
            return;
        int w = 0, h = 0;
        if (surface != null && surface.matches("\\d+x\\d+")) {
            String[] wh = surface.split("x");
            w = Integer.parseInt(wh[0]);
            h = Integer.parseInt(wh[1]);
        } else {
            Point screen = new Point();
            getWindowManager().getDefaultDisplay().getRealSize(screen);
            int sw = Math.max(screen.x, screen.y), sh = Math.min(screen.x, screen.y);
            h = 384;
            w = (int)Math.round(384.0 * sw / sh);
        }
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
