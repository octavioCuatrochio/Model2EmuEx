/*
 * Which of the two picture buffers the geometry processor works on.
 *
 * The board toggles bit 2 of the copro status (0x98000c) on every frame it
 * displays. The game follows it: the buffer it builds the next frame's
 * display list in is the other one, and the geometry processor is told
 * through its control register at 0x1008 (g10 holds 0x800000, its base).
 */
#include "decomp.h"

#define COPRO_BASE     0x980000u
#define COPRO_STATUS   0xcu        /* offset of the status register */
#define COPRO_PAGE     0x1008u     /* offset in the copro's registers (g10) */
#define STATUS_SHADOW  0x500004u
#define PAGE_SHADOW    0x501368u
#define PAGE_SIZE      0x10000u    /* the second buffer starts here */

/* 0x17f1c: frame_page */
RT_FN(f_00017f1c)
{
    ENTRY(0x17f1cu);

    s->r[4] = COPRO_BASE;
    UPTO(0x17f24u);
    s->r[7] = rd32(s, COPRO_BASE + COPRO_STATUS);
    UPTO(0x17f28u);
    wr8(s, STATUS_SHADOW, (uint8_t)s->r[7]);
    UPTO(0x17f30u);

    if (s->r[7] & 4) {
        s->cc = 2;
        BRANCH(0x17f34u, 0x17f3cu);
        s->r[3] = PAGE_SIZE;
        UPTO(0x17f44u);
    } else {
        s->cc = 0;
        UPTO(0x17f34u);
        s->r[3] = 0;
        BRANCH(0x17f3cu, 0x17f44u);
    }

    wr32(s, PAGE_SHADOW, s->r[3]);
    UPTO(0x17f4cu);
    wr32(s, G(10) + COPRO_PAGE, s->r[3]);
    UPTO(0x17f54u);
    RET();
}
