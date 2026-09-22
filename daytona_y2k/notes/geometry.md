# The geometry processor's command set

How the Daytona USA program drives the TGP (the Fujitsu MB86234 geometry DSP
running microcode the game uploads at boot). Worked out from the traffic
(`TRACE_TGP=1 build/trace ...`, tools/trace.c) and port/tgp/tgp.c.

## Protocol

- A command is sent by writing (anything) to `0x880000 + command * 16`;
  its parameters follow as words written to `0x884000-0x887fff`.
- The game keeps the port addresses in global registers set up by
  `boot_main`: `g10` = 0x800000 (copro control), `g11` = 0x880000 (the
  command FIFO), `g12` = 0x4000 (from the command page to the parameter
  page). So the machine code never names the ports; it stores through `g11`
  (3,101 store instructions in 158 functions).
- Every command takes a fixed number of parameter words, and 38 commands
  are used in attract mode and a race. Most parameters are floats; angles
  are 16-bit (65536 = one turn).
- In Daytona the TGP is a **maths coprocessor**, not a display-list engine:
  almost nothing it does goes to the display list (only command 0x36 writes,
  5 words). The i960 asks it for transforms, trigonometry and collision
  tests, reads the results back, and writes the display list itself
  (23,843 words in 20 frames of racing, straight to 0x80xxxx).
- It keeps a **matrix stack** (push/pop/identity/translate/scale/rotate) and
  a single-accumulator **calculator** (load/add/sub/mul/div/get).

## Checking the meanings

`tools/tgpmodel.py` models the commands and checks the model against a
logged command stream:

    make trace
    TRACE_TGP=1 TRACE_TGP_FROM=2300 TRACE_TGP_TO=2500 \
        TRACE_TGP_RAW=raw.txt TRACE_TGP_RAW_MAX=2000000 \
        build/trace ~/Desktop/ROMs 2500 /dev/null "<keys>"
    python3 tools/tgpmodel.py raw.txt            # every command with results
    python3 tools/tgpmodel.py raw.txt --search   # which matrix conventions fit

Over 200 frames of racing (365,677 commands) every result of the 17
commands below matches the model, including all 7,053 get-matrix and 5,660
transform results, so the matrix conventions are settled: rotations turn by
**minus** the angle about x, y and z, and are applied after the current
matrix; translate works in the current frame; scale multiplies the columns.



## Commands

Counted over 6,000 frames (attract mode, the menus, a race). "From" is a few
of the instruction addresses that send it. Named commands are confirmed by
`tools/tgpmodel.py`; 0x2a and 0x30 are still open (they look like track
lookups).

Command 0x36 is the one that takes long enough to be worth waiting for, and
it answers differently from all the others. Its first parameter is an
address in buffer RAM; the i960 puts -1 in a word there, sends the command
and spins until the processor has cleared it and left the answer in the two
words after it (`src/road.c`). Buffer RAM is where the display list is
built, so the mailbox sits at the far end of it, past anything a frame
writes.

| Cmd | Name | Words | Packets | From | What it does |
|---|---|---|---|---|---|
| 02 | mul | 2 | 366463 | 00e92c 006738 006760 | (a, b) -> a*b |
| 03 | div | 2 | 212853 | 00e954 00669c 006f84 | (a, b) -> a/b |
| 05 | push | 0 | 165491 | 012138 0065bc 007224 | push the current matrix |
| 06 | pop | 0 | 165491 | 0122b4 006a08 0072e4 | pop the current matrix |
| 08 |  | 0 | 1 | 000ffc |  |
| 0a | atan2 | 2 | 23450 | 006968 0069c4 0070dc | (x, y) -> angle of (x, y), 16-bit |
| 0d |  | 1 | 2 | 0046dc |  |
| 0f | angle_to | 4 | 321050 | 0111a4 011818 011614 | (x1, z1, x2, z2) -> angle from point 2 to point 1 |
| 10 | identity | 0 | 42272 | 012148 006a2c 0071cc | current matrix = identity |
| 11 | get_matrix | 0 | 144685 | 007484 017b5c 017c94 | -> the current matrix: 3x3 column by column, then the translation |
| 12 | translate | 3 | 159450 | 012164 006a68 007428 | (x, y, z), in the current frame |
| 13 | scale | 3 | 34032 | 226368 226504 013114 | (sx, sy, sz) |
| 14 | rotate_x | 1 | 104772 | 006aa4 0071fc 007578 | (angle): rotate by minus the angle about x |
| 15 | rotate_y | 1 | 166685 | 012188 006a90 007210 | (angle): about y |
| 16 | rotate_z | 1 | 94164 | 01219c 006ab8 0071e8 | (angle): about z |
| 18 |  | 1 | 1498 | 221c94 01693c |  |
| 1a | transform | 3 | 119714 | 0121c4 012204 012244 | (x, y, z) -> the point through the current matrix |
| 1b | sin | 1 | 15449 | 006f58 00710c 00f664 | (angle) -> sin |
| 1c | cos | 1 | 8585 | 006f70 007124 00f9c0 | (angle) -> cos |
| 1d | mul_sin | 2 | 101510 | 00698c 00801c 008038 | (angle, r) -> r*sin |
| 1e | mul_cos | 2 | 87538 | 0069a8 0062a4 0062fc | (angle, r) -> r*cos |
| 24 | acc_load | 1 | 634539 | 014200 01426c 0142d8 | accumulator = x |
| 25 | acc_get | 0 | 640451 | 017efc 014254 0142c0 | -> the accumulator |
| 26 | acc_add | 1 | 494298 | 014240 0142ac 014318 | accumulator += x |
| 27 | acc_sub | 1 | 760395 | 014218 014284 0142f0 | accumulator -= x |
| 28 | acc_mul | 1 | 795089 | 01422c 014298 014304 | accumulator *= x |
| 29 | acc_div | 1 | 314895 | 007174 011aac 0118b4 | accumulator /= x |
| 2a |  | 3 | 4076 | 00ea0c 011f9c |  |
| 30 |  | 1 | 9553 | 00ea5c 011fec 00bbd0 |  |
| 31 | set_quad | 12 | 13002 | 2215a8 014b60 014c18 | four (x, y, z) corners: a car's footprint |
| 32 | in_quad | 3 | 42995 | 221890 014be4 014c9c | (x, y, z) -> -1 if (x, z) is inside the quad, else 0 |
| 36 | road_at | 5 | 32439 | 011c58 | (mailbox, y, piece, x, z): the answer goes in buffer RAM, see below |
| 40 | set_circle | 3 | 32716 | 00e730 00af20 014764 | (x, z, r) |
| 41 | circle_dist | 2 | 325591 | 014798 011588 0136c4 | (x, z) -> distance from the circle's edge |
| 43 | distance | 4 | 241788 | 0117c4 012a90 012adc | (x1, z1, x2, z2) -> distance between the points |
| 44 | polar | 2 | 7992 | 00fbcc 00fd3c | (angle, r) -> (r*sin, r*cos) |
| 46 |  | 4 | 31968 | 00fae8 00fb10 00fb7c |  |
| 47 | step | 4 | 164660 | 011908 011754 00f168 | (angle, d, x, z) -> (dx, dz, x+dx, z+dz), dx = d*sin, dz = d*cos |

## The display list

What the geometry processor computes goes back to the i960, which writes
the frame's display list into buffer RAM itself. A word written through
`g10` (0x800000) carries its opcode in the address: the copro board takes
the float's exponent from there (`tgp_m2_ctrl_write32` in
`../../port/tgp/tgp.c`), so `st r3,0x90(g10)` is opcode 9. The renderer
reads the list at `../../port/m2/m2geo.c`:

| Opcode | Words | What it sets |
|---|---|---|
| 1 | 4 | an object: texture parameters, texture headers, polygons, and how many polygons to take |
| 7 | 1 | the drawing mode |
| 8 | 1 | the z adjustment |
| 9 | 2 | the projection: the eye's distance from the screen, x and y |
| a | 3 | the direction the light comes from |
| b | 12 | the matrix that places what follows (3x3 then the translation) |
| c | 3 | just the translation of that matrix |
| 16 | 1 | the level of detail |
| f | - | the end of the list |

`draw_model` (0x17acc, `src/model.c`) writes 9, a and b when the object's
view record changes, then one opcode-1 command per chunk of the model.
