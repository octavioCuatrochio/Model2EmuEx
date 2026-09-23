# Sega Model 2 Emulator 1.1a (ElSemi) - Ghidra decompilation

A decompilation of ElSemi's Model 2 Emulator and a portable C port of it
(`port/`), aimed at Linux and Windows PCs and at an Android TV box with an
Allwinner H3 (32-bit ARM Cortex-A7, Mali-400 GPU).

## Branches
- `master`: the decompilation and the portable port, everything on one thread.
- `optimized`: the same port, faster (sound, emulation and drawing on separate threads, less work per frame, same pictures and sound), and building for Windows and desktop OpenGL too.
- `coop`: `optimized` plus two-player linked play of Daytona USA in one window (`--coop`, split screen), and a launcher window (Games and Configuration tabs).

## Games known to work
- Daytona USA (`daytona`): played through races, with sound, controls and gears.
- Virtua Fighter 2 (`vf2`) and Sega Rally Championship (`srallyc`): boot and run their attract mode with 2D, 3D and sound; gameplay not tested in depth.
- The other games in the list may boot but haven't been tried.

## Platforms
- Linux x86 (SDL2; desktop OpenGL, or OpenGL ES 2 with `make GL=gles`): builds and runs; everything above was tested here.
- Windows x86-64 (MinGW-w64: MSYS2 or cross-compiled; desktop OpenGL): builds; tested under Wine, where the emulation, sound and pictures match Linux. Not yet tried on a real Windows PC.
- Android on the Allwinner H3 (TV box, Mali-400): the app (`make apk`, see `port/README.md`) runs Daytona USA with 3D, sound and widescreen, drawn at 683x384 and scaled to 1080p by the display. Full speed (57.5 fps) in the menus and most of a race; heavy scenes still drop to 30-40 fps (the i960 and TGP interpreters), and the box's thermal limit (85 °C) takes a CPU core away after a while.

## Run example
    cd port
    make
    build/m2emu

opens the launcher: choose the ROM folder in the Games tab, set the options
in the Configuration tab, and play.

Straight from the command line, Daytona USA in a 1920x1080 window with the
colour saturation at 1.5, ROMs (MAME-style `daytona.zip`) in `~/Desktop/ROMs`:

    build/m2emu --no-gui -r ~/Desktop/ROMs --size 1920x1080 --saturation 1.5 daytona

Add `--fullscreen` to use the whole screen at its own resolution (1080p on
a 1080p display). On Windows the program is `build\m2emu.exe` (building:
`port/README.md`, "Windows"). Two players linked: add `--coop`. All options
and controls: `port/README.md`.

Input: `../M2emulator_1.1a/EMULATOR.EXE` (PE32, MSVC, not packed, no PDB).
`emulator_multicpu.exe` differs by 6 bytes (PE checksum + one constant), so it is not decompiled separately.

Tool: Ghidra 12.1.3 (`~/.local/opt/ghidra_12.1.3_PUBLIC`), headless auto-analysis + a C export script.

## Files
- `ghidra_project/` - the analysed Ghidra project. Open with `~/.local/opt/ghidra_12.1.3_PUBLIC/ghidraRun`
  and rename/retype things there; re-export when done.
- `src/all_functions.c` - decompiled C for all 2041 functions (0 decompile failures).
- `src/prototypes.h`, `src/types.h` - recovered signatures / data types.
- `src/functions.tsv` - address, name, size of every function.
- `src/strings.tsv` - defined strings with addresses (best starting point for naming functions).

## Map of the parts that were ported (by address)
- 0x440000-0x4a0000  A68K 68000 core (x86 assembly; replaced by Musashi in the port)
- 0x4a81c0           TGP (MB86234) interpreter; 0x4aa040 SCSP DSP recompiler
- 0x4ab000-0x4b3000  SCSP, MultiPCM, YM2612/YM3438
- 0x4b6000-0x4bbdd0  3D renderer: display-list parsers 0x4b9f70 / 0x4baa60, object processor 0x4bb400
- 0x4bbdd0-0x4c0000  SHARC (Model 2B), not ported
- 0x4c5290-0x4cf960  Model 2 board: copro drivers, I/O handlers, frame loop 0x4cc7a0, init 0x4cd450,
                     per-game input routines (0x4c9080 Daytona, 0x4cb870 Sega Rally, ...)
- 0x451a90-0x453400  tilemap line composer and tile decoder
- 0x4ed5e0-0x4ee200  tilemap layer drawing
- The i960 core's opcode handlers are only reachable through tables (see `i960/`, `tools/I960Label.java`).
- Statically linked: MSVC runtime + STL, Lua 5.1.4, zlib 1.2.2, minizip 0.22.

## The port
`port/` is a portable C rewrite that runs Daytona USA, Virtua Fighter 2 and
Sega Rally with video, sound and input (SDL2 + OpenGL, or OpenGL ES 2 with
`make GL=gles`), on Linux and Windows. The machine, the drawing and the
sound run on separate threads. `--coop` runs two linked Daytona USA
machines in one window for two players (split screen). **Start with
`port/README.md`**: build, options, controls, save data and debugging.
Each subsystem documents the original's bugs and the port's differences in
its own `QUIRKS.md` (`port/i960`, `port/tgp`, `port/m2`, `port/snd`).

## Tools
- `tools/ExportDecomp.java`: whole-program decompile export (headless Ghidra).
- `tools/RegionExport.java`: create functions in an address range (pointer-table
  code) and export their decompiles: `analyzeHeadless ... -postScript RegionExport.java out.c start end [extra entries]`.
- `tools/I960Label.java`, `tools/i960tab.py`, `tools/mnem.py`: i960 opcode table recovery.
- `tools/dump_games.py` + `tools/gen_gamedb.py`: dump the game definitions (ROM lists,
  inputs, hooks) by running the EXE's static initialisers under Unicorn, then
  generate `port/m2/gamedb.inc`.
- `tools/i960dis.py`, `tools/m68kdis.c`: disassemblers for i960 and 68000 dumps.
- `tools/ptrtables.py`: pointer-table finder.

## Status of the raw decompile
Raw decompiler output, **not compilable**: `gcc -m32 -fsyntax-only` reports ~16k errors
(undeclared globals, unknown prototypes, MSVC ABI artefacts like `in_ECX` thiscall params).
Platform layer is Direct3D 9 + D3DX9 (HLSL pixel shaders), DirectInput 8, XInput, Winsock, Win32 GUI.
