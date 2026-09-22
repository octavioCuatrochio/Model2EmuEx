/*
 * Drawing objects: the game walks the scene, and for each thing in it sets
 * up the geometry processor's matrix and hands it a model.
 *
 * The matrix commands and the ones below are described in
 * ../notes/geometry.md; g11 and g12 address the processor's ports.
 */
#include "decomp.h"

#define OBJ_X      0x1cu     /* world position */
#define OBJ_Z      0x24u
#define OBJ_ANGLE  0x58u     /* heading, 16-bit */

#define MODEL_SET  0x501464u /* which set of models to use */
#define MODEL_TAB  0x13764u  /* -> the model for that set */

/* 0x13250: draw_object. g6 -> the object, g3 = the scale (all three axes).
   The object stands at its x and z with y = 1.0, turned to its heading. */
RT_FN(f_00013250)
{
    ENTRY(0x13250u);

    G(14) = 0x505;
    UPTO(0x13254u);
    TGP_CMD(0x05);                       /* push the matrix */
    UPTO(0x13258u);
    G(14) = 0;
    s->r[4] = 0x3f800000u;               /* 1.0f: the y the object sits at */
    UPTO(0x13264u);

    s->r[3] = rd32(s, G(6) + OBJ_X);
    UPTO(0x13268u);
    s->r[5] = rd32(s, G(6) + OBJ_Z);
    UPTO(0x1326cu);
    G(14) = 0x1212;
    UPTO(0x13274u);
    TGP_CMD(0x12);                       /* translate */
    UPTO(0x13278u);
    TGP_PARAM(s->r[3]);
    UPTO(0x1327cu);
    TGP_PARAM(s->r[4]);
    UPTO(0x13280u);
    TGP_PARAM(s->r[5]);
    UPTO(0x13284u);
    G(14) = 0;
    UPTO(0x13288u);

    s->r[3] = (uint32_t)(int32_t)(int16_t)rd16(s, G(6) + OBJ_ANGLE);
    UPTO(0x1328cu);
    G(14) = 0x1515;
    UPTO(0x13294u);
    TGP_CMD(0x15);                       /* rotate about y: the heading */
    UPTO(0x13298u);
    TGP_PARAM(s->r[3]);
    UPTO(0x1329cu);
    G(14) = 0;
    G(14) = 0x1313;
    UPTO(0x132a8u);
    TGP_CMD(0x13);                       /* scale */
    UPTO(0x132acu);
    for (int k = 0; k < 3; k++) {
        TGP_PARAM(G(3));
        UPTO(pc_ + 4);
    }
    G(14) = 0;
    UPTO(0x132bcu);

    s->r[11] = rd8(s, MODEL_SET);
    UPTO(0x132c4u);
    G(0) = rd32(s, MODEL_TAB + s->r[11] * 4);
    UPTO(0x132ccu);
    CALL(f_00017acc, 0x17accu);          /* hand the model to the processor */

    G(14) = 0x606;
    UPTO(0x132d4u);
    TGP_CMD(0x06);                       /* pop the matrix */
    UPTO(0x132d8u);
    G(14) = 0;
    UPTO(0x132dcu);
    RET();
}
