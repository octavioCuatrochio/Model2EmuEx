/*
 * The display list as a whole: the settings a frame opens with, and the
 * marker it ends with.
 *
 * The list lives in one of the two buffer pages (src/video.c) and the
 * board's renderer walks it (../../port/m2/m2geo.c). Each entry's opcode is
 * the address it is written to through g10, not part of the word:
 *
 *   3   a window: the part of the screen drawn into, and the four points
 *       perspective can converge on
 *   7   the drawing mode
 *   8   the z adjust: the exponent depth values are measured against
 *   f   the end of the list
 *
 * The four convergence points are what the game uses to put the road's
 * horizon where the driver's eye is and to draw the rear view from another
 * place on the screen; the renderer picks one of them per object (the two
 * high bits of an object's opcode address).
 */
#include "decomp.h"

#define WINDOWS       0x502800u    /* the race view: four different centres */
#define WINDOWS_MID   0x502820u    /* ... and all four on the screen's middle */
#define PAGE_SHADOW   0x501368u    /* which buffer page the list is in */
#define COPRO_LIST    0x3008u      /* where the copro is told to read it from */

/* The six words of a window, read from a seven-word record whose first word
   is not part of it. Both entries below share these instructions (0x17e28
   branches over 0x17e34's first one into the middle of it). */
#define WINDOW_WORDS() do { \
        s->r[3] = 0; \
        UPTO(0x17e40u); \
        DL_EXP(3, s->r[3]); \
        UPTO(0x17e44u); \
        { uint32_t a_ = G(0);                    /* ldt: the viewport */ \
          s->r[4] = rd32(s, a_); \
          s->r[5] = rd32(s, a_ + 4); \
          s->r[6] = rd32(s, a_ + 8); } \
        UPTO(0x17e48u); \
        { uint32_t a_ = G(0) + 0xcu;             /* ldq: the four centres */ \
          s->r[8] = rd32(s, a_); \
          s->r[9] = rd32(s, a_ + 4); \
          s->r[10] = rd32(s, a_ + 8); \
          s->r[11] = rd32(s, a_ + 12); } \
        UPTO(0x17e4cu); \
        DL_WORD(s->r[5]); \
        UPTO(0x17e50u); \
        DL_WORD(s->r[6]); \
        UPTO(0x17e54u); \
        DL_WORD(s->r[8]); \
        UPTO(0x17e58u); \
        DL_WORD(s->r[9]); \
        UPTO(0x17e5cu); \
        DL_WORD(s->r[10]); \
        UPTO(0x17e60u); \
        DL_WORD(s->r[11]); \
        UPTO(0x17e64u); \
    } while (0)

/* 0x17e28: set_windows_mid. The plain window, perspective converging on the
   middle of the screen wherever it is used. */
RT_FN(f_00017e28)
{
    ENTRY(0x17e28u);

    G(0) = WINDOWS_MID;
    UPTO(0x17e30u);
    BRANCH(0x17e34u, 0x17e3cu);
    WINDOW_WORDS();
    RET();
}

/* 0x17e34: set_windows. The race view's window. */
RT_FN(f_00017e34)
{
    ENTRY(0x17e34u);

    G(0) = WINDOWS;
    UPTO(0x17e3cu);
    WINDOW_WORDS();
    RET();
}

/* 0x17abc: set_draw_mode. g0 is the mode word (../../port/m2/m2geo.c reads
   it as g->mode). */
RT_FN(f_00017abc)
{
    ENTRY(0x17abcu);

    s->r[3] = 0;
    UPTO(0x17ac0u);
    DL_EXP(7, s->r[3]);
    UPTO(0x17ac4u);
    DL_WORD(G(0));
    UPTO(0x17ac8u);
    RET();
}

/* 0x17ed8: set_z_adjust. Always 4.0: depth is measured against 2^4, so a
   polygon 16 units away sorts at the top of the range. */
RT_FN(f_00017ed8)
{
    ENTRY(0x17ed8u);

    s->r[3] = 0;
    UPTO(0x17edcu);
    DL_EXP(8, s->r[3]);
    UPTO(0x17ee0u);
    s->r[3] = 0x40800000u;               /* 4.0f */
    UPTO(0x17ee8u);
    DL_WORD(s->r[3]);
    UPTO(0x17eecu);
    RET();
}

/* 0x17ef0: end_display_list. Reading the geometry processor's accumulator
   first waits for the commands still in it; then the end marker goes in the
   list and the copro is pointed at the page the list is in. */
RT_FN(f_00017ef0)
{
    ENTRY(0x17ef0u);

    G(14) = 0x2525u;
    UPTO(0x17ef8u);
    TGP_CMD(0x25);                       /* acc_get */
    UPTO(0x17efcu);
    s->r[3] = TGP_RESULT();              /* the value itself is not wanted */
    UPTO(0x17f00u);
    G(14) = 0;
    UPTO(0x17f04u);
    s->r[3] = rd32(s, PAGE_SHADOW);
    UPTO(0x17f0cu);
    DL_EXP(0xf, s->r[3]);
    UPTO(0x17f10u);
    wr32(s, G(10) + COPRO_LIST, s->r[3]);
    UPTO(0x17f18u);
    RET();
}

/* 0x59b4: open_frame. What every frame's list starts with: the drawing mode
   twice (3, then 1), and the level of detail at its farthest. */
RT_FN(f_000059b4)
{
    ENTRY(0x59b4u);

    G(0) = 3;
    UPTO(0x59b8u);
    CALL(f_00017abc, 0x17abcu);          /* set_draw_mode */
    G(0) = 1;
    UPTO(0x59c0u);
    CALL(f_00017abc, 0x17abcu);
    s->r[3] = 0;
    UPTO(0x59c8u);
    s->r[4] = 0x43000000u;               /* 128.0f */
    UPTO(0x59d0u);
    DL_EXP(0x16, s->r[3]);               /* the level of detail */
    UPTO(0x59d4u);
    DL_WORD(s->r[4]);
    UPTO(0x59d8u);
    RET();
}

/* 0x225534: ease_in_detail. The level of detail brought in over a quarter
   turn of an angle kept at 0x48 of the object: 128 * (1 - cos) of it, so
   what is drawn grows from nothing to everything and then stays. */
RT_FN(f_00225534)
{
    ENTRY(0x225534u);

    s->r[4] = rd32(s, G(13) + 0x48);     /* how far in it is */
    UPTO(0x225538u);
    s->r[7] = 0x43000000u;               /* 128.0f */
    UPTO(0x225540u);
    G(14) = 0x1e1eu;
    UPTO(0x225548u);
    TGP_CMD(0x1e);                       /* r_cos: 128 * cos(angle) */
    UPTO(0x22554cu);
    TGP_PARAM(s->r[4]);
    UPTO(0x225550u);
    TGP_PARAM(s->r[7]);
    UPTO(0x225554u);
    s->r[8] = TGP_RESULT();
    UPTO(0x225558u);
    G(14) = 0;
    UPTO(0x22555cu);
    wrF(&s->r[7], rdF(&s->r[7]) - rdF(&s->r[8]));
    UPTO(0x225560u);
    s->r[3] = 0;
    UPTO(0x225564u);
    DL_EXP(0x16, s->r[3]);
    UPTO(0x225568u);
    DL_WORD(s->r[7]);
    UPTO(0x22556cu);

    s->r[3] = 1u << 6;                   /* the angle moves on ... */
    UPTO(0x225570u);
    s->r[4] += s->r[3];
    UPTO(0x225574u);
    s->r[6] = 1u << 14;                  /* ... until a quarter turn */
    UPTO(0x225578u);
    rt_cc_u(s, s->r[4], s->r[6]);
    if (s->r[4] < s->r[6]) {
        BRANCH(0x225580u, 0x225584u);
    } else {
        UPTO(0x225580u);
        s->r[4] = s->r[6];
        UPTO(0x225584u);
    }
    wr32(s, G(13) + 0x48, s->r[4]);
    UPTO(0x225588u);
    RET();
}
