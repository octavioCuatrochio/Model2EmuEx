/*
 * What the road is doing under a point.
 *
 * This is the one thing the game asks the geometry processor that it cannot
 * answer straight away, so command 0x36 works differently from all the
 * others (../notes/geometry.md): the five parameters include an address in
 * buffer RAM, the i960 leaves -1 in a word there, sends the command and
 * spins until the processor has cleared it and left the answer next to it.
 * Buffer RAM is the same memory the display list is built in; the mailbox
 * sits at its very end, out of the list's way.
 *
 * The course is not searched from the start every time. It is divided into
 * a grid of square cells, and a table says which piece of the course each
 * cell belongs to, so the processor is told where to start looking.
 */
#include "decomp.h"

#define CELL_TO_PIECE 0x501428u    /* -> which piece of course each cell is */
#define PIECE_FLAGS   0x50142cu    /* -> what kind of surface each piece is */
#define MAILBOX       0x91fff0u    /* -1 while the answer is not there yet */
#define ANSWER        0x91fff4u    /* how far above or below the road ... */
#define ANSWER_PIECE  0x91fff8u    /* ... and which piece it was */
#define TGP_MAILBOX   0x17ffcu     /* the same word, as the processor sees it */

/* 0x11bec: grid_cell. The cell (g5, g6) = (x, z) is in, as one number:
   cells of 16 units, 128 of them each way, the middle of the course at the
   middle of the grid. */
RT_FN(f_00011bec)
{
    ENTRY(0x11becu);

    wrI(&G(5), rt_round_ac(s, (double)rdF(&G(5))));
    UPTO(0x11bf0u);
    s->r[3] = 1u << 10;
    UPTO(0x11bf4u);
    G(5) += s->r[3];
    UPTO(0x11bf8u);
    G(5) >>= 4;
    UPTO(0x11bfcu);
    s->r[3] = 0x7f;
    UPTO(0x11c00u);
    G(5) &= s->r[3];
    UPTO(0x11c04u);

    wrI(&G(6), rt_round_ac(s, (double)rdF(&G(6))));
    UPTO(0x11c08u);
    s->r[3] = 1u << 10;
    UPTO(0x11c0cu);
    G(6) += s->r[3];
    UPTO(0x11c10u);
    G(6) >>= 4;
    UPTO(0x11c14u);
    s->r[3] = 0x7f;
    UPTO(0x11c18u);
    G(6) &= s->r[3];
    UPTO(0x11c1cu);
    G(6) <<= 7;
    UPTO(0x11c20u);
    G(5) += G(6);
    UPTO(0x11c24u);
    RET();
}

/* 0x11c28: road_at. (g0, g1, g2) is a point; the answer is how far it is
   above the road (g0), the piece of course under it (g2) and whether there
   was one at all (g3). g1 keeps the piece the point is on when that piece is
   the road proper; anything else leaves it as the caller had it. */
RT_FN(f_00011c28)
{
    ENTRY(0x11c28u);

    G(5) = G(0);
    UPTO(0x11c2cu);
    G(6) = G(2);
    UPTO(0x11c30u);
    CALL(f_00011bec, 0x11becu);           /* grid_cell */

    s->r[5] = rd32(s, CELL_TO_PIECE);
    UPTO(0x11c3cu);
    s->r[5] = (uint32_t)(int32_t)(int16_t)rd16(s, s->r[5] + G(5) * 2);
    UPTO(0x11c40u);
    s->r[3] = 0u - 1u;
    UPTO(0x11c44u);
    wr32(s, MAILBOX, s->r[3]);            /* nothing there yet */
    UPTO(0x11c4cu);
    s->r[3] = 0x3636;
    UPTO(0x11c54u);
    TGP_CMD_AS(0x36, s->r[3]);            /* road_at, and it takes its time */
    UPTO(0x11c58u);
    s->r[3] = TGP_MAILBOX;
    UPTO(0x11c60u);
    TGP_PARAM(s->r[3]);                   /* where to leave the answer */
    UPTO(0x11c64u);
    TGP_PARAM(G(1));
    UPTO(0x11c68u);
    TGP_PARAM(s->r[5]);                   /* where to start looking */
    UPTO(0x11c6cu);
    TGP_PARAM(G(0));
    UPTO(0x11c70u);
    TGP_PARAM(G(2));
    UPTO(0x11c74u);

    for (;;) {                            /* wait for it */
        s->r[3] = rd32(s, MAILBOX);
        UPTO(0x11c7cu);
        rt_cc_i(s, 0, s->r[3]);
        if (s->r[3] != 0) {
            BRANCH(0x11c80u, 0x11c74u);
            continue;
        }
        UPTO(0x11c80u);
        break;
    }

    G(0) = rd32(s, ANSWER);
    UPTO(0x11c88u);
    G(1) = rd32(s, ANSWER_PIECE);
    UPTO(0x11c90u);
    rt_cc_u(s, 0, G(0));
    if (G(0) == 0) {                      /* the point is off the course */
        BRANCH(0x11c94u, 0x11cbcu);
        G(0) = 0xbdcccccdu;               /* -0.1f */
        UPTO(0x11cc4u);
        G(5) = 1;
        UPTO(0x11cc8u);
    } else {
        UPTO(0x11c94u);
        G(4) = G(1);
        UPTO(0x11c98u);
        s->r[3] = rd32(s, PIECE_FLAGS);
        UPTO(0x11ca0u);
        G(2) = rd8(s, s->r[3] + G(4));
        UPTO(0x11ca4u);
        G(2) &= 15;
        UPTO(0x11ca8u);
        G(5) = 1u << (G(2) & 0x1f);
        UPTO(0x11cacu);
        s->r[3] = 2;
        UPTO(0x11cb0u);
        rt_cc_u(s, G(5), s->r[3]);
        if (G(5) != s->r[3]) {            /* not the road proper */
            BRANCH(0x11cb4u, 0x11cc8u);
        } else {
            UPTO(0x11cb4u);
            G(3) = G(4);
            BRANCH(0x11cbcu, 0x11cc8u);
        }
    }

    G(1) = G(3);
    UPTO(0x11cccu);
    G(2) = G(4);
    UPTO(0x11cd0u);
    G(3) = G(5);
    UPTO(0x11cd4u);
    RET();
}
