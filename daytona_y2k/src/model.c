/*
 * Handing a model to the drawing hardware.
 *
 * A frame's picture is a display list the game builds in buffer RAM, and
 * the board's renderer walks it (../../port/m2/m2geo.c). A word written
 * through g10 carries its opcode in the address (the copro reads the
 * float's exponent from there, ../notes/geometry.md):
 *
 *   9   the projection: how far the eye is from the screen, x and y
 *   a   the direction the light comes from
 *   b   the matrix that places what follows
 *   1   an object: where its textures and its polygons are, and how many
 *
 * Each object carries a record (at 0x10 of it) with the projection and
 * light it wants, and the opcode to draw it with; draw_model only writes
 * those again when the record changes.
 */
#include "decomp.h"

#define MODELS_FROM   0x283fbb4u   /* model data lives between these */
#define MODELS_TO     0x284f4ecu
#define POLY_BUDGET   0x5010e8u    /* polygons in the list so far */
#define POLY_LIMIT    0x5010f4u    /* ... and how many fit in a frame (5000) */
#define MODELS_DRAWN  0x501094u    /* chunks drawn this frame */
#define LAST_VIEW 0x501360u    /* the material the list is set to */
#define OBJ_VIEW      0x10u       /* -> its view record (opcode, projection, light) */
#define MODEL_GROUP   0x5016c0u    /* a count, then that many model numbers */
#define MODEL_TABLE   0x501420u    /* -> the models, by number */

/* 0x17acc: draw_model. g0 -> the model (a list of chunks, ended by 0),
   g13 -> the object it belongs to. */
RT_FN(f_00017acc)
{
    ENTRY(0x17accu);

    s->r[3] = MODELS_FROM;
    s->r[4] = MODELS_TO;
    rt_cc_u(s, G(0), s->r[3]);
    if (G(0) < s->r[3]) {                /* not a model: nothing to draw */
        BRANCH(0x17ae0u, 0x17ae8u);
        BRANCH(0x17aecu, 0x17c34u);
        RET();
    }
    UPTO(0x17ae0u);
    rt_cc_u(s, G(0), s->r[4]);
    if (G(0) >= s->r[4]) {
        BRANCH(0x17ae4u, 0x17ae8u);
        BRANCH(0x17aecu, 0x17c34u);
        RET();
    }
    BRANCH(0x17ae8u, 0x17aecu);

    s->r[3] = 0;
    UPTO(0x17af0u);
    s->r[4] = rd32(s, POLY_LIMIT);
    UPTO(0x17af8u);
    s->r[5] = rd32(s, POLY_BUDGET);
    UPTO(0x17b00u);
    rt_cc_u(s, s->r[5], s->r[4]);
    if (s->r[5] > s->r[4]) {             /* the frame is full */
        BRANCH(0x17b04u, 0x17c34u);
        RET();
    }
    UPTO(0x17b04u);

    s->r[10] = rd32(s, G(13) + OBJ_VIEW);   /* the projection and light it wants */
    UPTO(0x17b08u);
    s->r[4] = rd32(s, LAST_VIEW);
    UPTO(0x17b10u);
    rt_cc_u(s, s->r[10], s->r[4]);
    if (s->r[10] == s->r[4]) {           /* already set to it */
        BRANCH(0x17b14u, 0x17b4cu);
    } else {
        UPTO(0x17b14u);
        wr32(s, LAST_VIEW, s->r[10]);
        UPTO(0x17b1cu);
        DL_EXP(9, s->r[3]);              /* the projection */
        UPTO(0x17b20u);
        s->r[4] = rd32(s, s->r[10] + 4);
        UPTO(0x17b24u);
        s->r[5] = rd32(s, s->r[10] + 8);
        UPTO(0x17b28u);
        DL_WORD(s->r[4]);
        UPTO(0x17b2cu);
        DL_WORD(s->r[5]);
        UPTO(0x17b30u);
        DL_EXP(0xa, s->r[3]);            /* the light */
        UPTO(0x17b34u);
        s->r[4] = rd32(s, s->r[10] + 0xc);
        UPTO(0x17b38u);
        s->r[5] = rd32(s, s->r[10] + 0x10);
        UPTO(0x17b3cu);
        s->r[6] = rd32(s, s->r[10] + 0x14);
        UPTO(0x17b40u);
        DL_WORD(s->r[4]);
        UPTO(0x17b44u);
        DL_WORD(s->r[5]);
        UPTO(0x17b48u);
        DL_WORD(s->r[6]);
        UPTO(0x17b4cu);
    }

    DL_EXP(0xb, s->r[3]);                /* the matrix that places it */
    UPTO(0x17b50u);
    s->r[3] = 0x1111;
    UPTO(0x17b58u);
    TGP_CMD(0x11);                       /* get_matrix */
    UPTO(0x17b5cu);
    for (int k = 0; k < 12; k++) {
        s->r[3] = TGP_RESULT();
        UPTO(pc_ + 4);
        DL_WORD(s->r[3]);
        UPTO(pc_ + 4);
    }
    s->r[3] = 0;
    UPTO(0x17bc0u);

    /* the model's chunks, until a zero */
    for (;;) {
        s->r[11] = rd32(s, G(0));
        UPTO(0x17bc4u);
        G(0) += 4;
        rt_cc_u(s, 0, s->r[11]);
        if (s->r[11] == 0) {
            BRANCH(0x17bccu, 0x17c34u);
            RET();
        }
        UPTO(0x17bccu);
        s->r[4] = rd32(s, MODELS_DRAWN);
        UPTO(0x17bd4u);
        s->r[4] += 1;
        UPTO(0x17bd8u);
        wr32(s, MODELS_DRAWN, s->r[4]);
        UPTO(0x17be0u);

        s->r[12] = rd32(s, G(0));
        UPTO(0x17be4u);
        G(0) += 4;
        UPTO(0x17be8u);
        s->r[13] = rd32(s, G(0));
        UPTO(0x17becu);
        G(0) += 4;
        UPTO(0x17bf0u);
        s->r[14] = rd32(s, G(0));        /* how many polygons the chunk has */
        UPTO(0x17bf4u);
        G(0) += 4;
        UPTO(0x17bf8u);
        s->r[4] = rd32(s, POLY_BUDGET);
        UPTO(0x17c00u);
        s->r[4] += s->r[14];
        UPTO(0x17c04u);
        wr32(s, POLY_BUDGET, s->r[4]);
        UPTO(0x17c0cu);

        s->r[4] = rd32(s, s->r[10]);     /* the opcode this object draws with */
        UPTO(0x17c10u);
        wr32(s, G(10) + 0x10 + s->r[4], s->r[3]);
        UPTO(0x17c18u);
        DL_WORD(s->r[12]);               /* where its texture parameters are */
        UPTO(0x17c1cu);
        DL_WORD(s->r[13]);               /* ... its texture headers */
        UPTO(0x17c20u);
        DL_WORD(s->r[11]);               /* ... and its polygons */
        UPTO(0x17c24u);
        s->r[4] = rd32(s, POLY_LIMIT);
        UPTO(0x17c2cu);
        DL_WORD(s->r[4]);                /* how many to take at most */
        UPTO(0x17c30u);
        BRANCH(0x17c34u, 0x17bc0u);
    }
}

/* 0x17c38: set_model_view. draw_model's first half on its own: the
   projection and light the object wants (only when they changed), then the
   geometry processor's current matrix as the transform for what follows.
   0x17a80 uses this and draw_model_chunks in place of draw_model. */
RT_FN(f_00017c38)
{
    ENTRY(0x17c38u);

    s->r[3] = 0;
    UPTO(0x17c3cu);
    s->r[10] = rd32(s, G(13) + OBJ_VIEW);
    UPTO(0x17c40u);
    s->r[4] = rd32(s, LAST_VIEW);
    UPTO(0x17c48u);
    rt_cc_u(s, s->r[10], s->r[4]);
    if (s->r[10] == s->r[4]) {           /* the list is already set to it */
        BRANCH(0x17c4cu, 0x17c84u);
    } else {
        UPTO(0x17c4cu);
        wr32(s, LAST_VIEW, s->r[10]);
        UPTO(0x17c54u);
        DL_EXP(9, s->r[3]);              /* the projection */
        UPTO(0x17c58u);
        s->r[4] = rd32(s, s->r[10] + 4);
        UPTO(0x17c5cu);
        s->r[5] = rd32(s, s->r[10] + 8);
        UPTO(0x17c60u);
        DL_WORD(s->r[4]);
        UPTO(0x17c64u);
        DL_WORD(s->r[5]);
        UPTO(0x17c68u);
        DL_EXP(0xa, s->r[3]);            /* the light */
        UPTO(0x17c6cu);
        s->r[4] = rd32(s, s->r[10] + 0xc);
        UPTO(0x17c70u);
        s->r[5] = rd32(s, s->r[10] + 0x10);
        UPTO(0x17c74u);
        s->r[6] = rd32(s, s->r[10] + 0x14);
        UPTO(0x17c78u);
        DL_WORD(s->r[4]);
        UPTO(0x17c7cu);
        DL_WORD(s->r[5]);
        UPTO(0x17c80u);
        DL_WORD(s->r[6]);
        UPTO(0x17c84u);
    }

    DL_EXP(0xb, s->r[3]);                /* the matrix that places it */
    UPTO(0x17c88u);
    s->r[3] = 0x1111;
    UPTO(0x17c90u);
    TGP_CMD(0x11);                       /* get_matrix */
    UPTO(0x17c94u);
    for (int k = 0; k < 12; k++) {
        s->r[3] = TGP_RESULT();
        UPTO(pc_ + 4);
        DL_WORD(s->r[3]);
        UPTO(pc_ + 4);
    }
    RET();
}

/* 0x17cf8: draw_model_chunks. draw_model's second half: the model's chunks
   go in the list under the view already set. g0 -> the model, g13 -> the
   object it belongs to. */
RT_FN(f_00017cf8)
{
    ENTRY(0x17cf8u);

    s->r[3] = MODELS_FROM;
    UPTO(0x17d00u);
    s->r[4] = MODELS_TO;
    UPTO(0x17d08u);
    rt_cc_u(s, G(0), s->r[3]);
    if (G(0) < s->r[3]) {                /* not a model: nothing to draw */
        BRANCH(0x17d0cu, 0x17d14u);
        BRANCH(0x17d18u, 0x17d9cu);
        RET();
    }
    UPTO(0x17d0cu);
    rt_cc_u(s, G(0), s->r[4]);
    if (G(0) >= s->r[4]) {
        BRANCH(0x17d10u, 0x17d14u);
        BRANCH(0x17d18u, 0x17d9cu);
        RET();
    }
    BRANCH(0x17d14u, 0x17d18u);

    s->r[4] = rd32(s, POLY_LIMIT);
    UPTO(0x17d20u);
    s->r[5] = rd32(s, POLY_BUDGET);
    UPTO(0x17d28u);
    rt_cc_u(s, s->r[5], s->r[4]);
    if (s->r[5] > s->r[4]) {             /* the frame is full */
        BRANCH(0x17d2cu, 0x17d9cu);
        RET();
    }
    UPTO(0x17d2cu);

    for (;;) {
        s->r[11] = rd32(s, G(0));        /* where its polygons are, 0 at the end */
        UPTO(0x17d30u);
        rt_cc_u(s, 0, s->r[11]);
        if (s->r[11] == 0) {
            BRANCH(0x17d34u, 0x17d9cu);
            RET();
        }
        UPTO(0x17d34u);
        s->r[12] = rd32(s, G(0) + 4);    /* its texture parameters */
        UPTO(0x17d38u);
        s->r[13] = rd32(s, G(0) + 8);    /* ... its texture headers */
        UPTO(0x17d3cu);
        s->r[14] = rd32(s, G(0) + 0xc);  /* how many polygons the chunk has */
        UPTO(0x17d40u);
        G(0) += 0x10;
        UPTO(0x17d44u);
        s->r[4] = rd32(s, POLY_BUDGET);
        UPTO(0x17d4cu);
        s->r[4] += s->r[14];
        UPTO(0x17d50u);
        wr32(s, POLY_BUDGET, s->r[4]);
        UPTO(0x17d58u);
        s->r[4] = rd32(s, MODELS_DRAWN);
        UPTO(0x17d60u);
        s->r[4] += 1;
        UPTO(0x17d64u);
        wr32(s, MODELS_DRAWN, s->r[4]);
        UPTO(0x17d6cu);

        s->r[3] = 0;
        UPTO(0x17d70u);
        s->r[10] = rd32(s, G(13) + OBJ_VIEW);
        UPTO(0x17d74u);
        s->r[4] = rd32(s, s->r[10]);     /* the opcode this object draws with */
        UPTO(0x17d78u);
        wr32(s, G(10) + 0x10 + s->r[4], s->r[3]);
        UPTO(0x17d80u);
        DL_WORD(s->r[12]);
        UPTO(0x17d84u);
        DL_WORD(s->r[13]);
        UPTO(0x17d88u);
        DL_WORD(s->r[11]);
        UPTO(0x17d8cu);
        s->r[4] = rd32(s, POLY_LIMIT);
        UPTO(0x17d94u);
        DL_WORD(s->r[4]);                /* how many to take at most */
        UPTO(0x17d98u);
        BRANCH(0x17d9cu, 0x17d2cu);
    }
}

/* 0x17a80: draw_model_group. The models listed at 0x5016c0 (a count, then
   their numbers), all under one view: the view goes in the list once and
   each model's chunks follow. The scenery around the course is drawn this
   way, since it does not move. */
RT_FN(f_00017a80)
{
    ENTRY(0x17a80u);

    s->r[7] = MODEL_GROUP;
    UPTO(0x17a88u);
    s->r[8] = rd8(s, s->r[7]);           /* how many */
    UPTO(0x17a8cu);
    s->r[7] += 1;
    UPTO(0x17a90u);
    rt_cc_u(s, 0, s->r[8]);
    if (s->r[8] == 0) {
        BRANCH(0x17a94u, 0x17ab8u);
        RET();
    }
    UPTO(0x17a94u);
    CALL(f_00017c38, 0x17c38u);           /* set_model_view */

    for (;;) {
        s->r[9] = rd8(s, s->r[7]);       /* which model */
        UPTO(0x17a9cu);
        G(0) = rd32(s, MODEL_TABLE);
        UPTO(0x17aa4u);
        G(0) = rd32(s, G(0) + s->r[9] * 4);
        UPTO(0x17aa8u);
        CALL(f_00017cf8, 0x17cf8u);       /* draw_model_chunks */
        s->r[7] += 1;
        UPTO(0x17ab0u);
        rt_cc_u(s, 1, s->r[8]);
        s->r[8] -= 1;
        if (s->cc & 4) {                 /* more than one left */
            BRANCH(0x17ab8u, 0x17a98u);
            continue;
        }
        UPTO(0x17ab8u);
        RET();
    }
}
