/* SCSP; see scsp.h. Follows MAME's scsp.cpp / scspdsp.cpp function by
   function (BSD-3-Clause, ElSemi, R. Belmont; thanks to kingshriek). */
#include "scsp.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define SHIFT     12
#define LFO_SHIFT 8
#define EG_SHIFT  16
#define FIX(v)    ((uint32_t)((float)(1 << SHIFT) * (v)))

enum { ATTACK, DECAY1, DECAY2, RELEASE };

typedef struct {
    int volume, state, step, AR, D1R, D2R, RR, DL;
    uint8_t EGHOLD, LPLINK;
} eg_t;

typedef struct {
    uint16_t phase;
    uint32_t phase_step;
    const int *table, *scale;
} lfo_t;

typedef struct {
    union { uint16_t data[0x10]; uint8_t datab[0x20]; } u;
    uint8_t Backwards, active;
    uint32_t cur_addr, nxt_addr, step;
    eg_t EG;
    lfo_t PLFO, ALFO;
    int slot;
    int16_t Prev;
} slot_t;

typedef struct {
    uint32_t RBP, RBL;
    int16_t COEF[64];
    uint16_t MADRS[32];
    uint16_t MPRO[128 * 4];
    int32_t TEMP[128], MEMS[32];
    uint32_t DEC;
    int32_t MIXS[16];
    int16_t EXTS[2];
    int16_t EFREG[16];
    int Stopped, LastStep;
} dsp_t;

struct scsp {
    uint8_t *ram;
    uint32_t ram_mask;
    scsp_irq_fn irq;
    void *user;

    union { uint16_t data[0x30 / 2]; uint8_t datab[0x30]; } u;
    slot_t Slots[32];
    int16_t RINGBUF[128];
    uint8_t BUFPTR;
    uint32_t IrqTimA, IrqTimBC, IrqMidi, IrqCPU, IrqDMA;
    uint8_t latched_MSLC;
    uint16_t latched_MSLC_data;
    uint8_t MidiStack[32], MidiW, MidiR;
    int32_t EG_TABLE[0x400];
    int LPANTABLE[0x10000], RPANTABLE[0x10000];
    int TimCnt[3];
    struct { uint32_t dmea; uint16_t drga, dtlg; uint8_t dgate, ddir; } dma;
    uint16_t mcieb, mcipd;
    int ARTABLE[64], DRTABLE[64];
    dsp_t DSP;
    int16_t *RBUFDST;
    int PLFO_TRI[256], PLFO_SQR[256], PLFO_SAW[256], PLFO_NOI[256];
    int ALFO_TRI[256], ALFO_SQR[256], ALFO_SAW[256], ALFO_NOI[256];
    int PSCALES[8][256], ASCALES[8][256];
    uint32_t rng;
};

/* slot parameters */
#define KEYONEX(s) ((s)->u.data[0x0] & 0x1000)
#define KEYONB(s)  ((s)->u.data[0x0] & 0x0800)
#define SBCTL(s)   (((s)->u.data[0x0] >> 0x9) & 0x0003)
#define SSCTL(s)   (((s)->u.data[0x0] >> 0x7) & 0x0003)
#define LPCTL(s)   (((s)->u.data[0x0] >> 0x5) & 0x0003)
#define PCM8B(s)   ((s)->u.data[0x0] & 0x0010)
#define SA(s)      ((((uint32_t)(s)->u.data[0x0] & 0xF) << 16) | (s)->u.data[0x1])
#define LSA(s)     ((s)->u.data[0x2])
#define LEA(s)     ((s)->u.data[0x3])
#define D2R(s)     (((s)->u.data[0x4] >> 0xB) & 0x001F)
#define D1R(s)     (((s)->u.data[0x4] >> 0x6) & 0x001F)
#define EGHOLD(s)  ((s)->u.data[0x4] & 0x0020)
#define AR(s)      ((s)->u.data[0x4] & 0x001F)
#define LPSLNK(s)  ((s)->u.data[0x5] & 0x4000)
#define KRS(s)     (((s)->u.data[0x5] >> 0xA) & 0x000F)
#define DL(s)      (((s)->u.data[0x5] >> 0x5) & 0x001F)
#define RR(s)      ((s)->u.data[0x5] & 0x001F)
#define STWINH(s)  ((s)->u.data[0x6] & 0x0200)
#define SDIR(s)    ((s)->u.data[0x6] & 0x0100)
#define TL(s)      ((s)->u.data[0x6] & 0x00FF)
#define MDL(s)     (((s)->u.data[0x7] >> 0xC) & 0x000F)
#define MDXSL(s)   (((s)->u.data[0x7] >> 0x6) & 0x003F)
#define MDYSL(s)   ((s)->u.data[0x7] & 0x003F)
#define OCT(s)     (((s)->u.data[0x8] >> 0xB) & 0x000F)
#define FNS(s)     ((s)->u.data[0x8] & 0x03FF)
#define LFORE(s)   ((s)->u.data[0x9] & 0x8000)
#define LFOF(s)    (((s)->u.data[0x9] >> 0xA) & 0x001F)
#define PLFOWS(s)  (((s)->u.data[0x9] >> 0x8) & 0x0003)
#define PLFOS(s)   (((s)->u.data[0x9] >> 0x5) & 0x0007)
#define ALFOWS(s)  (((s)->u.data[0x9] >> 0x3) & 0x0003)
#define ALFOS(s)   ((s)->u.data[0x9] & 0x0007)
#define ISEL(s)    (((s)->u.data[0xA] >> 0x3) & 0x000F)
#define IMXL(s)    ((s)->u.data[0xA] & 0x0007)
#define DISDL(s)   (((s)->u.data[0xB] >> 0xD) & 0x0007)
#define DIPAN(s)   (((s)->u.data[0xB] >> 0x8) & 0x001F)
#define EFSDL(s)   (((s)->u.data[0xB] >> 0x5) & 0x0007)
#define EFPAN(s)   ((s)->u.data[0xB] & 0x001F)

/* common registers */
#define DAC18B(c)  ((c)->u.data[0] & 0x0100)
#define MVOL(c)    ((c)->u.data[0] & 0x000F)
#define RBL(c)     (((c)->u.data[1] >> 0x7) & 0x0003)
#define RBP(c)     ((c)->u.data[1] & 0x003F)
#define SCILV0(c)  ((c)->u.data[0x24 / 2] & 0xff)
#define SCILV1(c)  ((c)->u.data[0x26 / 2] & 0xff)
#define SCILV2(c)  ((c)->u.data[0x28 / 2] & 0xff)
#define SCIMID 3
#define SCIDMA 4
#define SCIIRQ 5
#define SCITMA 6
#define SCITMB 7

static const double ARTimes[64] = { 100000, 100000, 8100.0, 6900.0, 6000.0, 4800.0, 4000.0, 3400.0, 3000.0, 2400.0, 2000.0, 1700.0, 1500.0,
    1200.0, 1000.0, 860.0, 760.0, 600.0, 500.0, 430.0, 380.0, 300.0, 250.0, 220.0, 190.0, 150.0, 130.0, 110.0, 95.0,
    76.0, 63.0, 55.0, 47.0, 38.0, 31.0, 27.0, 24.0, 19.0, 15.0, 13.0, 12.0, 9.4, 7.9, 6.8, 6.0, 4.7, 3.8, 3.4, 3.0, 2.4,
    2.0, 1.8, 1.6, 1.3, 1.1, 0.93, 0.85, 0.65, 0.53, 0.44, 0.40, 0.35, 0.0, 0.0 };
static const double DRTimes[64] = { 100000, 100000, 118200.0, 101300.0, 88600.0, 70900.0, 59100.0, 50700.0, 44300.0, 35500.0, 29600.0, 25300.0, 22200.0, 17700.0,
    14800.0, 12700.0, 11100.0, 8900.0, 7400.0, 6300.0, 5500.0, 4400.0, 3700.0, 3200.0, 2800.0, 2200.0, 1800.0, 1600.0, 1400.0, 1100.0,
    920.0, 790.0, 690.0, 550.0, 460.0, 390.0, 340.0, 270.0, 230.0, 200.0, 170.0, 140.0, 110.0, 98.0, 85.0, 68.0, 57.0, 49.0, 43.0, 34.0,
    28.0, 25.0, 22.0, 18.0, 14.0, 12.0, 11.0, 8.5, 7.1, 6.1, 5.4, 4.3, 3.6, 3.1 };
static const float SDLT[8] = { -1000000.0f, -36.0f, -30.0f, -24.0f, -18.0f, -12.0f, -6.0f, 0.0f };
static const float LFOFreq[32] = { 0.17f, 0.19f, 0.23f, 0.27f, 0.34f, 0.39f, 0.45f, 0.55f, 0.68f, 0.78f, 0.92f, 1.10f, 1.39f, 1.60f, 1.87f, 2.27f,
    2.87f, 3.31f, 3.92f, 4.79f, 6.15f, 7.18f, 8.60f, 10.8f, 14.4f, 17.2f, 21.5f, 28.7f, 43.1f, 57.4f, 86.1f, 172.3f };
static const float ASCALE[8] = { 0.0f, 0.4f, 0.8f, 1.5f, 3.0f, 6.0f, 12.0f, 24.0f };
static const float PSCALE[8] = { 0.0f, 7.0f, 13.5f, 27.0f, 55.0f, 112.0f, 230.0f, 494.0f };

static inline int clampi(int v, int lo, int hi) { return v < lo ? lo : v > hi ? hi : v; }
static inline int32_t sext(int32_t v, int bits) { return (int32_t)((uint32_t)v << (32 - bits)) >> (32 - bits); }
static uint32_t rnd(scsp *s) { s->rng = s->rng * 1103515245u + 12345u; return s->rng >> 8; }

/* sound RAM, big-endian */
static inline uint8_t read_byte(scsp *s, uint32_t a) { return s->ram[a & s->ram_mask]; }
static inline uint16_t read_word(scsp *s, uint32_t a)
{
    a &= s->ram_mask & ~1u;
    return (uint16_t)(s->ram[a] << 8 | s->ram[a + 1]);
}
static inline void write_word(scsp *s, uint32_t a, uint16_t v)
{
    a &= s->ram_mask & ~1u;
    s->ram[a] = (uint8_t)(v >> 8);
    s->ram[a + 1] = (uint8_t)v;
}

/* ------------------------------------------------------------ DSP (scspdsp.cpp) */

static uint16_t PACK(int32_t val)
{
    int sign = (val >> 23) & 1;
    uint32_t temp = (uint32_t)(val ^ (val << 1)) & 0xFFFFFF;
    int exponent = 0;
    for (int k = 0; k < 12; k++) {
        if (temp & 0x800000)
            break;
        temp <<= 1;
        exponent++;
    }
    if (exponent < 12)
        val = (val << exponent) & 0x3FFFFF;
    else
        val <<= 11;
    val >>= 11;
    val &= 0x7FF;
    val |= sign << 15;
    val |= exponent << 11;
    return (uint16_t)val;
}

static int32_t UNPACK(uint16_t val)
{
    int sign = (val >> 15) & 1;
    int exponent = (val >> 11) & 0xF;
    int mantissa = val & 0x7FF;
    int32_t uval = mantissa << 11;
    if (exponent > 11) {
        exponent = 11;
        uval |= sign << 22;
    } else {
        uval |= (sign ^ 1) << 22;
    }
    uval |= sign << 23;
    uval = sext(uval, 24);
    uval >>= exponent;
    return uval;
}

static void dsp_init(dsp_t *d)
{
    memset(d, 0, sizeof *d);
    d->RBL = 8 * 1024;
    d->Stopped = 1;
}

static void dsp_start(dsp_t *d)
{
    int i;
    d->Stopped = 0;
    for (i = 127; i >= 0; --i) {
        const uint16_t *p = d->MPRO + i * 4;
        if (p[0] || p[1] || p[2] || p[3])
            break;
    }
    d->LastStep = i + 1;
}

static void dsp_step(scsp *s)
{
    dsp_t *d = &s->DSP;
    if (d->Stopped)
        return;
    memset(d->EFREG, 0, sizeof d->EFREG);
    int32_t ACC = 0, MEMVAL = 0, FRC_REG = 0, Y_REG = 0;
    uint32_t ADRS_REG = 0;

    for (int step = 0; step < d->LastStep; ++step) {
        const uint16_t *IPtr = d->MPRO + step * 4;
        uint32_t TRA = (IPtr[0] >> 8) & 0x7f, TWT = (IPtr[0] >> 7) & 1, TWA = IPtr[0] & 0x7f;
        uint32_t XSEL = (IPtr[1] >> 15) & 1, YSEL = (IPtr[1] >> 13) & 3, IRA = (IPtr[1] >> 6) & 0x3f;
        uint32_t IWT = (IPtr[1] >> 5) & 1, IWA = IPtr[1] & 0x1f;
        uint32_t TABLE = (IPtr[2] >> 15) & 1, MWT = (IPtr[2] >> 14) & 1, MRD = (IPtr[2] >> 13) & 1;
        uint32_t EWT = (IPtr[2] >> 12) & 1, EWA = (IPtr[2] >> 8) & 0xf, ADRL = (IPtr[2] >> 7) & 1;
        uint32_t FRCL = (IPtr[2] >> 6) & 1, SHIFTV = (IPtr[2] >> 4) & 3, YRL = (IPtr[2] >> 3) & 1;
        uint32_t NEGB = (IPtr[2] >> 2) & 1, ZERO = (IPtr[2] >> 1) & 1, BSEL = IPtr[2] & 1;
        uint32_t NOFL = (IPtr[3] >> 15) & 1, COEF = (IPtr[3] >> 9) & 0x3f, MASA = (IPtr[3] >> 2) & 0x1f;
        uint32_t ADREB = (IPtr[3] >> 1) & 1, NXADR = IPtr[3] & 1;

        int32_t INPUTS;
        if (IRA <= 0x1f)
            INPUTS = d->MEMS[IRA];
        else if (IRA <= 0x2f)
            INPUTS = d->MIXS[IRA - 0x20] << 4;
        else if (IRA <= 0x31)
            INPUTS = d->EXTS[IRA - 0x30] << 8;
        else
            return;
        INPUTS = sext(INPUTS, 24);
        if (IWT) {
            d->MEMS[IWA] = MEMVAL;
            if (IRA == IWA)
                INPUTS = MEMVAL;
        }

        int32_t B;
        if (!ZERO) {
            B = BSEL ? ACC : sext(d->TEMP[(TRA + d->DEC) & 0x7f], 24);
            if (NEGB)
                B = 0 - B;
        } else {
            B = 0;
        }
        int32_t X = XSEL ? INPUTS : sext(d->TEMP[(TRA + d->DEC) & 0x7f], 24);
        int32_t Y = 0;
        if (YSEL == 0) Y = FRC_REG;
        else if (YSEL == 1) Y = d->COEF[COEF] >> 3;
        else if (YSEL == 2) Y = (Y_REG >> 11) & 0x1fff;
        else Y = (Y_REG >> 4) & 0x0fff;
        if (YRL)
            Y_REG = INPUTS;

        int32_t SHIFTED;
        if (SHIFTV == 0) SHIFTED = clampi(ACC, -0x00800000, 0x007fffff);
        else if (SHIFTV == 1) SHIFTED = clampi(ACC * 2, -0x00800000, 0x007fffff);
        else if (SHIFTV == 2) SHIFTED = sext(ACC * 2, 24);
        else SHIFTED = sext(ACC, 24);

        Y = sext(Y, 13);
        int64_t v = ((int64_t)X * (int64_t)Y) >> 12;
        ACC = (int32_t)(v + B);

        if (TWT)
            d->TEMP[(TWA + d->DEC) & 0x7f] = SHIFTED;
        if (FRCL)
            FRC_REG = SHIFTV == 3 ? (SHIFTED & 0x0fff) : ((SHIFTED >> 11) & 0x1fff);

        if (MRD || MWT) {
            uint32_t ADDR = d->MADRS[MASA];
            if (!TABLE) ADDR += d->DEC;
            if (ADREB) ADDR += ADRS_REG & 0x0FFF;
            if (NXADR) ADDR++;
            if (!TABLE) ADDR &= d->RBL - 1;
            else ADDR &= 0xffff;
            ADDR += d->RBP << 12;
            ADDR <<= 1;
            if (MRD && (step & 1))
                MEMVAL = NOFL ? (int32_t)read_word(s, ADDR) << 8 : UNPACK(read_word(s, ADDR));
            if (MWT && (step & 1))
                write_word(s, ADDR, NOFL ? (uint16_t)(SHIFTED >> 8) : PACK(SHIFTED));
        }
        if (ADRL)
            ADRS_REG = SHIFTV == 3 ? (uint32_t)((SHIFTED >> 12) & 0xfff) : (uint32_t)(INPUTS >> 16);
        if (EWT)
            d->EFREG[EWA] = (int16_t)(d->EFREG[EWA] + (SHIFTED >> 8));
    }
    --d->DEC;
    memset(d->MIXS, 0, sizeof d->MIXS);
}

/* ------------------------------------------------------------ interrupts */

static void irq_line(scsp *s, uint32_t level, int asserted)
{
    if (level)   /* MAME: line 0 does not exist, clearing it does nothing */
        s->irq(s->user, (int)level, asserted);
}

static uint8_t DecodeSCI(scsp *s, uint8_t irq)
{
    uint8_t SCI = 0;
    SCI |= (SCILV0(s) & (1 << irq)) ? 1 : 0;
    SCI |= ((SCILV1(s) & (1 << irq)) ? 1 : 0) << 1;
    SCI |= ((SCILV2(s) & (1 << irq)) ? 1 : 0) << 2;
    return SCI;
}

static void CheckPendingIRQ(scsp *s)
{
    uint32_t pend = s->u.data[0x20 / 2], en = s->u.data[0x1e / 2];
    if (s->MidiW != s->MidiR) {
        s->u.data[0x20 / 2] |= 8;
        pend |= 8;
    }
    if (!pend)
        return;
    if (pend & en & 0x20) { irq_line(s, s->IrqCPU, 1); return; }
    if ((pend & 0x40) && (en & 0x40)) { irq_line(s, s->IrqTimA, 1); return; }
    if ((pend & 0x80) && (en & 0x80)) { irq_line(s, s->IrqTimBC, 1); return; }
    if ((pend & 0x100) && (en & 0x100)) { irq_line(s, s->IrqTimBC, 1); return; }
    if ((pend & 8) && (en & 8)) { irq_line(s, s->IrqMidi, 1); return; }
}

static void ResetInterrupts(scsp *s)
{
    uint32_t reset = s->u.data[0x22 / 2];
    if (reset & 0x40) irq_line(s, s->IrqTimA, 0);
    if (reset & 0x180) irq_line(s, s->IrqTimBC, 0);
    if (reset & 0x20) {
        s->u.data[0x20 / 2] &= ~0x20;
        irq_line(s, s->IrqCPU, 0);
    }
    if (reset & 0x8) irq_line(s, s->IrqMidi, 0);
    CheckPendingIRQ(s);
}

/* Timers counted in output samples (ElSemi's original SCSP timer code) */
static void timers_add_ticks(scsp *s, int ticks)
{
    static const int reg[3] = { 0x18 / 2, 0x1a / 2, 0x1c / 2 }, bit[3] = { 0x40, 0x80, 0x100 };
    int fired = 0;
    for (int t = 0; t < 3; t++) {
        if (s->TimCnt[t] <= 0xff00) {
            s->TimCnt[t] += ticks << (8 - ((s->u.data[reg[t]] >> 8) & 7));
            if (s->TimCnt[t] >= 0xff00) {
                s->TimCnt[t] = 0xffff;
                s->u.data[0x20 / 2] |= bit[t];
                fired = 1;
            }
            s->u.data[reg[t]] = (uint16_t)((s->u.data[reg[t]] & 0xff00) | (s->TimCnt[t] >> 8));
        }
    }
    if (fired)
        CheckPendingIRQ(s);
}

/* ------------------------------------------------------------ envelope, LFO */

static int Get_AR(scsp *s, int base, int R) { return s->ARTABLE[clampi(base + (R << 1), 0, 63)]; }
static int Get_DR(scsp *s, int base, int R) { return s->DRTABLE[clampi(base + (R << 1), 0, 63)]; }

static void Compute_EG(scsp *s, slot_t *slot)
{
    int octave = (OCT(slot) ^ 8) - 8;
    int rate = KRS(slot) != 0xf ? octave + 2 * KRS(slot) + ((FNS(slot) >> 9) & 1) : 0;
    slot->EG.volume = 0x17F << EG_SHIFT;
    slot->EG.AR = Get_AR(s, rate, AR(slot));
    slot->EG.D1R = Get_DR(s, rate, D1R(slot));
    slot->EG.D2R = Get_DR(s, rate, D2R(slot));
    slot->EG.RR = Get_DR(s, rate, RR(slot));
    slot->EG.DL = 0x1f - DL(slot);
    slot->EG.EGHOLD = (uint8_t)(EGHOLD(slot) != 0);
}

static void StopSlot(slot_t *slot, int keyoff)
{
    if (keyoff)
        slot->EG.state = RELEASE;
    else
        slot->active = 0;
    slot->u.data[0] &= ~0x800;
}

static int EG_Update(slot_t *slot)
{
    switch (slot->EG.state) {
    case ATTACK:
        slot->EG.volume += slot->EG.AR;
        if (slot->EG.volume >= (0x3ff << EG_SHIFT)) {
            if (!LPSLNK(slot)) {
                slot->EG.state = DECAY1;
                if (slot->EG.D1R >= (1024 << EG_SHIFT))
                    slot->EG.state = DECAY2;
            }
            slot->EG.volume = 0x3ff << EG_SHIFT;
        }
        if (slot->EG.EGHOLD)
            return 0x3ff << (SHIFT - 10);
        break;
    case DECAY1:
        slot->EG.volume -= slot->EG.D1R;
        if (slot->EG.volume <= 0)
            slot->EG.volume = 0;
        if (slot->EG.volume >> (EG_SHIFT + 5) <= slot->EG.DL)
            slot->EG.state = DECAY2;
        break;
    case DECAY2:
        if (D2R(slot) == 0)
            return (slot->EG.volume >> EG_SHIFT) << (SHIFT - 10);
        slot->EG.volume -= slot->EG.D2R;
        if (slot->EG.volume <= 0)
            slot->EG.volume = 0;
        break;
    case RELEASE:
        slot->EG.volume -= slot->EG.RR;
        if (slot->EG.volume <= 0) {
            slot->EG.volume = 0;
            StopSlot(slot, 0);
        }
        break;
    default:
        return 1 << SHIFT;
    }
    return (slot->EG.volume >> EG_SHIFT) << (SHIFT - 10);
}

static uint32_t Step(slot_t *slot)
{
    int octave = (OCT(slot) ^ 8) - 8 + SHIFT - 10;
    uint32_t Fn = FNS(slot) + (1 << 10);
    return octave >= 0 ? Fn << octave : Fn >> -octave;
}

static void LFO_ComputeStep(scsp *s, lfo_t *LFO, uint32_t LFOF, uint32_t LFOWS, uint32_t LFOS, int ALFO)
{
    float step = LFOFreq[LFOF] * 256.0f / 44100.0f;
    LFO->phase_step = (uint32_t)((float)(1 << LFO_SHIFT) * step);
    if (ALFO) {
        const int *t[4] = { s->ALFO_SAW, s->ALFO_SQR, s->ALFO_TRI, s->ALFO_NOI };
        LFO->table = t[LFOWS & 3];
        LFO->scale = s->ASCALES[LFOS];
    } else {
        const int *t[4] = { s->PLFO_SAW, s->PLFO_SQR, s->PLFO_TRI, s->PLFO_NOI };
        LFO->table = t[LFOWS & 3];
        LFO->scale = s->PSCALES[LFOS];
    }
}

static void Compute_LFO(scsp *s, slot_t *slot)
{
    if (PLFOS(slot))
        LFO_ComputeStep(s, &slot->PLFO, LFOF(slot), PLFOWS(slot), PLFOS(slot), 0);
    if (ALFOS(slot))
        LFO_ComputeStep(s, &slot->ALFO, LFOF(slot), ALFOWS(slot), ALFOS(slot), 1);
}

static int32_t PLFO_Step(lfo_t *LFO)
{
    LFO->phase = (uint16_t)(LFO->phase + LFO->phase_step);
    int p = LFO->table[LFO->phase >> LFO_SHIFT];
    p = LFO->scale[p + 128];
    return p << (SHIFT - LFO_SHIFT);
}

static int32_t ALFO_Step(lfo_t *LFO)
{
    LFO->phase = (uint16_t)(LFO->phase + LFO->phase_step);
    int p = LFO->table[LFO->phase >> LFO_SHIFT];
    p = LFO->scale[p];
    return p << (SHIFT - LFO_SHIFT);
}

static void StartSlot(scsp *s, slot_t *slot)
{
    slot->active = 1;
    slot->cur_addr = 0;
    slot->nxt_addr = 1 << SHIFT;
    slot->step = Step(slot);
    Compute_EG(s, slot);
    slot->EG.state = ATTACK;
    slot->EG.volume = 0x17F << EG_SHIFT;
    slot->Prev = 0;
    slot->Backwards = 0;
    Compute_LFO(s, slot);
}

/* ------------------------------------------------------------ registers */

static void exec_dma(scsp *s);

static void UpdateSlotReg(scsp *s, int sn, int r)
{
    slot_t *slot = s->Slots + sn;
    switch (r & 0x3f) {
    case 0: case 1:
        if (KEYONEX(slot)) {
            for (int sl = 0; sl < 32; ++sl) {
                slot_t *s2 = s->Slots + sl;
                if (KEYONB(s2) && s2->EG.state == RELEASE)
                    StartSlot(s, s2);
                if (!KEYONB(s2))
                    StopSlot(s2, 1);
            }
            slot->u.data[0] &= ~0x1000;
        }
        break;
    case 0x10: case 0x11:
        slot->step = Step(slot);
        break;
    case 0xA: case 0xB:
        slot->EG.RR = Get_DR(s, 0, RR(slot));
        slot->EG.DL = 0x1f - DL(slot);
        break;
    case 0x12: case 0x13:
        Compute_LFO(s, slot);
        break;
    }
}

static void UpdateReg(scsp *s, int reg)
{
    switch (reg & 0x3f) {
    case 0x2: case 0x3:
        s->DSP.RBL = (8 * 1024) << RBL(s);
        s->DSP.RBP = RBP(s);
        break;
    case 0x6: case 0x7:
        break;   /* MIDI out: nothing listens on Model 2 (the i960 never reads it) */
    case 0x8: case 0x9:
        s->latched_MSLC = (uint8_t)((s->u.data[0x8 / 2] & 0xf800) >> 11);
        break;
    case 0x12: case 0x13:
        s->dma.dmea = (s->u.data[0x12 / 2] & 0xfffe) | (s->dma.dmea & 0xf0000);
        break;
    case 0x14: case 0x15:
        s->dma.dmea = ((uint32_t)(s->u.data[0x14 / 2] & 0xf000) << 4) | (s->dma.dmea & 0xfffe);
        s->dma.drga = s->u.data[0x14 / 2] & 0x0ffe;
        break;
    case 0x16: case 0x17:
        s->dma.dtlg = s->u.data[0x16 / 2] & 0x0ffe;
        s->dma.ddir = (uint8_t)((s->u.data[0x16 / 2] & 0x2000) >> 13);
        s->dma.dgate = (uint8_t)((s->u.data[0x16 / 2] & 0x4000) >> 14);
        if (s->u.data[0x16 / 2] & 0x1000)
            exec_dma(s);
        break;
    case 0x18: case 0x19:
        s->TimCnt[0] = (s->u.data[0x18 / 2] & 0xff) << 8;
        break;
    case 0x1a: case 0x1b:
        s->TimCnt[1] = (s->u.data[0x1a / 2] & 0xff) << 8;
        break;
    case 0x1c: case 0x1d:
        s->TimCnt[2] = (s->u.data[0x1c / 2] & 0xff) << 8;
        break;
    case 0x1e: case 0x1f:   /* SCIEB */
        CheckPendingIRQ(s);
        break;
    case 0x20: case 0x21:   /* SCIPD */
        if (s->u.data[0x1e / 2] & s->u.data[0x20 / 2] & 0x20)
            CheckPendingIRQ(s);
        break;
    case 0x22: case 0x23:   /* SCIRE */
        s->u.data[0x20 / 2] &= ~s->u.data[0x22 / 2];
        ResetInterrupts(s);
        if (s->TimCnt[0] == 0xffff) s->u.data[0x20 / 2] |= 0x40;
        if (s->TimCnt[1] == 0xffff) s->u.data[0x20 / 2] |= 0x80;
        if (s->TimCnt[2] == 0xffff) s->u.data[0x20 / 2] |= 0x100;
        break;
    case 0x24: case 0x25: case 0x26: case 0x27: case 0x28: case 0x29:
        s->IrqTimA = DecodeSCI(s, SCITMA);
        s->IrqTimBC = DecodeSCI(s, SCITMB);
        s->IrqMidi = DecodeSCI(s, SCIMID);
        s->IrqCPU = DecodeSCI(s, SCIIRQ);
        s->IrqDMA = DecodeSCI(s, SCIDMA);
        break;
    case 0x2a: case 0x2b:
        s->mcieb = s->u.data[0x2a / 2];
        break;
    case 0x2c: case 0x2d:
        if (s->u.data[0x2c / 2] & 0x20)
            s->mcipd |= 0x20;
        break;
    case 0x2e: case 0x2f:
        s->mcipd &= ~s->u.data[0x2e / 2];
        break;
    }
}

static void UpdateRegR(scsp *s, int reg)
{
    switch (reg & 0x3f) {
    case 4: case 5: {   /* MIDI in */
        uint16_t v = (uint16_t)((s->u.data[0x4 / 2] & 0xff00) | s->MidiStack[s->MidiR]);
        if (s->MidiR != s->MidiW)
            s->MidiR = (s->MidiR + 1) & 31;
        if (s->MidiR == s->MidiW) {
            irq_line(s, s->IrqMidi, 0);
            s->u.data[0x20 / 2] &= ~8;
        }
        s->u.data[0x4 / 2] = v;
        break;
    }
    case 8: case 9:
        s->u.data[0x8 / 2] = s->latched_MSLC_data;
        break;
    case 0x2a: case 0x2b:
        s->u.data[0x2a / 2] = s->mcieb;
        break;
    case 0x2c: case 0x2d:
        s->u.data[0x2c / 2] = s->mcipd;
        break;
    }
}

static void w16(scsp *s, uint32_t addr, uint16_t val)
{
    addr &= 0xffff;
    if (addr < 0x400) {
        int slot = (int)(addr / 0x20);
        addr &= 0x1f;
        s->Slots[slot].u.data[addr / 2] = val;
        UpdateSlotReg(s, slot, (int)(addr & 0x1f));
    } else if (addr < 0x600) {
        if (addr < 0x430) {
            if (addr == 0x420 || addr == 0x42e)   /* SCIPD, MCIPD: only bit 5 writable */
                s->u.data[(addr & 0x3f) / 2] |= val & 0x20;
            else
                s->u.data[(addr & 0x3f) / 2] = val;
            UpdateReg(s, (int)(addr & 0x3f));
        }
    } else if (addr < 0x700) {
        s->RINGBUF[(addr - 0x600) / 2] = (int16_t)val;
    } else if (addr < 0x780) {
        s->DSP.COEF[(addr - 0x700) / 2] = (int16_t)val;
    } else if (addr < 0x7c0) {
        s->DSP.MADRS[(addr - 0x780) / 2] = val;
    } else if (addr < 0x800) {
        s->DSP.MADRS[(addr - 0x7c0) / 2] = val;
    } else if (addr < 0xC00) {
        s->DSP.MPRO[(addr - 0x800) / 2] = val;
        if (addr == 0xBF0)
            dsp_start(&s->DSP);
    }
}

static uint16_t r16(scsp *s, uint32_t addr)
{
    uint16_t v = 0;
    addr &= 0xffff;
    if (addr < 0x400) {
        v = s->Slots[addr / 0x20].u.data[(addr & 0x1f) / 2];
    } else if (addr < 0x600) {
        if (addr < 0x430) {
            UpdateRegR(s, (int)(addr & 0x3f));
            v = s->u.data[(addr & 0x3f) / 2];
        }
    } else if (addr < 0x700) {
        v = (uint16_t)s->RINGBUF[(addr - 0x600) / 2];
    } else if (addr < 0x780) {
        v = (uint16_t)s->DSP.COEF[(addr - 0x700) / 2];
    } else if (addr < 0x7c0) {
        v = s->DSP.MADRS[(addr - 0x780) / 2];
    } else if (addr < 0x800) {
        v = s->DSP.MADRS[(addr - 0x7c0) / 2];
    } else if (addr < 0xC00) {
        v = s->DSP.MPRO[(addr - 0x800) / 2];
    } else if (addr < 0xE00) {
        int32_t t = s->DSP.TEMP[(addr >> 2) & 0x7f];
        v = (uint16_t)((addr & 2) ? t & 0xffff : (uint32_t)t >> 16);
    } else if (addr < 0xE80) {
        int32_t t = s->DSP.MEMS[(addr >> 2) & 0x1f];
        v = (uint16_t)((addr & 2) ? t & 0xffff : (uint32_t)t >> 16);
    } else if (addr < 0xEC0) {
        int32_t t = s->DSP.MIXS[(addr >> 2) & 0xf];
        v = (uint16_t)((addr & 2) ? t & 0xffff : (uint32_t)t >> 16);
    } else if (addr < 0xEE0) {
        v = (uint16_t)s->DSP.EFREG[(addr - 0xec0) / 2];
    } else if (addr < 0xEE4) {
        v = (uint16_t)s->DSP.EXTS[(addr - 0xee0) / 2];
    }
    return v;
}

static void exec_dma(scsp *s)
{
    uint16_t tmp_dma[3];
    int i;
    if (!s->dma.ddir)
        for (i = 0; i < 3; i++)
            tmp_dma[i] = s->u.data[(0x12 + i * 2) / 2];
    if (s->dma.ddir) {
        for (i = 0; i < s->dma.dtlg; i += 2) {
            write_word(s, s->dma.dmea, s->dma.dgate ? 0 : r16(s, s->dma.drga));
            s->dma.dmea += 2;
            if (!s->dma.dgate)
                s->dma.drga += 2;
        }
    } else {
        for (i = 0; i < s->dma.dtlg; i += 2) {
            w16(s, s->dma.drga, s->dma.dgate ? 0 : read_word(s, s->dma.dmea));
            if (!s->dma.dgate)
                s->dma.dmea += 2;
            s->dma.drga += 2;
        }
        for (i = 0; i < 3; i++)
            s->u.data[(0x12 + i * 2) / 2] = tmp_dma[i];
    }
    s->u.data[0x16 / 2] &= ~0x1000;
    if (s->u.data[0x1e / 2] & 0x10) {   /* DMA end irq (MAME: HOLD_LINE) */
        irq_line(s, s->IrqDMA, 1);
    }
}

/* ------------------------------------------------------------ voices */

static int32_t UpdateSlot(scsp *s, slot_t *slot)
{
    if (SSCTL(slot) == 3)
        return 0;
    int32_t sample = 0;
    int step = (int)slot->step;
    uint32_t addr1, addr2;
    uint32_t *addr[2] = { &addr1, &addr2 };
    uint32_t *slot_addr[2] = { &slot->cur_addr, &slot->nxt_addr };

    if (PLFOS(slot)) {
        step = step * PLFO_Step(&slot->PLFO);
        step >>= SHIFT;
    }
    if (PCM8B(slot)) {
        addr1 = slot->cur_addr >> SHIFT;
        addr2 = slot->nxt_addr >> SHIFT;
    } else {
        addr1 = (slot->cur_addr >> (SHIFT - 1)) & ~1u;
        addr2 = (slot->nxt_addr >> (SHIFT - 1)) & ~1u;
    }
    if (MDL(slot) || MDXSL(slot) || MDYSL(slot)) {
        int32_t smp = (s->RINGBUF[(s->BUFPTR + MDXSL(slot)) & 63] + s->RINGBUF[(s->BUFPTR + MDYSL(slot)) & 63]) / 2;
        smp <<= 0xA;
        smp >>= 0x1A - MDL(slot);
        if (!PCM8B(slot))
            smp <<= 1;
        addr1 += (uint32_t)smp;
        addr2 += (uint32_t)smp;
    }
    if (SSCTL(slot) == 0) {
        int32_t fpart = (int32_t)(slot->cur_addr & ((1 << SHIFT) - 1));
        if (PCM8B(slot)) {
            int8_t p1 = (int8_t)read_byte(s, SA(slot) + addr1);
            int8_t p2 = (int8_t)read_byte(s, SA(slot) + addr2);
            sample = ((int)(p1 << 8) * ((1 << SHIFT) - fpart) + (int)(p2 << 8) * fpart) >> SHIFT;
        } else {
            int16_t p1 = (int16_t)read_word(s, SA(slot) + addr1);
            int16_t p2 = (int16_t)read_word(s, SA(slot) + addr2);
            sample = ((int)p1 * ((1 << SHIFT) - fpart) + (int)p2 * fpart) >> SHIFT;
        }
    } else if (SSCTL(slot) == 1) {
        sample = (int16_t)(rnd(s) & 0xffff);
    } else {
        sample = 0;
    }
    if (SBCTL(slot) & 1)
        sample ^= 0x7FFF;
    if (SBCTL(slot) & 2)
        sample = (int16_t)(sample ^ 0x8000);

    if (slot->Backwards)
        slot->cur_addr -= (uint32_t)step;
    else
        slot->cur_addr += (uint32_t)step;
    slot->nxt_addr = slot->cur_addr + (1 << SHIFT);
    addr1 = slot->cur_addr >> SHIFT;
    addr2 = slot->nxt_addr >> SHIFT;

    if (addr1 >= LSA(slot) && !slot->Backwards)
        if (LPSLNK(slot) && slot->EG.state == ATTACK)
            slot->EG.state = DECAY1;

    for (int sel = 0; sel < 2; sel++) {
        int32_t rem;
        switch (LPCTL(slot)) {
        case 0:   /* no loop */
            if (*addr[sel] >= LSA(slot) && *addr[sel] >= LEA(slot))
                StopSlot(slot, 0);
            break;
        case 1:   /* normal loop */
            if (*addr[sel] >= LEA(slot)) {
                rem = (int32_t)(*slot_addr[sel] - ((uint32_t)LEA(slot) << SHIFT));
                *slot_addr[sel] = ((uint32_t)LSA(slot) << SHIFT) + (uint32_t)rem;
            }
            break;
        case 2:   /* reverse loop */
            if (*addr[sel] >= LSA(slot) && !slot->Backwards) {
                rem = (int32_t)(*slot_addr[sel] - ((uint32_t)LSA(slot) << SHIFT));
                *slot_addr[sel] = ((uint32_t)LEA(slot) << SHIFT) - (uint32_t)rem;
                slot->Backwards = 1;
            } else if ((*addr[sel] < LSA(slot) || (*slot_addr[sel] & 0x80000000)) && slot->Backwards) {
                rem = (int32_t)(((uint32_t)LSA(slot) << SHIFT) - *slot_addr[sel]);
                *slot_addr[sel] = ((uint32_t)LEA(slot) << SHIFT) - (uint32_t)rem;
            }
            break;
        case 3:   /* ping-pong */
            if (*addr[sel] >= LEA(slot)) {
                rem = (int32_t)(*slot_addr[sel] - ((uint32_t)LEA(slot) << SHIFT));
                *slot_addr[sel] = ((uint32_t)LEA(slot) << SHIFT) - (uint32_t)rem;
                slot->Backwards = 1;
            } else if ((*addr[sel] < LSA(slot) || (*slot_addr[sel] & 0x80000000)) && slot->Backwards) {
                rem = (int32_t)(((uint32_t)LSA(slot) << SHIFT) - *slot_addr[sel]);
                *slot_addr[sel] = ((uint32_t)LSA(slot) << SHIFT) + (uint32_t)rem;
                slot->Backwards = 0;
            }
            break;
        }
    }

    if (!SDIR(slot)) {
        if (ALFOS(slot)) {
            sample = sample * ALFO_Step(&slot->ALFO);
            sample >>= SHIFT;
        }
        if (slot->EG.state == ATTACK)
            sample = (sample * EG_Update(slot)) >> SHIFT;
        else
            sample = (sample * s->EG_TABLE[EG_Update(slot) >> (SHIFT - 10)]) >> SHIFT;
    }
    if (!STWINH(slot)) {
        uint16_t Enc = SDIR(slot) ? (uint16_t)(0x7 << 0xd) : (uint16_t)(TL(slot) | (0x7 << 0xd));
        *s->RBUFDST = (int16_t)((sample * s->LPANTABLE[Enc]) >> (SHIFT + 1));
    }
    return sample;
}

void scsp_render(scsp *s, int32_t *out, int n)
{
    int mvol = MVOL(s);
    for (int i = 0; i < n; ++i) {
        int32_t smpl = 0, smpr = 0;
        for (int sl = 0; sl < 32; ++sl) {
            s->RBUFDST = s->RINGBUF + s->BUFPTR;
            if (s->Slots[sl].active) {
                slot_t *slot = s->Slots + sl;
                int32_t sample = UpdateSlot(s, slot);
                uint16_t eff_tl = SDIR(slot) ? 0 : TL(slot);
                uint16_t Enc = (uint16_t)(eff_tl | (IMXL(slot) << 0xd));
                s->DSP.MIXS[ISEL(slot)] += (sample * s->LPANTABLE[Enc]) >> (SHIFT - 2);
                Enc = (uint16_t)(eff_tl | (DIPAN(slot) << 0x8) | (DISDL(slot) << 0xd));
                smpl += (sample * s->LPANTABLE[Enc]) >> SHIFT;
                smpr += (sample * s->RPANTABLE[Enc]) >> SHIFT;
            }
            s->BUFPTR = (s->BUFPTR + 1) & 63;
        }
        dsp_step(s);
        for (int e = 0; e < 16; ++e) {
            slot_t *slot = s->Slots + e;
            if (EFSDL(slot)) {
                uint16_t Enc = (uint16_t)((EFPAN(slot) << 0x8) | (EFSDL(slot) << 0xd));
                smpl += (s->DSP.EFREG[e] * s->LPANTABLE[Enc]) >> SHIFT;
                smpr += (s->DSP.EFREG[e] * s->RPANTABLE[Enc]) >> SHIFT;
            }
        }
        /* EXTS inputs are silent on Model 2 */
        smpl = (smpl >> 2) * mvol / 15;
        smpr = (smpr >> 2) * mvol / 15;
        out[i * 2] = smpl;
        out[i * 2 + 1] = smpr;

        /* MSLC monitor, latched every sample */
        slot_t *m = s->Slots + s->latched_MSLC;
        uint32_t SGC = (uint32_t)m->EG.state & 3, CA = (m->cur_addr >> (SHIFT + 12)) & 0xf;
        uint32_t EG = (0x1f - (m->EG.volume >> (EG_SHIFT + 5))) & 0x1f;
        s->latched_MSLC_data = (uint16_t)((CA << 7) | (SGC << 5) | EG);
        timers_add_ticks(s, 1);
    }
}

/* ------------------------------------------------------------ interface */

static void init_tables(scsp *s)
{
    for (int i = 0; i < 0x400; ++i) {
        float envDB = (float)(3 * (i - 0x3ff)) / 32.0f;
        s->EG_TABLE[i] = (int32_t)(powf(10.0f, envDB / 20.0f) * (float)(1 << SHIFT));
    }
    for (int i = 0; i < 0x10000; ++i) {
        int iTL = i & 0xff, iPAN = (i >> 8) & 0x1f, iSDL = (i >> 0xD) & 7;
        float SegaDB = 0, TLv, PAN, LPAN, RPAN, fSDL;
        if (iTL & 0x01) SegaDB -= 0.4f;
        if (iTL & 0x02) SegaDB -= 0.8f;
        if (iTL & 0x04) SegaDB -= 1.5f;
        if (iTL & 0x08) SegaDB -= 3.0f;
        if (iTL & 0x10) SegaDB -= 6.0f;
        if (iTL & 0x20) SegaDB -= 12.0f;
        if (iTL & 0x40) SegaDB -= 24.0f;
        if (iTL & 0x80) SegaDB -= 48.0f;
        TLv = powf(10.0f, SegaDB / 20.0f);
        SegaDB = 0;
        if (iPAN & 0x1) SegaDB -= 3.0f;
        if (iPAN & 0x2) SegaDB -= 6.0f;
        if (iPAN & 0x4) SegaDB -= 12.0f;
        if (iPAN & 0x8) SegaDB -= 24.0f;
        PAN = (iPAN & 0xf) == 0xf ? 0.0f : powf(10.0f, SegaDB / 20.0f);
        if (iPAN < 0x10) { LPAN = PAN; RPAN = 1.0f; }
        else { RPAN = PAN; LPAN = 1.0f; }
        fSDL = iSDL ? powf(10.0f, SDLT[iSDL] / 20.0f) : 0.0f;
        s->LPANTABLE[i] = (int)FIX(4.0f * LPAN * TLv * fSDL);
        s->RPANTABLE[i] = (int)FIX(4.0f * RPAN * TLv * fSDL);
    }
    s->ARTABLE[0] = s->DRTABLE[0] = 0;
    s->ARTABLE[1] = s->DRTABLE[1] = 0;
    for (int i = 2; i < 64; ++i) {
        double t = ARTimes[i], scale = (double)(1 << EG_SHIFT);
        s->ARTABLE[i] = t != 0.0 ? (int)((1023 * 1000.0) / (44100.0 * t) * scale) : 1024 << EG_SHIFT;
        t = DRTimes[i];
        s->DRTABLE[i] = (int)((1023 * 1000.0) / (44100.0 * t) * scale);
    }
    for (int i = 0; i < 256; ++i) {
        int a, p;
        a = 255 - i;
        p = i < 128 ? i : i - 256;
        s->ALFO_SAW[i] = a; s->PLFO_SAW[i] = p;
        if (i < 128) { a = 255; p = 127; } else { a = 0; p = -128; }
        s->ALFO_SQR[i] = a; s->PLFO_SQR[i] = p;
        a = i < 128 ? 255 - i * 2 : i * 2 - 256;
        if (i < 64) p = i * 2;
        else if (i < 128) p = 255 - i * 2;
        else if (i < 192) p = 256 - i * 2;
        else p = i * 2 - 511;
        s->ALFO_TRI[i] = a; s->PLFO_TRI[i] = p;
        a = (int)(rnd(s) & 0xff);
        p = 128 - a;
        s->ALFO_NOI[i] = a; s->PLFO_NOI[i] = p;
    }
    for (int sc = 0; sc < 8; ++sc) {
        float limit = PSCALE[sc];
        for (int i = -128; i < 128; ++i)
            s->PSCALES[sc][i + 128] = (int)(uint32_t)((float)(1 << LFO_SHIFT) * powf(2.0f, (limit * (float)i / 128.0f) / 1200.0f));
        limit = -ASCALE[sc];
        for (int i = 0; i < 256; ++i)
            s->ASCALES[sc][i] = (int)(uint32_t)((float)(1 << LFO_SHIFT) * powf(10.0f, (limit * (float)i / 256.0f) / 20.0f));
    }
}

scsp *scsp_create(uint8_t *ram, uint32_t ram_mask, scsp_irq_fn irq, void *user)
{
    scsp *s = calloc(1, sizeof *s);
    if (!s)
        return NULL;
    s->ram = ram;
    s->ram_mask = ram_mask;
    s->irq = irq;
    s->user = user;
    s->rng = 0x12345678;
    init_tables(s);
    scsp_reset(s);
    return s;
}

void scsp_destroy(scsp *s) { free(s); }

void scsp_reset(scsp *s)
{
    memset(&s->u, 0, sizeof s->u);
    memset(s->Slots, 0, sizeof s->Slots);
    memset(s->RINGBUF, 0, sizeof s->RINGBUF);
    s->BUFPTR = 0;
    dsp_init(&s->DSP);
    s->IrqTimA = s->IrqTimBC = s->IrqMidi = s->IrqCPU = s->IrqDMA = 0;
    s->MidiR = s->MidiW = 0;
    s->mcieb = s->mcipd = 0;
    s->latched_MSLC = 0;
    for (int i = 0; i < 32; ++i) {
        s->Slots[i].slot = i;
        s->Slots[i].active = 0;
        s->Slots[i].EG.state = RELEASE;
    }
    s->TimCnt[0] = s->TimCnt[1] = s->TimCnt[2] = 0xffff;
}

uint16_t scsp_read16(scsp *s, uint32_t offset) { return r16(s, offset & 0xffe); }

void scsp_write16(scsp *s, uint32_t offset, uint16_t data, uint16_t mask)
{
    offset &= 0xffe;
    uint16_t tmp = r16(s, offset);
    tmp = (uint16_t)((tmp & ~mask) | (data & mask));
    w16(s, offset, tmp);
}

void scsp_midi_in(scsp *s, uint8_t data)
{
    s->MidiStack[s->MidiW] = data;
    s->MidiW = (s->MidiW + 1) & 31;
    CheckPendingIRQ(s);
}

int scsp_slots_active(const scsp *s, int *mvol)
{
    int n = 0;
    for (int i = 0; i < 32; i++)
        n += s->Slots[i].active != 0;
    if (mvol)
        *mvol = MVOL(s);
    return n;
}
