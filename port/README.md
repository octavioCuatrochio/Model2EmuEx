# Model 2 port — user guide

A portable C port of ElSemi's Sega Model 2 Emulator 1.1a, run by an SDL2 +
OpenGL ES 2 frontend (`m2emu`). The target games are Daytona USA
(`daytona`), Virtua Fighter 2 (`vf2`) and Sega Rally Championship
(`srallyc`), with video, sound and input. Other games in the list may boot but
haven't been tested.

## Building

Needs a C99 and C++14 compiler, SDL2, OpenGL ES 2 headers/libraries (Mesa's
`libGLESv2`) and zlib.

    cd port
    make            # build/m2emu (the emulator), build/m2run (headless tool)
    make clean

## Running

    build/m2emu [options] <game>

ROMs are MAME-style zip sets (`daytona.zip`, `vf2.zip`, `srallyc.zip`, ...).
Each file is looked up in the game's zip, then its parent's, then
`model2.zip`. Run `build/m2emu` without a game to list the supported names.

Example, 16:9 fullscreen with a pad-friendly shifter:

    build/m2emu -r ~/roms --widescreen 16:9 --fullscreen --shifter sequential daytona

## Options

| Option | Default | Effect |
|--------|---------|--------|
| `-r DIR` | `$M2_ROMS`, else `./roms` | ROM directory |
| `-n DIR` | `$XDG_DATA_HOME/m2emu/NVDATA`, else `~/.local/share/m2emu/NVDATA` | where save data goes (see below) |
| `--size WxH` | 1024x768; with widescreen 1365x768 (16:9) or 1229x768 (16:10) | window size |
| `--fullscreen` | off | start fullscreen (F11 toggles) |
| `--vsync` | off | sync buffer swaps to the display; pacing is by the game's own frame rate either way |
| `--widescreen 16:9\|16:10\|off` | off | wider picture with extra field of view (see below); F9 toggles |
| `--scale N\|auto` | auto | render resolution: N times 496x384 (auto = window height / 384); F10 cycles |
| `--sharp` | smooth | nearest-neighbour instead of linear filtering when scaling to the window |
| `--mesh blend\|checker` | blend | mesh polygons (shadows, fences): 50% see-through, or the hardware's every-other-pixel checkerboard; F8 toggles |
| `--saturation S` | 1.0 | colour saturation boost in the final pass (1.0 = as the hardware); F7 cycles 1.0 / 1.2 / 1.4 |
| `--gamma G` or `--gamma R,G,B` | 1.0 | colour gamma, as the original's GammaR/G/B (applies to tiles and 3D); above 1 brightens mid-tones |
| `--shifter sequential\|hpattern` | hpattern | keyboard gears: one key per gear, or A/S step down/up (the original's UpDownGears) |
| `--hold-gears` | off | H-pattern: drop to neutral when no gear key is held (the original's HoldGears) |

`-h` / `--help` (or no game name) prints a short usage summary and the game list.
Unknown options stop the program with a message.

## Frontend keys

| Key | Action |
|-----|--------|
| Esc | quit (saves backup RAM / EEPROM) |
| F3 | reset the machine (save data kept) |
| P | pause |
| Tab (hold) | fast forward (sound is dropped while the queue is full) |
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
| `M2EMU_BENCH=N` | m2emu | run N frames unthrottled and print a timing split |

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
| `M2_WAV=file.wav` | run the sound board and record its output; prints sound CPU state |
| `M2_PROF=n` | from frame n, print the 20 hottest i960 addresses |
| `M2_IOTRACE=n` | from frame n, count I/O board reads |
| `M2_RAM=file` | write main RAM at the end |
| `M2_ROMS=prefix` | write every ROM region to `prefix-regionN.bin` |

`tools/i960dis.py` and `tools/m68kdis.c` disassemble the RAM/ROM dumps.

## Where things are

| Directory | Contents |
|-----------|----------|
| `i960/` | main CPU (Intel i960KB), with its bug list `QUIRKS.md` |
| `tgp/` | geometry DSP (Fujitsu MB86234 TGP) |
| `m2/` | board, ROM loader, 2D tile layers, 3D, renderer, input, widescreen rules; `m2/README.md` and `m2/QUIRKS.md` |
| `snd/` | sound boards (SCSP, MultiPCM, YM3438), `snd/QUIRKS.md` |
| `m68k/`, `ymfm/` | third-party cores: Musashi 68000 (MIT), ymfm (BSD-3) |
| `sdl/` | the desktop frontend |

Each `QUIRKS.md` lists the original's bugs, and where and why the port
differs from it.
