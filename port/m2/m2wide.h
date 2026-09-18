/*
 * Per-game widescreen rules: when the 3D may be widened and which tile
 * layers stretch to the full width. Ported from the original emulator's
 * SCRIPTS\<game>.lua (Model2_SetWideScreen / Model2_SetStretch*), which look
 * at game-state bytes each frame. Games without a script get widescreen
 * all the time and no stretching.
 */
#ifndef M2WIDE_H
#define M2WIDE_H

#ifdef __cplusplus
extern "C" {
#endif

struct m2_board;

/* stretch flags: layer A/B in the pass below (low) / above (high) the 3D */
enum { M2_STRETCH_A_LOW = 1, M2_STRETCH_B_LOW = 2, M2_STRETCH_A_HIGH = 4, M2_STRETCH_B_HIGH = 8 };

typedef struct {
    int widescreen;   /* 3D may use the extra width this frame */
    int stretch;      /* M2_STRETCH_* */
} m2_wide_state;

void m2wide_update(const struct m2_board *b, m2_wide_state *st);

#ifdef __cplusplus
}
#endif

#endif
