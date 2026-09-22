/*
 * Filling the renderer's tables, once, while the machine starts up.
 *
 * Texture headers, texture points, polygons and the logarithm table are all
 * loaded through the display list like anything else (../notes/geometry.md),
 * but they are far too big for one frame's list, so each of these walks its
 * table a page at a time: write the page, end the list, wait for the copro
 * to have read it (wait_two_frames), and go on with the next.
 *
 * Every one of them is called once, from the reset code and again when the
 * board is told to reload.
 */
#include "decomp.h"

#define HEADERS_ROM   0x28f0000u   /* the tables, in data ROM */
#define POINTS_ROM    0x28f5000u
#define POLYGONS_ROM  0x28e0000u
#define LOG_ROM       0x2802fa4u   /* the logarithm table, packed */
#define TEX_HEADERS   0x800000u    /* ... and where they go in the renderer */
#define TEX_POINTS    0x805000u
#define TEX_PARAMS    0x2805120u   /* texture parameters, and their coefficients */
#define TEX_COEFS     0x28051a8u

/* 0x1664: load_texture_headers. Ten pages of 2048 words, four words each. */
RT_FN(f_00001664)
{
    ENTRY(0x1664u);

    G(1) = HEADERS_ROM;
    UPTO(0x166cu);
    G(2) = 1u << 23;                     /* texture RAM, from its start */
    UPTO(0x1670u);
    G(4) = 10;                           /* pages */
    UPTO(0x1674u);

    for (;;) {
        s->r[3] = 0;
        UPTO(0x1678u);
        G(5) = 1u << 11;                 /* words in a page */
        UPTO(0x167cu);
        DL_EXP(4, s->r[3]);
        UPTO(0x1680u);
        DL_WORD(G(2));                   /* where they go */
        UPTO(0x1684u);
        DL_WORD(G(5));                   /* how many */
        UPTO(0x1688u);
        G(2) += G(5);
        UPTO(0x168cu);
        G(5) >>= 2;                      /* four words to a header */
        UPTO(0x1690u);

        for (;;) {
            s->r[3] = rd16(s, G(1));
            UPTO(0x1694u);
            s->r[4] = rd16(s, G(1) + 2);
            UPTO(0x1698u);
            s->r[5] = rd16(s, G(1) + 4);
            UPTO(0x169cu);
            s->r[6] = rd16(s, G(1) + 6);
            UPTO(0x16a0u);
            DL_WORD(s->r[3]);
            UPTO(0x16a4u);
            DL_WORD(s->r[4]);
            UPTO(0x16a8u);
            DL_WORD(s->r[5]);
            UPTO(0x16acu);
            DL_WORD(s->r[6]);
            UPTO(0x16b0u);
            G(1) += 8;
            UPTO(0x16b4u);
            rt_cc_u(s, 1, G(5));
            G(5) -= 1;
            if (s->cc & 4) {
                BRANCH(0x16bcu, 0x1690u);
                continue;
            }
            UPTO(0x16bcu);
            break;
        }

        CALL(f_00017ef0, 0x17ef0u);       /* end_display_list */
        CALL(f_000013d4, 0x13d4u);        /* wait_two_frames */
        rt_cc_u(s, 1, G(4));
        G(4) -= 1;
        if (s->cc & 4) {
            BRANCH(0x16ccu, 0x1674u);
            continue;
        }
        UPTO(0x16ccu);
        RET();
    }
}

/* 0x16d0: load_texture_points. Twenty-two pages of pairs. */
RT_FN(f_000016d0)
{
    ENTRY(0x16d0u);

    G(1) = POINTS_ROM;
    UPTO(0x16d8u);
    G(2) = TEX_POINTS;
    UPTO(0x16e0u);
    G(4) = 22;
    UPTO(0x16e4u);

    for (;;) {
        s->r[3] = 0;
        UPTO(0x16e8u);
        G(5) = 1u << 11;
        UPTO(0x16ecu);
        DL_EXP(4, s->r[3]);
        UPTO(0x16f0u);
        DL_WORD(G(2));
        UPTO(0x16f4u);
        DL_WORD(G(5));
        UPTO(0x16f8u);
        G(2) += G(5);
        UPTO(0x16fcu);
        G(5) >>= 1;                      /* two words to a point */
        UPTO(0x1700u);

        for (;;) {
            s->r[3] = rd16(s, G(1));
            UPTO(0x1704u);
            s->r[4] = rd16(s, G(1) + 2);
            UPTO(0x1708u);
            DL_WORD(s->r[3]);
            UPTO(0x170cu);
            DL_WORD(s->r[4]);
            UPTO(0x1710u);
            G(1) += 4;
            UPTO(0x1714u);
            rt_cc_u(s, 1, G(5));
            G(5) -= 1;
            if (s->cc & 4) {
                BRANCH(0x171cu, 0x1700u);
                continue;
            }
            UPTO(0x171cu);
            break;
        }

        CALL(f_00017ef0, 0x17ef0u);
        CALL(f_000013d4, 0x13d4u);
        rt_cc_u(s, 1, G(4));
        G(4) -= 1;
        if (s->cc & 4) {
            BRANCH(0x172cu, 0x16e4u);
            continue;
        }
        UPTO(0x172cu);
        RET();
    }
}

/* 0x1730: load_polygons. Sixteen pages of 0x800 words into polygon RAM
   (opcode 5), straight from data ROM. */
RT_FN(f_00001730)
{
    ENTRY(0x1730u);

    G(1) = POLYGONS_ROM;
    UPTO(0x1738u);
    G(2) = 0;
    UPTO(0x173cu);
    G(4) = 16;
    UPTO(0x1740u);

    for (;;) {
        G(5) = 0x800;
        UPTO(0x1744u);
        s->r[3] = 0;
        UPTO(0x1748u);
        DL_EXP(5, s->r[3]);
        UPTO(0x174cu);
        DL_WORD(G(2));
        UPTO(0x1750u);
        DL_WORD(G(5));
        UPTO(0x1754u);
        G(2) += G(5);
        UPTO(0x1758u);

        for (;;) {
            s->r[3] = rd32(s, G(1));
            UPTO(0x175cu);
            DL_WORD(s->r[3]);
            UPTO(0x1760u);
            G(1) += 4;
            UPTO(0x1764u);
            rt_cc_u(s, 1, G(5));
            G(5) -= 1;
            if (s->cc & 4) {
                BRANCH(0x176cu, 0x1758u);
                continue;
            }
            UPTO(0x176cu);
            break;
        }

        CALL(f_00017ef0, 0x17ef0u);
        CALL(f_000013d4, 0x13d4u);
        rt_cc_u(s, 1, G(4));
        G(4) -= 1;
        if (s->cc & 4) {
            BRANCH(0x177cu, 0x1740u);
            continue;
        }
        UPTO(0x177cu);
        RET();
    }
}

/* 0x5958: load_texture_parameters. Opcode 6: a first texture number, how
   many follow, then each one's shading parameters and its coefficient (the
   two come from separate tables). */
RT_FN(f_00005958)
{
    ENTRY(0x5958u);

    s->r[3] = 0;
    UPTO(0x595cu);
    DL_EXP(6, s->r[3]);
    UPTO(0x5960u);
    s->r[4] = TEX_PARAMS;
    UPTO(0x5968u);
    s->r[3] = rd32(s, s->r[4]);          /* which texture to start at */
    UPTO(0x596cu);
    s->r[4] += 4;
    UPTO(0x5970u);
    s->r[5] = rd32(s, s->r[4]);          /* how many */
    UPTO(0x5974u);
    s->r[4] += 4;
    UPTO(0x5978u);
    DL_WORD(s->r[3]);
    UPTO(0x597cu);
    DL_WORD(s->r[5]);
    UPTO(0x5980u);
    s->r[6] = TEX_COEFS;
    UPTO(0x5988u);

    for (;;) {
        s->r[3] = rd32(s, s->r[4]);
        UPTO(0x598cu);
        DL_WORD(s->r[3]);
        UPTO(0x5990u);
        s->r[3] = rd32(s, s->r[6]);
        UPTO(0x5994u);
        DL_WORD(s->r[3]);
        UPTO(0x5998u);
        s->r[4] += 4;
        UPTO(0x599cu);
        s->r[6] += 4;
        UPTO(0x59a0u);
        rt_cc_u(s, 1, s->r[5]);
        s->r[5] -= 1;
        if (s->cc & 4) {
            BRANCH(0x59a8u, 0x5988u);
            continue;
        }
        UPTO(0x59a8u);
        break;
    }

    CALL(f_00017ef0, 0x17ef0u);
    CALL(f_000013d4, 0x13d4u);
    RET();
}

/* 0x15b0: load_log_ram. The renderer's logarithm table, four pages of 2048
   words (opcode 0x14 takes four bytes to the word). The table is not stored
   as it is: next_log_word unpacks it. */
RT_FN(f_000015b0)
{
    ENTRY(0x15b0u);

    G(1) = LOG_ROM;
    UPTO(0x15b8u);
    G(2) = 0xb1;                         /* the first value ... */
    UPTO(0x15bcu);
    G(3) = 0;
    UPTO(0x15c0u);
    G(4) = rd32(s, G(1));                /* ... and the first packed word */
    UPTO(0x15c4u);
    G(1) += 4;
    UPTO(0x15c8u);
    G(5) = 0x5a;                         /* how long the first run is */
    UPTO(0x15ccu);
    G(6) = 16;                           /* runs left in the packed word */
    UPTO(0x15d0u);
    s->r[6] = 0;
    UPTO(0x15d4u);
    s->r[9] = 4;                         /* pages */
    UPTO(0x15d8u);

    for (;;) {
        s->r[7] = s->r[6] << 2;          /* where this page goes, in bytes */
        UPTO(0x15dcu);
        s->r[8] = 1u << 11;
        UPTO(0x15e0u);
        s->r[6] += s->r[8];
        UPTO(0x15e4u);
        s->r[3] = 0;
        UPTO(0x15e8u);
        DL_EXP(0x14, s->r[3]);
        UPTO(0x15ecu);
        DL_WORD(s->r[7]);
        UPTO(0x15f0u);
        DL_WORD(s->r[8]);
        UPTO(0x15f4u);

        for (;;) {
            CALL(f_00001614, 0x1614u);    /* next_log_word */
            rt_cc_u(s, 1, s->r[8]);
            s->r[8] -= 1;
            if (s->cc & 4) {
                BRANCH(0x1600u, 0x15f4u);
                continue;
            }
            UPTO(0x1600u);
            break;
        }

        CALL(f_00017ef0, 0x17ef0u);
        CALL(f_000013d4, 0x13d4u);
        rt_cc_u(s, 1, s->r[9]);
        s->r[9] -= 1;
        if (s->cc & 4) {
            BRANCH(0x1610u, 0x15d8u);
            continue;
        }
        UPTO(0x1610u);
        RET();
    }
}

/* 0x1614: next_log_word. One word of the logarithm table, four bytes at a
   time. The table is a run-length encoding: a value (g3) that goes up, and a
   run (g5) saying how many entries keep it; the amount it goes up by is two
   bits of a packed word (g4), sixteen of them to a word, read from g1. */
RT_FN(f_00001614)
{
    ENTRY(0x1614u);

    s->r[14] = 0;
    UPTO(0x1618u);
    s->r[11] = 4;                        /* the four bytes of the word */
    UPTO(0x161cu);

    for (;;) {
        rt_cc_u(s, 1, G(5));
        G(5) -= 1;
        if (s->cc & 4) {                 /* the run goes on */
            BRANCH(0x1624u, 0x164cu);
            goto pack;
        }
        UPTO(0x1624u);
        s->r[13] = G(4) & 3;             /* how much the next run goes up by */
        UPTO(0x1628u);
        G(2) += s->r[13];
        UPTO(0x162cu);
        G(5) = G(2);                     /* ... and how long it is */
        UPTO(0x1630u);
        G(3) += 1;
        UPTO(0x1634u);
        G(4) >>= 2;
        UPTO(0x1638u);
        rt_cc_u(s, 1, G(6));
        G(6) -= 1;
        if (s->cc & 4) {                 /* the packed word still has some */
            BRANCH(0x1640u, 0x164cu);
            goto pack;
        }
        UPTO(0x1640u);
        G(6) = 16;
        UPTO(0x1644u);
        G(4) = rd32(s, G(1));
        UPTO(0x1648u);
        G(1) += 4;
        UPTO(0x164cu);

pack:
        s->r[14] |= G(3);
        UPTO(0x1650u);
        s->r[14] = rt_rotate(24, s->r[14]);
        UPTO(0x1654u);
        rt_cc_u(s, 1, s->r[11]);
        s->r[11] -= 1;
        if (s->cc & 4) {
            BRANCH(0x165cu, 0x161cu);
            continue;
        }
        UPTO(0x165cu);
        break;
    }

    DL_WORD(s->r[14]);
    UPTO(0x1660u);
    RET();
}
