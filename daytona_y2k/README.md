# Daytona USA: the game program in C

The Model 2 emulator in `../port` runs Daytona USA by interpreting the
i960 machine code in the ROM. This directory turns that machine code into
C source instead: `tools/i960rec.py` translates every function of the
256 KB program ROM into a C function, and the result runs as the CPU of the
same portable board (SDL2 + OpenGL, Linux and Windows).

The generated C is faithful but unreadable — registers, addresses, no
names. It is the starting point, not the goal: functions are rewritten by
hand into ordinary C one at a time (`src/`), and every rewrite is checked
by running the game and comparing its output with the emulator's, frame by
frame (`make check`).

Nothing here needs the emulator's interpreter to be *correct*, but it does
have to behave **exactly** like it, bugs included (`../port/i960/QUIRKS.md`),
because that is what "unchanged output" is measured against.

## Status

| | |
|---|---|
| Functions recompiled | 633 (27,101 instructions, every instruction the traces reach) |
| Left to the interpreter | 2 (`synmov`/`synmovq` in the reset code) |
| Functions rewritten by hand | 51 (`src/`): the frame interrupt and the frame wait, the object list, the switches, coins and bookkeeping, backup RAM, `memset`, all of the drawing below `draw_scene` — the scene walk, the course map and its markers, placing an object, writing a model into the display list — and **every place the display list is written**: the window, the mode, the projection, the light, the matrix, the objects, the level of detail and the end marker, the scrolling sky, and the start-up loads of texture, polygon and logarithm data |
| Names worked out | `symbols.txt` (functions and memory; they appear in the generated C) |
| Verified | exact: every time slice ends on the same instruction as the interpreter's, and the 3D and tile output, the sound and all of main RAM are byte-identical, through a scripted race of 12,000 frames with resets, linked play and the threaded pipeline (`make check-full`) |

## Building and running

    make ROMS=~/Desktop/ROMs        # generate the C from daytona.zip, then build
    build/daytona                   # the launcher, as ../port/build/m2emu
    build/daytona --no-gui -r ~/Desktop/ROMs daytona
    build/daytona-run daytona 600 ~/Desktop/ROMs     # headless

`build/daytona` is the port's own frontend (same options, keys and
launcher, see `../port/README.md`), with the recompiled program in place of
the CPU interpreter. Only Daytona USA runs recompiled, and only when the
ROM matches the one the C was made from; anything else, and
`DAYTONA_INTERP=1`, falls back to the interpreter.

| Variable | Effect |
|----------|--------|
| `DAYTONA_INTERP=1` | run the interpreter instead (for comparisons) |
| `DAYTONA_RT_LOG=1` | report the recompiled program, and every place the interpreter had to take over |

The generated C is derived from your ROM: it is written into `build/gen`
at build time and is not part of the repository.

## How it works

The recompiled program is not a separate emulator: it works on the
interpreter's own machine state (`i960_state` — registers, frame cache,
condition codes, page tables), so the board's interrupts, save states and
the interpreter itself all still apply to it.

- **One C function per i960 function.** Entry points come from the reset
  vector, the interrupt table and the call/`bal` targets found by
  following the code; a second pass cuts each function at the others'
  entries, so shared code isn't duplicated. A branch into another
  function's entry is a tail jump: the function returns it as a
  continuation and a trampoline runs it, so guest loops that jump between
  two functions don't grow the C stack.
- **Same instruction, same cycle.** Every instruction charges the cycle
  cost the interpreter charges, so a time slice ends on the same
  instruction as it would there, and the board's timers and interrupts land
  between the same two instructions.
- **Slices without unwinding.** The program runs on its own stack
  (`rt/coro.c`: ucontext, or fibers on Windows). When a slice's cycles run
  out, the code yields back to the board in the middle of whatever it was
  doing; when the board re-enters it, an interrupt that the board raised is
  run as a call of the handler function, and returns where the guest was.
- **Nothing has to be understood to be correct.** When the code reaches
  something the recompiler couldn't follow (an untraced jump-table target,
  a call to code it never saw), it hands the guest to the interpreter until
  the function returns, and the interpreter hands it back at the next
  recompiled function entry. `DAYTONA_RT_LOG=1` reports where that happens.

Instructions the recompiler doesn't translate itself (`synmov`, `synmovq`,
invalid encodings, the odd operand forms in `../port/i960/QUIRKS.md`) are
run by the interpreter one at a time, in place.

### Coverage: traces

Static analysis can't know where an indirect jump or call goes. The tracer
records it from a real run:

    make trace
    build/trace ~/Desktop/ROMs 12000 traces/race.txt "1300:06:5,1400:02:5,..."

`traces/` holds three runs so far: a race, a race with the manual gearbox on
another course, and the test-mode menus. (Keys are `frame:code:frames`,
DirectInput codes as in `../port/README.md`;
`TRACE_SHOTS=prefix` writes a picture every 600 frames to check that the
run does what you think.) `traces/*.txt` hold executed addresses, call,
`bal`, `bx` and interrupt edges — addresses only, no ROM content. Add a
trace whenever a new part of the game is reached (a menu, test mode,
another course) and rebuild: more code becomes real C instead of falling
back to the interpreter.

## The 3D

`notes/geometry.md` describes how the game drives the geometry processor
(the TGP): the command protocol, all 38 commands it uses, and what each one
does. In Daytona the TGP is a maths coprocessor — transforms, trigonometry,
collision tests — and the i960 writes the display list itself, so the
drawing code is i960 code, and `tools/i960rec.py` annotates every access to
the processor and the display list with what it means:

    /* 00013274  st g14,0x120(g11) TGP translate */
    /* 00017b1c  st r3,0x90(g10) display list, exponent 9 */

`tools/tgpmodel.py` models the commands and checks the model against a
logged command stream (365,677 commands of a race: every result matches).
`tools/dlmap.py` draws what the game puts in the list and which C writes
each opcode; every writer is now hand-written C.

`tools/lift.py <romdir> <addr>` prints a first draft of a function in the
style of `src/`, with the cursor markers, memory and the geometry macros
already in place — correct but plain, to be renamed and reshaped by hand.

## Rewriting a function by hand

1. Find the function in `build/gen` (`f_<address>`), and read what it does.
   `../tools/i960dis.py` disassembles, and the generated C keeps the
   original instruction next to every line.
2. Write it in `src/` as `RT_FN(f_<address>) { ... }` — same name, same
   address. The generator finds it there and stops generating its body, so
   yours is the one that runs.
3. A hand-written function has to leave the same guest state behind as the
   machine code did: the global registers its callers use (i960: `g0`-`g7`
   carry arguments and the result), the condition code, and every access
   to guest memory, in the same order. Use `rd32`/`wr32` (`rt/rt.h`) for
   guest memory until the data it touches is understood well enough to
   move out of it.
4. It also has to spend the cycles the machine code spent, instruction by
   instruction, because the board ends its time slices and lets interrupts
   in between two particular instructions. `src/decomp.h` does the
   counting from a cursor into the machine code: after the C that does
   what the instructions up to address `b` did, `UPTO(b)` pays for them one
   by one (from the recompiler's cost table), `BRANCH(b, t)` when the last
   is a taken branch, and `CALL`, `BAL`, `CALLX`, `RET()`, `RET_BX()` and
   `TAIL()` do the transfers. Pay for everything before a memory access
   before making it, so the frame interrupt, which can arrive in the middle
   of a function, sees memory as it would have. Registers the machine code
   would hold at such a point go in `s->r[]`/`G()` as it goes (the local
   ones, `r3`-`r15`, only matter there and to a call from the function).
5. `make check` — the game must still produce exactly the same output.

`src/text.c` is the example of a loop that can outlast a time slice;
`src/sound.c` of code the frame interrupt can arrive in the middle of.

## Checking

    make check        # about half a minute: 3,000 frames, three modes
    make check-full   # 12,000 frames, and the two linked boards

Both run the same scripted race (coin, the two menus, then driving) twice,
once interpreted and once recompiled, and compare the 3D and tile output
checksums, the sound as a WAV file and main RAM. Anything that differs is a
bug in the recompiled code or in a hand-written function. The interpreted
side is kept in `build/ref` and only re-run when the emulator, the
recompiler or the ROM changes, so a check after a change costs one run.

`M2_RESET_AT`, `M2_COOP=1` and `M2_PIPE=1` cover resets, linked play and
the threaded pipeline.

### When the two disagree

The tools for finding out *where*, roughly in the order they are useful:

| | |
|---|---|
| `M2_RAMHASH=1 build/daytona-run ...` | a checksum of main RAM every frame: which frame first differs |
| `M2_SLICES=1` (or `=2`) | where the CPU stands at the end of each of a frame's 210 time slices, as a checksum (or every slice, with the cycles left and the board's idle count): whether the two run the same code at the same time |
| `TRACE_CYCLES=-1 build/trace ...` | the interpreter's side: every instruction with the cycles it was charged |
| `RT_LOG_CYCLES` (`make BUILD=build-log CFLAGS="-O1 -g -DRT_LOG_CYCLES"`) | the recompiled side: the same, plus slice ends and each `rec_execute` |
| `TRACE_WATCH=<addr>` | every change of one byte of guest memory, with the instruction that did it |
| `TRACE_TGP=1` (`TRACE_TGP_RAW=file`) | the geometry processor's command stream: each command with its parameter count and where it is sent from (see `notes/geometry.md`) |

Comparing the two instruction streams is how the one difference this
project has had was found: hand-written functions used to pay their cycles
in lumps, so when a time slice ended inside a lump it ended a few cycles
late, every later slice moved, and in a long run a sound command reached
the sound board one frame apart. Paying per instruction (`UPTO`) fixed it.
Keep in mind that each side logs from different places: the interpreter
once per instruction, the recompiled code once per charge.

## Files

| Path | What it is |
|------|------------|
| `tools/i960dec.py` | i960 decoder, following the interpreter's own decoding |
| `tools/i960rec.py` | the recompiler: analysis and C emission |
| `tools/trace.c` | coverage tracer (the interpreter built with `-DI960_TRACE`) |
| `rt/rt.h`, `rt/rt.c` | runtime: instruction helpers, slices, dispatch, interpreter fallback, the board's CPU backend |
| `rt/coro.c` | the program's own stack |
| `src/decomp.h` | what a hand-written function has to keep: registers, memory, cycles |
| `src/` | functions rewritten by hand (they replace the generated ones) |
| `tools/map.py` | the program's functions, sizes, callers and callees (`--dis ADDR` disassembles one) |
| `tools/lift.py` | a first draft of one function for `src/` |
| `tools/dlmap.py`, `tools/callgraph.py` | pictures: the display list's opcodes and their writers, the call tree below a function |
| `tools/check.sh` | `make check`: the whole game, recompiled against interpreted |
| `traces/` | recorded runs: addresses, call and jump edges |
| `build/gen/` | the generated C (made from your ROM; not in the repository) |

The port needed three small additions for this (`../port`): the CPU
interpreter exports a single-step and the call/return/region helpers
(`i960.h`), the board can be given another CPU backend (`m2board.h`), and
the interpreter can be built with a per-instruction trace callback. With no
backend attached, the port behaves exactly as before.
