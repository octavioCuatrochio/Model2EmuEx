# TGP (MB86234) core: bugs and quirks in the original, and what the port does

Source: Sega Model 2 Emulator 1.1a by ElSemi, `EMULATOR.EXE`:
- interpreter at 0x4a81c0, with helpers at 0x4a7440–0x4a8140
- reset at 0x4a9a80
- Model 2/2A copro driver at 0x4c5290–0x4c5850 and 0x4c79c0

ElSemi reverse-engineered this DSP's instruction set without documentation
(README.TXT, v0.4). Nothing here has been checked against real hardware. "Bug"
means the original's code is internally inconsistent or unchecked; it does not
mean I know what the real chip does.

**Rule for the port.** Anything a game program can observe is reproduced
exactly, including out-of-range accesses that land on neighbouring state (the
port keeps the original's memory layout for that). Only accesses that would
leave that state block are cut off.

Each entry says how sure I am:
- **confirmed**: read from the machine code and checked by the differential test.
- **unverified**: odd, but it might be what the hardware does.

## 1. Bugs and oddities that games can see (reproduced)

| # | Where | What the original does | Sure? |
|---|-------|------------------------|-------|
| 1 | no bounds checks anywhere | RAM addresses come from 32-bit registers and are never masked. The call stack has 4 entries and an 8-bit stack pointer that is never checked. The per-instruction flag table is indexed by a 16-bit PC but only has 0x1000 entries. So overflows land on neighbouring state: with SP = 4 a call overwrites the stack pointer itself, then the flags, bank and shift registers, then RAM; at SP = 0x2b it overwrites the emulator's own program pointer (see section 3). | confirmed |
| 2 | branch-if-false, sub-op 6 (0x4a9852) | The jump through a RAM word doesn't push a return address and ignores the register-indirect bit (0x4000). The branch-if-true form does both. | confirmed |
| 3 | unknown branch conditions (0x4a7520) | Only conditions 0, 1, 2, 0xa, 0x10, 0x11 and 0x16 exist; every other condition is "false". So the branch-if-false opcode (0x3f) with an unknown condition always branches. | confirmed |
| 4 | register-read helper 0x4a77f0 compared with inline addressing | Used by register moves, branch targets and repeat counts. Mode 2 uses a 5-bit displacement where inline addressing uses 6 bits. Mode 6 doesn't wrap. Mode 7 returns address 0 where inline addressing wraps the offset to 64. The same encoding therefore means different addresses depending on the instruction. | confirmed |
| 5 | FIFO stall detection (reset 0x4a9a80) | Stalls come from flags that reset precomputes, and only for class-7 sub-ops 0x1f, 0x1c and 0x1d. Other ways of reading register 0x21 (input FIFO) or writing 0x22 (output FIFO) never stall: branch-through-register, repeat-count-from-register, sub-op 0x1e. They can pop an empty FIFO (stale data, free count above 32) or push a full one (overwrites unread data, free count negative). | confirmed |
| 6 | output FIFO read (0x4c5730/5790/57f0) | Pops without checking for empty, then always runs the TGP again (the `je` after `inc free` only skips when the free count becomes 0, which can't happen). | confirmed |
| 7 | reset flag scan | ORs the FIFO flags into entries past 0x1000 for programs longer than 4096 words, i.e. into the registers after the table. | confirmed |
| 8 | register writes to r12/r13 (0x4a770b) | Masked to 8 bits, while opcode 0x13 loads the same loop counters with 24 bits. | confirmed code, unverified intent |
| 9 | ALU flags | Divide (op 0x10), B + A (0x13) and to-int (0x0f) set N/Z from the integer bits of the result rather than by float comparison, so -0.0 sets N and not Z. The other float ops use a float compare. | confirmed code, unverified intent |
| 10 | to-int rounding (ALU op 0x0f, mode byte bits 1–2) | 0 = floor(x + 0.5), so halves round up rather than to even; 1 = ceil; 2 = floor; 3 = truncate. The i960 core in the same program uses 1 = floor and 2 = ceil, so one of the two orders is probably wrong. | confirmed code, unverified intent |
| 11 | command port 0x880000–0x883fff (0x4c5620) | Ignores the written data. It pushes `b<<23 \| b<<8 \| b` with `b = (address >> 4) & 0xff`. | confirmed code, unverified intent |
| 12 | 16-bit writes to the data-upload port (0x4c5400) | Masks the address to 12 bits and then compares it with 0x804000, so it never matches and 16-bit data uploads are lost. | confirmed |
| 13 | TGP writes to shared copro RAM (0x4a7690) | Index is `addr & 0xfffff` (dword index, i.e. 4 MB), but the i960-side pointer wraps at 512 KB; reads use `bank & 0x3f0000 \| addr & 0xffff`. Reads and writes cover different ranges. | confirmed |
| 14 | unimplemented encodings | Opcode classes other than 0x00, 0x07, 0x0d–0x18, 0x1d, 0x2f and 0x3f do nothing. So do ALU ops 0, 0x12, 0x15, 0x19 and 0x1c–0x1f, class-0 sub-ops 2, 3, 5–0xb and 0xe, and class-7 sub-ops 0–2, 5, 6, 9, 0xa, 0xe, 0x11, 0x12, 0x15, 0x16 and 0x18–0x1b. | confirmed |
| 15 | `atan2` shortcut (flag 0x10, 0x4a8245) | Dead code: nothing in 1.1a sets the flag. Ported anyway. | confirmed |

## 2. x87 behaviour the port reproduces

The original computes TGP floats on the x87. The port matches it bit for bit:
- **Precision:** add, subtract, multiply and divide are computed at the x87's
  53-bit precision and then rounded to single. That gives the same result as
  single-precision arithmetic (double rounding is innocuous here), so the port
  uses plain `float` math.
- **Signalling NaNs:** plain moves (`fld`/`fst`, `fabs`, `fchs`) turn them into
  quiet NaNs.
- **NaN results:** these follow the x87 rules. A signalling NaN plus a quiet
  NaN gives the quiet one; otherwise the NaN with the larger significand wins,
  and on a tie the positive one. With no NaN operand, an invalid operation
  gives the default NaN `0xffc00000`, whatever the host FPU would produce.
- **Float-to-int:** conversions go through MSVC's `_ftol2` SSE2 path, so an
  out-of-range value or NaN gives `0x80000000`.
- **`atan2` shortcut:** its "< 0" tests also treat NaN as negative, like
  `fcompp` + `test ah,1`.

Not reproduced: if Direct3D left the x87 in 24-bit precision mode,
round-to-int mode 0 computes `x + 0.5` at single precision, so 0.49999997
rounds to 1. The port uses the default 53-bit behaviour. It's the only
operation where the precision setting shows.

## 3. Not reproducible

- **Host pointers.** The original keeps two host pointers inside its state
  block: the table ROM (block offset 0x8000) and program memory (offset
  0x8075). The port never dereferences those slots. Instead it writes
  `tgp.table_ptr_value` / `tgp.prog_ptr_value` there on reset, so
  out-of-range reads see whatever value the embedder chose. A program that
  overwrites them (stack overflow to SP ≥ 0x2a, or out-of-range RAM writes)
  would make the original fetch from a corrupted host address, which in
  practice means a crash.
- **Accesses outside the state block.** Out-of-range RAM, stack or flag-table
  indices beyond 0x10ed0b0, program fetches past the program buffer, and
  external-memory or table-ROM accesses past their buffers all read other
  emulator data in the original. The port reads 0 and drops writes. It also
  returns 0 when the table ROM is missing, where the original dereferences a
  null pointer.

## 4. How this was checked

`test/difftest.py` runs the original interpreter from `EMULATOR.EXE` under
Unicorn next to the port. Both get the same random state block, program,
FIFOs, table ROM and external memory. They run one instruction or a burst of
up to 1,000, and then the whole 0x90c0-byte state block, both FIFOs and 1 MB
of external RAM are compared. Result: **18,161 cases over 8 seeds, 0
mismatches.** Inputs include signalling and quiet NaNs, infinities, denormals,
stack overflow and FIFO stalls.

`test/boardtest.py` drives the original i960-side handlers and the
`tgp_m2_*` functions with random access sequences:
- command and data ports
- 8/16/32-bit output-FIFO reads, each of which re-runs the TGP
- the copro RAM port with exponent injection
- control registers, and program and data upload

The TGP runs a FIFO-copy program throughout. Result: **23,104 driver calls in
900 sequences, 0 mismatches.**

Two things were excluded:
- **Unicorn's `fld` doesn't quiet signalling NaNs.** The test library is built
  with `-DTGP_TEST_UNICORN_FLD`, which makes the port copy that inaccuracy so
  the comparison can be exact. The real build quiets them like hardware does.
- **Stray accesses and pointer overwrites.** Cases where the original touches
  memory outside the modelled state, or overwrites one of its pointer slots,
  are skipped (see section 3).
