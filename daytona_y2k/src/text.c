/*
 * Text layer.
 *
 * The tile layers hold 16-bit entries, 64 to a row (0x80 bytes), and the
 * game writes character codes into them; 0x20 is the space.
 */
#include "decomp.h"

#define TILE_ROW   0x80u   /* bytes from one row of tiles to the next */
#define TILE_SPACE 0x20u

/* 0x192c4: blank a rectangle of text.
   g0: a descriptor (a word the caller keeps, then rows and columns), left
   pointing past it; g1: the first tile, left one row past the last.
   A zero row or column count still writes one, as the machine code's
   post-tested loops do.

   Blanking a large area takes longer than a time slice, so the loop keeps
   its counters where the machine code keeps them (r4 rows left, r6 the
   tile, r7 columns left): the board may end the slice, and let the frame
   interrupt in, after any instruction of it. */
RT_FN(f_000192c4)
{
    ENTRY(0x192c4u);

    G(0) += 4;
    s->r[3] = TILE_SPACE;
    UPTO(0x192ccu);
    s->r[4] = rd32(s, G(0));             /* rows */
    UPTO(0x192d0u);
    G(0) += 4;
    UPTO(0x192d4u);
    s->r[5] = rd32(s, G(0));             /* columns */
    UPTO(0x192d8u);
    G(0) += 4;
    UPTO(0x192dcu);

    for (;;) {
        s->r[6] = G(1);
        s->r[7] = s->r[5];
        UPTO(0x192e4u);
        for (;;) {
            wr16(s, s->r[6], TILE_SPACE);
            UPTO(0x192e8u);
            s->r[6] += 2;
            rt_cc_u(s, 1, s->r[7]);      /* cmpdeco: count down */
            s->r[7] -= 1;
            if (!(s->cc & 4))
                break;
            BRANCH(0x192f4u, 0x192e4u);
        }
        UPTO(0x192f4u);
        G(1) += TILE_ROW;
        rt_cc_u(s, 1, s->r[4]);
        s->r[4] -= 1;
        if (!(s->cc & 4))
            break;
        BRANCH(0x19300u, 0x192dcu);
    }
    UPTO(0x19300u);
    RET();
}
