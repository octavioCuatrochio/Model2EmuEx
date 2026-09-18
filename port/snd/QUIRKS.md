# Sound boards: what the port does, and how it differs from the original

Source: Sega Model 2 Emulator 1.1a, `EMULATOR.EXE`:
- sound CPU memory maps at 0x4ccbe0 (both boards)
- UART at 0x4c6a40/0x4c82d0
- MultiPCM at 0x4b01b0–0x4b0cd0, YM2612/3438 at 0x4b0b30–0x4b2580
- SCSP at 0x4ab000–0x4b0000, with its DSP recompiler at 0x4aa040

The original's 68000 core is A68K, x86 assembly (its init at 0x462ae0
unpacks a compressed opcode table). Machine-generated assembly can't be
ported to ARM from a decompile, so the port uses **Musashi** (MIT). The chips
are C ports of MAME's code, which ElSemi (the original's author)
co-wrote. The board wiring follows the original.

| Part | Port | Notes |
|------|------|-------|
| 68000 | Musashi 4.60, `m68kconf.h` with `M68K_EMULATE_INT_ACK` on | Interrupt lines are levels, held until the device drops them. Musashi's default clears the level when an interrupt is taken, which starves the UART: Daytona's driver reads one byte per IRQ 2. |
| MultiPCM | `multipcm.c`, from MAME gew.cpp + multipcm.cpp | Runs at 44.1 kHz, not the chip's 44.64 kHz: the envelope tables already assume 44.1 kHz, and pitch uses the output rate. Writes to the unused slot numbers (7, 15, 23, 31) are dropped; MAME would index slot −1. |
| YM3438 | `ym3438.cpp` via ymfm (BSD-3) | The original has its own YM2612 core. Daytona's driver takes its music tempo from the timer overflow flags, so without the timers it never keys a note on. Rendered at 55.5 kHz and resampled to 44.1 kHz; mixed at MAME's 0.30. |
| SCSP + DSP | `scsp.c`, from MAME scsp.cpp/scspdsp.cpp | Timers count in output samples (ElSemi's earlier SCSP timer code) instead of MAME's scheduler. MIDI in is a byte FIFO; MIDI out is dropped (the i960 never reads it, as in the original). EXTS inputs are silent. As in MAME, a register write reads the register first, so writing the MIDI-in register pops a byte. |

## Board wiring

| # | Where | Original | Port |
|---|-------|----------|------|
| 1 | Model 2 UART 0xc20001 | Pops the i960's command FIFO; clears the 68000's pending interrupt when the FIFO empties. Status reads (0xc20003) return 0. | Same, with IRQ 2 asserted while the FIFO holds data. The driver's handler treats a 0xff byte as "reset": it reloads its stack and restarts at 0x300. |
| 2 | Model 2 MultiPCM banks 0xc50001/0xc70001 | `value & 7` | `value & 3`, as MAME; bank n maps sample ROM n MB to chip addresses 0x100000–0x1fffff. Each chip sees its own 4 MB of region 6. |
| 3 | command FIFO full | The i960 write handler overwrites unread entries. | A full FIFO drops the new byte. |
| 4 | Model 2A 0x400000 (sample banks) | Ignored. | Ignored: the samples are mapped linearly at 0x800000, which equals MAME's default banks. |
| 5 | ROM byte order | Regions 5 and 6 are stored as A68K wants them, each 16-bit word little-endian (checked: Daytona's vectors only make sense that way). | The 68000 reads ROM through that swap. RAM is kept big-endian, which is what the SCSP expects. MultiPCM reads region 6 byte for byte. |
| 6 | timing | Sound runs in its own thread, synced to the frame. | Rendered per frame after the i960 frame, in slices of 16 samples: the 68000 runs its share of cycles, then the chips render. The i960's commands therefore reach the sound CPU up to a frame later. |

Sega Rally's long warning screen is not caused by sound: it lasts just as long with the sound board running.
