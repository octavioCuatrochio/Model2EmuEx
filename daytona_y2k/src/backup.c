/*
 * Backup RAM (0x1d00000): the settings and bookkeeping that survive
 * power-off. The game keeps a CRC over them and a second copy, so that a
 * write interrupted by the power going off can be noticed and undone.
 */
#include "decomp.h"

#define CRC_TABLE    0x239de0u   /* 256 halfwords, in ROM */

/* 0x2292d8: crc16, a bal procedure (g14 holds the return address).
   g0: first byte, g1: step between bytes, g2: how many; the CRC comes back
   in g0. It isn't cut to 16 bits between bytes, so the upper half of g0
   collects garbage: callers store the low halfword. */
RT_FN(f_002292d8)
{
    ENTRY(0x2292d8u);

    G(3) = G(14);                        /* the return address */
    G(14) = 0;
    rt_cc_i(s, G(2), 0);
    G(6) = G(0);                         /* the pointer */
    G(0) = 0;                            /* the CRC */
    G(7) = 0xff;
    if ((int32_t)G(2) <= 0) {
        BRANCH(0x2292f4u, 0x229324u);
        RET_BX(G(3));
    }
    UPTO(0x2292f4u);

    for (;;) {
        G(5) = rd8(s, G(6));
        UPTO(0x2292f8u);
        G(4) = ((G(0) >> 8) ^ G(5)) & G(7);
        UPTO(0x229304u);
        G(5) = rd16(s, CRC_TABLE + G(4) * 2);
        UPTO(0x22930cu);
        G(4) = G(0) << 8;
        G(2) -= 1;
        rt_cc_i(s, G(2), 0);
        G(6) += G(1);
        G(0) = G(4) ^ G(5);
        if ((int32_t)G(2) <= 0)
            break;
        BRANCH(0x229324u, 0x2292f4u);
    }
    UPTO(0x229324u);
    RET_BX(G(3));
}

/* 0x229230: backup_refresh. The CRC of the bookkeeping (0x1d00106, 0x7a
   bytes) goes to 0x1d00104, then the block 0x1d00100-0x1d0017f is copied
   to 0x1d00180-0x1d001ff, a quad at a time, in the machine code's order
   (the first quad is read after the CRC has gone into it). */
#define BOOKKEEPING   0x1d00100u
#define BK_CRC        0x1d00104u
#define BK_CRC_FROM   0x1d00106u
#define BK_CRC_LEN    0x7au
#define BK_COPY       0x80u      /* the copy follows the block */

RT_FN(f_00229230)
{
    ENTRY(0x229230u);

    G(0) = BK_CRC_FROM;
    G(1) = 1;
    G(2) = BK_CRC_LEN;
    UPTO(0x229240u);
    BAL(f_002292d8, 0x2292d8u);          /* crc16 */

    /* the copy: which quad goes through which registers (r4, r8, r12 or
       g4), and the one halfword store, the CRC, in between */
    static const struct { uint8_t load, reg; uint16_t offset; } step[] = {
        { 1, 4, 0x20 }, { 1, 20, 0x10 }, { 0, 4, 0xa0 },
        { 1, 4, 0x50 }, { 1, 8, 0x30 },  { 0, 4, 0xd0 },
        { 1, 4, 0x60 }, { 1, 12, 0x40 }, { 0, 4, 0xe0 },
        { 1, 4, 0x70 }, { 2, 16, 0x04 }, { 0, 20, 0x90 }, { 0, 4, 0xf0 },
        { 1, 20, 0x00 }, { 0, 8, 0xb0 }, { 0, 12, 0xc0 }, { 0, 20, 0x80 },
    };
    for (unsigned k = 0; k < sizeof step / sizeof step[0]; k++) {
        uint32_t a = BOOKKEEPING + step[k].offset;
        uint32_t *r = &s->r[step[k].reg];
        if (step[k].load == 1)
            for (int w = 0; w < 4; w++) r[w] = rd32(s, a + 4 * w);
        else if (step[k].load == 0)
            for (int w = 0; w < 4; w++) wr32(s, a + 4 * w, r[w]);
        else
            wr16(s, a, (uint16_t)r[0]);  /* stos g0: the CRC */
        UPTO(pc_ + 8);
    }
    RET();
}
