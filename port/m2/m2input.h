/*
 * Maps host controls onto the board's input ports, using the per-game input
 * definitions from the original (m2_game.inputs). Host independent: the
 * frontend reports keys as DirectInput scan codes (what the definitions
 * use) and pad state as abstract buttons/axes.
 */
#ifndef M2INPUT_H
#define M2INPUT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct m2_board;

enum {   /* pad buttons, same meaning as SDL_GameController's */
    M2PAD_A, M2PAD_B, M2PAD_X, M2PAD_Y, M2PAD_BACK, M2PAD_GUIDE, M2PAD_START,
    M2PAD_LSTICK, M2PAD_RSTICK, M2PAD_LB, M2PAD_RB, M2PAD_UP, M2PAD_DOWN,
    M2PAD_LEFT, M2PAD_RIGHT, M2PAD_BUTTONS
};
enum { M2PAD_LX, M2PAD_LY, M2PAD_RX, M2PAD_RY, M2PAD_LT, M2PAD_RT, M2PAD_AXES };

typedef struct {
    uint8_t key[256];              /* DirectInput scan codes currently down */
    uint8_t pad[2][M2PAD_BUTTONS]; /* two pads */
    int16_t axis[2][M2PAD_AXES];   /* -32768..32767; triggers 0..32767 */
    float   steer;                 /* keyboard steering, ramps -1..1 */
    int     gear[2];               /* shifter: 0 = neutral, 1-4 (pad 2: generic games only) */
    uint8_t gear_up_held[2], gear_down_held[2];
    uint8_t shift_keys_prev;       /* keyboard gear keys last frame (bit k = Shift k, bit 0 = neutral) */
    float   steer_u, accel_u, brake_u;   /* original's units: -128..127, 1..254 */

    /* options (the original's EMULATOR.INI [Input]) */
    int     updown_gears;          /* UpDownGears: gear keys 1/2 shift down/up */
    int     hold_gears;            /* HoldGears: neutral when no gear key is held */
} m2_input_state;

void m2input_init(m2_input_state *s);
/* Computes port[] and analog[] for this frame (call once per frame). */
void m2input_apply(m2_input_state *s, struct m2_board *b);

#ifdef __cplusplus
}
#endif

#endif
