/* MultiPCM; see multipcm.h. Structure and constants as MAME's gew.cpp and
   multipcm.cpp (BSD-3-Clause, Miguel Angel Horna). */
#include "multipcm.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define VOICES   28
#define TL_SHIFT 12
#define EG_SHIFT 16
#define LFO_SHIFT 8

enum { ATTACK, DECAY1, DECAY2, RELEASE };

typedef struct {
    uint32_t start, loop, end;
    uint8_t attack_reg, decay1_reg, decay2_reg, decay_level, release_reg, key_rate_scale;
    uint8_t lfo_vibrato_reg, lfo_amplitude_reg, format;
} sample_t;

typedef struct {
    int32_t volume;
    int state;
    uint8_t reverb;
    int32_t attack_rate, decay1_rate, decay2_rate, release_rate, decay_level;
} envelope_t;

typedef struct {
    uint16_t phase;
    uint32_t phase_step;
    const int32_t *table, *scale;
} lfo_t;

typedef struct {
    uint8_t regs[8];
    int playing;
    sample_t sample;
    uint32_t offset;
    uint8_t octave;
    uint16_t pitch;
    uint32_t step;
    int reverse;
    uint32_t pan;
    uint32_t total_level, dest_total_level;
    int32_t total_level_step;
    int32_t prev_sample;
    envelope_t eg;
    uint8_t lfo_frequency;
    lfo_t pitch_lfo;
    uint8_t vibrato;
    lfo_t amplitude_lfo;
    uint8_t tremolo;
} slot_t;

struct multipcm {
    multipcm_read_fn read;
    void *user;
    float rate;
    slot_t slots[VOICES];
    int32_t cur_slot;           /* -1: unused slot number, writes dropped */
    uint32_t address;

    uint32_t attack_step[0x40], decay_release_step[0x40];
    uint32_t freq_step_table[0x400];
    int32_t left_pan_table[0x800], right_pan_table[0x800];
    int32_t linear_to_exp_volume[0x400];
    int32_t total_level_steps[2];
    int32_t pitch_table[256], amplitude_table[256];
    int32_t pitch_scale_tables[8][256], amplitude_scale_tables[8][256];
};

static const double BASE_TIMES[64] = {
    0, 0, 0, 0, 6222.95, 4978.37, 4148.66, 3556.01, 3111.47, 2489.21, 2074.33, 1778.00,
    1555.74, 1244.63, 1037.19, 889.02, 777.87, 622.31, 518.59, 444.54, 388.93, 311.16, 259.32, 222.27,
    194.47, 155.60, 129.66, 111.16, 97.23, 77.82, 64.85, 55.60, 48.62, 38.91, 32.43, 27.80,
    24.31, 19.46, 16.24, 13.92, 12.15, 9.75, 8.12, 6.98, 6.08, 4.90, 4.08, 3.49,
    3.04, 2.49, 2.13, 1.90, 1.72, 1.41, 1.18, 1.04, 0.91, 0.73, 0.59, 0.50,
    0.45, 0.45, 0.45, 0.45
};
static const float LFO_FREQ[8] = { 0.168f, 2.019f, 3.196f, 4.206f, 5.215f, 5.888f, 6.224f, 7.066f };
static const float PHASE_SCALE_LIMIT[8] = { 0.0f, 3.378f, 5.065f, 6.750f, 10.114f, 20.170f, 40.180f, 79.307f };
static const float AMPLITUDE_SCALE_LIMIT[8] = { 0.0f, 0.4f, 0.8f, 1.5f, 3.0f, 6.0f, 12.0f, 24.0f };
static const int32_t VALUE_TO_CHANNEL[32] = {
    0, 1, 2, 3, 4, 5, 6, -1, 7, 8, 9, 10, 11, 12, 13, -1,
    14, 15, 16, 17, 18, 19, 20, -1, 21, 22, 23, 24, 25, 26, 27, -1
};

static uint32_t value_to_fixed(uint32_t bits, float value) { return (uint32_t)((float)(1 << bits) * value); }
static uint8_t rb(multipcm *m, uint32_t a) { return m->read(m->user, a & 0x3fffff); }

/* ---------------------------------------------------------------- envelope */

static void envelope_init(multipcm *m, double attack_decay_ratio)
{
    for (int i = 4; i < 0x40; i++) {
        m->attack_step[i] = (uint32_t)((float)(0x400 << EG_SHIFT) / (float)(BASE_TIMES[i] * 44100.0 / 1000.0));
        m->decay_release_step[i] = (uint32_t)((float)(0x400 << EG_SHIFT) /
                                              (float)(BASE_TIMES[i] * attack_decay_ratio * 44100.0 / 1000.0));
    }
    for (int i = 0; i < 4; i++)
        m->attack_step[i] = m->decay_release_step[i] = 0;
    m->attack_step[0x3f] = 0x400 << EG_SHIFT;
}

static int32_t envelope_update(multipcm *m, slot_t *s)
{
    envelope_t *e = &s->eg;
    switch (e->state) {
    case ATTACK:
        e->volume += e->attack_rate;
        if (e->volume >= (0x3ff << EG_SHIFT)) {
            e->state = DECAY1;
            if (e->decay1_rate >= (0x400 << EG_SHIFT))
                e->state = DECAY2;
            e->volume = 0x3ff << EG_SHIFT;
        }
        break;
    case DECAY1:
        e->volume -= e->decay1_rate;
        if (e->volume <= 0)
            e->volume = 0;
        if (e->volume >> (EG_SHIFT + 6) <= e->decay_level)
            e->state = DECAY2;
        break;
    case DECAY2:
        e->volume -= e->decay2_rate;
        if (e->volume <= 0)
            e->volume = 0;
        break;
    case RELEASE:
        e->volume -= e->release_rate;
        if (e->volume <= 0) {
            e->volume = 0;
            s->playing = 0;
        }
        break;
    default:
        return 1 << TL_SHIFT;
    }
    if (e->reverb && e->state != ATTACK && (e->volume >> EG_SHIFT) <= 0x300)
        e->decay1_rate = e->decay2_rate = e->release_rate = (int32_t)m->decay_release_step[17];
    return m->linear_to_exp_volume[e->volume >> EG_SHIFT];
}

static uint32_t get_rate(const uint32_t *steps, int32_t rate, uint32_t val)
{
    if (val == 0)
        return steps[0];
    if (val == 0xf)
        return steps[0x3f];
    int r = 4 * (int)val + rate;
    if (r < 0) r = 0;
    if (r > 0x3f) r = 0x3f;
    return steps[r];
}

static void envelope_calc(multipcm *m, slot_t *s)
{
    int32_t octave = s->octave;
    if (octave & 8)
        octave -= 16;
    int32_t rate = s->sample.key_rate_scale != 0xf
                       ? (octave + s->sample.key_rate_scale) * 2 + ((s->pitch >> 9) & 1) : 0;
    s->eg.attack_rate = (int32_t)get_rate(m->attack_step, rate, s->sample.attack_reg);
    s->eg.decay1_rate = (int32_t)get_rate(m->decay_release_step, rate, s->sample.decay1_reg);
    s->eg.decay2_rate = (int32_t)get_rate(m->decay_release_step, rate, s->sample.decay2_reg);
    s->eg.release_rate = (int32_t)get_rate(m->decay_release_step, rate, s->sample.release_reg);
    s->eg.decay_level = 0xf - s->sample.decay_level;
    s->eg.reverb = 0;
}

/* ---------------------------------------------------------------- LFO */

static void lfo_init(multipcm *m)
{
    for (int i = 0; i < 256; i++) {
        if (i < 64) m->pitch_table[i] = i * 2 + 128;
        else if (i < 128) m->pitch_table[i] = 383 - i * 2;
        else if (i < 192) m->pitch_table[i] = 384 - i * 2;
        else m->pitch_table[i] = i * 2 - 383;
        m->amplitude_table[i] = i < 128 ? 255 - i * 2 : i * 2 - 256;
    }
    for (int t = 0; t < 8; t++) {
        float limit = PHASE_SCALE_LIMIT[t];
        for (int i = -128; i < 128; i++) {
            float value = limit * (float)i / 128.0f;
            m->pitch_scale_tables[t][i + 128] = (int32_t)value_to_fixed(LFO_SHIFT, powf(2.0f, value / 1200.0f));
        }
        limit = -AMPLITUDE_SCALE_LIMIT[t];
        for (int i = 0; i < 256; i++) {
            float value = limit * (float)i / 256.0f;
            m->amplitude_scale_tables[t][i] = (int32_t)value_to_fixed(LFO_SHIFT, powf(10.0f, value / 20.0f));
        }
    }
}

static int32_t lfo_step(lfo_t *l)
{
    l->phase = (uint16_t)(l->phase + l->phase_step);
    int32_t p = l->table[(l->phase >> LFO_SHIFT) & 0xff];
    p = l->scale[p];
    return p << (TL_SHIFT - LFO_SHIFT);
}

static void lfo_compute_step(multipcm *m, lfo_t *l, uint32_t freq, uint32_t scale, int amplitude)
{
    float step = LFO_FREQ[freq] * 256.0f / m->rate;
    l->phase_step = (uint32_t)((float)(1 << LFO_SHIFT) * step);
    if (amplitude) {
        l->table = m->amplitude_table;
        l->scale = m->amplitude_scale_tables[scale];
    } else {
        l->table = m->pitch_table;
        l->scale = m->pitch_scale_tables[scale];
    }
}

/* ---------------------------------------------------------------- voices */

static void retrigger(multipcm *m, slot_t *s)
{
    s->offset = 0;
    s->prev_sample = 0;
    s->total_level = s->dest_total_level << TL_SHIFT;
    envelope_calc(m, s);
    s->eg.state = ATTACK;
    s->eg.volume = 0;
}

static void update_step(multipcm *m, slot_t *s)
{
    uint8_t oct = (uint8_t)((s->octave - 1) & 0xf);
    uint32_t pitch = m->freq_step_table[s->pitch];
    if (oct & 8)
        pitch >>= (16 - oct);
    else
        pitch <<= oct;
    s->step = (uint32_t)(pitch / m->rate);
}

static void init_sample(multipcm *m, sample_t *smp, uint32_t index)
{
    uint32_t a = index * 12;
    smp->start = (uint32_t)rb(m, a) << 16 | (uint32_t)rb(m, a + 1) << 8 | rb(m, a + 2);
    smp->format = (uint8_t)((smp->start >> 20) & 0xfe);
    smp->start &= 0x3fffff;
    smp->loop = (uint32_t)rb(m, a + 3) << 8 | rb(m, a + 4);
    smp->end = 0x10000 - ((uint32_t)rb(m, a + 5) << 8 | rb(m, a + 6));
    smp->attack_reg = (rb(m, a + 8) >> 4) & 0xf;
    smp->decay1_reg = rb(m, a + 8) & 0xf;
    smp->decay2_reg = rb(m, a + 9) & 0xf;
    smp->decay_level = (rb(m, a + 9) >> 4) & 0xf;
    smp->release_reg = rb(m, a + 10) & 0xf;
    smp->key_rate_scale = (rb(m, a + 10) >> 4) & 0xf;
    smp->lfo_vibrato_reg = rb(m, a + 7);
    smp->lfo_amplitude_reg = rb(m, a + 11) & 0xf;
}

static void write_slot(multipcm *m, slot_t *s, int reg, uint8_t data)
{
    s->regs[reg] = data;
    switch (reg) {
    case 0:   /* pan */
        s->pan = (data >> 4) & 0xf;
        break;
    case 1:   /* sample: also loads envelope and LFO settings */
        init_sample(m, &s->sample, s->regs[1] | ((s->regs[2] & 1) << 8));
        write_slot(m, s, 6, s->sample.lfo_vibrato_reg);
        write_slot(m, s, 7, s->sample.lfo_amplitude_reg);
        if (s->playing)
            retrigger(m, s);
        break;
    case 2: case 3:   /* pitch */
        s->octave = s->regs[3] >> 4;
        s->pitch = (uint16_t)(((s->regs[3] & 0xf) << 6) | (s->regs[2] >> 2));
        update_step(m, s);
        break;
    case 4:   /* key on/off */
        if (data & 0x80) {
            s->playing = 1;
            retrigger(m, s);
        } else if (s->playing) {
            if (s->sample.release_reg != 0xf)
                s->eg.state = RELEASE;
            else
                s->playing = 0;
        }
        break;
    case 5:   /* total level + interpolation */
        s->dest_total_level = (data >> 1) & 0x7f;
        if (!(data & 1))
            s->total_level_step = (s->total_level >> TL_SHIFT) > s->dest_total_level
                                      ? m->total_level_steps[0] : m->total_level_steps[1];
        else
            s->total_level = s->dest_total_level << TL_SHIFT;
        break;
    case 6: case 7:   /* LFO */
        s->lfo_frequency = (s->regs[6] >> 3) & 7;
        s->vibrato = s->regs[6] & 7;
        s->tremolo = s->regs[7] & 7;
        if (data) {
            lfo_compute_step(m, &s->pitch_lfo, s->lfo_frequency, s->vibrato, 0);
            lfo_compute_step(m, &s->amplitude_lfo, s->lfo_frequency, s->tremolo, 1);
        }
        break;
    }
}

/* ---------------------------------------------------------------- interface */

multipcm *multipcm_create(float rate, multipcm_read_fn read, void *user)
{
    multipcm *m = calloc(1, sizeof *m);
    if (!m)
        return NULL;
    m->read = read;
    m->user = user;
    m->rate = rate;

    for (int level = 0; level < 0x80; level++) {
        float vol_db = (float)level * -24.0f / 64.0f;
        float total_level = powf(10.0f, vol_db / 20.0f) / 4.0f;
        for (int pan = 0; pan < 0x10; pan++) {
            float pl, pr;
            if (pan == 8) {
                pl = pr = 0;
            } else if (pan == 0) {
                pl = pr = 1;
            } else if (pan & 8) {
                pl = 1;
                int inv = 0x10 - pan;
                pr = powf(10.0f, (float)inv * -12.0f / 4.0f / 20.0f);
                if ((inv & 7) == 7) pr = 0;
            } else {
                pr = 1;
                pl = powf(10.0f, (float)pan * -12.0f / 4.0f / 20.0f);
                if ((pan & 7) == 7) pl = 0;
            }
            m->left_pan_table[(pan << 7) | level] = (int32_t)value_to_fixed(TL_SHIFT, pl * total_level);
            m->right_pan_table[(pan << 7) | level] = (int32_t)value_to_fixed(TL_SHIFT, pr * total_level);
        }
    }
    for (int i = 0; i < 0x400; i++)
        m->freq_step_table[i] = value_to_fixed(TL_SHIFT, rate * (1024.0f + (float)i) / 1024.0f);
    envelope_init(m, 14.32833);
    m->total_level_steps[0] = (int32_t)(-(float)(0x80 << TL_SHIFT) / (78.2f * 44100.0f / 1000.0f));
    m->total_level_steps[1] = (int32_t)((float)(0x80 << TL_SHIFT) / (78.2f * 2 * 44100.0f / 1000.0f));
    for (int i = 0; i < 0x400; i++) {
        float db = -(96.0f - 96.0f * (float)i / (float)0x400);
        m->linear_to_exp_volume[i] = (int32_t)value_to_fixed(TL_SHIFT, powf(10.0f, db / 20.0f));
    }
    lfo_init(m);
    multipcm_reset(m);
    return m;
}

void multipcm_destroy(multipcm *m) { free(m); }

void multipcm_reset(multipcm *m)
{
    for (int i = 0; i < VOICES; i++) {
        m->slots[i].playing = 0;
        lfo_compute_step(m, &m->slots[i].pitch_lfo, 0, 0, 0);
        lfo_compute_step(m, &m->slots[i].amplitude_lfo, 0, 0, 1);
    }
    m->cur_slot = 0;
    m->address = 0;
}

void multipcm_write(multipcm *m, int offset, uint8_t data)
{
    switch (offset) {
    case 0: {
        if (m->cur_slot >= 0)
            write_slot(m, &m->slots[m->cur_slot], (int)m->address, data);
        break;
    }
    case 1: {
        m->cur_slot = VALUE_TO_CHANNEL[data & 0x1f];
        break;
    }
    case 2:
        m->address = data > 7 ? 7 : data;
        break;
    }
}

uint8_t multipcm_read(multipcm *m) { (void)m; return 0; }

void multipcm_render(multipcm *m, int32_t *out, int n)
{
    for (int i = 0; i < n; i++) {
        int32_t sl = 0, sr = 0;
        for (int v = 0; v < VOICES; v++) {
            slot_t *s = &m->slots[v];
            if (!s->playing)
                continue;
            uint32_t vol = (s->total_level >> TL_SHIFT) | (s->pan << 7);
            uint32_t spos = s->offset >> TL_SHIFT;
            uint32_t step = s->step;
            int32_t csample;
            int32_t fpart = (int32_t)(s->offset & ((1 << TL_SHIFT) - 1));
            if (s->reverse)
                spos = s->sample.end - spos - 1;
            if (s->sample.format & 4) {   /* 12-bit linear */
                uint32_t adr = s->sample.start + (spos >> 1) * 3;
                if (!(spos & 1))
                    csample = (int16_t)(rb(m, adr) << 8 | ((rb(m, adr + 1) & 0xf) << 4));
                else
                    csample = (int16_t)(rb(m, adr + 2) << 8 | (rb(m, adr + 1) & 0xf0));
            } else {
                csample = (int16_t)(rb(m, s->sample.start + spos) << 8);
            }
            int32_t sample = (csample * fpart + s->prev_sample * ((1 << TL_SHIFT) - fpart)) >> TL_SHIFT;
            if (s->vibrato) {
                step = step * (uint32_t)lfo_step(&s->pitch_lfo);
                step >>= TL_SHIFT;
            }
            s->offset += step;
            if (spos ^ (s->offset >> TL_SHIFT))
                s->prev_sample = csample;
            if (s->offset >= (s->sample.end << TL_SHIFT)) {
                s->offset -= (s->sample.end - s->sample.loop) << TL_SHIFT;
                s->reverse = 0;
            }
            if ((s->total_level >> TL_SHIFT) != s->dest_total_level)
                s->total_level += (uint32_t)s->total_level_step;
            if (s->tremolo) {
                sample = sample * lfo_step(&s->amplitude_lfo);
                sample >>= TL_SHIFT;
            }
            sample = (sample * envelope_update(m, s)) >> 10;
            sl += (m->left_pan_table[vol & 0x7ff] * sample) >> TL_SHIFT;
            sr += (m->right_pan_table[vol & 0x7ff] * sample) >> TL_SHIFT;
        }
        out[i * 2] += sl;
        out[i * 2 + 1] += sr;
    }
}

int multipcm_voices_playing(const multipcm *m)
{
    int n = 0;
    for (int i = 0; i < VOICES; i++)
        n += m->slots[i].playing != 0;
    return n;
}
