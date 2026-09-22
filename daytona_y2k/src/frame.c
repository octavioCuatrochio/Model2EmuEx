/*
 * The frame interrupt, the game's heartbeat.
 *
 * The board raises it once per displayed frame (interrupt 0, vector from
 * ICON). Everything that has to happen every frame hangs off it, and the
 * main loop does nothing but wait for the counter it bumps.
 */
#include "decomp.h"

#define LINK_STATUS   0x1a14002u   /* network board, status byte */
#define LINK_SHADOW   0x540054u    /* where the game keeps it */
#define FRAME_COUNT   0x500000u    /* bumped here, waited on by wait_frame */
#define IRQ_PENDING   0xe80000u    /* writing clears the bits that are 0 */
#define IRQ_ACK_FRAME 0xfffffffeu  /* ... so this acknowledges interrupt 0 */

/* 0xe00: frame_interrupt */
RT_FN(f_00000e00)
{
    ENTRY(0xe00u);

    s->r[3] = rd8(s, LINK_STATUS);
    UPTO(0xe08u);
    wr8(s, LINK_SHADOW, (uint8_t)s->r[3]);
    UPTO(0xe10u);

    /* the globals belong to the interrupted code: save them on the stack,
       g15 (the frame pointer) included */
    for (int g = 0; g < 16; g += 4) {
        for (int k = 0; k < 4; k++)
            wr32(s, s->r[1] + 4 * k, G(g + k));
        UPTO(pc_ + 4);
        s->r[1] += 16;
        UPTO(pc_ + 4);
    }
    G(14) = 0;
    UPTO(0xe34u);

    CALL(f_0001d750, 0x1d750u);   /* frame_io_update: watchdog, counters, link */
    CALL(f_0001b9a0, 0x1b9a0u);   /* frame_tasks: the frame hook, the scroll registers */

    s->r[5] = (uint32_t)(int8_t)rd8(s, FRAME_COUNT);
    UPTO(0xe44u);
    s->r[5] += 1;
    UPTO(0xe48u);
    wr8(s, FRAME_COUNT, (uint8_t)s->r[5]);
    UPTO(0xe50u);

    for (int g = 12; g >= 0; g -= 4) {
        s->r[1] -= 16;
        UPTO(pc_ + 4);
        for (int k = 0; k < 4; k++)
            G(g + k) = rd32(s, s->r[1] + 4 * k);
        UPTO(pc_ + 4);
    }

    s->r[4] = IRQ_PENDING;
    s->r[5] = IRQ_ACK_FRAME;
    UPTO(0xe7cu);
    wr32(s, IRQ_PENDING, IRQ_ACK_FRAME);
    UPTO(0xe80u);
    RET();
}
