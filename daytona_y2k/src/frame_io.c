/*
 * The first half of the frame interrupt: the housekeeping that happens
 * whatever the game is doing.
 *
 * It kicks the watchdog on the I/O board, counts the frame, reads the
 * cabinet's switches, and — unless the game has been held, which the
 * service and test buttons do — runs the four jobs that have to happen
 * every frame whatever screen is up.
 */
#include "decomp.h"

#define IO_WATCHDOG    0x1c00040u
#define IO_FRAME_COUNT 0x5fe610u   /* a byte, wraps every 256 frames */
#define SUSPENDED      0x5fe606u   /* set while the game is held (service, test) */
#define IRQ_PENDING    0xe80000u
#define IRQ_ACK_FRAME  0xfffffffeu

#define FLAG_5FE5FC    0x5fe5fcu   /* cleared once the machine has run a frame */
#define FLAG_5FE5F8    0x5fe5f8u   /* ... and this one set in its place */

/* 0x1d750: frame_io_update */
RT_FN(f_0001d750)
{
    ENTRY(0x1d750u);

    /* the interrupted code's globals, kept in this frame's locals */
    for (int k = 0; k < 4; k++) {
        s->r[4 + k] = G(k);
        s->r[8 + k] = G(4 + k);
    }
    s->r[12] = G(13);
    s->r[13] = G(14);
    G(14) = 0;
    UPTO(0x1d764u);

    G(4) = rd8(s, IO_WATCHDOG);
    UPTO(0x1d76cu);
    rt_cc_i(s, 0, G(4));
    if (!G(4)) {                          /* it answers 0 until it is kicked */
        UPTO(0x1d770u);
        G(5) = 1;
        UPTO(0x1d774u);
        wr8(s, IO_WATCHDOG, 1);
        UPTO(0x1d77cu);
    } else {
        BRANCH(0x1d770u, 0x1d77cu);
    }

    G(4) = rd8(s, IO_FRAME_COUNT);
    UPTO(0x1d784u);
    G(4) += 1;
    UPTO(0x1d788u);
    wr8(s, IO_FRAME_COUNT, (uint8_t)G(4));
    UPTO(0x1d790u);
    G(4) = rd8(s, IO_FRAME_COUNT);
    UPTO(0x1d798u);
    G(5) = 0xff;
    G(4) &= G(5);
    rt_cc_u(s, 1, G(4));
    if (G(4) > 1) {                       /* not the very first frame */
        UPTO(0x1d7a4u);
        G(4) = rd8(s, FLAG_5FE5FC);
        UPTO(0x1d7acu);
        rt_cc_i(s, 0, G(4));
        if (G(4)) {
            UPTO(0x1d7b0u);
            wr8(s, FLAG_5FE5FC, (uint8_t)G(14));
            UPTO(0x1d7b8u);
            G(5) = 1;
            UPTO(0x1d7bcu);
            wr8(s, FLAG_5FE5F8, 1);
            UPTO(0x1d7c4u);
        } else {
            BRANCH(0x1d7b0u, 0x1d7c4u);
        }
    } else {
        BRANCH(0x1d7a4u, 0x1d7c4u);
    }

    BAL(f_0001d8e8, 0x1d8e8u);            /* read_buttons */

    G(4) = rd8(s, SUSPENDED);
    UPTO(0x1d7d0u);
    rt_cc_i(s, 0, G(4));
    if (!G(4)) {
        UPTO(0x1d7d4u);
        BAL(f_0001db08, 0x1db08u);        /* check_suspend */
        CALL(f_0001db40, 0x1db40u);
        CALL(f_00229440, 0x229440u);
        CALL(f_0001de40, 0x1de40u);
    } else {
        BRANCH(0x1d7d4u, 0x1d7e4u);
    }

    G(5) = IRQ_ACK_FRAME;
    UPTO(0x1d7e8u);
    wr32(s, IRQ_PENDING, IRQ_ACK_FRAME);
    UPTO(0x1d7f0u);

    for (int k = 0; k < 4; k++) {
        G(k) = s->r[4 + k];
        G(4 + k) = s->r[8 + k];
    }
    G(13) = s->r[12];
    G(14) = s->r[13];
    UPTO(0x1d800u);
    RET();
}
