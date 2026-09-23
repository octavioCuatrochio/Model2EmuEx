# Model 2 port — user guide

A portable C port of ElSemi's Sega Model 2 Emulator 1.1a, run by an SDL2 +
OpenGL frontend (`m2emu`, with a launcher for games and settings) on Linux
and Windows. The target games are Daytona USA
(`daytona`), Virtua Fighter 2 (`vf2`) and Sega Rally Championship
(`srallyc`), with video, sound and input. Other games in the list may boot but
haven't been tested.

## Building

Needs a C11 and C++14 compiler (GCC or Clang), SDL2 and zlib. OpenGL is
loaded at run time, so no GL headers or libraries are needed to build.

| Build | Graphics | Command |
|-------|----------|---------|
| default | desktop OpenGL 2.1 or later (Windows, Linux) | `make` |
| OpenGL ES | OpenGL ES 2 (Android, Mali GPUs, single-board computers) | `make GL=gles` |

Both run the same renderer; `GL=` only chooses the kind of GL context
`m2emu` asks for and the shader dialect. Pictures are the same: desktop GL
computes in full precision where GLES rounds to mediump, which changes a
few pixels by 1–6 steps of 255.

### Linux

    sudo pacman -S sdl2 zlib          # or: apt install libsdl2-dev zlib1g-dev
    cd port
    make            # build/m2emu (the emulator), build/m2run (headless tool)
    make clean

### Windows

With MSYS2 (UCRT64 shell), natively:

    pacman -S make mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-SDL2 mingw-w64-ucrt-x86_64-zlib
    cd port
    make            # build/m2emu.exe, build/m2run.exe

Copy `SDL2.dll` (and `zlib1.dll`, from `/ucrt64/bin`) next to the programs
to run them outside the MSYS2 shell.

Cross-compiled from Linux with MinGW-w64 (the distribution's
`mingw-w64-gcc`, or the standalone llvm-mingw):

    # SDL2: SDL2-devel-2.x-mingw.tar.gz from github.com/libsdl-org/SDL/releases
    # zlib: from zlib.net, built with the same compiler:
    #   make -f win32/Makefile.gcc PREFIX=x86_64-w64-mingw32- libz.a
    #   then put zlib.h, zconf.h in <zlib>/include and libz.a in <zlib>/lib
    make windows SDL2=<SDL2-2.x>/x86_64-w64-mingw32 ZLIB=<zlib> [MINGW=<path>/bin/x86_64-w64-mingw32-]

This writes `build-windows/m2emu.exe`, `m2run.exe` and `SDL2.dll` (the only
DLL needed). On Windows, save data goes to an `NVDATA` folder next to
`m2emu.exe`, as with the original emulator. MSVC isn't supported (the code
uses GCC/Clang built-ins and C11 atomics).

### Android

The app (`m2emu` with OpenGL ES 2, for TV boxes such as the Allwinner H3
with a Mali-400; 32-bit ARM, tuned for the Cortex-A7) is built with the
Android NDK and SDK and SDL2's source (2.32.x, from libsdl.org); no Gradle:

    make apk NDK=~/Android/Sdk/ndk/<version> SDL=<SDL2-2.32.x source>   # build-apk/m2emu.apk
    adb install -r build-apk/m2emu.apk
    adb shell mkdir -p /sdcard/Android/data/org.m2emu/files/roms
    adb push daytona.zip /sdcard/Android/data/org.m2emu/files/roms/

The app's folder `/sdcard/Android/data/org.m2emu/files` holds `roms/`,
`NVDATA/`, `m2emu.ini` and `m2emu.log` (everything the program prints,
including the frame rate once a second). Started from the home screen it
opens the launcher; a game can be started from adb with the command line
and environment variables of the desktop version:

    adb shell am start -n org.m2emu/.M2Activity --es args "'--no-gui --widescreen 16:9 daytona'" \
        --es env "'M2EMU_BENCH=2500;M2EMU_BENCH_FROM=1300'"

(`env`: `NAME=value` pairs separated by `;`.) The 3D is drawn at the
game's own resolution (the render scale is automatic: 1 on a 720p screen)
and the GPU stretches the frame to the full screen; on a TV box the display
hardware then scales Android's 720p to 1080p. `--es surface WxH` draws into
a smaller surface for the display hardware to scale instead (less work for
the GPU), but some boxes, the Allwinner H3 one among them, show it unscaled
in a corner. The headless tool alone, for timing:

    make android NDK=~/Android/Sdk/ndk/<version>    # build-android/m2run
    adb push build-android/m2run /data/local/tmp/
    adb shell 'cd /data/local/tmp && M2_BENCH=1 ./m2run daytona 3000 /sdcard/roms'

### Automatic builds (GitHub Actions)

Every push and pull request is built by `.github/workflows/build.yml`: Linux
(desktop OpenGL and OpenGL ES), Windows (MSYS2, with the DLLs it needs) and
the Android `m2run`. The programs are on the run's page in the Actions tab,
under Artifacts (ROMs not included). "Run workflow" on the Build workflow
starts one by hand.

### Build options

`CFLAGS`, `CXXFLAGS` and `LDFLAGS` can be given in the environment (for
example `CFLAGS="-O1 -g -fsanitize=thread" LDFLAGS=-fsanitize=thread
make BUILD=build-tsan`). `SDL_CFLAGS`/`SDL_LIBS` and
`ZLIB_CFLAGS`/`ZLIB_LIBS` replace the pkg-config / `-lz` defaults.

## Running

    build/m2emu                              # the launcher
    build/m2emu --no-gui [options] <game>    # a game straight from the command line

ROMs are MAME-style zip sets (`daytona.zip`, `vf2.zip`, `srallyc.zip`, ...).
Each file is looked up in the game's zip, then its parent's, then
`model2.zip`. `build/m2emu --help` lists the options and the supported game
names.

Example without the launcher, 16:9 fullscreen with a pad-friendly shifter:

    build/m2emu --no-gui -r ~/roms --widescreen 16:9 --fullscreen --shifter sequential daytona

## Launcher

`m2emu` on its own opens a window with two tabs. It works with the mouse,
the keyboard (arrows, Enter, Tab) or a pad (d-pad, A to choose, B to go
back).

- **Games**: choose the folder with the ROM zip files (type it, or Browse
  for it). The games found there are listed with their set name, board,
  whether they're known to work with this emulator, and whether every ROM
  file is there. Select one and press Play, or double-click it. For
  Daytona USA, "Two players, linked" starts linked play (see below).
  In a game, Esc comes back to the launcher; closing the window quits.
- **Configuration**: every option below, grouped (display, colour, timing,
  controls, linked play, folders), each with an explanation. Changes are
  saved at once, with the ROM folder and the last game played, in
  `m2emu.ini`: next to `m2emu.exe` on Windows, in
  `$XDG_CONFIG_HOME/m2emu/` or `~/.config/m2emu/` on Linux.

A game named on the command line (`m2emu daytona`) is selected in the
launcher. The other command-line options only apply with `--no-gui`; the
launcher uses its own settings.

## Options

These are the command-line forms (with `--no-gui`); the launcher's
Configuration tab has the same settings.

| Option | Default | Effect |
|--------|---------|--------|
| `--no-gui` | off | start the game named on the command line, without the launcher |
| `-r DIR` | `$M2_ROMS`, else `./roms` | ROM directory |
| `-n DIR` | Linux: `$XDG_DATA_HOME/m2emu/NVDATA`, else `~/.local/share/m2emu/NVDATA`; Windows: `NVDATA` next to `m2emu.exe` | where save data goes (see below) |
| `--size WxH` | 1024x768; with widescreen 1365x768 (16:9) or 1229x768 (16:10) | window size |
| `--fullscreen` | off | start fullscreen (F11 toggles) |
| `--vsync` | off | sync buffer swaps to the display; pacing is by the game's own frame rate either way |
| `--no-frame-cap` | capped | run as fast as the computer can instead of at the game's 57.5 frames per second (the sound keeps its normal speed) |
| `--widescreen 16:9\|16:10\|fill\|off` | off | wider picture with extra field of view (see below); `fill` = the window's own ratio (4:3 up to 4:1); F9 toggles |
| `--aspect keep\|stretch\|crop` | keep | when the window (or a player's half) is narrower than the picture: keep = bands above and below; stretch = squeeze it to fill (distorted); crop = fill it at the right proportions by cutting the picture's sides while the game shows 3D, with the HUD squeezed in whole (other screens are squeezed) |
| `--scale N\|auto` | auto | render resolution: N times 496x384 (auto = window height / 384); F10 cycles |
| `--sharp` | smooth | nearest-neighbour instead of linear filtering when scaling the finished picture to the window |
| `--texture-filter nearest\|bilinear\|trilinear` | nearest | filtering of the 3D textures: nearest = square texels, as the hardware and MAME; bilinear = smooth, as the original emulator's Direct3D; trilinear = bilinear, and distant or slanted textures averaged over each pixel so they don't shimmer. F6 cycles. Bilinear and trilinear are for PC graphics cards (on the Mali-400 keep nearest) |
| `--mesh blend\|checker` | blend | mesh polygons (shadows, fences): 50% see-through, or the hardware's every-other-pixel checkerboard; F8 toggles |
| `--saturation S` | 1.0 | colour saturation boost in the final pass (1.0 = as the hardware); F7 cycles 1.0 / 1.2 / 1.4 |
| `--gamma G` or `--gamma R,G,B` | 1.0 | colour gamma, as the original's GammaR/G/B (applies to tiles and 3D); above 1 brightens mid-tones |
| `--shifter sequential\|hpattern` | hpattern | keyboard gears: one key per gear, or A/S step down/up (the original's UpDownGears) |
| `--hold-gears` | off | H-pattern: drop to neutral when no gear key is held (the original's HoldGears) |
| `--coop` | off | Daytona USA: two linked machines in one window, one per player (see "Linked play") |
| `--split side\|stack` | side | `--coop` layout: side by side, or one above the other (each player then gets a wide view that fills their half) |
| `--pipeline on\|off` | on | on: the machine runs on its own thread, one frame ahead of the drawing (much faster on multi-core CPUs, one frame more input latency); off: emulate and draw each frame in turn |
| `--frame-skip auto\|off` | auto | auto: when running late, skip drawing a frame (at most two in a row), only while the machine itself keeps up, so the game, its sound and the controls keep their speed; never on a fast enough computer |

`-h` / `--help` (or `--no-gui` without a game) prints a short usage summary
and the game list. Unknown options stop the program with a message.

## Frontend keys

| Key | Action |
|-----|--------|
| Esc | back to the launcher, or quit with `--no-gui` (saves backup RAM / EEPROM either way) |
| F3 | reset the machine (save data kept) |
| P | pause |
| Tab (hold) | fast forward (sound keeps normal speed; some sound commands may be dropped) |
| F6 | texture filter: nearest / bilinear / trilinear |
| F7 | saturation 1.0 / 1.2 / 1.4 |
| F8 | mesh: blend / checker |
| F9 | widescreen on/off |
| F10 | render scale: auto, 1, 2, 3, 4 |
| F11 | fullscreen on/off |

The window title shows the frame rate, the gear (driving games), and
whether widescreen is on.

## Game controls — keyboard

The keys are the original emulator's defaults, taken from its per-game input
definitions.

| Key | Daytona USA | Virtua Fighter 2 (player 1) | Sega Rally |
|-----|-------------|------------------------------|------------|
| 5 / 6 | coin 1 / coin 2 | coin (6: player 2 coin) | coin 1 / coin 2 |
| 1 | start | start | start |
| 2 | — | player 2 start | — |
| arrows | steer (left/right), accelerate (up), brake (down) | move | steer, accelerate, brake |
| Z X C V | view buttons VR 1–4 | buttons 1–4 (guard, punch, kick, –) | Z: view (VR) |
| A S D F / G | gears 1–4 / neutral (sequential: A down, S up) | — | gears 1–4 / neutral (sequential: A down, S up) |
| F1 | service | service | service |
| F2 | test mode | test mode | test mode |

Keyboard steering and pedals ramp in over a few frames, as in the original
(steering: 8 units a frame, full lock in about 16 frames; pedals: full in
about half a second). VF2's player 2 only has pad controls (second pad).

In Daytona's test menu: VR 1 (Z) moves the cursor down, VR 4 (V) up, and Start
(1) selects.

## Game controls — pads

SDL game controllers, up to two, plugged in before or during play (the
program prints each pad's name). Pads SDL doesn't know can be added with
`gamecontrollerdb.txt` (SDL_GameControllerDB format) next to the `m2emu`
binary.

| Pad | Input |
|-----|-------|
| d-pad / left stick | directions |
| A B X Y | buttons 1–4 (VF2: guard, punch, kick) |
| Start / Back | start / coin (second pad: player 2) |
| Y B X A | Daytona view buttons VR 1–4 (Sega Rally: Y = view) |
| left stick X | steering |
| right / left trigger | accelerator / brake |
| RB / LB | gear up / down |

The stick has an 8% dead zone and the triggers 5%.

## Gears (Daytona, Sega Rally)

The cabinets have a 4-speed H-pattern shifter. The emulator keeps one gear
(neutral, 1–4; it starts in 1st), which the game only uses with the manual
transmission:

- **H-pattern keys** (default): A S D F select gears 1–4, G neutral. With
  `--hold-gears` the gear returns to neutral when the key is released.
- **Sequential** (`--shifter sequential`): A shifts down, S shifts up.
- **Pad**: RB up, LB down, in either mode.

## Widescreen

`--widescreen 16:9` (or F9) makes the frame wider. For full-screen 3D views
the game renders the extra area on both sides (more field of view, not
stretched); the 2D text and HUD stay 4:3 in the middle. Per-game rules,
ported from the original's Lua scripts, decide when it applies:

- **Daytona**: in races and attract mode, with the sky layer stretched to
  the full width; course/car select and other menus stay 4:3 in the middle.
- **Sega Rally**: everywhere except one screen the original script excludes.
- **VF2**: always, with the stage sky stretched (the original had no
  widescreen for VF2; this rule is new).

## Linked play (`--coop`, Daytona USA)

    build/m2emu --no-gui --coop daytona

In the launcher: select Daytona USA and tick "Two players, linked". Two complete Daytona boards run in one window, linked through their network
boards like two cabinets on the arcade's link: each player drives their own
car and sees the other one on the track. Player 1 (left or top, red car 1)
uses the keyboard and pad 1; player 2 (right or bottom, blue car 2) uses
pad 2. Each side takes its own coin and start. You hear player 1's machine
only (player 2's runs silently).

Layouts, on a 16:9 screen:

| Command | Each player sees |
|---------|------------------|
| `--coop` | the normal 4:3 picture in their half, with bands above and below |
| `--coop --aspect stretch` | the picture squeezed to fill their half (everything a third narrower) |
| `--coop --aspect crop` | their half filled at the right proportions: the sides of the 3D view are cut, the HUD is squeezed in whole; menus are squeezed |
| `--coop --split stack` | one above the other, each a wide view (extra field of view to the sides, as `--widescreen`) filling their half; menus stay 4:3 in the middle |

Without `--size`, the window starts twice as wide as usual (16:9 with
`--split stack`). F3 resets both machines; the
other keys work as in single play. Each machine keeps its own save data
(`daytona-coop1.DAT`, `daytona-coop2.DAT`), separate from single play's;
link mode and car number are set on every start. About twice the CPU of
single play (one thread per machine). Only in-process linking so far: no
LAN play between PCs yet.

## Save data

Backup RAM and EEPROM (settings, high scores, bookkeeping) are saved to
`<NVDATA dir>/<game>.DAT` on exit and loaded at start. It's the same format
as the Windows emulator's `NVDATA\<game>.DAT`, so its files can be copied
over. With no file, Daytona starts with its link setting on "single"
(otherwise it would wait forever for a network board).

## Environment variables

| Variable | Program | Effect |
|----------|---------|--------|
| `M2_ROMS` | m2emu | default ROM directory (`-r` overrides) |
| `M2EMU_SHOT=file.ppm:N` | m2emu | save the window's picture at frame N, then quit |
| `M2EMU_KEYS=frame:key:frames,...` | m2emu | press keys by script (`key` = DirectInput code in hex, e.g. `06` coin, `02` start, `c8` up) |
| `M2EMU_KEYS2=...` | m2emu | the same for player 2 with `--coop` |
| `M2EMU_BENCH=N` | m2emu | run N frames unthrottled and print a timing split |
| `M2EMU_BENCH_FROM=F` | m2emu | with `M2EMU_BENCH`: time only frames F to N (e.g. a race, after the attract mode) |
| `M2EMU_ACTIONS=iter:a,...` | m2emu | frontend actions by main-loop iteration: `p` pause/unpause, `r` reset (F3), `q` quit |
| `M2EMU_GUI_SHOT=file.ppm:N` | m2emu | save the launcher's picture after N frames, then quit |
| `M2EMU_GUI_TAB=games\|config` | m2emu | open the launcher on that tab |
| `M2EMU_GUI_PLAY=1` | m2emu | press Play once, for the selected (last) game |
| `M2EMU_GUI_BROWSE=1` | m2emu | open the launcher's folder browser |

The M2EMU_SHOT, KEYS, BENCH and ACTIONS variables work in games started
from the launcher too (the game then returns to it).

## Performance and threads

`m2emu` spreads the work over four threads, each frame passing through
three of them:

| Thread | Work |
|--------|------|
| board | the i960 and the TGP: frame N + 2 |
| geometrizer | the 3D geometrizer and its vertex arrays: frame N + 1 |
| main | window, input; draws frame N, then makes frame N + 1's tile layers and sends its changes to the GPU |
| audio (SDL's) | the sound board: 68000 and sound chips, at the audio device's pace |

`m2/m2pipe.c` hands frames from the board to the others; the geometrizer
keeps its last frame's output while it makes the next (`m2geo_frame`).

With `--coop` each board has its own board and geometrizer threads; the two boards wait for each
other between frames, when the network link passes data from one board to
the other. The audio thread plays player 1's sound board only.

- `--pipeline on` (default): the board runs ahead of the drawing, so a
  frame costs the slowest thread instead of their sum, for about two frames
  (35 ms) more input latency. `--pipeline off` runs the board on the main
  thread, between drawing frames (the geometrizer still overlaps the
  drawing, one frame behind).
- The game's sound commands reach the sound board through a lock-free
  queue. When the emulation can't keep full speed, music and effects keep
  their tempo.
- Frame skip (`--frame-skip auto`, default): when a frame would be drawn
  late while the machine keeps up, it isn't drawn, nor its tile layers made
  or uploaded (what changed carries over to the next frame); the board and
  the geometrizer still run every frame. The fps shown then adds how many
  were drawn.
- Other savings, all with the same output: tile layers kept as palette
  indices (colours looked up on the GPU), and of those only the rows that
  changed are uploaded; the tile layer textures and the polygon buffers in
  rings of three, so the GPU driver never has to copy one still in use (the
  Mali-400's does, on the CPU); a radix sort and fast paths in the
  geometrizer; 32-byte indexed vertices; the hand-over copies only the
  memory pages written in a frame.

To check that a change leaves the output alone, compare two builds:

| What | How |
|------|-----|
| 3D and tile output | `M2_GEOHASH=1 build/m2run <game> 3000 <romdir>`: the checksums must match (also with `M2_PIPE=1`, which goes through the hand-over, and `M2_RESET_AT`) |
| sound | `M2_WAV=out.wav build/m2run ...`: the files must be byte-identical |
| the picture | `M2EMU_SHOT=file.ppm:frame build/m2emu --no-gui ...` with a fresh `-n` directory: `cmp` the files |
| speed | `M2EMU_BENCH=N` (frontend) and `M2_BENCH=1` (m2run) print a per-part timing split |

## Headless tool: m2run

    build/m2run <game> <frames> <romdir>

Runs the machine without a window and prints board state every 60 frames.
Controlled by environment variables:

| Variable | Effect |
|----------|--------|
| `M2_NVRAM=file` | load/save backup RAM + EEPROM from `file` |
| `M2_FROM=a M2_TO=b` | print state on every frame from a to b |
| `M2_KEYS=frame:key:frames,...` | scripted key presses (as `M2EMU_KEYS`) |
| `M2_SHOT=file.ppm` | write the composited 2D tile layers at the end |
| `M2_DUMP=prefix`, `M2_NIB=n` | write each raw tile layer to `prefix-layerN.ppm` (`M2_NIB`: pixel order, default 2) |
| `M2_GEO=1` | run the 3D geometrizer and print polygon counts |
| `M2_PIPE=1` | pass the video through the frame hand-over (`m2pipe`) as `m2emu` does; with `M2_GEOHASH` the checksums must match a run without it |
| `M2_RESET_AT=n` | reset the machine after frame n, as F3 |
| `M2_COOP=1` | a second board linked to the first (Daytona), both run in lockstep; prints the link state; with `M2_SHOT` also writes `<file>.2.ppm` |
| `M2_BENCH=1` | time the per-frame CPU work (emulation, tile layers, geometrizer, sound) and print ms per frame at the end |
| `M2_GEOHASH=1` | run the geometrizer and tile layers every frame and print a checksum of their output (polygons in drawing order, both layers, the palette) every 300 frames; for checking that a change leaves the video output identical |
| `M2_WAV=file.wav` | run the sound board and record its output; prints sound CPU state |
| `M2_PROF=n` | from frame n, print the 20 hottest i960 addresses |
| `M2_IOTRACE=n` | from frame n, count I/O board reads |
| `M2_RAM=file` | write main RAM at the end |
| `M2_ROMS=prefix` | write every ROM region to `prefix-regionN.bin` |
| `M2_TGP_RECORD=file:from:to` | record the TGP's state at frame `from` and every access to it until `to`, for `tgp/test/replay.c` |

`tools/i960dis.py` and `tools/m68kdis.c` disassemble the RAM/ROM dumps.

## Where things are

| Directory | Contents |
|-----------|----------|
| `i960/` | main CPU (Intel i960KB), with its bug list `QUIRKS.md` |
| `tgp/` | geometry DSP (Fujitsu MB86234 TGP) |
| `m2/` | board, ROM loader, 2D tile layers, 3D, renderer, input, widescreen rules, frame hand-over between threads, network link between boards; `m2/README.md` and `m2/QUIRKS.md` |
| `snd/` | sound boards (SCSP, MultiPCM, YM3438), `snd/QUIRKS.md` |
| `m68k/`, `ymfm/` | third-party cores: Musashi 68000 (MIT), ymfm (BSD-3) |
| `sdl/` | the desktop frontend: `m2emu.c` (games), `launcher.cpp` (the launcher), `options.c` (command line and `m2emu.ini`), `dirlist.c` (folder listing), `imgui_impl_m2gl.cpp` (Dear ImGui drawn with the port's GL layer), `font_roboto.inc` (Roboto Medium, Apache License 2.0) |
| `imgui/` | Dear ImGui 1.92.9b (MIT, `imgui/LICENSE.txt`): its core and SDL2 backend, for the launcher |

Each `QUIRKS.md` lists the original's bugs, and where and why the port
differs from it.
