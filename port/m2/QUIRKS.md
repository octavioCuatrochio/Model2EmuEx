# Model 2 / 2A board: bugs and quirks in the original, and what the port does

Source: Sega Model 2 Emulator 1.1a by ElSemi, `EMULATOR.EXE`:
- board init and memory map at 0x4cd450
- frame loop at 0x4cc7a0
- I/O handlers at 0x4c6780–0x4c8840 and 0x4cb480–0x4cc710
- EEPROM (old MAME `eeprom.c`) from 0x4ecde0
- NVDATA loading at the end of board init (0x4cd450)

Same rules as the CPU and TGP notes. Anything a game can observe is
reproduced. Accesses that would corrupt the emulator itself, rather than
emulated state, are contained. "Bug" means the original's code is internally
inconsistent; it doesn't mean I know what the hardware does.

- **confirmed**: read from the machine code.
- **unverified**: odd, but it might be what the hardware does.

## 1. Bugs and oddities that games can see (reproduced)

| # | Where | What the original does | Sure? |
|---|-------|------------------------|-------|
| 1 | 32-bit reads of 0x980000–0x98ffff, Model 2 (0x4c7a80) and 2A (0x4c8770) | Compares the address with 0x900008 instead of 0x980008, so the data-upload control register can't be read at 0x980008 (it reads 0). 0x900008 is buffer RAM and never reaches this handler, so the branch is dead. | confirmed |
| 2 | 32-bit reads of the I/O board 0x1c00000 (0x4c6780...) | The handler returns the byte in `al` and leaves the rest of `eax` from an earlier `lea`, so a 32-bit read returns `0x004c52XX`. Ported as `0x4c5200 \| byte`. | confirmed |
| 3 | I/O board writes at 0x1c00200 and up, Model 2 only (0x4cb5e0 / 0x4c86c0) | Stored at `M2BACK + 0x7e00 + (addr & 0xffff)`. Offsets from 0x8200 up run past the 64 KB backup RAM; up to 0x17dff is reachable. The 16-bit form also drops the single write of 0x4d to 0x1c00202. | confirmed |
| 4 | reads of the same range | 8-bit reads return that memory on Model 2 and 0xff on 2A. At 0x1c00000–0x1c001ff, analog inputs are only at even offsets up to 0x10; other offsets read 0 on Model 2 and 0xff on 2A. | confirmed |
| 5 | network board 0x1a00000 (0x4c7ae0–0x4c7ba0) | Only the 8-bit read checks whether networking is active (flag set when `m2network.ini` networking starts, 0x4cf960). The 16- and 32-bit reads always return shared RAM, which is at `M2ZSHARED + 0x8000`. With networking off, an 8-bit read of 0x1a14000 returns 0xff: bit 0 set means "no board". The 16-bit read returns the upper half of the host pointer in the high word, which a 16-bit bus drops. | confirmed |
| 6 | sound board ports 0x9c0000 / 0x1c80000 (0x4c7650 / 0x4cb570) | Reads return 7 at byte offsets 2 and 4 (status: ready), and 0xff elsewhere, decrementing a counter. On Model 2 a byte write at offset 0 goes into a 32-byte FIFO. On 2A it goes into the SCSP's 128-byte MIDI input buffer, and the SCSP code at 0x4b2bc0 is called when sound is on. Either way it raises i960 IRQ 10 on a later slice. The i960 can never read a reply. | confirmed |
| 7 | EEPROM erase / write (0x4ece50) | Neither resets `serial_count` after a successful erase or write, so the command buffer keeps growing until the next reset line. Erase writes 0x0000, where a real 93C46 erases to 0xffff. | confirmed code, unverified intent |
| 8 | 16-bit writes to the TGP data-upload port | Never match (see `tgp/QUIRKS.md` #12). | confirmed |
| 9 | 32-bit read of 0x9c0004 via the 0x98 page, Model 2 | Returns 5; any other unknown address in the page returns 0. | confirmed |

## 2. Differences in the port (the original's behaviour would hurt the host, or is Windows-only)

| # | Where | Original | Port |
|---|-------|----------|------|
| 1 | timers 0xf00000 (0x4c7d80 / 0x4c7db0) | Index `(addr >> 2) & 0x3f` into a 4-entry array, so offsets 0x10–0xfc read and write the globals after it. | `m2_board.timer` has 64 entries; offsets past 3 are stored but don't alias anything. No game seen so far accesses them. |
| 2 | backup and shared RAM overruns (#3, #5 above) | Run into whatever the Windows heap put next. | `back` and `zshared` are allocated with 0x18000 bytes, so every reachable offset stays inside the buffer. |
| 3 | speed hacks: `M2_HOOK_IDLE_COUNT` / `M2_HOOK_STOP_AFTER` (game table), plus the 0x98000c read counter | Stop the i960 for the rest of the frame once the game is in its wait-for-vblank loop. | Same (`i960_hook_insn` / `i960_stop_after_insn`). They only save host time: stopping early changes nothing the game can see, except that the timers keep running while it's stopped. |
| 5 | buffer sizes | tex0/tex1 (1 MB), translation (64 KB) and luma (128 KB) RAM are masked, but a 32-bit store at the last masked offset writes 2 bytes past the end. | Allocated with 4 bytes of slack. |
| 4 | NVDATA\\*game*.DAT | Loaded after reset, saved on exit: 64 KB of backup RAM, then the 64x16-bit EEPROM (128 bytes). | Same file format: `m2_nvram_load` / `m2_nvram_save`. Files from the Windows emulator work. |

## 3. Game behaviour worth knowing (not emulator bugs)

| Game | What happens | What the port does |
|------|--------------|--------------------|
| Daytona USA (checked on `daytona`; the defaults search should also cover the clones) | The factory defaults in ROM (daytona: 0x39d60, i.e. i960 0x239d60, followed by the CRC table) set link mode (backup RAM +0x0b, copied to 0x5fe588) to 1, linked cabinet. With blank backup RAM the boot code (0x1200) calls the network-board probe (0x1b5f4). The probe prints "CANCELLED / NETWORK BOARD NOT PRESENT", waits 320 frames and resets, in an endless loop: the "frame 453" event. On real hardware an operator sets single mode in the test menu (code at 0x2a4b0). | `m2_nvram_defaults` recognises the defaults table by its "SEGA@" header and the CRC-16 table after it. It writes both settings copies (backup RAM 0x00 and 0x80) with link mode 0 and recomputes their CRC-16/XMODEM (at +8, over +0x0a..+0x7f, as the game's routine at 0x292d8). Only used when no NVDATA file exists. |
| Sega Rally | The "WARNING — this game is to be used only in Japan" screen stays for about 1000 frames (~17 s) before attract mode. | Nothing. Not a sound-board wait: it lasts just as long with sound running. |

## 4. Video

The 2D path (`m2tile.c`) is a straight port of the original. The 3D path
(`m2geo.c`, `m2gl.c`) keeps the original's structure but not its Direct3D 9
code, which can't run on a Mali-400. The original's renderer passes most
values in registers and x87 slots that the decompiler loses. So the
field-level details were checked against MAME's model2 video code
(`src/mame/sega/model2_v.cpp`, BSD-3-Clause), which documents the same
hardware from the 2B manual and from ElSemi's own Direct3D implementation.

| # | Where | Original | Port |
|---|-------|----------|------|
| 1 | tile colours (0x4c6b10) | Keeps 4 bits per channel for tiles, to fit its A4R4G4B4 textures. | 8 bits per channel (the same table's 32-bit form). |
| 2 | tile decode (0x4c0410) | Any tile or CG write re-decodes all four 512x512 maps next frame. | A tile write re-decodes that tile only; CG writes still re-decode everything. Same output, much less work on ARM. |
| 3 | 1024-line tilemaps | Indexes past the end of the first map's decoded buffer. | Lines 512–1023 come from the pair's other map. Unverified: no game seen so far uses it. |
| 4 | layer lines that draw nothing (primary map off, no window) | Leave last frame's texture contents. | Transparent. |
| 5 | window command (0x4bac20) and projection | Reads the corners and centres as 16-bit fields, minus 8 (x) and 90 (y), with its own screen offsets. | 12-bit sign-extended fields, as the hardware (and MAME). Projection as MAME: column = 84 + hsync + cx + x/z, row = 384 − cy + 130 + vsync − y/z. hsync and vsync are the CRTC sync registers at 0x1040000/0x1060000 (mirrored at 0x1140000/0x1160000), now stored by the board. Daytona and Sega Rally write −84/−2 and VF2 −84/−3, for windows of 0–496 x 128–512. A first version used the 2B manual's fixed 8/474, which left the bottom 38 lines without 3D. |
| 6 | drawing order | Direct3D with its own sorting. | Hardware order: z buckets (4.12 float of min/max/previous z plus z adjust) within windows. Drawn back to front with no depth buffer (window ascending, z descending, then oldest first). That's the exact reverse of the hardware's front-to-back fill. |
| 7 | textures | A texture cache, with one Direct3D texture per texture. | Both texture sheets live in one 2048x2048 atlas. Polygons are cut on the CPU where their texture repeats or mirrors, so every piece samples one copy of the texture from interpolated coordinates. The Mali-400 has no highp in fragment shaders, so per-pixel wrapping would lose texels. Polygons that would need more than 256 pieces are clamped. |
| 8 | texture filtering, mipmaps, microtextures, texture LOD | Bilinear and optional trilinear filtering in Direct3D. | Nearest texel, no mipmaps, no microtextures (not done yet). |
| 9 | colour | Its pixel shader: texel → luma table → colour translation. | Same pipeline: texel → luma RAM[lumabase + texel·8] × polygon luma / 256 (max 63) → a 64x1024 table of palette × luma through the translation RAM, remap and gamma. Untextured polygons use luma/4. Translucent texel 15 and the mesh (checker) flag discard pixels. |
| 10 | master z clip, polygon count register | Not checked. | Not implemented. |
| 11 | widescreen (`m2wide.c`) | Lua scripts per game (SCRIPTS\\*.lua) call `Model2_SetWideScreen` and `Model2_SetStretch*` from game-state bytes: Daytona 0x5010a4 (in game and attract), Sega Rally 0x202098/0x20209c/0x2020ac. | The same rules in C. The frame gets wider (16:9: 662 native pixels), full-width 3D windows get the extra field of view, and tile layers stay 4:3 in the middle unless stretched. When a rule turns widescreen off, the frame stays wide and the 4:3 picture is pillarboxed. VF2 had no script (no widescreen in the original); the port adds one that stretches its sky layer (B below the 3D). |
| 13 | mesh polygons (shadows, fences) | Checkerboard, or with MeshTransparency=1 50% translucency (its PShaderBlend). | Both: `--mesh blend` (default; F8 toggles) blends mesh pixels at 50% in the back-to-front pass; `--mesh checker` is the hardware's every-other-pixel pattern, which a CRT blurs into the same effect. |
| 14 | colour | GammaR/G/B in EMULATOR.INI (default 1.0) feed its colour tables; no saturation control. | `--gamma G` or `--gamma R,G,B` feed the same tables (tiles and 3D). `--saturation S` (F7 cycles 1.0, 1.2, 1.4) is new: a final-pass adjustment. The default look matches the original and MAME; real cabinets had their monitors set up individually. |
| 12 | resolution | Direct3D at the window's resolution. | The frame renders at an integer scale of the native 496x384 (`--scale`, auto = window height / 384), then scales to the window. The mesh (checker) effect stays at native pixel size. Tile layers are upscaled, not re-rendered. |

## 5. Inputs

| # | Where | What the port does |
|---|-------|--------------------|
| 1 | port polarity | Active low. The ports idle at the game table's defaults, and pressing clears the bit. Checked in Daytona's test menu (VR 1/VR 4 move the cursor, Start selects, coins count). |
| 2 | Model 2 (original) analog | Daytona reads its controls from the Model 1 I/O board's dual-port RAM. Bytes 0, 1 and 2 (steering, accelerator, brake) arrive through the I/O handler's ports 2, 0 and 1, so the analog channels go there, not into the analog registers. Range 0x20–0xe0, centre 0x80, pedals resting at 0x20 (MAME's calibration). The port-2 arrow-key entries drive steering and pedals instead of setting bits. Checked in Daytona's INPUT TEST (HANDLE/ACCEL/BRAKE). |
| 5 | per-game input routines (game record +0x40) | Daytona 0x4c9080 and Sega Rally 0x4cb870 (plus 0x4ca450) turn the controls into I/O-board values. Keyboard steering and pedals ramp 8 units per frame. Daytona: steering (−126..125 + 129) · 0xc0/256 + 0x20 into port 2, pedals 1..254 clamped to 3..0xfd into ports 0/1. Sega Rally: steering (−127..127 + 127) · 0xc0/256 + 0x20 into analog 0, pedals into analog 1/2, port 2 = drive-board read-back (last command, 0xff at start). The gear is kept as a number and delivered as a code in bits 4–6 (1st 0x20, 2nd 0x10, 3rd 0x60, 4th 0x50): port 6 for Daytona, port 1 for Sega Rally. The gear keys themselves are only inputs to this. INI options: HoldGears (neutral when no gear key is held), UpDownGears (Daytona only: gear keys 1/2 step down/up). | Ported: `input_fn` in the game table selects the routine. `--hold-gears` = HoldGears; `--shifter sequential` = UpDownGears, applied to Sega Rally too. The pad's RB/LB always step the gear. A first version put the gear on the key bits, leaving an invalid code 7 in bits 4–6. Other games' routines (Desert Tank, Virtua Cop and more) are not ported yet. |
| 4 | pads | Mapped by the input's name in the original's definitions (the original maps by DirectInput joystick codes). The H-pattern shifters (Daytona, Sega Rally) become sequential on RB/LB; the selected gear's bit is held. Steering has an 8% dead zone, triggers 5%. |
| 3 | Model 2A analog | The analog registers, read through the 0x1c0001e auto-increment. Range 0x00–0xff. |
