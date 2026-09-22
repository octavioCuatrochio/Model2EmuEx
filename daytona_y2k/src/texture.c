/*
 * Texture data sent with the frame's display list.
 *
 * Opcode 4 of the list (../notes/geometry.md) writes words straight into the
 * renderer's texture RAM: an address, a count, then the data
 * (../../port/m2/m2geo.c, write_texram). The renderer reads two kinds of
 * thing from there: texture points, pairs of texture coordinates in 13.3
 * fixed point, and texture headers, four words each.
 *
 * Both functions here copy a table out of data ROM into the list and change
 * it on the way, which is how the game animates a texture without a second
 * copy of it: the sky's coordinates move with the car, and a texture's
 * header fields are replaced to point it somewhere else.
 *
 * Both are given a list of 8-byte records, ended by -1 in the first word:
 * each names an offset into the table, and the entry there is the one that
 * gets changed. Whatever lies between two named offsets is copied as it is.
 * The count word can only be written once the end is known, so a zero goes
 * in its place and the copro's write pointer (0x2008 of g10) remembers where
 * it was; buffer RAM is at 0x900000 of the i960, so it can be gone back to.
 */
#include "decomp.h"

#define HEADERS       0x28f0000u   /* the texture headers, four words each */
#define POINTS        0x28f5000u   /* ... and the texture points, in pairs */
#define TEX_HEADERS   0x800000u    /* where they go in texture RAM */
#define TEX_POINTS    0x805000u
#define LIST_PTR      0x2008u      /* of g10: the copro's write pointer */
#define LIST_RAM      0x900000u    /* the same words, as the i960 sees them */

/* 0x1a238: upload_texture_headers. g1 to g4 replace fields of the four words
   of each named header: the mask is in the high half of the register and the
   bits to set in the low half. */
RT_FN(f_0001a238)
{
    ENTRY(0x1a238u);

    s->r[11] = G(1) >> 16;               /* the four masks */
    UPTO(0x1a23cu);
    s->r[12] = G(2) >> 16;
    UPTO(0x1a240u);
    s->r[13] = G(3) >> 16;
    UPTO(0x1a244u);
    s->r[14] = G(4) >> 16;
    UPTO(0x1a248u);
    G(1) &= s->r[11];                    /* ... and the bits, within them */
    UPTO(0x1a24cu);
    G(2) &= s->r[12];
    UPTO(0x1a250u);
    G(3) &= s->r[13];
    UPTO(0x1a254u);
    G(4) &= s->r[14];
    UPTO(0x1a258u);

    s->r[3] = 0u - 1u;
    UPTO(0x1a25cu);
    s->r[4] = (uint32_t)(int32_t)(int16_t)rd16(s, G(0));
    UPTO(0x1a260u);
    rt_cc_i(s, s->r[3], s->r[4]);
    if (s->r[3] == s->r[4]) {            /* an empty list */
        BRANCH(0x1a264u, 0x1a34cu);
        RET();
    }
    UPTO(0x1a264u);

    s->r[4] = rd16(s, G(0) + 4);
    UPTO(0x1a268u);
    s->r[4] += TEX_HEADERS;              /* where the first one goes */
    UPTO(0x1a270u);
    s->r[3] = 0;
    UPTO(0x1a274u);
    DL_EXP(4, s->r[3]);
    UPTO(0x1a278u);
    DL_WORD(s->r[4]);
    UPTO(0x1a27cu);
    s->r[10] = rd32(s, G(10) + LIST_PTR);
    UPTO(0x1a284u);
    DL_WORD(s->r[3]);                    /* the count, filled in at the end */
    UPTO(0x1a288u);

    G(6) = rd16(s, G(0) + 4);
    UPTO(0x1a28cu);
    G(6) = HEADERS + G(6) * 2;
    UPTO(0x1a294u);
    BRANCH(0x1a298u, 0x1a2dcu);          /* the first record needs no copying */
    goto changed;

    for (;;) {
        G(5) = rd16(s, G(0) + 4);        /* the next one to change */
        UPTO(0x1a29cu);
        G(5) = HEADERS + G(5) * 2;
        UPTO(0x1a2a4u);
        rt_cc_u(s, G(5), G(6));
        if (G(5) <= G(6)) {
            BRANCH(0x1a2a8u, 0x1a2dcu);
            goto changed;
        }
        UPTO(0x1a2a8u);
        G(5) -= G(6);                    /* the headers in between, as they are */
        UPTO(0x1a2acu);
        G(5) >>= 3;
        UPTO(0x1a2b0u);
        for (;;) {
            s->r[3] = rd16(s, G(6));
            UPTO(0x1a2b4u);
            s->r[4] = rd16(s, G(6) + 2);
            UPTO(0x1a2b8u);
            s->r[5] = rd16(s, G(6) + 4);
            UPTO(0x1a2bcu);
            s->r[6] = rd16(s, G(6) + 6);
            UPTO(0x1a2c0u);
            DL_WORD(s->r[3]);
            UPTO(0x1a2c4u);
            DL_WORD(s->r[4]);
            UPTO(0x1a2c8u);
            DL_WORD(s->r[5]);
            UPTO(0x1a2ccu);
            DL_WORD(s->r[6]);
            UPTO(0x1a2d0u);
            G(6) += 8;
            UPTO(0x1a2d4u);
            rt_cc_u(s, 1, G(5));
            G(5) -= 1;
            if (s->cc & 4) {
                BRANCH(0x1a2dcu, 0x1a2b0u);
                continue;
            }
            UPTO(0x1a2d8u);
            break;
        }

changed:
        s->r[3] = rd16(s, G(6));
        UPTO(0x1a2e0u);
        s->r[4] = rd16(s, G(6) + 2);
        UPTO(0x1a2e4u);
        s->r[5] = rd16(s, G(6) + 4);
        UPTO(0x1a2e8u);
        s->r[6] = rd16(s, G(6) + 6);
        UPTO(0x1a2ecu);
        s->r[3] &= ~s->r[11];
        UPTO(0x1a2f0u);
        s->r[4] &= ~s->r[12];
        UPTO(0x1a2f4u);
        s->r[5] &= ~s->r[13];
        UPTO(0x1a2f8u);
        s->r[6] &= ~s->r[14];
        UPTO(0x1a2fcu);
        s->r[3] |= G(1);
        UPTO(0x1a300u);
        s->r[4] |= G(2);
        UPTO(0x1a304u);
        s->r[5] |= G(3);
        UPTO(0x1a308u);
        s->r[6] |= G(4);
        UPTO(0x1a30cu);
        DL_WORD(s->r[3]);
        UPTO(0x1a310u);
        DL_WORD(s->r[4]);
        UPTO(0x1a314u);
        DL_WORD(s->r[5]);
        UPTO(0x1a318u);
        DL_WORD(s->r[6]);
        UPTO(0x1a31cu);
        G(6) += 8;
        UPTO(0x1a320u);
        G(0) += 8;
        UPTO(0x1a324u);
        s->r[4] = (uint32_t)(int32_t)(int16_t)rd16(s, G(0));
        UPTO(0x1a328u);
        s->r[3] = 0u - 1u;
        UPTO(0x1a32cu);
        rt_cc_i(s, s->r[3], s->r[4]);
        if (s->r[3] != s->r[4]) {
            BRANCH(0x1a330u, 0x1a298u);
            continue;
        }
        UPTO(0x1a330u);
        break;
    }

    s->r[3] = rd32(s, G(10) + LIST_PTR); /* how many words went in */
    UPTO(0x1a338u);
    s->r[3] -= s->r[10];
    UPTO(0x1a33cu);
    s->r[3] >>= 2;
    UPTO(0x1a340u);
    s->r[3] -= 1;
    UPTO(0x1a344u);
    wr32(s, LIST_RAM + s->r[10], s->r[3]);
    UPTO(0x1a34cu);
    RET();
}

/* 0x1a350: scroll_texture_points. The same, for texture coordinates: each
   record says how many pairs to move by (g1, g2). The sky is drawn this way,
   its coordinates following the car's heading. */
RT_FN(f_0001a350)
{
    ENTRY(0x1a350u);

    s->r[3] = 0u - 1u;
    UPTO(0x1a354u);
    s->r[4] = (uint32_t)(int32_t)(int16_t)rd16(s, G(0));
    UPTO(0x1a358u);
    rt_cc_i(s, s->r[3], s->r[4]);
    if (s->r[3] == s->r[4]) {
        BRANCH(0x1a35cu, 0x1a418u);
        RET();
    }
    UPTO(0x1a35cu);

    G(5) = rd16(s, G(0) + 6);
    UPTO(0x1a360u);
    G(5) += TEX_POINTS;
    UPTO(0x1a368u);
    s->r[3] = 0;
    UPTO(0x1a36cu);
    DL_EXP(4, s->r[3]);
    UPTO(0x1a370u);
    DL_WORD(G(5));
    UPTO(0x1a374u);
    s->r[10] = rd32(s, G(10) + LIST_PTR);
    UPTO(0x1a37cu);
    DL_WORD(s->r[3]);                    /* the count, filled in at the end */
    UPTO(0x1a380u);

    G(4) = rd16(s, G(0) + 6);
    UPTO(0x1a384u);
    G(4) = POINTS + G(4) * 2;
    UPTO(0x1a38cu);
    BRANCH(0x1a390u, 0x1a3c4u);
    goto moved;

    for (;;) {
        G(3) = (uint32_t)(int32_t)(int16_t)rd16(s, G(0) + 6);
        UPTO(0x1a394u);
        G(3) = POINTS + G(3) * 2;
        UPTO(0x1a39cu);
        rt_cc_u(s, G(3), G(4));
        if (G(3) <= G(4)) {
            BRANCH(0x1a3a0u, 0x1a3c4u);
            goto moved;
        }
        UPTO(0x1a3a0u);
        G(3) -= G(4);                    /* the pairs in between, as they are */
        UPTO(0x1a3a4u);
        G(3) >>= 2;
        UPTO(0x1a3a8u);
        for (;;) {
            s->r[3] = rd16(s, G(4));
            UPTO(0x1a3acu);
            s->r[4] = rd16(s, G(4) + 2);
            UPTO(0x1a3b0u);
            DL_WORD(s->r[3]);
            UPTO(0x1a3b4u);
            DL_WORD(s->r[4]);
            UPTO(0x1a3b8u);
            G(4) += 4;
            UPTO(0x1a3bcu);
            rt_cc_u(s, 1, G(3));
            G(3) -= 1;
            if (s->cc & 4) {
                BRANCH(0x1a3c4u, 0x1a3a8u);
                continue;
            }
            UPTO(0x1a3c0u);
            break;
        }

moved:
        G(6) = rd16(s, G(0));            /* how many pairs to move */
        UPTO(0x1a3c8u);
        for (;;) {
            G(7) = rd16(s, G(4));
            UPTO(0x1a3ccu);
            G(8) = rd16(s, G(4) + 2);
            UPTO(0x1a3d0u);
            G(4) += 4;
            UPTO(0x1a3d4u);
            G(7) += G(1);
            UPTO(0x1a3d8u);
            G(8) += G(2);
            UPTO(0x1a3dcu);
            DL_WORD(G(7));
            UPTO(0x1a3e0u);
            DL_WORD(G(8));
            UPTO(0x1a3e4u);
            rt_cc_u(s, 1, G(6));
            G(6) -= 1;
            if (s->cc & 4) {
                BRANCH(0x1a3ecu, 0x1a3c8u);
                continue;
            }
            UPTO(0x1a3e8u);
            break;
        }

        G(0) += 8;
        UPTO(0x1a3f0u);
        s->r[4] = (uint32_t)(int32_t)(int16_t)rd16(s, G(0));
        UPTO(0x1a3f4u);
        s->r[3] = 0u - 1u;
        UPTO(0x1a3f8u);
        rt_cc_i(s, s->r[3], s->r[4]);
        if (s->r[3] != s->r[4]) {
            BRANCH(0x1a3fcu, 0x1a390u);
            continue;
        }
        UPTO(0x1a3fcu);
        break;
    }

    s->r[3] = rd32(s, G(10) + LIST_PTR);
    UPTO(0x1a404u);
    s->r[3] -= s->r[10];
    UPTO(0x1a408u);
    s->r[3] >>= 2;
    UPTO(0x1a40cu);
    s->r[3] -= 1;
    UPTO(0x1a410u);
    wr32(s, LIST_RAM + s->r[10], s->r[3]);
    UPTO(0x1a418u);
    RET();
}
