/*
 * The tile layers' scroll registers.
 *
 * The tilemap chip has eight 16-bit registers at 0x100a000: four
 * horizontal scroll positions and four vertical ones, for the primary map
 * and the window of each of the two layers (../port/m2/m2tile.c). The game
 * keeps its own copies from 0x501300 and copies them in once a frame, with
 * four flag bits that go in the top bit of the vertical registers (the
 * primary map off, or no window).
 */
#include "decomp.h"

#define SCROLL_SHADOW  0x501300u   /* x, y for A primary, A window, B primary, B window */
#define SCROLL_FLAGS   0x501320u   /* one bit each, into bit 15 of the y registers */
#define TILE_SCROLL    0x100a000u  /* the chip's registers: 4 x, then 4 y */
#define FRAME_HOOK     0x50000cu

/* 0x1b9a0: frame_tasks */
RT_FN(f_0001b9a0)
{
    ENTRY(0x1b9a0u);

    s->r[3] = rd32(s, FRAME_HOOK);
    UPTO(0x1b9a8u);
    rt_cc_i(s, 0, s->r[3]);
    if (s->r[3]) {
        UPTO(0x1b9acu);
        CALLX(s->r[3]);                  /* the game's per-frame hook */
    } else {
        BRANCH(0x1b9acu, 0x1b9b0u);
    }

    /* the shadow registers, read in the machine code's order: x, y of the
       primary map of layer A, then layer B's, then the windows' */
    static const uint8_t reg[8] = { 0, 1, 4, 5, 2, 3, 6, 7 };   /* halfword in the shadow */
    uint32_t v[8];
    for (int k = 0; k < 8; k++) {
        v[k] = rd16(s, SCROLL_SHADOW + 2 * reg[k]);
        s->r[3 + k] = v[k];
        UPTO(pc_ + 8);
    }
    uint32_t flags = rd16(s, SCROLL_FLAGS);
    s->r[11] = flags;
    UPTO(0x1b9f8u);

    /* bit i of the flags goes into bit 15 of y register i */
    s->r[4] |= (flags & 1) << 15;        /* A primary */
    s->r[6] |= (flags & 4) << 13;        /* B primary */
    s->r[8] |= (flags & 2) << 14;        /* A window */
    s->r[10] |= (flags & 8) << 12;       /* B window */
    s->r[12] = (flags & 8) << 12;
    s->r[13] = 8;
    UPTO(0x1ba48u);

    /* into the chip, in the machine code's order */
    static const uint16_t port[8] = { 0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe };
    for (int k = 0; k < 8; k++) {
        wr16(s, TILE_SCROLL + port[k], (uint16_t)s->r[3 + k]);
        UPTO(pc_ + 8);
    }
    RET();
}
