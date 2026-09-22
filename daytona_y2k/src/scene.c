/*
 * Walking the scene: what goes into a frame's display list, and in which
 * order. Each step leaves the geometry processor's matrix as it found it.
 */
#include "decomp.h"

#define STATE_FLAGS   0x501090u   /* what the machine is doing; a bit per part */
#define SCENE_PARTS   0x7f80000u  /* the bits that mean "draw the world" */
#define VIEW_MODE     0x501a80u   /* 1: the two last parts are left out */


/* 0x12f1c: draw_scene */
RT_FN(f_00012f1c)
{
    ENTRY(0x12f1cu);

    s->r[4] = rd32(s, STATE_FLAGS);
    UPTO(0x12f24u);
    s->r[3] = SCENE_PARTS;
    s->r[4] &= s->r[3];
    rt_cc_u(s, 0, s->r[4]);
    if (s->r[4] == 0) {
        BRANCH(0x12f34u, 0x12f80u);
        RET();
    }
    UPTO(0x12f34u);

    G(14) = 0x505;
    UPTO(0x12f38u);
    TGP_CMD(0x05);                       /* push */
    UPTO(0x12f3cu);
    G(14) = 0;
    G(14) = 0x1010;
    UPTO(0x12f48u);
    TGP_CMD(0x10);                       /* identity: the world's own frame */
    UPTO(0x12f4cu);
    G(14) = 0;
    UPTO(0x12f50u);

    CALL(f_00013050, 0x13050u);
    CALL(f_0001308c, 0x1308cu);
    CALL(f_00013184, 0x13184u);
    CALL(f_00013250, 0x13250u);          /* draw_object */

    G(14) = 0x606;
    UPTO(0x12f64u);
    TGP_CMD(0x06);                       /* pop */
    UPTO(0x12f68u);
    G(14) = 0;
    UPTO(0x12f6cu);

    s->r[3] = rd32(s, VIEW_MODE);
    UPTO(0x12f74u);
    rt_cc_i(s, 1, s->r[3]);
    if (s->r[3] == 1) {
        BRANCH(0x12f78u, 0x12f80u);
        RET();
    }
    UPTO(0x12f78u);
    CALL(f_000133ac, 0x133acu);
    CALL(f_000134c8, 0x134c8u);
    RET();
}

/* 0x13050: a switch on the cabinet picks one of sixteen extra views (the
   test switches: bit 4 turns it on, bits 5-7 and 16 pick which). */
#define SWITCHES_HELD 0x5fe120u
#define VIEW_BITS     0x100e0u    /* the bits that choose */
#define VIEW_TABLE    0x1382cu    /* -> the function that draws that view */

RT_FN(f_00013050)
{
    ENTRY(0x13050u);

    s->r[3] = rd32(s, SWITCHES_HELD);
    UPTO(0x13058u);
    if (!(s->r[3] & (1u << 4))) {
        s->cc = 2;
        BRANCH(0x1305cu, 0x13088u);
        RET();
    }
    s->cc = 0;
    UPTO(0x1305cu);

    s->r[4] = VIEW_BITS;
    s->r[3] &= s->r[4];
    s->r[4] = (s->r[3] >> 5) & 7;
    s->r[3] = ((s->r[3] >> 13) | s->r[4]) & 15;
    UPTO(0x1307cu);
    s->r[3] = rd32(s, VIEW_TABLE + s->r[3] * 4);
    UPTO(0x13084u);
    CALLX(s->r[3]);
    RET();
}

/* 0x133ac: draw_course_map, the map in the corner of the screen.
   It sets up a view of its own — identity, a small offset in front of the
   camera, a quarter turn about x so the course is seen from above — draws
   the course outline, and leaves that view in place, scaled, for the car
   markers that draw_map_cars puts on it. */
#define MAP_X       0x3f70a3d7u  /* 0.94f  */
#define MAP_Y       0x3ec28f5cu  /* 0.38f  */
#define MAP_Z       0x3fc00000u  /* 1.5f   */
#define MAP_SX      0x3cbc6a7fu  /* 0.023f */
#define MAP_SY      0x3cdd2f1bu  /* 0.027f */
#define MAP_SZ      0x3cc710cbu  /* 0.0243f */
#define MAP_MODEL   0x2843804u   /* the course outline */
#define MAP_KIND    0x4bu        /* of the object (g13) */
#define MAP_SIZES   0x280533cu   /* the marker size for that kind */
#define MAP_SCALE   0x34u        /* the size now, easing towards it */
#define EASE_MAP    0x3e4ccccdu  /* 0.2f */

RT_FN(f_000133ac)
{
    ENTRY(0x133acu);

    G(14) = 0x505;
    UPTO(0x133b0u);
    TGP_CMD(0x05);                       /* push */
    UPTO(0x133b4u);
    G(14) = 0;
    G(14) = 0x1010;
    UPTO(0x133c0u);
    TGP_CMD(0x10);                       /* identity: a view of its own */
    UPTO(0x133c4u);
    G(14) = 0;
    s->r[3] = MAP_X;
    s->r[4] = MAP_Y;
    s->r[5] = MAP_Z;
    G(14) = 0x1212;
    UPTO(0x133e8u);
    TGP_CMD(0x12);                       /* in front of the camera */
    UPTO(0x133ecu);
    TGP_PARAM(s->r[3]);
    UPTO(0x133f0u);
    TGP_PARAM(s->r[4]);
    UPTO(0x133f4u);
    TGP_PARAM(s->r[5]);
    UPTO(0x133f8u);
    G(14) = 0;
    s->r[3] = 1u << 14;                  /* a quarter turn: seen from above */
    G(14) = 0x1414;
    UPTO(0x13408u);
    TGP_CMD(0x14);                       /* rotate about x */
    UPTO(0x1340cu);
    TGP_PARAM(s->r[3]);
    UPTO(0x13410u);
    G(14) = 0;
    G(14) = 0x505;
    UPTO(0x13418u);
    TGP_CMD(0x05);                       /* push: the outline's own scale */
    UPTO(0x1341cu);
    G(14) = 0;
    s->r[4] = MAP_SX;
    s->r[5] = MAP_SY;
    s->r[6] = MAP_SZ;
    G(14) = 0x1313;
    UPTO(0x13440u);
    TGP_CMD(0x13);                       /* scale */
    UPTO(0x13444u);
    TGP_PARAM(s->r[4]);
    UPTO(0x13448u);
    TGP_PARAM(s->r[5]);
    UPTO(0x1344cu);
    TGP_PARAM(s->r[6]);
    UPTO(0x13450u);
    G(14) = 0;
    G(0) = MAP_MODEL;
    UPTO(0x1345cu);
    CALL(f_00017acc, 0x17accu);          /* draw_model: the course outline */
    G(14) = 0x606;
    UPTO(0x13464u);
    TGP_CMD(0x06);                       /* pop: back to the map's view */
    UPTO(0x13468u);
    G(14) = 0;
    UPTO(0x1346cu);

    G(0) = rd8(s, G(13) + MAP_KIND);
    UPTO(0x13470u);
    G(0) = rd32(s, MAP_SIZES + G(0) * 4);        /* how big the markers go */
    UPTO(0x13478u);
    G(2) = rd32(s, G(13) + MAP_SCALE);
    UPTO(0x1347cu);
    s->r[4] = EASE_MAP;
    wrF(&G(1), rdF(&G(0)) - rdF(&G(2)));
    UPTO(0x13488u);
    G(14) = 0x202;
    UPTO(0x1348cu);
    TGP_CMD(0x02);                               /* multiply */
    UPTO(0x13490u);
    TGP_PARAM(s->r[4]);
    UPTO(0x13494u);
    TGP_PARAM(G(1));
    UPTO(0x13498u);
    G(1) = TGP_RESULT();
    UPTO(0x1349cu);
    G(14) = 0;
    wrF(&G(2), rdF(&G(2)) + rdF(&G(1)));
    UPTO(0x134a4u);
    wr32(s, G(13) + MAP_SCALE, G(2));
    UPTO(0x134a8u);
    G(14) = 0x1313;
    UPTO(0x134b0u);
    TGP_CMD(0x13);                               /* scale: for the markers */
    UPTO(0x134b4u);
    for (int k = 0; k < 3; k++) {
        TGP_PARAM(G(2));
        UPTO(pc_ + 4);
    }
    G(14) = 0;
    UPTO(0x134c4u);
    RET();
}
