/*
 * The main loop's frame sync.
 *
 * The game runs its logic once per frame interrupt. It waits by watching
 * the counter at 0x500000 that the interrupt bumps, and that wait is also
 * what tells the emulator the CPU has nothing to do: the board patches the
 * read at 0x1394 (gamedb.inc, M2_HOOK_IDLE_COUNT) and stops the i960 for
 * the rest of the frame once it has gone round ten times.
 */
#include "decomp.h"

#define FRAME_COUNT   0x500000u
#define FRAMES_MISSED 0x5010b4u   /* counted when more than one frame passed */

/* 0x138c: wait_frame */
RT_FN(f_0000138c)
{
    ENTRY(0x138cu);

    G(0) = rd8(s, FRAME_COUNT);
    UPTO(0x1394u);
    for (;;) {
        s->r[3] = rd8(s, FRAME_COUNT);   /* 0x1394: the read the board watches */
        if (s->hook)
            s->hook(s->user);
        UPTO(0x139cu);                   /* the patched read costs nothing */
        rt_cc_i(s, s->r[3], G(0));
        if (s->r[3] != G(0)) {
            UPTO(0x13a0u);
            break;
        }
        BRANCH(0x13a0u, 0x1394u);
    }

    s->r[4] = rd8(s, FRAME_COUNT);
    UPTO(0x13a8u);
    rt_cc_u(s, 2, s->r[4]);
    if (s->r[4] >= 2) {                  /* the game didn't keep up: count it */
        UPTO(0x13acu);
        s->r[3] = rd32(s, FRAMES_MISSED);
        UPTO(0x13b4u);
        s->r[3] += 1;
        UPTO(0x13b8u);
        wr32(s, FRAMES_MISSED, s->r[3]);
        UPTO(0x13c0u);
    } else {
        BRANCH(0x13acu, 0x13c0u);
    }

    s->r[3] = 0;
    UPTO(0x13c4u);
    wr8(s, FRAME_COUNT, 0);
    UPTO(0x13ccu);
    CALL(f_00017f1c, 0x17f1cu);          /* frame_page */
    RET();
}
