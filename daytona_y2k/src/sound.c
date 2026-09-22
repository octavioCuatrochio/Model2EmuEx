/*
 * Sound commands.
 *
 * The game drives the sound board with MIDI-style commands, but doesn't
 * send them itself: each one goes into a 16-entry ring in RAM, and the
 * frame interrupt takes them out again a byte at a time and writes them to
 * the sound port at 0x1c80000. The interrupt can arrive in the middle of
 * any of these functions, so their memory accesses and cycles are kept in
 * the machine code's order exactly.
 */
#include "decomp.h"

#define IRQ_ENABLE  0xe80004u    /* board interrupt mask */
#define IRQ_NORMAL  0x401u       /* frame interrupt and sound interrupt */

#define SND_COUNT   0x503001u    /* commands waiting in the ring */
#define SND_WRITE   0x503003u    /* where the next one goes */
#define SND_RING    0x503020u    /* 16 words, one command each */
#define SND_RING_N  16

#define SOUND_FLAGS 0x501090u    /* bits 20 and 22: sound may play */
#define SOUND_ON    0x500000u

/* 0x1a064: put one command (g0) in the ring, unless it is full. */
RT_FN(f_0001a064)
{
    ENTRY(0x1a064u);

    /* keep the frame interrupt's sound interrupt out while writing
       (the frame interrupt itself stays on) */
    s->r[6] = IRQ_ENABLE;
    s->r[3] = 1;
    UPTO(0x1a070u);
    wr32(s, IRQ_ENABLE, 1);
    UPTO(0x1a074u);
    wr32(s, IRQ_ENABLE, 1);
    s->r[3] = SND_RING_N;
    UPTO(0x1a07cu);

    s->r[5] = rd8(s, SND_COUNT);
    UPTO(0x1a084u);
    rt_cc_u(s, s->r[5], SND_RING_N);
    if (s->r[5] < SND_RING_N) {
        UPTO(0x1a088u);
        s->r[5] += 1;
        UPTO(0x1a08cu);
        wr8(s, SND_COUNT, (uint8_t)s->r[5]);
        UPTO(0x1a094u);
        s->r[3] = rd8(s, SND_WRITE);
        UPTO(0x1a09cu);
        wr32(s, SND_RING + s->r[3] * 4, G(0));
        UPTO(0x1a0a4u);
        s->r[4] = SND_RING_N - 1;
        s->r[3] = (s->r[3] + 1) & s->r[4];
        UPTO(0x1a0b0u);
        wr8(s, SND_WRITE, (uint8_t)s->r[3]);
        UPTO(0x1a0b8u);
    } else {
        BRANCH(0x1a088u, 0x1a0b8u);
    }

    s->r[3] = IRQ_NORMAL;
    UPTO(0x1a0bcu);
    wr32(s, IRQ_ENABLE, IRQ_NORMAL);
    UPTO(0x1a0c0u);
    wr32(s, IRQ_ENABLE, IRQ_NORMAL);
    UPTO(0x1a0c4u);
    RET();
}

/* 0x1a04c: the same, but only while sound may play (it falls into 0x1a064). */
RT_FN(f_0001a04c)
{
    ENTRY(0x1a04cu);

    s->r[3] = rd32(s, SOUND_FLAGS);
    UPTO(0x1a054u);
    s->r[4] = SOUND_ON;
    s->r[3] &= s->r[4];
    rt_cc_u(s, 0, s->r[3]);
    if (!s->r[3]) {
        BRANCH(0x1a064u, 0x1a0c4u);
        RET();
    }
    UPTO(0x1a064u);
    TAIL(f_0001a064);
}

/* 0x1a0c8: engine note.
   The engine's speed (0x158 of the player's car, in RPM) becomes MIDI
   controller 0x17 on channel 15, 1 to 127 over the range 1800-8500 rpm.
   It is only sent when it differs from the last one. */
#define CAR_STATE     0x501220u   /* -> the player's car */
#define CAR_RPM       0x158u
#define RPM_LOW       0x708u      /* 1800 */
#define RPM_HIGH      0x2134u     /* 8500 */
#define RPM_SCALE     0x14b4u     /* 5300: rpm range per 128 steps */
#define SND_LAST_RPM  0x503006u
#define MIDI_ENGINE   0xbe1700u   /* control change, channel 15, controller 0x17 */

RT_FN(f_0001a0c8)
{
    ENTRY(0x1a0c8u);

    s->r[3] = rd32(s, SOUND_FLAGS);
    UPTO(0x1a0d0u);
    s->r[4] = SOUND_ON;
    s->r[3] &= s->r[4];
    rt_cc_u(s, 0, s->r[3]);
    if (!s->r[3]) {
        BRANCH(0x1a0e0u, 0x1a16cu);
        RET();
    }
    UPTO(0x1a0e0u);

    G(9) = rd32(s, CAR_STATE);
    UPTO(0x1a0e8u);
    s->r[5] = rd32(s, G(9) + CAR_RPM);   /* the rpm, clamped to the range */
    UPTO(0x1a0ecu);
    G(14) = RPM_LOW;
    rt_cc_i(s, G(14), s->r[5]);
    if ((int32_t)s->r[5] >= (int32_t)RPM_LOW) {
        BRANCH(0x1a0f4u, 0x1a0fcu);
        G(14) = RPM_HIGH;
        rt_cc_i(s, G(14), s->r[5]);
        if ((int32_t)s->r[5] <= (int32_t)RPM_HIGH) {
            BRANCH(0x1a108u, 0x1a10cu);
        } else {
            UPTO(0x1a108u);
            s->r[5] = G(14);
            UPTO(0x1a10cu);
        }
    } else {
        UPTO(0x1a0f4u);
        s->r[5] = G(14);
        BRANCH(0x1a0fcu, 0x1a10cu);
    }

    /* the note: 1..127 */
    G(14) = 0;
    s->r[3] = RPM_LOW;
    s->r[5] = ((s->r[5] - s->r[3]) << 7);
    s->r[3] = RPM_SCALE;
    s->r[5] = rt_divi(s->r[3], s->r[5]);
    G(14) = 1;
    rt_cc_i(s, G(14), s->r[5]);
    if ((int32_t)s->r[5] >= 1) {
        BRANCH(0x1a130u, 0x1a138u);
        G(14) = 0x7f;
        rt_cc_i(s, G(14), s->r[5]);
        if ((int32_t)s->r[5] <= 0x7f) {
            BRANCH(0x1a140u, 0x1a144u);
        } else {
            UPTO(0x1a140u);
            s->r[5] = G(14);
            UPTO(0x1a144u);
        }
    } else {
        UPTO(0x1a130u);
        s->r[5] = G(14);
        BRANCH(0x1a138u, 0x1a144u);
    }
    G(14) = 0;
    UPTO(0x1a148u);

    s->r[3] = (uint32_t)(int16_t)rd16(s, SND_LAST_RPM);
    UPTO(0x1a150u);
    rt_cc_i(s, s->r[3], s->r[5]);
    if (s->r[3] == s->r[5]) {            /* unchanged: nothing to send */
        BRANCH(0x1a154u, 0x1a16cu);
        RET();
    }
    UPTO(0x1a154u);
    wr16(s, SND_LAST_RPM, (uint16_t)s->r[5]);
    UPTO(0x1a15cu);
    s->r[3] = MIDI_ENGINE;
    G(0) = s->r[3] | s->r[5];
    UPTO(0x1a168u);
    CALL(f_0001a064, 0x1a064u);          /* snd_queue */
    RET();
}
