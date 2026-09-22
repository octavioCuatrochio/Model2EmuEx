/*
 * The cabinet's switches: start, view buttons, coin, service, test.
 *
 * They come from the I/O board as active-low bytes, and the game keeps
 * three words: the buttons held now, the ones pressed since the last frame
 * and the ones released. Everything that reacts to a button reads one of
 * those (0x5fe11c, for instance, is what the menus watch).
 */
#include "decomp.h"

#define IO_SWITCHES_LO 0x1c00010u
#define IO_SWITCHES_HI 0x1c0001cu
#define IO_SWITCHES_2  0x1c00012u
#define IO_SWITCHES_3  0x1c00014u

#define BUTTONS_HELD   0x5fe5ecu   /* active high, this frame */
#define BUTTONS_LAST   0x5fe5f0u   /* held last frame */
#define BUTTONS_DOWN   0x5fe11cu   /* pressed since last frame */
#define BUTTONS_UP     0x5fe5e8u   /* released since last frame */

#define SWITCHES_HELD  0x5fe120u   /* the same for all four ports */
#define SWITCHES_LAST  0x5fe128u
#define SWITCHES_DOWN  0x5fe584u
#define SWITCHES_UP    0x5fe124u

#define BUTTONS_SUSPEND 0x404u     /* bits 2 and 10: service and test */
#define SUSPENDED       0x5fe606u

/* 0x1d8e8: read_buttons, a bal procedure (g14 holds the return address) */
RT_FN(f_0001d8e8)
{
    ENTRY(0x1d8e8u);

    G(0) = G(14);
    G(14) = 0;
    UPTO(0x1d8f0u);
    G(5) = rd8(s, IO_SWITCHES_LO);
    UPTO(0x1d8f8u);
    G(4) = rd8(s, IO_SWITCHES_HI);
    UPTO(0x1d900u);
    G(7) = 0xff;
    UPTO(0x1d904u);
    G(6) = rd32(s, BUTTONS_HELD);         /* last frame's */
    UPTO(0x1d90cu);
    G(5) = (G(5) & G(7)) | ((G(4) & G(7)) << 8);   /* raw, active low */
    G(4) = ~G(5);                         /* held */
    UPTO(0x1d920u);
    wr32(s, BUTTONS_HELD, G(4));
    UPTO(0x1d928u);
    G(4) &= ~G(6);                        /* pressed since last frame */
    UPTO(0x1d92cu);
    wr32(s, BUTTONS_DOWN, G(4));
    UPTO(0x1d934u);
    G(5) &= G(6);                         /* released since last frame */
    UPTO(0x1d938u);
    wr32(s, BUTTONS_LAST, G(6));
    UPTO(0x1d940u);
    wr32(s, BUTTONS_UP, G(5));
    UPTO(0x1d948u);
    RET_BX(G(0));
}

/* 0x1d950: read_switches, the same for all four switch ports at once. It
   ends with a jump to a lone ret at 0x1d9e0. */
RT_FN(f_0001d950)
{
    ENTRY(0x1d950u);

    G(14) = 0x1d9e0u;
    G(0) = G(14);
    G(14) = 0;
    UPTO(0x1d960u);
    G(5) = rd8(s, IO_SWITCHES_LO);
    UPTO(0x1d968u);
    G(4) = rd8(s, IO_SWITCHES_HI);
    UPTO(0x1d970u);
    G(7) = 0xff;
    G(5) = (G(5) & G(7)) | ((G(4) & G(7)) << 8);
    UPTO(0x1d984u);
    G(4) = rd8(s, IO_SWITCHES_2);
    UPTO(0x1d98cu);
    G(5) |= (G(7) & G(4)) << 16;
    UPTO(0x1d998u);
    G(4) = rd8(s, IO_SWITCHES_3);
    UPTO(0x1d9a0u);
    G(6) = rd32(s, SWITCHES_HELD);
    UPTO(0x1d9a8u);
    G(5) |= G(4) << 24;
    G(4) = ~G(5);
    UPTO(0x1d9b4u);
    wr32(s, SWITCHES_HELD, G(4));
    UPTO(0x1d9bcu);
    G(4) &= ~G(6);
    UPTO(0x1d9c0u);
    wr32(s, SWITCHES_DOWN, G(4));
    UPTO(0x1d9c8u);
    G(5) &= G(6);
    UPTO(0x1d9ccu);
    wr32(s, SWITCHES_LAST, G(6));
    UPTO(0x1d9d4u);
    wr32(s, SWITCHES_UP, G(5));
    UPTO(0x1d9dcu);
    BRANCH(0x1d9e0u, 0x1d9e0u);           /* bx (g0) */
    RET();
}

/* 0x1db08: the service and test buttons together hold the game (the frame
   work stops until the flag is cleared again). A bal procedure. */
RT_FN(f_0001db08)
{
    ENTRY(0x1db08u);

    G(0) = G(14);
    G(14) = 0;
    UPTO(0x1db10u);
    G(4) = rd16(s, BUTTONS_DOWN);
    UPTO(0x1db18u);
    G(5) = BUTTONS_SUSPEND;
    G(4) &= G(5);
    rt_cc_i(s, 0, G(4));
    if (G(4)) {
        UPTO(0x1db24u);
        G(5) = 1;
        UPTO(0x1db28u);
        wr8(s, SUSPENDED, 1);
        UPTO(0x1db30u);
    } else {
        BRANCH(0x1db24u, 0x1db30u);
    }
    RET_BX(G(0));
}
