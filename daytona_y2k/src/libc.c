/*
 * The game's own C-library routines.
 */
#include "decomp.h"

/* 0x2301d0: memset(g0 = address, g1 = byte, g2 = count).
   Bytes until the address is 16-aligned, then 16 bytes at a time as words,
   then the bytes left. Leaves g6 past the end and g7 at 0. */
RT_FN(f_002301d0)
{
    ENTRY(0x2301d0u);

    G(3) = 0xff;
    G(4) = G(0) & 15 & G(3);
    rt_cc_i(s, G(4), 0);
    G(6) = G(0);                         /* where */
    G(7) = G(2);                         /* how many left */

    /* up to 16-alignment, a byte at a time */
    if (G(4) == 0) {
        BRANCH(0x2301ecu, 0x230210u);
    } else {
        UPTO(0x2301ecu);
        rt_cc_i(s, 0, G(2));
        if (G(2) == 0) {
            BRANCH(0x2301f0u, 0x230210u);
        } else {
            UPTO(0x2301f0u);
            for (;;) {
                wr8(s, G(6), (uint8_t)G(1));
                UPTO(0x2301f4u);
                G(6) += 1;
                G(4) = G(6) & 15 & G(3);
                rt_cc_i(s, G(4), 0);
                G(7) -= 1;
                if (G(4) == 0) {
                    BRANCH(0x23020cu, 0x230210u);
                    break;
                }
                UPTO(0x23020cu);
                rt_cc_i(s, 0, G(7));
                if (G(7) == 0) {
                    UPTO(0x230210u);
                    break;
                }
                BRANCH(0x230210u, 0x2301f0u);
            }
        }
    }

    rt_cc_i(s, 0, G(7));
    if (G(7) == 0) {
        UPTO(0x230214u);
        RET();
    }
    BRANCH(0x230214u, 0x230218u);

    /* 16 bytes at a time, the byte repeated in a word */
    rt_cc_u(s, 15, G(7));
    if (G(7) > 15) {
        UPTO(0x23021cu);
        G(4) = G(1) & G(3);
        G(5) = (G(4) << 8) | G(4);
        G(4) = (G(5) << 16) | G(5);
        UPTO(0x230230u);
        for (;;) {
            wr32(s, G(6), G(4));
            UPTO(0x230234u);
            wr32(s, G(6) + 4, G(4));
            UPTO(0x230238u);
            wr32(s, G(6) + 8, G(4));
            UPTO(0x23023cu);
            G(7) -= 16;
            UPTO(0x230240u);
            wr32(s, G(6) + 12, G(4));
            UPTO(0x230244u);
            rt_cc_u(s, G(7), 15);
            G(6) += 16;
            if (G(7) <= 15) {
                UPTO(0x230250u);
                break;
            }
            BRANCH(0x230250u, 0x230230u);
        }
    } else {
        BRANCH(0x23021cu, 0x230250u);
    }

    /* the rest, a byte at a time */
    rt_cc_i(s, 0, G(7));
    if (G(7) == 0) {
        BRANCH(0x230254u, 0x230268u);
        RET();
    }
    UPTO(0x230254u);
    for (;;) {
        G(7) -= 1;
        UPTO(0x230258u);
        wr8(s, G(6), (uint8_t)G(1));
        UPTO(0x23025cu);
        rt_cc_i(s, G(7), 0);
        G(6) += 1;
        if (G(7) == 0) {
            UPTO(0x230268u);
            break;
        }
        BRANCH(0x230268u, 0x230254u);
    }
    RET();
}
