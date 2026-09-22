/*
 * The object list.
 *
 * Everything the game has on the go — cars, the course, the screens, the
 * effects — is a record in a list at 0x504000. Each record says whether it
 * is active, how long it is, and what to call for it. That call is what
 * draws it, and it gets the record in g13, which is why the drawing
 * functions read the object through g13.
 */
#include "decomp.h"

#define OBJECTS      0x504000u   /* the list */
#define OBJECT_LEFT  0x5011fcu   /* how many are still to come this pass */
#define OBJECT_TOTAL 0x2802444u  /* how many there are (in data ROM) */

#define REC_FLAGS    0x0u        /* bit 31: active */
#define REC_SIZE     0x8u
#define REC_RUN      0xcu        /* -> what to call for it */

/* 0x1900: run_objects */
RT_FN(f_00001900)
{
    ENTRY(0x1900u);

    s->r[13] = G(13);                    /* the caller's object */
    G(13) = OBJECTS;
    UPTO(0x190cu);
    s->r[3] = rd32(s, OBJECT_TOTAL);
    UPTO(0x1914u);
    wr32(s, OBJECT_LEFT, s->r[3]);
    UPTO(0x191cu);

    for (;;) {
        s->r[4] = rd32(s, G(13) + REC_FLAGS);
        UPTO(0x1920u);
        if (!(s->r[4] & (1u << 31))) {   /* not active: skip it */
            s->cc = 2;
            BRANCH(0x1924u, 0x192cu);
        } else {
            s->cc = 0;
            UPTO(0x1924u);
            s->r[5] = rd32(s, G(13) + REC_RUN);
            UPTO(0x1928u);
            CALLX(s->r[5]);
        }

        s->r[6] = rd32(s, OBJECT_LEFT);
        UPTO(0x1934u);
        s->r[6] -= 1;
        UPTO(0x1938u);
        wr32(s, OBJECT_LEFT, s->r[6]);
        UPTO(0x1940u);
        rt_cc_i(s, 0, s->r[6]);
        if ((int32_t)s->r[6] <= 0) {
            BRANCH(0x1948u, 0x1954u);
            break;
        }
        UPTO(0x1948u);
        s->r[7] = rd32(s, G(13) + REC_SIZE);
        UPTO(0x194cu);
        G(13) += s->r[7];
        BRANCH(0x1954u, 0x191cu);
    }
    G(13) = s->r[13];
    UPTO(0x1958u);
    RET();
}
