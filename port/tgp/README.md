# TGP (Fujitsu MB86234) geometry DSP: portable C port

This is the Model 2 / Model 2A geometry coprocessor from ElSemi's Sega Model 2
Emulator 1.1a, recovered from `EMULATOR.EXE` and rewritten as C99. It covers:
- the DSP interpreter
- the "table unit" (sin/cos, atan, reciprocal and inverse-square-root lookups
  from the mainboard's table ROM)
- the i960-facing driver: command/data FIFO ports, copro RAM port, program and
  data upload

It builds without warnings with gcc, clang and the 32-bit ARM cross-compiler.
The Model 2B SHARC and Model 2C MB86235 geometry boards are separate cores and
aren't ported yet.

## Files

| File | What it is |
|------|------------|
| `tgp.h`, `tgp.c` | the core and the Model 2/2A driver |
| `QUIRKS.md` | bugs and oddities of the original, and what the port does about each |
| `test/difftest.py` | interpreter vs. original machine code (Unicorn) |
| `test/boardtest.py` | driver functions vs. original handlers |
| `test/bench.c` | throughput benchmark |

## Using it

```c
tgp *t = malloc(sizeof *t);
tgp_init(t);
t->prog = prog_ram;       t->prog_words = PROG_WORDS;       /* the i960 uploads into it */
t->table_rom = tables;    t->table_rom_size = 0x40000;      /* game's copro table ROM */
t->ext_ram = copro_ram;   t->ext_ram_words = 0x20000;       /* shared RAM, i960 port 0x80xxxx */
t->data_buf = data_ram;   t->data_buf_words = DATA_WORDS;
tgp_m2_board_reset(t);

/* i960 memory map glue (addresses as in the original driver): */
/* 0x880000-0x887fff write -> tgp_m2_port_write32/16, read -> tgp_m2_port_read32/16/8 */
/* 0x800000-0x80ffff write -> tgp_m2_ctrl_write32/16, read -> tgp_m2_ctrl_read32 */
/* 0x980000-0x98000c write -> tgp_m2_upload_write */
```

Like the original, the TGP runs synchronously. Every FIFO write or read from
the i960 runs it until it stalls on an empty input FIFO or a full output FIFO.
The embedder never schedules it.

## Tests

```sh
# differential tests (need `pip install unicorn` and the original EMULATOR.EXE)
gcc -std=c99 -O1 -fPIC -shared -DTGP_TEST_UNICORN_FLD -o libtgptest.so tgp.c test/shim.c -lm
python3 test/difftest.py  /path/to/EMULATOR.EXE ./libtgptest.so 2500 1
python3 test/boardtest.py /path/to/EMULATOR.EXE ./libtgptest.so 300 1

# benchmark, native or on the TV box
arm-none-linux-gnueabihf-gcc -std=c99 -D_POSIX_C_SOURCE=199309L -O2 \
    -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -static \
    -o bench-armv7 test/bench.c tgp.c -lm
adb push bench-armv7 /data/local/tmp/ && adb shell /data/local/tmp/bench-armv7
```

Results:
- **Interpreter:** 18,161 random cases (8 seeds) with 0 mismatches.
- **Driver:** 23,104 driver calls with 0 mismatches.
- **Speed:** about 98 M TGP instructions/s on this PC.
