/*
 * Coins and credits.
 *
 * Two coin chutes and the service button give credits; the settings in
 * backup RAM say how many coins make a credit; each coin is counted in the
 * bookkeeping; and the mechanical coin meters on the cabinet are pulsed
 * through the I/O board's output port. A coin sound plays for each one.
 *
 * A chute's state is a few bytes (from 0x5fe020 for the credits, 0x5fe030
 * and 0x5fe038 for the chutes): coins pending, a counter, a remainder.
 */
#include "decomp.h"

#define COIN_SETTING   0x1d0001bu  /* backup RAM: 0 -> switch limit 24, else 9 */
#define COIN_LIMIT     0x5fe045u   /* how many a switch may count */
#define COIN_DISABLED  0x5fe040u   /* coins ignored (test mode...) */
#define COIN_CHUTES    0x5fe041u   /* nonzero: one chute feeds both meters */
#define COIN_SOUND_1   0x5fe043u   /* coin sounds still to play */
#define COIN_SOUND_2   0x5fe044u
#define MIDI_COIN      0x9e127fu   /* the coin sound */

#define CREDITS        0x5fe020u
#define CHUTE_1        0x5fe030u
#define CHUTE_2        0x5fe038u
#define BK_CREDITS     0x1d00110u  /* bookkeeping: credits */
#define BK_SERVICE     0x1d00114u  /* ... service credits */
#define BK_COINS_1     0x1d00118u  /* ... coins in chute 1 */
#define BK_COINS_2     0x1d0011cu  /* ... coins in chute 2 */
#define BK_COUNTERS    0x1d00128u  /* the four halfwords from 0x5fe020 */

#define COINS_PER_CREDIT 0x1d00011u
#define COIN_BONUS       0x1d00014u

#define IO_OUTPUTS     0x1c0001eu  /* the I/O board's output port: the meters */

#define BUTTONS_DOWN   0x5fe11cu
#define BUTTONS_UP     0x5fe5e8u
#define BUTTONS_SERVICE 0x808u     /* the service buttons */
#define COIN_SWITCHES  3u          /* coin 1, coin 2: counted when released */

/* 0x229440: coin_update, once a frame */
RT_FN(f_00229440)
{
    ENTRY(0x229440u);

    G(4) = rd8(s, COIN_SETTING);
    UPTO(0x229448u);
    rt_cc_i(s, G(4), 0);
    G(5) = 9;
    if (G(4) != 0) {
        BRANCH(0x229454u, 0x229458u);
    } else {
        UPTO(0x229454u);
        G(5) = 24;
        UPTO(0x229458u);
    }
    wr8(s, COIN_LIMIT, (uint8_t)G(5));
    UPTO(0x229460u);

    G(4) = rd8(s, COIN_DISABLED);
    UPTO(0x229468u);
    rt_cc_i(s, 0, G(4));
    if (G(4) != 0) {
        UPTO(0x22946cu);
        RET();
    }
    BRANCH(0x22946cu, 0x229470u);

    /* a coin sound for each coin still waiting for one */
    static const struct { uint32_t flag, at; } snd[2] = {
        { COIN_SOUND_1, 0x229470u }, { COIN_SOUND_2, 0x22949cu },
    };
    for (int k = 0; k < 2; k++) {
        uint32_t at = snd[k].at;
        G(4) = rd8(s, snd[k].flag);
        UPTO(at + 8);
        rt_cc_i(s, 0, G(4));
        if (G(4) == 0) {
            BRANCH(at + 12, at + 0x2c);
            continue;
        }
        UPTO(at + 12);
        G(0) = MIDI_COIN;
        UPTO(at + 20);
        CALL(f_0001a064, 0x1a064u);      /* snd_queue */
        G(4) = rd8(s, snd[k].flag);
        UPTO(at + 32);
        G(4) -= 1;
        UPTO(at + 36);
        wr8(s, snd[k].flag, (uint8_t)G(4));
        UPTO(at + 0x2c);
    }

    /* the service buttons give a credit */
    s->r[5] = rd32(s, BUTTONS_DOWN);
    UPTO(0x2294d0u);
    G(5) = BUTTONS_SERVICE;
    G(4) = s->r[5] & G(5);
    rt_cc_i(s, 0, G(4));
    if (G(4)) {
        UPTO(0x2294dcu);
        G(0) = CREDITS;
        UPTO(0x2294e4u);
        BAL(f_002295c8, 0x2295c8u);      /* service_credit */
        G(4) = rd32(s, BK_SERVICE);
        UPTO(0x2294f0u);
        G(0) += G(4);
        UPTO(0x2294f4u);
        wr32(s, BK_SERVICE, G(0));
        UPTO(0x2294fcu);
        CALL(f_002297e0, 0x2297e0u);     /* coin_counters_save */
    } else {
        BRANCH(0x2294dcu, 0x229500u);
    }

    /* coins: counted when the switch is released */
    s->r[5] = rd32(s, BUTTONS_UP);
    UPTO(0x229508u);
    G(5) = 0xff;
    G(4) = s->r[5] & COIN_SWITCHES & G(5);
    rt_cc_i(s, 0, G(4));
    if (G(4)) {
        UPTO(0x229518u);
        s->cc = (s->r[5] & 2) ? 2 : 0;   /* chkbit 1 */
        s->r[4] = 0;                     /* credits given */
        if (!(s->r[5] & 2)) {
            BRANCH(0x229524u, 0x229554u);
        } else {                         /* chute 2 */
            UPTO(0x229524u);
            G(4) = rd8(s, COIN_CHUTES);
            UPTO(0x22952cu);
            G(1) = CHUTE_2;
            rt_cc_i(s, 0, G(4));
            if (G(4) == 0) {
                BRANCH(0x229538u, 0x22953cu);
            } else {
                UPTO(0x229538u);
                G(1) -= 8;               /* counted as chute 1 */
                UPTO(0x22953cu);
            }
            G(0) = CREDITS;
            G(2) = BK_COINS_2;
            UPTO(0x22954cu);
            CALL(f_00229630, 0x229630u); /* coin_inserted */
            s->r[4] = G(0);
            UPTO(0x229554u);
        }
        if (!(s->r[5] & 1)) {            /* bbc 0: chute 1 */
            s->cc = 2;
            BRANCH(0x229558u, 0x229578u);
        } else {
            s->cc = 0;
            UPTO(0x229558u);
            G(0) = CREDITS;
            G(1) = CHUTE_1;
            G(2) = BK_COINS_1;
            UPTO(0x229570u);
            CALL(f_00229630, 0x229630u); /* coin_inserted */
            s->r[4] += G(0);
            UPTO(0x229578u);
        }
        G(4) = rd32(s, BK_CREDITS);
        UPTO(0x229580u);
        G(4) += s->r[4];
        UPTO(0x229584u);
        wr32(s, BK_CREDITS, G(4));
        UPTO(0x22958cu);
        CALL(f_002297e0, 0x2297e0u);     /* coin_counters_save */
    } else {
        BRANCH(0x229518u, 0x229590u);
    }

    /* the coin meters: chute 2's (unless one chute feeds both), then chute 1's */
    G(4) = rd8(s, COIN_CHUTES);
    UPTO(0x229598u);
    rt_cc_i(s, 0, G(4));
    if (G(4) == 0) {
        UPTO(0x22959cu);
        G(0) = CHUTE_2;
        UPTO(0x2295a4u);
        BAL(f_00229728, 0x229728u);      /* coin_meter */
    } else {
        BRANCH(0x22959cu, 0x2295a8u);
    }
    G(0) = CHUTE_1;
    BRANCH(0x2295b4u, 0x229720u);
    G(14) = 0x2297d0u;                   /* back to the ret at 0x2297d0 */
    UPTO(0x229728u);
    INTO(f_00229728, 0x2297d0u);         /* coin_meter */
    RET();
}

/* 0x2295c8: service_credit, a bal procedure; g0 -> the credit state.
   Returns 1 in g0 (and asks for a coin sound) while the count stays within
   the limit, else clamps it and returns 0. */
RT_FN(f_002295c8)
{
    ENTRY(0x2295c8u);

    G(1) = G(14);
    G(14) = 0;
    UPTO(0x2295d0u);
    G(4) = rd8(s, G(0));
    UPTO(0x2295d4u);
    G(4) += 1;
    UPTO(0x2295d8u);
    wr8(s, G(0), (uint8_t)G(4));
    UPTO(0x2295dcu);
    G(5) = rd8(s, COIN_LIMIT);
    UPTO(0x2295e4u);
    G(6) = 0;
    G(7) = 0xff;
    G(4) &= G(7);
    rt_cc_u(s, G(4), G(5));
    if (G(4) <= G(5)) {
        BRANCH(0x2295f4u, 0x229614u);
        G(7) = 1;
        G(6) = 1;
        UPTO(0x22961cu);
        wr8(s, COIN_SOUND_2, (uint8_t)G(7));
        UPTO(0x229624u);
    } else {
        UPTO(0x2295f4u);
        G(4) = rd8(s, G(0));
        UPTO(0x2295f8u);
        wr8(s, G(0) + 2, (uint8_t)G(14));
        UPTO(0x2295fcu);
        wr8(s, G(0) + 1, (uint8_t)G(14));
        UPTO(0x229600u);
        rt_cc_u(s, G(4), G(5));
        if (G(4) <= G(5)) {
            BRANCH(0x229604u, 0x229624u);
        } else {
            UPTO(0x229604u);
            G(4) = rd8(s, COIN_LIMIT);
            UPTO(0x22960cu);
            wr8(s, G(0), (uint8_t)G(4));
            UPTO(0x229610u);
            BRANCH(0x229614u, 0x229624u);
        }
    }
    G(0) = G(6);
    UPTO(0x229628u);
    RET_BX(G(1));
}

/* 0x229630: coin_inserted(g0 -> credit state, g1 -> the chute's state,
   g2 -> its bookkeeping counter). Counts the coin, asks for a coin sound,
   and turns coins into credits by the backup RAM settings; returns in g0
   the credits it gave. */
RT_FN(f_00229630)
{
    ENTRY(0x229630u);

    G(4) = rd8(s, G(1));
    UPTO(0x229634u);
    G(4) += 1;
    UPTO(0x229638u);
    wr8(s, G(1), (uint8_t)G(4));
    UPTO(0x22963cu);
    G(4) = rd32(s, G(2));
    UPTO(0x229640u);
    G(4) += 1;
    UPTO(0x229644u);
    wr32(s, G(2), G(4));
    UPTO(0x229648u);

    G(5) = rd8(s, G(0));
    UPTO(0x22964cu);
    G(4) = rd8(s, COIN_LIMIT);
    UPTO(0x229654u);
    rt_cc_u(s, G(5), G(4));
    if (G(5) >= G(4)) {                  /* too many at once: ignored */
        UPTO(0x229658u);
        G(0) = 0;
        UPTO(0x22965cu);
        RET();
    }
    BRANCH(0x229658u, 0x229660u);

    G(4) = rd8(s, COIN_SOUND_1);
    UPTO(0x229668u);
    G(4) += 1;
    UPTO(0x22966cu);
    wr8(s, COIN_SOUND_1, (uint8_t)G(4));
    UPTO(0x229674u);
    G(1) = rd8(s, G(1) + 3);             /* the chute's coin value */
    UPTO(0x229678u);
    BRANCH(0x22967cu, 0x229680u);

    /* bonus coins: every COIN_BONUS coins in the remainder give one more */
    G(6) = rd8(s, COIN_BONUS);
    UPTO(0x229688u);
    G(3) = 0;                            /* credits given */
    rt_cc_i(s, 0, G(6));
    if (G(6) != 0) {
        UPTO(0x229690u);
        G(4) = rd8(s, G(0) + 2);
        UPTO(0x229694u);
        G(5) = G(1) + G(4);
        G(7) = rt_divi(G(6), G(5));
        rt_cc_i(s, 0, G(7));
        if (G(7) != 0) {
            UPTO(0x2296a0u);
            G(4) = rd8(s, G(0) + 1);
            UPTO(0x2296a4u);
            G(4) += G(7);
            G(5) = rt_remi(G(6), G(5));
            UPTO(0x2296acu);
            wr8(s, G(0) + 1, (uint8_t)G(4));
            UPTO(0x2296b0u);
        } else {
            BRANCH(0x2296a0u, 0x2296b0u);
        }
        wr8(s, G(0) + 2, (uint8_t)G(5));
        UPTO(0x2296b4u);
    } else {
        BRANCH(0x229690u, 0x2296b4u);
    }

    /* coins to credits */
    G(4) = rd8(s, G(0) + 1);
    UPTO(0x2296b8u);
    G(2) = rd8(s, COINS_PER_CREDIT);
    UPTO(0x2296c0u);
    G(1) += G(4);
    G(6) = rt_divi(G(2), G(1));          /* new credits */
    rt_cc_i(s, 0, G(6));
    if (G(6) != 0) {
        UPTO(0x2296ccu);
        G(7) = rd8(s, G(0));
        UPTO(0x2296d0u);
        G(5) = rd8(s, COIN_LIMIT);
        UPTO(0x2296d8u);
        G(1) = rt_remi(G(2), G(1));      /* coins left over */
        s->r[4] = 0xff;
        G(4) = (G(7) & s->r[4]) + G(6) - G(5);
        rt_cc_i(s, 0, G(4));
        if ((int32_t)G(4) < 0) {
            BRANCH(0x2296f0u, 0x2296fcu);
        } else {                         /* over the limit: only up to it */
            UPTO(0x2296f0u);
            G(1) = 0;
            G(6) -= G(4);
            UPTO(0x2296f8u);
            wr8(s, G(0) + 2, (uint8_t)G(14));
            UPTO(0x2296fcu);
        }
        G(7) += G(6);
        G(3) = G(6);
        UPTO(0x229704u);
        wr8(s, G(0), (uint8_t)G(7));
        UPTO(0x229708u);
    } else {
        BRANCH(0x2296ccu, 0x229708u);
    }
    wr8(s, G(0) + 1, (uint8_t)G(1));
    UPTO(0x22970cu);
    G(0) = G(3);
    UPTO(0x229710u);
    RET();
}

/* 0x229728: coin_meter, a bal procedure; g0 -> a chute's meter state:
   pulses pending (byte 0), on time (1), off time (2), and at +4 the bit of
   the output port that drives this meter. One pulse is 6 frames on. */
RT_FN(f_00229728)
{
    ENTRY(0x229728u);

    G(2) = G(14);
    G(14) = 0;
    UPTO(0x229730u);
    G(6) = rd32(s, G(0));                /* ldl */
    G(7) = rd32(s, G(0) + 4);
    UPTO(0x229734u);
    G(5) = G(0) + 4;
    G(1) = 0xffff00u;
    G(4) = G(6) & G(1);                  /* a pulse under way? */
    rt_cc_i(s, 0, G(4));
    if (G(4) == 0) {
        UPTO(0x229748u);
        G(4) = rd8(s, G(0));
        UPTO(0x22974cu);
        rt_cc_i(s, 0, G(4));
        if (G(4) == 0) {                 /* nothing to count */
            BRANCH(0x229750u, 0x229780u);
            RET_BX(G(2));
        }
        UPTO(0x229750u);                 /* start a pulse: 6 on, 6 off */
        G(1) = 6;
        UPTO(0x229754u);
        wr8(s, G(0) + 1, (uint8_t)G(1));
        UPTO(0x229758u);
        G(4) = rd8(s, G(0));
        UPTO(0x22975cu);
        wr8(s, G(0) + 2, (uint8_t)G(1));
        UPTO(0x229760u);
        G(4) -= 1;
        UPTO(0x229764u);
        wr8(s, G(0), (uint8_t)G(4));
        UPTO(0x229768u);
        G(5) = rd8(s, G(5));
        UPTO(0x22976cu);
        G(4) = rd8(s, IO_OUTPUTS);
        UPTO(0x229774u);
        G(4) |= G(5);                    /* meter on */
        UPTO(0x229778u);
        wr8(s, IO_OUTPUTS, (uint8_t)G(4));
        UPTO(0x229780u);
        RET_BX(G(2));
    }
    BRANCH(0x229748u, 0x229784u);

    G(4) = rd8(s, G(0) + 1);
    UPTO(0x229788u);
    rt_cc_i(s, 0, G(4));
    if (G(4) == 0) {                     /* in the off time */
        BRANCH(0x22978cu, 0x2297c0u);
        G(4) = rd8(s, G(0) + 2);
        UPTO(0x2297c4u);
        G(4) -= 1;
        UPTO(0x2297c8u);
        wr8(s, G(0) + 2, (uint8_t)G(4));
        UPTO(0x2297ccu);
        RET_BX(G(2));
    }
    UPTO(0x22978cu);                     /* in the on time */
    G(0) += 1;
    UPTO(0x229790u);
    G(4) = rd8(s, G(0));
    UPTO(0x229794u);
    G(1) = 0xff;
    G(4) -= 1;
    G(5) = G(1) & G(4);
    UPTO(0x2297a0u);
    wr8(s, G(0), (uint8_t)G(4));
    UPTO(0x2297a4u);
    rt_cc_i(s, 0, G(5));
    if (G(5) != 0) {
        BRANCH(0x2297a8u, 0x2297bcu);
        RET_BX(G(2));
    }
    UPTO(0x2297a8u);
    G(4) = rd8(s, IO_OUTPUTS);
    UPTO(0x2297b0u);
    G(4) &= ~G(7);                       /* meter off */
    UPTO(0x2297b4u);
    wr8(s, IO_OUTPUTS, (uint8_t)G(4));
    UPTO(0x2297bcu);
    RET_BX(G(2));
}

/* 0x2297e0: coin_counters_save: the four halfwords from 0x5fe020 into
   backup RAM, then the bookkeeping's CRC and mirror (it falls into
   backup_refresh). */
RT_FN(f_002297e0)
{
    ENTRY(0x2297e0u);

    for (uint32_t k = 0; k < 4; k++) {
        G(4) = rd16(s, CREDITS + 4 * k);
        UPTO(pc_ + 8);
        wr16(s, BK_COUNTERS + 2 * k, (uint16_t)G(4));
        UPTO(pc_ + 8);
    }
    BRANCH(0x229824u, 0x229230u);
    TAIL(f_00229230);                    /* backup_refresh */
}
