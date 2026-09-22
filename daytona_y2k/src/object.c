/*
 * Cars and other moving objects. Each one has its world position as three
 * floats at 0x1c (x), 0x20 (y) and 0x24 (z) of its state, and g13 points at
 * the one being worked on.
 */
#include "decomp.h"

#define OBJ_X     0x1cu
#define OBJ_Z     0x24u
#define OBJ_CELL  0x2eu    /* the grid cell below, x in bits 0-3, z in 4-7 */

/* 0x172a0: which cell of the 16x16 grid the object is in. The grid covers
   -1024..+1023 in x and z, 128 units to a cell, and wraps outside that. */
RT_FN(f_000172a0)
{
    ENTRY(0x172a0u);

    s->r[3] = rd32(s, G(13) + OBJ_X);
    UPTO(0x172a4u);
    G(0) = (uint32_t)rt_round_ac(s, rdF(&s->r[3]));
    G(0) = ((G(0) + 1024) >> 7) & 15;
    s->r[3] = 15;
    UPTO(0x172bcu);

    s->r[3] = rd32(s, G(13) + OBJ_Z);
    UPTO(0x172c0u);
    G(1) = (uint32_t)rt_round_ac(s, rdF(&s->r[3]));
    G(1) = (((G(1) + 1024) >> 7) & 15) << 4;
    s->r[3] = 15;
    G(0) += G(1);
    UPTO(0x172e0u);

    wr16(s, G(13) + OBJ_CELL, (uint16_t)G(0));
    UPTO(0x172e4u);
    RET();
}
