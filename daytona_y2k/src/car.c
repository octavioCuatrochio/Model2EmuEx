/*
 * Cars on the course map (see draw_course_map in scene.c): the marker under
 * one, the other car's, and the ones around the player.
 *
 * These run inside draw_scene's push/pop, and each leaves the matrix set up
 * for the next: the marker's scale is undone through g3, which the object
 * after it scales by.
 */
#include "decomp.h"

#define VIEW_MODE    0x501a80u   /* 1: no other car */
#define PLAYER_CAR   0x501220u   /* -> the car the scene is drawn around */
#define OBJECT_LIST  0x501224u   /* -> the other objects, one after another */
#define OBJECT_COUNT 0x501084u
#define SHADOW_SET   0x501460u   /* which shadow sizes to use */
#define SHADOW_SIZES 0x280534cu  /* the target size, by kind and set */

#define OBJ_MODEL    0x14u       /* of the object being drawn (g13) */
#define OBJ_SCALE    0x38u       /* its size now, easing towards the target */
#define OBJ_SHADOW_X 0x3cu       /* where the shadow goes */
#define OBJ_SHADOW_Y 0x40u
#define OBJ_SHADOW_Z 0x44u
#define OBJ_TILT     0x48u       /* the shadow's turn */
#define OBJ_KIND     0x4au
#define CAR_X        0x1cu
#define CAR_Z        0x24u
#define CAR_ANGLE    0x58u
#define CAR_SIDE     0x6cu       /* which half of the scene the car is in */
#define CAR_MODELS   0x13794u    /* -> the model for that half */
#define EASE         0x3e4ccccdu /* 0.2f: how fast the size follows */
#define SHADOW_UNDO  0x3380d959u /* the scale draw_object is given back */

/* 0x1308c: draw_flat_marker. g13 -> the object; leaves g3 = the scale that
   undoes this one's, for whatever is drawn next. The model is laid flat
   (a quarter turn about x) and its size eases towards one from a table. */
RT_FN(f_0001308c)  /* draw_flat_marker */
{
    ENTRY(0x1308cu);

    G(6) = rd32(s, PLAYER_CAR);
    UPTO(0x13094u);
    s->r[3] = rd32(s, G(13) + OBJ_SHADOW_X);
    UPTO(0x13098u);
    s->r[4] = rd32(s, G(13) + OBJ_SHADOW_Y);
    UPTO(0x1309cu);
    s->r[5] = rd32(s, G(13) + OBJ_SHADOW_Z);
    UPTO(0x130a0u);
    G(14) = 0x1212;
    UPTO(0x130a8u);
    TGP_CMD(0x12);                       /* translate to the shadow */
    UPTO(0x130acu);
    TGP_PARAM(s->r[3]);
    UPTO(0x130b0u);
    TGP_PARAM(s->r[4]);
    UPTO(0x130b4u);
    TGP_PARAM(s->r[5]);
    UPTO(0x130b8u);
    G(14) = 0;
    UPTO(0x130bcu);

    G(0) = rd8(s, G(13) + OBJ_KIND);
    UPTO(0x130c0u);
    G(1) = rd8(s, SHADOW_SET);
    UPTO(0x130c8u);
    G(1) <<= 2;
    G(0) += G(1);
    UPTO(0x130d0u);
    G(0) = rd32(s, SHADOW_SIZES + G(0) * 4);   /* the size it should be */
    UPTO(0x130d8u);
    G(2) = rd32(s, G(13) + OBJ_SCALE);         /* the size it is */
    UPTO(0x130dcu);
    s->r[3] = EASE;
    wrF(&G(1), rdF(&G(0)) - rdF(&G(2)));       /* how far to go */
    UPTO(0x130e8u);
    G(14) = 0x202;
    UPTO(0x130ecu);
    TGP_CMD(0x02);                             /* multiply: a fifth of it */
    UPTO(0x130f0u);
    TGP_PARAM(s->r[3]);
    UPTO(0x130f4u);
    TGP_PARAM(G(1));
    UPTO(0x130f8u);
    G(1) = TGP_RESULT();
    UPTO(0x130fcu);
    G(14) = 0;
    wrF(&G(2), rdF(&G(2)) + rdF(&G(1)));
    UPTO(0x13104u);
    wr32(s, G(13) + OBJ_SCALE, G(2));
    UPTO(0x13108u);

    G(14) = 0x1313;
    UPTO(0x13110u);
    TGP_CMD(0x13);                             /* scale */
    UPTO(0x13114u);
    for (int k = 0; k < 3; k++) {
        TGP_PARAM(G(2));
        UPTO(pc_ + 4);
    }
    G(14) = 0;
    s->r[3] = 1u << 14;                        /* a quarter turn: lay it flat */
    UPTO(0x13128u);
    s->r[4] = rd32(s, G(13) + OBJ_TILT);
    UPTO(0x1312cu);
    G(14) = 0x1414;
    UPTO(0x13134u);
    TGP_CMD(0x14);                             /* rotate about x */
    UPTO(0x13138u);
    TGP_PARAM(s->r[3]);
    UPTO(0x1313cu);
    G(14) = 0;
    G(14) = 0x1515;
    UPTO(0x13148u);
    TGP_CMD(0x15);                             /* rotate about y */
    UPTO(0x1314cu);
    TGP_PARAM(s->r[4]);
    UPTO(0x13150u);
    G(14) = 0;
    UPTO(0x13154u);
    G(0) = rd32(s, G(13) + OBJ_MODEL);
    UPTO(0x13158u);
    CALL(f_00017acc, 0x17accu);                /* draw_model */

    s->r[3] = rd32(s, G(13) + OBJ_SCALE);
    UPTO(0x13160u);
    s->r[4] = SHADOW_UNDO;
    G(14) = 0x303;
    UPTO(0x1316cu);
    TGP_CMD(0x03);                             /* divide: undo the scale */
    UPTO(0x13170u);
    TGP_PARAM(s->r[4]);
    UPTO(0x13174u);
    TGP_PARAM(s->r[3]);
    UPTO(0x13178u);
    G(3) = TGP_RESULT();
    UPTO(0x1317cu);
    G(14) = 0;
    UPTO(0x13180u);
    RET();
}

/* 0x13184: draw_matching_object. Walks the objects after the player's and
   draws the first one on the same side of the course, at g3's scale. */
RT_FN(f_00013184)
{
    ENTRY(0x13184u);

    s->r[3] = rd32(s, VIEW_MODE);
    UPTO(0x1318cu);
    rt_cc_i(s, 1, s->r[3]);
    if (s->r[3] == 1) {
        BRANCH(0x13190u, 0x1324cu);
        RET();
    }
    UPTO(0x13190u);
    s->r[10] = 0;
    UPTO(0x13194u);
    s->r[4] = rd8(s, G(6) + CAR_SIDE);
    UPTO(0x13198u);
    rt_cc_u(s, 0, s->r[4]);
    if (s->r[4] != 0) {
        BRANCH(0x1319cu, 0x131a0u);
    } else {
        UPTO(0x1319cu);
        s->r[10] += 1;
        UPTO(0x131a0u);
    }
    G(9) = rd32(s, OBJECT_LIST);
    UPTO(0x131a8u);
    G(7) = rd32(s, OBJECT_COUNT);
    UPTO(0x131b0u);

    for (;;) {
        s->r[4] = rd8(s, G(9) + CAR_SIDE);
        UPTO(0x131b4u);
        rt_cc_u(s, s->r[4], s->r[10]);
        if (s->r[4] != s->r[10]) {             /* not this one: the next */
            BRANCH(0x131b8u, 0x1323cu);
            s->r[3] = rd32(s, G(9) + 8);       /* how long its record is */
            UPTO(0x13240u);
            G(9) += s->r[3];
            rt_cc_u(s, 1, G(7));
            G(7) -= 1;
            if (!(s->cc & 4)) {
                UPTO(0x1324cu);
                RET();
            }
            BRANCH(0x1324cu, 0x131b0u);
            continue;
        }
        UPTO(0x131b8u);

        G(14) = 0x505;
        UPTO(0x131bcu);
        TGP_CMD(0x05);                         /* push */
        UPTO(0x131c0u);
        G(14) = 0;
        s->r[4] = 0;                           /* the y it stands at */
        UPTO(0x131c8u);
        s->r[3] = rd32(s, G(9) + CAR_X);
        UPTO(0x131ccu);
        s->r[5] = rd32(s, G(9) + CAR_Z);
        UPTO(0x131d0u);
        G(14) = 0x1212;
        UPTO(0x131d8u);
        TGP_CMD(0x12);                         /* translate */
        UPTO(0x131dcu);
        TGP_PARAM(s->r[3]);
        UPTO(0x131e0u);
        TGP_PARAM(s->r[4]);
        UPTO(0x131e4u);
        TGP_PARAM(s->r[5]);
        UPTO(0x131e8u);
        G(14) = 0;
        UPTO(0x131ecu);
        s->r[3] = (uint32_t)(int32_t)(int16_t)rd16(s, G(9) + CAR_ANGLE);
        UPTO(0x131f0u);
        G(14) = 0x1515;
        UPTO(0x131f8u);
        TGP_CMD(0x15);                         /* rotate about y: its heading */
        UPTO(0x131fcu);
        TGP_PARAM(s->r[3]);
        UPTO(0x13200u);
        G(14) = 0;
        G(14) = 0x1313;
        UPTO(0x1320cu);
        TGP_CMD(0x13);                         /* scale */
        UPTO(0x13210u);
        for (int k = 0; k < 3; k++) {
            TGP_PARAM(G(3));
            UPTO(pc_ + 4);
        }
        G(14) = 0;
        UPTO(0x13220u);
        G(0) = rd32(s, CAR_MODELS + s->r[10] * 4);
        UPTO(0x13228u);
        CALL(f_00017acc, 0x17accu);            /* draw_model */
        G(14) = 0x606;
        UPTO(0x13230u);
        TGP_CMD(0x06);                         /* pop */
        UPTO(0x13234u);
        G(14) = 0;
        BRANCH(0x1323cu, 0x1324cu);
        RET();
    }
}

/* 0x136b0: draw_nearby_object. g9 -> an object, g6 -> the player's car.
   Only things inside the circle draw_cars set around the player are drawn,
   and they are placed relative to it. */
#define OBJ_TURN     0xa4u       /* added to the heading */
#define OBJ_MODEL_88 0x88u
#define GROUND_Y     0xbc23d70au /* -0.01f: just below the ground */

RT_FN(f_000136b0)
{
    ENTRY(0x136b0u);

    s->r[3] = rd32(s, G(9) + CAR_X);
    UPTO(0x136b4u);
    s->r[4] = rd32(s, G(9) + CAR_Z);
    UPTO(0x136b8u);
    G(14) = 0x4141;
    UPTO(0x136c0u);
    TGP_CMD(0x41);                       /* how far outside the circle it is */
    UPTO(0x136c4u);
    TGP_PARAM(s->r[3]);
    UPTO(0x136c8u);
    TGP_PARAM(s->r[4]);
    UPTO(0x136ccu);
    G(0) = TGP_RESULT();
    UPTO(0x136d0u);
    G(14) = 0;
    if (!(G(0) & (1u << 31))) {          /* outside: too far to bother with */
        s->cc = 2;
        BRANCH(0x136d8u, 0x13760u);
        RET();
    }
    s->cc = 0;
    UPTO(0x136d8u);
    s->r[3] = rd32(s, VIEW_MODE);
    UPTO(0x136e0u);
    rt_cc_i(s, 1, s->r[3]);
    if (s->r[3] == 1) {
        BRANCH(0x136e4u, 0x13760u);
        RET();
    }
    UPTO(0x136e4u);

    G(14) = 0x505;
    UPTO(0x136e8u);
    TGP_CMD(0x05);                       /* push */
    UPTO(0x136ecu);
    G(14) = 0;
    UPTO(0x136f0u);
    s->r[3] = rd32(s, G(6) + CAR_X);
    UPTO(0x136f4u);
    s->r[4] = rd32(s, G(6) + CAR_Z);
    UPTO(0x136f8u);
    s->r[5] = rd32(s, G(9) + CAR_X);
    UPTO(0x136fcu);
    s->r[6] = rd32(s, G(9) + CAR_Z);
    UPTO(0x13700u);
    wrF(&G(0), rdF(&s->r[5]) - rdF(&s->r[3]));   /* where it is from here */
    UPTO(0x13704u);
    wrF(&G(2), rdF(&s->r[6]) - rdF(&s->r[4]));
    UPTO(0x13708u);
    s->r[7] = GROUND_Y;
    G(14) = 0x1212;
    UPTO(0x13718u);
    TGP_CMD(0x12);                       /* translate */
    UPTO(0x1371cu);
    TGP_PARAM(G(0));
    UPTO(0x13720u);
    TGP_PARAM(s->r[7]);
    UPTO(0x13724u);
    TGP_PARAM(G(2));
    UPTO(0x13728u);
    G(14) = 0;
    UPTO(0x1372cu);
    s->r[4] = (uint32_t)(int32_t)(int16_t)rd16(s, G(9) + CAR_ANGLE);
    UPTO(0x13730u);
    s->r[3] = (uint32_t)(int32_t)(int16_t)rd16(s, G(9) + OBJ_TURN);
    UPTO(0x13734u);
    s->r[4] += s->r[3];
    G(14) = 0x1515;
    UPTO(0x13740u);
    TGP_CMD(0x15);                       /* rotate about y */
    UPTO(0x13744u);
    TGP_PARAM(s->r[4]);
    UPTO(0x13748u);
    G(14) = 0;
    UPTO(0x1374cu);
    G(0) = rd32(s, G(9) + OBJ_MODEL_88);
    UPTO(0x13750u);
    CALL(f_00017acc, 0x17accu);          /* draw_model */
    G(14) = 0x606;
    UPTO(0x13758u);
    TGP_CMD(0x06);                       /* pop */
    UPTO(0x1375cu);
    G(14) = 0;
    UPTO(0x13760u);
    RET();
}

/* 0x134c8: draw_cars. The player's car, then a circle around it, and every
   other object near enough to be worth drawing. */
#define MODEL_SET    0x501464u   /* which set of models to use */
#define CAR_BODY_TAB 0x1376cu    /* the player's car model, by set */
#define CAR_LONG     0x40400000u /* 3.0f: the body is longer than it is wide */
#define CAR_WIDE     0x3fc00000u /* 1.5f */
#define NEAR_RADIUS  0x3e570a3du /* 0.21f, divided by the scale in g2 */

RT_FN(f_000134c8)
{
    ENTRY(0x134c8u);

    G(14) = 0x505;
    UPTO(0x134ccu);
    TGP_CMD(0x05);                       /* push */
    UPTO(0x134d0u);
    G(14) = 0;
    s->r[3] = CAR_LONG;
    s->r[4] = CAR_WIDE;
    G(14) = 0x1313;
    UPTO(0x134ecu);
    TGP_CMD(0x13);                       /* scale */
    UPTO(0x134f0u);
    TGP_PARAM(s->r[3]);
    UPTO(0x134f4u);
    TGP_PARAM(s->r[4]);
    UPTO(0x134f8u);
    TGP_PARAM(s->r[4]);
    UPTO(0x134fcu);
    G(14) = 0;
    UPTO(0x13500u);
    s->r[11] = rd8(s, MODEL_SET);
    UPTO(0x13508u);
    G(0) = rd32(s, CAR_BODY_TAB + s->r[11] * 4);
    UPTO(0x13510u);
    CALL(f_00017acc, 0x17accu);          /* draw_model: the car */
    G(14) = 0x606;
    UPTO(0x13518u);
    TGP_CMD(0x06);                       /* pop */
    UPTO(0x1351cu);
    G(14) = 0;
    UPTO(0x13520u);

    s->r[3] = rd32(s, G(6) + CAR_X);
    UPTO(0x13524u);
    s->r[4] = rd32(s, G(6) + CAR_Z);
    UPTO(0x13528u);
    s->r[5] = NEAR_RADIUS;
    G(14) = 0x303;
    UPTO(0x13534u);
    TGP_CMD(0x03);                       /* divide: how far "near" reaches */
    UPTO(0x13538u);
    TGP_PARAM(s->r[5]);
    UPTO(0x1353cu);
    TGP_PARAM(G(2));
    UPTO(0x13540u);
    s->r[5] = TGP_RESULT();
    UPTO(0x13544u);
    G(14) = 0x4040;
    UPTO(0x13550u);
    TGP_CMD(0x40);                       /* the circle around the player */
    UPTO(0x13554u);
    TGP_PARAM(s->r[3]);
    UPTO(0x13558u);
    TGP_PARAM(s->r[4]);
    UPTO(0x1355cu);
    TGP_PARAM(s->r[5]);
    UPTO(0x13560u);
    G(14) = 0;
    UPTO(0x13564u);

    G(0) = (uint32_t)(int32_t)(int16_t)rd16(s, G(6) + CAR_ANGLE);
    UPTO(0x13568u);
    s->r[3] = (uint32_t)(int32_t)(int16_t)rd16(s, G(6) + OBJ_TURN);
    UPTO(0x1356cu);
    G(0) += s->r[3];
    G(0) = 0 - G(0);                     /* the world turns the other way */
    G(14) = 0x1515;
    UPTO(0x1357cu);
    TGP_CMD(0x15);                       /* rotate about y */
    UPTO(0x13580u);
    TGP_PARAM(G(0));
    UPTO(0x13584u);
    G(14) = 0;
    UPTO(0x13588u);
    G(9) = rd32(s, OBJECT_LIST);
    UPTO(0x13590u);
    G(7) = rd32(s, OBJECT_COUNT);
    UPTO(0x13598u);

    for (;;) {
        CALL(f_000136b0, 0x136b0u);      /* draw_nearby_object */
        s->r[3] = rd32(s, G(9) + 8);
        UPTO(0x135a0u);
        G(9) += s->r[3];
        rt_cc_u(s, 1, G(7));
        G(7) -= 1;
        if (!(s->cc & 4))
            break;
        BRANCH(0x135acu, 0x13598u);
    }
    UPTO(0x135acu);
    G(14) = 0x606;
    UPTO(0x135b0u);
    TGP_CMD(0x06);                       /* pop */
    UPTO(0x135b4u);
    G(14) = 0;
    UPTO(0x135b8u);
    RET();
}
