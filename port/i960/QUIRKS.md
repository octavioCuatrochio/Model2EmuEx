# i960 core: bugs and quirks in the original, and what the port does

Source: Sega Model 2 Emulator 1.1a by ElSemi, `EMULATOR.EXE`, i960 core at
0x4c0000–0x4c5900. Addresses below are in that file.

**Rule for the port.** If a bug changes what game code sees, the port keeps it
by default, because games were tuned against this emulator. Build with
`-DI960_ACCURATE=1` to get the behaviour described in Intel's documentation
instead. If a bug can only crash or corrupt the host program, the port fixes it
unconditionally.

Each entry says how sure I am:
- **confirmed**: read from the machine code and checked by the differential test.
- **likely**: the original clearly differs from my reading of Intel's i960KB
  documentation, but I have not checked real hardware or the manual text itself.
- **unverified**: the original does something odd, but it might be what the
  hardware does.

## 1. Bugs that change what games see (kept by default)

| # | Where | What the original does | i960 behaviour | Sure? | `I960_ACCURATE` |
|---|-------|------------------------|----------------|-------|-----------------|
| 1 | `addc` 0x4c1b50 | `dst = src1 + ((src2 + (cc & 2)) != 0)`. Adds 0 or 1 instead of `src2 + carry`, and never sets the overflow bit. | `dst = src1 + src2 + carry`; cc = carry, overflow | confirmed | fixed |
| 2 | `subc` 0x4c1ae0 (inline x86 asm) | Uses x86 `sbb`: `dst = src2 - src1 - c`, where c means *borrow*. | `dst = src2 - src1 - 1 + c`, where c means *no borrow* | confirmed code, likely bug | fixed |
| 3 | interrupt entry 0x4c4c60 | Masks process controls with `0xffffe0ff` (bits 8–12) before OR-ing the new priority into bits 16–20, so the old priority is never cleared and priorities accumulate until the interrupt returns. | priority field replaced | confirmed | fixed |
| 4 | interrupt entry 0x4c4c60 | Reads the interrupt stack pointer from PRCB+0x18, throws it away and builds the interrupt frame on the current stack. | switches to the interrupt stack when not already interrupted | confirmed | not changed (would move where frames land in memory) |
| 5 | `modpc` 0x4c18d0 | Updates process controls but never writes the old value back to the src/dst register. | old value returned in src/dst | confirmed code, likely bug | fixed |
| 6 | `shlo` `shli` `shro` `shri` | Shift count masked to 5 bits (x86 `shl`/`shr`), so a shift by 32 or more wraps around. | shift by ≥ 32 gives 0 (or all sign bits for `shri`) | confirmed code, likely bug | fixed |
| 7 | `bbc` 0x4c0a50 | Sets cc = 010 when the bit is clear (branch taken) and 000 when set, the same as `bbs`. | cc = 000 when the bit is clear, 010 when set | likely | fixed |
| 8 | `ret` 0x4c3080 | Handles only return type 0 (local) and 7 (interrupt). Types 1–6 (fault, supervisor) do nothing, and execution simply continues after the `ret`. | all return types | confirmed | not changed |
| 9 | REG destinations with the m3 bit set | For non-FP ops, and for FP ops naming anything other than fp0–fp3, the result is written *into the literal table*, so later uses of that literal read the wrong value. Only invalid encodings do this. | reserved encoding | confirmed | writes are discarded |
| 10 | `dmovt` 0x4c1e00 | Tests the whole 32-bit word, not the low byte, and sets cc = 010 for a non-digit and 000 for a digit. | unclear | unverified | not changed |
| 11 | `logbnr` | Returns `log2(x)` instead of the unbiased exponent (`floor(log2 |x|)`). | exponent | confirmed code | not changed |
| 12 | `remr` `remrl` | Uses `fmod` (the CRT's `_CIfmod`) instead of the IEEE remainder. | IEEE remainder | confirmed code | not changed |
| 13 | `cvtri` `roundr` `roundrl` | Rounding mode "nearest" (AC bits 30–31 = 0) is `floor(x + 0.5)`, so halves round up instead of to even. | round half to even | confirmed code | not changed |
| 14 | 32 instructions missing | `faultno`–`faulto`, `testo`, `cmpibno`, `cmpibo`, `scanbyte`, `eshro`, `synmovl`, `atmod`, `atadd`, `synld`, `daddc`, `dsubc`, `modify`, `extract`, `modtc`, `calls`, `mark`, `fmark`, `syncf`, `cmpor`, `cmporl`, `classr`, `classrl`, `logbnrl`, `cvtril`. The original shows a Windows message box ("Invalid opcode at %X"); the port calls `s->invalid_opcode` and carries on. | implemented | confirmed (compared with the i960KB instructions I know of) | not implemented |

Not listed as a bug: memory-format addressing mode 5 (IP + displacement)
computes `displacement + (instruction address + 4) + 8`. Intel describes this
mode as "IP + displacement + 8", and I'm not certain which address that IP
means. Games would visibly break if this were off by 4, so the port keeps the
original's formula in every build.

## 2. Floating-point precision

- The original keeps fp0–fp3 as 64-bit doubles. On the real chip they are
  80-bit.
- The SHARC DSP code (0x4bd033, 0x4be065 → 0x460f20) loads x87 control word
  `0x003f`, which is **24-bit precision**, and never restores it. Direct3D 9
  also does this unless the device is created with `D3DCREATE_FPU_PRESERVE`.
  So when the original runs Model 2B games, the i960's `addr`, `subr`, `mulr`,
  `divr` and `sqrtr` (and their `rl` versions) are most likely rounded to
  single precision.
  - The port computes in double by default.
  - `-DI960_FP_SINGLE=1` rounds those results through `float`, which is close
    to, but not exactly, what 24-bit x87 does (x87 keeps a wider exponent range).
- Transcendental results (`sin`, `cos`, `tan`, `atan`, `log*`, `exp*`,
  `scale*`) come from the host's libm in the port. The original used x87
  instructions plus MSVC's `pow`. Expect differences in the last bit or so.
- `sinr`/`cosr`/`tanr` with |x| ≥ 2⁶³ return x unchanged, and an infinity
  gives NaN. The port copies this x87 behaviour.
- Out-of-range float→int conversions give `0x80000000`. The original's
  `_ftol2` does this on any SSE2 CPU. On a pre-SSE2 CPU it would return the low
  32 bits of a 64-bit conversion instead.

## 3. Engine quirks kept on purpose

- **Stale displacement words.** The instruction cache only compares the first
  word of an instruction. If code rewrites the 32-bit displacement word of a
  memory-format instruction but leaves the opcode word alone, the old
  displacement is still used.
- **Cycle charging after a code-region switch.** The cycle cost is taken
  from a global "current instruction" pointer. When an instruction moves
  execution into a region that has not been predecoded, predecoding leaves that
  pointer on the region's last entry, so that entry's cost is charged
  (`s->cur` in the port).
- **Region selection.** The region is re-checked only when the IP changes in bits 21–31
  (compared with the previous instruction), while regions are chosen by bits 20–31.
- **Free patched instructions.** Instructions patched with `i960_hook_insn`,
  `i960_stop_after_insn` or `i960_nop_insn` cost 0 cycles. They go back to
  normal as soon as the instruction word changes.
- **Frame cache.** The frame cache holds 128 frames, and the oldest is written to memory when it's
  full. `ret` searches the cache from newest to oldest, writing out every
  non-matching frame it passes.

## 4. Host-safety fixes (always on in the port)

| Where | Original | Port |
|-------|----------|------|
| `divi` `remi` `modi` `ediv` `remo`, divisor 0 or `INT_MIN / -1` | x86 divide fault, the emulator crashes (`divo` alone was guarded) | result 0 (`divi` by −1 gives `INT_MIN`) |
| memory-format modes 6 and 8–11 (reserved) | leaves the cache entry's address routine unchanged, so a fresh entry calls a null pointer | reports an invalid opcode and uses address 0 |
| IP outside every code region | indexes past the end of the region's cache | decodes into a scratch entry each time |
| `i960_iac_write` (0x4c2700) | index not checked, so any address writes into the state block | only 0xff000010–0xff00001f accepted |
| `hook`/`stop_after` patching (0x4c25f0, 0x4c2670) | picks the code region by the *current IP*, not by the patched address | picks it by the patched address and ignores addresses outside it |
| unmapped pages | the board maps every page | the port maps everything to "reads 0, writes ignored" at init |

## 5. Not reproducible

These depend on leftover machine state in the original and are excluded from
the differential test:

- `tanr`/`tanrl` with |x| ≥ 2⁶³. `fptan` refuses the operand and does not
  push 1.0, so the original's `fstp st(0)` pops the operand and stores whatever
  was below it on the x87 stack. The port returns x.
- `movre` whose source is the FP literal +0.0 or +1.0. It reads 12 bytes
  from an 8-byte constant. In the original the extra word is the next variable in
  its data section (`0x004c24e0` after +1.0; a SHARC variable after +0.0). The port reads zero padding.
- NaN sign: x86 produces negative quiet NaNs, ARM positive ones.

## 6. How this was checked

`test/difftest.py` runs the original machine code from `EMULATOR.EXE` under
Unicorn next to the port. Each case uses the same random CPU state, memory and
I/O pages, and runs one instruction or one interrupt entry. It then compares:
- all registers, the condition code, AC, PC, fp0–fp3 and the literal table
- the whole frame cache
- 64 KB of RAM
- every I/O write, in order
- the cycle count returned by `i960_execute`

Result on the default (original-compatible) build: **47,159 cases over 6 seeds
covering all 153 opcodes plus interrupt entry, 0 mismatches.**

Some cases are excluded:
- **Original crashed:** cases where the original faults (division by zero,
  `pow`/`fmod` error paths in the CRT). These are skipped.
- **Log arguments:** log instructions get positive arguments only, because
  Unicorn's `fyl2x` mishandles x ≤ 0.
- **`remr`/`remrl`:** checked against an exact `fmod` instead of Unicorn,
  because Unicorn's `fprem` is not exact.
- **Tolerance:** transcendental results are compared to a relative 1e-12.

`I960_ACCURATE=1` intentionally fails the rows marked "fixed" in section 1.
