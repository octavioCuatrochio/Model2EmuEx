# i960 CPU core: portable C port

This is the Intel i960KB main-CPU core from ElSemi's Sega Model 2 Emulator
1.1a, recovered from `EMULATOR.EXE` and rewritten as plain C99. It doesn't
depend on Windows, x86, the x87 FPU or pointer width. It builds without
warnings with gcc, clang and `arm-none-linux-gnueabihf-gcc -mcpu=cortex-a7`,
so it's ready for an Android NDK build (armeabi-v7a or arm64).

It is only the CPU. The Model 2 board still has to be built around it:
- memory map
- TGP/SHARC geometry DSPs
- sound 68000 and SCSP
- the renderer

## Files

| File | What it is |
|------|------------|
| `i960.h`, `i960.c` | the core |
| `i960_optable.inc` | the 153-opcode table (cycle costs, formats, operand masks), generated from the binary by `gen_optable.py` |
| `QUIRKS.md` | every bug and oddity found in the original, and what the port does about each |
| `test/difftest.py`, `test/shim.c` | differential test against the original machine code (Unicorn) |
| `test/bench.c` | throughput benchmark |

## Using it

```c
/* addresses below are placeholders, not the real Model 2 memory map */
i960_state *s = malloc(sizeof *s);   /* 2 MB on 32-bit ARM, 4 MB on 64-bit (page tables) */
i960_init(s, board);                         /* board = your context pointer */
i960_map_read (s, 0x000000, 0x1fffff, rom, NULL, NULL, NULL);        /* plain memory */
i960_map_read (s, 0x500000, 0x51ffff, ram, NULL, NULL, NULL);
i960_map_write(s, 0x500000, 0x51ffff, ram, NULL, NULL, NULL);
i960_map_read (s, 0x01800000, 0x0180ffff, NULL, io_r8, io_r16, io_r32); /* callbacks */
i960_set_code_region(s, 0, 0x000000, 0x200000);  /* code must be host-mapped */
i960_set_code_region(s, 1, 0x500000, 0x20000);
i960_set_code_region(s, 2, 0xfff00000, 0);     /* unused: size 0 sets the base only */
i960_reset(s);                               /* reads the boot record at 0 */

for (;;) {
    i960_execute(s, cycles_per_slice);
    if (vblank) i960_interrupt(s, 0);        /* pin 0-3, vector from ICON */
}
```

Callbacks get `s->user` as their first argument. Writes to 0xff000010–0xff00001f
should be passed to `i960_iac_write`. That's how the original lets the CPU
reinitialize itself through `synmovq`.

## Build switches

| Switch | Effect |
|--------|--------|
| (none) | behaves like the original emulator, bugs included |
| `-DI960_ACCURATE=1` | fixes the guest-visible bugs marked "fixed" in QUIRKS.md |
| `-DI960_FP_SINGLE=1` | rounds basic FP results to single precision, as the original's x87 usually did |

## Tests

```sh
# differential test (needs `pip install unicorn` and the original EMULATOR.EXE)
gcc -std=c99 -O1 -fPIC -shared -o libi960test.so i960.c test/shim.c -lm
python3 test/difftest.py /path/to/EMULATOR.EXE ./libi960test.so 8000 1

# benchmark, native or on the TV box
arm-none-linux-gnueabihf-gcc -std=c99 -D_POSIX_C_SOURCE=199309L -O2 \
    -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -static \
    -o bench-armv7 test/bench.c i960.c -lm
adb push bench-armv7 /data/local/tmp/ && adb shell /data/local/tmp/bench-armv7
```

Results:
- **Differential test:** 47,159 random cases over 6 seeds, covering all 153
  opcodes and interrupt entry, with 0 mismatches against the original.
- **Benchmark:** about 160 M instructions/s (405 M i960 cycles/s) on this PC.
  The real i960 in a Model 2 runs at 25 MHz.
