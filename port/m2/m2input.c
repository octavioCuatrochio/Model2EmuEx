/* Host controls -> Model 2 input ports; see m2input.h.

   Keyboard: the DirectInput keys of the original's input definitions.
   Pads: mapped by what each input is (its name in the definitions):
     directions       d-pad or left stick
     Button 1-4       A B X Y              (VF2: guard, punch, kick)
     Start / Coin     Start / Back         (player 2: second pad)
     VR 1-4 / VR      Y B X A / Y          (view buttons)
     Steering         left stick X
     Accelerate       right trigger, Brake: left trigger
     gears            RB up / LB down (sequential)

   Driving games then go through the original's per-game input routines
   (m2_game.input_fn): Daytona 0x4c9080 and Sega Rally 0x4cb870 turn the
   controls into the values their I/O boards deliver, including the gear
   number, which the games read as a code in bits 4-6 of a port. */
#include "m2input.h"
#include "m2board.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define STICK 16000   /* digital threshold */
#define FN_DAYTONA 0x4c9080
#define FN_SRALLY  0x4cb870

void m2input_init(m2_input_state *s)
{
    int updown = s->updown_gears, hold = s->hold_gears;   /* options survive */
    memset(s, 0, sizeof *s);
    s->updown_gears = updown;
    s->hold_gears = hold;
    s->gear[0] = s->gear[1] = 1;
    s->accel_u = s->brake_u = 1;
}

static uint8_t to_range(float v, int lo, int hi)   /* v in 0..1 */
{
    if (v < 0) v = 0;
    if (v > 1) v = 1;
    return (uint8_t)(lo + v * (float)(hi - lo) + 0.5f);
}

/* Model 2 (original) boards read the controls through a Model 1 I/O board's
   dual-port RAM: analog channels 0-2 are its bytes 0-2, which the I/O
   handler (0x4c88a0) serves from ports 2, 0 and 1 (checked in Daytona's
   input test, and what 0x4c9080 writes). */
static const int8_t dpram_port[3] = { 2, 0, 1 };

/* "Player 2 Up" -> player 1 (0-based), rest "Up" */
static int split_player(const char *name, const char **rest)
{
    if (!strncmp(name, "Player ", 7) && name[7] >= '1' && name[7] <= '4' && name[8] == ' ') {
        *rest = name + 9;
        return name[7] - '1';
    }
    *rest = name;
    return 0;
}

static float stick(int16_t v)   /* -1..1 with a small dead zone */
{
    float f = v / 32767.0f;
    if (f > -0.08f && f < 0.08f)
        return 0;
    f = f > 0 ? (f - 0.08f) / 0.92f : (f + 0.08f) / 0.92f;
    return f < -1 ? -1 : f > 1 ? 1 : f;
}

static float trigger(int16_t v)   /* 0..1 */
{
    float f = v / 32767.0f;
    return f < 0.05f ? 0 : f > 1 ? 1 : (f - 0.05f) / 0.95f;
}

/* pad state for a digital input, by name (after "Player N ") */
static int pad_digital(const m2_input_state *s, int p, const char *what)
{
    const uint8_t *b = s->pad[p];
    const int16_t *ax = s->axis[p];
    if (!strcmp(what, "Up")) return b[M2PAD_UP] || ax[M2PAD_LY] < -STICK;
    if (!strcmp(what, "Down")) return b[M2PAD_DOWN] || ax[M2PAD_LY] > STICK;
    if (!strcmp(what, "Left")) return b[M2PAD_LEFT] || ax[M2PAD_LX] < -STICK;
    if (!strcmp(what, "Right")) return b[M2PAD_RIGHT] || ax[M2PAD_LX] > STICK;
    if (!strcmp(what, "Start")) return b[M2PAD_START];
    if (!strcmp(what, "Coin")) return b[M2PAD_BACK];
    if (!strncmp(what, "Button ", 7)) {
        static const int map[4] = { M2PAD_A, M2PAD_B, M2PAD_X, M2PAD_Y };
        int k = what[7] - '1';
        return k >= 0 && k < 4 && b[map[k]];
    }
    if (!strcmp(what, "VR")) return b[M2PAD_Y];
    if (!strncmp(what, "VR ", 3)) {
        static const int map[4] = { M2PAD_Y, M2PAD_B, M2PAD_X, M2PAD_A };
        int k = what[3] - '1';
        return k >= 0 && k < 4 && b[map[k]];
    }
    if (!strncmp(what, "Shift ", 6)) {   /* generic games: the gear's bit */
        if (!strcmp(what + 6, "Neutral")) return s->gear[p] == 0;
        int k = what[6] - '0';
        return k >= 1 && k <= 4 && s->gear[p] == k;
    }
    if (!strcmp(what, "Trigger")) return b[M2PAD_A] || ax[M2PAD_RT] > STICK;
    if (!strcmp(what, "Missile")) return b[M2PAD_B];
    return 0;
}

/* keyboard gear keys held: bit k = "Shift k", bit 0 = "Shift Neutral" */
static unsigned gear_keys(const m2_input_state *s, const m2_game *g)
{
    unsigned m = 0;
    for (const m2_input *i = g->inputs; i && i->kind >= 0; i++) {
        if (strncmp(i->name, "Shift ", 6) || i->key >= 256 || !s->key[i->key])
            continue;
        if (!strcmp(i->name + 6, "Neutral"))
            m |= 1;
        else if (i->name[6] >= '1' && i->name[6] <= '4')
            m |= 1u << (i->name[6] - '0');
    }
    return m;
}

/* The shifter, as the original's input routines keep it (board+0x203438):
   HoldGears drops to neutral unless a gear key is held; H-pattern keys set
   the gear directly; with UpDownGears the "Shift 1"/"Shift 2" keys step it
   down/up. The pad's RB/LB always step it. */
static void update_gears(m2_input_state *s, const m2_game *g)
{
    unsigned keys = gear_keys(s, g), pressed = keys & ~s->shift_keys_prev;
    int *gear = &s->gear[0];

    if (s->hold_gears && !s->updown_gears)
        *gear = 0;
    if (!s->updown_gears) {
        for (int k = 1; k <= 4; k++)
            if (keys & (1u << k)) { *gear = k; break; }
        if (!(keys & 0x1e) && (keys & 1))
            *gear = 0;
    } else {
        if ((pressed & 4) && *gear < 4) (*gear)++;        /* "Shift 2": up */
        else if ((pressed & 2) && *gear > 0) (*gear)--;   /* "Shift 1": down */
    }
    s->shift_keys_prev = (uint8_t)keys;

    for (int p = 0; p < 2; p++) {
        int up = s->pad[p][M2PAD_RB], down = s->pad[p][M2PAD_LB];
        if (up && !s->gear_up_held[p] && s->gear[p] < 4) s->gear[p]++;
        if (down && !s->gear_down_held[p] && s->gear[p] > 0) s->gear[p]--;
        s->gear_up_held[p] = (uint8_t)up;
        s->gear_down_held[p] = (uint8_t)down;
    }
}

/* the code the shifter delivers in bits 4-6 (0x4c94ff; MAME daytona_gearbox_r) */
static uint8_t gear_code(int gear)
{
    static const uint8_t code[5] = { 0x00, 0x20, 0x10, 0x60, 0x50 };
    return code[gear < 0 ? 0 : gear > 4 ? 4 : gear];
}

/* Steering and pedals in the original's units (0x4c9080 / 0x4ca450):
   keyboard ramps of 8 per frame, pads directly. Returns 1 if the game's
   steering limit is +125 (Daytona) rather than +127. */
static void update_driving(m2_input_state *s, int steer_max)
{
    const int16_t *ax = s->axis[0];
    const uint8_t *pb = s->pad[0];
    int left = s->key[0xcb] || pb[M2PAD_LEFT], right = s->key[0xcd] || pb[M2PAD_RIGHT];
    int up = s->key[0xc8], down = s->key[0xd0];
    float st = stick(ax[M2PAD_LX]);

    if (st != 0) {
        s->steer_u = st * 127.0f;
    } else {
        if (left && !right) {
            if (s->steer_u > -128.0f) s->steer_u -= 8.0f;
        } else if (right && !left) {
            if (s->steer_u < (float)steer_max) s->steer_u += 8.0f;
        } else if (s->steer_u > 0) {
            s->steer_u -= 8.0f;
        } else if (s->steer_u < 0) {
            s->steer_u += 8.0f;
        }
        if (fabsf(s->steer_u) < 8.0f)
            s->steer_u = 0;
    }
    if (s->steer_u > (float)steer_max) s->steer_u = (float)steer_max;
    if (s->steer_u < -(float)(steer_max == 125 ? 126 : 127)) s->steer_u = -(float)(steer_max == 125 ? 126 : 127);

    float rt = trigger(ax[M2PAD_RT]), lt = trigger(ax[M2PAD_LT]);
    if (rt > 0) s->accel_u = 1.0f + rt * 253.0f;
    else if (up) { if (s->accel_u < 254.0f) s->accel_u += 8.0f; }
    else if (s->accel_u > 1.0f) s->accel_u -= 8.0f;
    if (lt > 0) s->brake_u = 1.0f + lt * 253.0f;
    else if (down) { if (s->brake_u < 254.0f) s->brake_u += 8.0f; }
    else if (s->brake_u > 1.0f) s->brake_u -= 8.0f;
    if (s->accel_u < 1) s->accel_u = 1;
    if (s->accel_u > 254) s->accel_u = 254;
    if (s->brake_u < 1) s->brake_u = 1;
    if (s->brake_u > 254) s->brake_u = 254;
}

static uint8_t clamp_pedal(float v)   /* 0x4c9080: pedals clamped to 3..0xfd */
{
    int i = (int)lrintf(v);
    return (uint8_t)(i < 3 ? 3 : i > 0xfd ? 0xfd : i);
}

void m2input_apply(m2_input_state *s, m2_board *b)
{
    const m2_game *g = b->game;
    const m2_input *in = g->inputs;
    uint8_t pressed[8] = { 0 };
    uint8_t analog_port[8] = { 0 };
    unsigned analog_set = 0;
    int fn = (int)g->input_fn;
    int driving = fn == FN_DAYTONA || fn == FN_SRALLY;

    update_gears(s, g);

    /* keyboard steering for games without a routine: ramp towards the
       held direction, recentre when released */
    int kl = s->key[0xcb], kr = s->key[0xcd];
    float target = (float)(kr - kl);
    s->steer += (target - s->steer) * (target != 0 ? 0.15f : 0.3f);

    for (; in && in->kind >= 0; in++) {
        const char *what;
        int p = split_player(in->name, &what);
        if (p > 1)
            p = 1;
        if (!strcmp(what, "Coin 1")) { what = "Coin"; p = 0; }
        else if (!strcmp(what, "Coin 2")) { what = "Coin"; p = 1; }

        if (in->kind == M2_IN_ANALOG) {
            if (driving)
                continue;   /* the game's routine below */
            const int16_t *ax = s->axis[p];
            float v;
            if (!strcmp(what, "Steering") || !strcmp(what, "Handle") || !strcmp(what, "Horizontal")) {
                float st = stick(ax[M2PAD_LX]);
                v = (st != 0 ? st : s->steer) * 0.5f + 0.5f;
            } else if (!strcmp(what, "Accelerate") || !strcmp(what, "Accelerator") || !strcmp(what, "Throttle")) {
                v = s->key[0xc8] ? 1.0f : trigger(ax[M2PAD_RT]);
            } else if (!strcmp(what, "Brake")) {
                v = s->key[0xd0] ? 1.0f : trigger(ax[M2PAD_LT]);
            } else if (!strcmp(what, "Vertical")) {
                v = stick(ax[M2PAD_LY]) * 0.5f + 0.5f;
            } else {
                v = 0.5f;
            }
            if (b->type == 0) {
                if (in->port < 3) {
                    int port = dpram_port[in->port];
                    analog_port[port] = to_range(v, 0x20, 0xe0);
                    analog_set |= 1u << port;
                }
            } else if (in->port < 16) {
                b->analog[in->port] = to_range(v, 0, 255);
            }
            continue;
        }
        if (in->kind == M2_IN_HOTKEY || in->port >= 8)
            continue;
        /* on Model 2 the arrow-key entries share a port with an analog
           channel: they drive it, not bits */
        if (b->type == 0 && (in->kind == M2_IN_LEFTRIGHT || in->kind == M2_IN_UPDOWN))
            continue;

        int down = in->key < 256 && s->key[in->key];
        if (!(driving && !strncmp(what, "Shift ", 6)))   /* driving games: gear code below */
            down = down || pad_digital(s, p, what);
        if (down)
            pressed[in->port] |= in->mask;
    }
    /* active low (checked in Daytona's test menu: VR 1/VR 4 move the
       cursor, Start selects) */
    for (int p = 0; p < 8; p++)
        b->port[p] = (analog_set >> p) & 1 ? analog_port[p]
                                            : (uint8_t)(g->port_defaults[p] & ~pressed[p]);

    if (fn == FN_DAYTONA) {   /* 0x4c9080 */
        update_driving(s, 125);
        b->port[2] = (uint8_t)(((((int)lrintf(s->steer_u + 129.0f)) & 0xffff) * 0xc0 >> 8) + 0x20);
        b->port[0] = clamp_pedal(s->accel_u);
        b->port[1] = clamp_pedal(s->brake_u);
        b->port[6] = (uint8_t)((b->port[6] & 0x8f) | gear_code(s->gear[0]));
    } else if (fn == FN_SRALLY) {   /* 0x4cb870 and 0x4ca450 */
        update_driving(s, 127);
        uint8_t steer = (uint8_t)lrintf(s->steer_u + 127.0f);
        b->analog[0] = (uint8_t)((steer * 0xc0 >> 8) + 0x20);
        b->analog[1] = (uint8_t)lrintf(s->accel_u);
        b->analog[2] = (uint8_t)lrintf(s->brake_u);
        b->port[1] = (uint8_t)((b->port[1] & 0x8f) | gear_code(s->gear[0]));
        /* port 2 reads back the drive board (orig 0x57285b = last command) */
        switch (b->drive_last) {
        case 1: b->port[2] = b->analog[0]; break;
        case 2: b->port[2] = 0xff; break;
        case 3: b->port[2] = (uint8_t)b->reg_574cf0; break;
        default: b->port[2] = (uint8_t)b->drive_last; break;
        }
    }
}
