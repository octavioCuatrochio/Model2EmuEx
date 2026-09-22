/*
 * Bookkeeping: the counters the operator reads in test mode.
 *
 * Every frame the game counts time, and keeps what matters in backup RAM
 * (0x1d00000), which survives a power cycle.
 */
#include "decomp.h"

#define FRAMES_TOTAL  0x5fe600u   /* frames since power-on */
#define BACKUP_FRAMES 0x1d00108u  /* ... saved to backup RAM every 16 */
#define TIMERS        0x5fe5b0u   /* four counters that run while their flag is set */
#define TIMERS_B      0x5fe5c0u   /* three more, and the flags */
#define TIMER_FLAGS   0x5fe5ccu   /* bits 0-6: which counters run; bit 7: set up */
#define TIMERS_SIZE   28u

/* 0x1db40: bookkeeping_tick, run every frame from frame_io_update */
RT_FN(f_0001db40)
{
    ENTRY(0x1db40u);

    G(4) = rd32(s, FRAMES_TOTAL);
    UPTO(0x1db48u);
    G(5) = G(4) + 1;
    UPTO(0x1db4cu);
    wr32(s, FRAMES_TOTAL, G(5));
    UPTO(0x1db54u);
    G(4) = rd8(s, FRAMES_TOTAL);
    UPTO(0x1db5cu);
    G(6) = 0xff;
    G(4) = G(4) & 15 & G(6);
    rt_cc_i(s, 0, G(4));
    if (G(4) == 0) {                     /* every 16 frames */
        UPTO(0x1db6cu);
        wr32(s, BACKUP_FRAMES, G(5));
        UPTO(0x1db74u);
        CALL(f_00229230, 0x229230u);     /* refresh the backup copy of the bookkeeping */
    } else {
        BRANCH(0x1db6cu, 0x1db78u);
    }

    G(4) = rd8(s, TIMER_FLAGS);
    UPTO(0x1db80u);
    if (!(G(4) & 0x80)) {
        /* the first time: clear the counters (memset), and say so */
        s->cc = 0;
        UPTO(0x1db84u);
        G(4) = rd32(s, TIMER_FLAGS);
        UPTO(0x1db8cu);
        G(0) = TIMERS;
        G(1) = 0;
        G(4) |= 0x80;
        G(2) = TIMERS_SIZE;
        UPTO(0x1dba0u);
        wr32(s, TIMER_FLAGS, G(4));
        UPTO(0x1dba8u);
        BRANCH(0x1dbacu, 0x2301d0u);
        TAIL(f_002301d0);
    }
    s->cc = 2;
    BRANCH(0x1db84u, 0x1dbacu);

    G(4) = 0;
    G(5) = TIMERS_B;
    UPTO(0x1dbb8u);
    for (int k = 0; k < 4; k++)          /* ldq: three counters and the flags */
        G(k) = rd32(s, TIMERS_B + 4 * k);
    UPTO(0x1dbbcu);

    /* counters 0-3 live in memory */
    for (uint32_t bit = 0; bit < 4; bit++) {
        uint32_t at = 0x1dbbcu + 0x18u * bit;   /* bbc bit,g3 */
        if (!(G(3) & (1u << bit))) {
            s->cc = 2;
            BRANCH(at + 4, at + 0x18);
            continue;
        }
        s->cc = 0;
        UPTO(at + 4);
        G(4) = rd32(s, TIMERS + 4 * bit);
        UPTO(at + 12);
        G(4) += 1;
        UPTO(at + 16);
        wr32(s, TIMERS + 4 * bit, G(4));
        UPTO(at + 0x18);
    }
    /* counters 4-6 are the words just loaded */
    for (uint32_t bit = 4; bit < 7; bit++) {
        uint32_t at = 0x1dc1cu + 8 * (bit - 4);   /* bbc bit,g3 */
        if (!(G(3) & (1u << bit))) {
            s->cc = 2;
            BRANCH(at + 4, at + 8);
            continue;
        }
        s->cc = 0;
        G(bit - 4) += 1;
        UPTO(at + 8);
    }

    for (int k = 0; k < 4; k++)          /* stq */
        wr32(s, G(5) + 4 * k, G(k));
    UPTO(0x1dc38u);
    RET();
}
