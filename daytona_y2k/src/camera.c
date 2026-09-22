/*
 * Where the frame is seen from.
 *
 * The scene is drawn through whatever matrix the geometry processor holds
 * when draw_model runs, so the view is set first, by putting the matrix at
 * one of the cars: its position, then its heading, pitch and roll. Which
 * car is chosen is an index the caller keeps, and the matrix is only
 * rebuilt when that index changes.
 */
#include "decomp.h"

#define CARS        0x501220u   /* the cars, one record after another */
#define CAR_SIZE    0x8u        /* how long a record is, in the first one */
#define CAR_X       0x1cu
#define CAR_Y       0x20u
#define CAR_Z       0x24u
#define CAR_PITCH   0x28u
#define CAR_YAW     0x2au
#define CAR_ROLL    0x2cu

/* 0x6a10: set_view. g2 -> the car to look from, g3 -> the one it was last
   time (or 0 to rebuild anyway). */
RT_FN(f_00006a10)
{
    ENTRY(0x6a10u);

    rt_cc_i(s, 0, G(3));
    if (G(3) == 0) {
        BRANCH(0x6a14u, 0x6a20u);
    } else {
        UPTO(0x6a14u);
        s->r[3] = rd32(s, G(3));
        UPTO(0x6a18u);
        s->r[4] = rd32(s, G(2));
        UPTO(0x6a1cu);
        rt_cc_u(s, s->r[4], s->r[3]);
        if (s->r[4] == s->r[3]) {        /* the same car as last time */
            BRANCH(0x6a20u, 0x6ac0u);
            RET();
        }
        UPTO(0x6a20u);
    }

    G(14) = 0x1010;
    UPTO(0x6a28u);
    TGP_CMD(0x10);                       /* identity: start from the world */
    UPTO(0x6a2cu);
    G(14) = 0;
    UPTO(0x6a30u);
    s->r[4] = rd32(s, G(2));
    UPTO(0x6a34u);
    s->r[3] = (uint32_t)-1;
    rt_cc_u(s, s->r[4], s->r[3]);
    if (s->r[4] == s->r[3]) {            /* no car: leave it at identity */
        BRANCH(0x6a3cu, 0x6ac0u);
        RET();
    }
    UPTO(0x6a3cu);

    s->r[6] = rd32(s, CARS);
    UPTO(0x6a44u);
    s->r[5] = rd32(s, s->r[6] + CAR_SIZE);
    UPTO(0x6a48u);
    s->r[5] *= s->r[4];                  /* the car that index picks */
    UPTO(0x6a4cu);
    s->r[6] += s->r[5];
    UPTO(0x6a50u);

    s->r[3] = rd32(s, s->r[6] + CAR_X);
    UPTO(0x6a54u);
    s->r[4] = rd32(s, s->r[6] + CAR_Y);
    UPTO(0x6a58u);
    s->r[5] = rd32(s, s->r[6] + CAR_Z);
    UPTO(0x6a5cu);
    G(14) = 0x1212;
    UPTO(0x6a64u);
    TGP_CMD(0x12);                       /* where it stands */
    UPTO(0x6a68u);
    TGP_PARAM(s->r[3]);
    UPTO(0x6a6cu);
    TGP_PARAM(s->r[4]);
    UPTO(0x6a70u);
    TGP_PARAM(s->r[5]);
    UPTO(0x6a74u);
    G(14) = 0;
    UPTO(0x6a78u);

    s->r[3] = (uint32_t)(int32_t)(int16_t)rd16(s, s->r[6] + CAR_PITCH);
    UPTO(0x6a7cu);
    s->r[4] = (uint32_t)(int32_t)(int16_t)rd16(s, s->r[6] + CAR_YAW);
    UPTO(0x6a80u);
    s->r[5] = (uint32_t)(int32_t)(int16_t)rd16(s, s->r[6] + CAR_ROLL);
    UPTO(0x6a84u);
    G(14) = 0x1515;
    UPTO(0x6a8cu);
    TGP_CMD(0x15);                       /* which way it faces */
    UPTO(0x6a90u);
    TGP_PARAM(s->r[4]);
    UPTO(0x6a94u);
    G(14) = 0;
    G(14) = 0x1414;
    UPTO(0x6aa0u);
    TGP_CMD(0x14);                       /* how it is pitched */
    UPTO(0x6aa4u);
    TGP_PARAM(s->r[3]);
    UPTO(0x6aa8u);
    G(14) = 0;
    G(14) = 0x1616;
    UPTO(0x6ab4u);
    TGP_CMD(0x16);                       /* and rolled */
    UPTO(0x6ab8u);
    TGP_PARAM(s->r[5]);
    UPTO(0x6abcu);
    G(14) = 0;
    UPTO(0x6ac0u);
    RET();
}
