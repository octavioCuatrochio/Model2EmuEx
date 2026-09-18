/* Model 2 sound boards; see m2snd.h and QUIRKS.md ("Sound"). */
#include "m2snd.h"
#include "multipcm.h"
#include "scsp.h"
#include "ym3438.h"
#include "../m2/m2board.h"
#include "../m68k/m68k.h"

#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

#define M1_RAM_SIZE  0x10000    /* Model 1 board: 0xf00000-0xf0ffff */
#define M2A_RAM_SIZE 0x80000    /* Model 2A: 0x000000-0x07ffff, mirrored to 0x0fffff */
#define SLICE 16
#define QSIZE 256              /* command queue, a power of two */

struct m2_snd {
    int type;
    const uint8_t *prog; uint32_t prog_size;     /* region 5, A68K word-swapped */
    const uint8_t *samples; uint32_t samples_size;   /* region 6 */
    uint8_t *ram; uint32_t ram_size;             /* big-endian */
    double cycles_per_sample, cycle_debt;
    unsigned irq_lines;                          /* asserted 68000 levels (bit n = level n) */

    /* Model 1 board */
    uint8_t fifo[32];
    unsigned fifo_rd, fifo_wr;
    multipcm *pcm[2];
    uint32_t pcm_bank[2];
    ym3438 *ym;

    /* Model 2A */
    scsp *scsp;

    void *ctx;                                   /* this board's Musashi context */
    int32_t mix[SLICE * 2];
    unsigned long chip_writes, commands, cmd_reads;

    /* i960 -> sound board: single producer (m2snd_command), single consumer
       (m2snd_render), so the board can run on the audio thread */
    uint8_t q[QSIZE];
    _Atomic unsigned qw, qr;
};

static m2_snd *cur;   /* Musashi's callbacks are global */

/* makes s the board Musashi runs (saving the other's context) */
static void enter(m2_snd *s)
{
    if (cur == s)
        return;
    if (cur)
        m68k_get_context(cur->ctx);
    m68k_set_context(s->ctx);
    cur = s;
}

/* ------------------------------------------------------------ helpers */

static void update_irq(m2_snd *s)
{
    int level = 0;
    for (int l = 7; l > 0; l--)
        if (s->irq_lines & (1u << l)) { level = l; break; }
    m68k_set_irq((unsigned)level);
}

static void scsp_irq(void *user, int level, int asserted)
{
    m2_snd *s = user;
    if (asserted) s->irq_lines |= 1u << level;
    else s->irq_lines &= ~(1u << level);
    update_irq(s);
}

/* ROM regions are stored like the original's 68000 core wants them: each
   16-bit word little-endian */
static inline uint8_t rom8(const uint8_t *r, uint32_t size, uint32_t a)
{
    a ^= 1;
    return a < size ? r[a] : 0xff;
}

static uint8_t pcm_read(void *user, uint32_t addr)   /* MultiPCM sample space */
{
    m2_snd *s = cur;
    int chip = user != NULL;   /* chip 0: NULL, chip 1: (void *)1 */
    uint32_t base = chip * 0x400000u, off;
    if (addr < 0x100000)
        off = addr;
    else if (addr < 0x200000)
        off = s->pcm_bank[chip] * 0x100000 + (addr - 0x100000);
    else
        return 0;
    off += base;
    return off < s->samples_size ? s->samples[off] : 0;
}

/* ------------------------------------------------------------ Model 1 board (type 0) */

static uint8_t m1_read8(m2_snd *s, uint32_t a)
{
    if (a < 0x80000)
        return rom8(s->prog, s->prog_size, a);
    if (a >= 0x80000 && a < 0xa0000)
        return rom8(s->prog, s->prog_size, a - 0x60000);
    if (a >= 0xf00000 && a < 0xf10000)
        return s->ram[a - 0xf00000];
    if (a == 0xc20001) {   /* UART data (orig 0x4c6a40): pops the FIFO, IRQ off when empty */
        uint8_t v = 0;
        s->cmd_reads++;
        if (s->fifo_rd != s->fifo_wr) {
            v = s->fifo[s->fifo_rd];
            s->fifo_rd = (s->fifo_rd + 1) & 31;
        }
        if (s->fifo_rd == s->fifo_wr) {
            s->irq_lines &= ~(1u << 2);
            update_irq(s);
        }
        return v;
    }
    if (a >= 0xd00000 && a <= 0xd00007 && (a & 1))
        return ym3438_read(s->ym, (int)((a >> 1) & 3));
    return 0;   /* UART status and MultiPCM status read 0 (as the original) */
}

static void m1_write8(m2_snd *s, uint32_t a, uint8_t v)
{
    if (a >= 0xf00000 && a < 0xf10000) {
        s->ram[a - 0xf00000] = v;
        return;
    }
    if (a >= 0xc40000 && a <= 0xc4000f) {
        if (a & 1)
            multipcm_write(s->pcm[0], (int)((a >> 1) & 3), v), s->chip_writes++;
        return;
    }
    if (a >= 0xc60000 && a <= 0xc6000f) {
        if (a & 1)
            multipcm_write(s->pcm[1], (int)((a >> 1) & 3), v), s->chip_writes++;
        return;
    }
    if (a == 0xc50001) { s->pcm_bank[0] = v & 3; return; }   /* orig: & 7 */
    if (a == 0xc70001) { s->pcm_bank[1] = v & 3; return; }
    if (a >= 0xd00000 && a <= 0xd00007 && (a & 1)) {
        ym3438_write(s->ym, (int)((a >> 1) & 3), v);
        s->chip_writes++;
    }
}

/* ------------------------------------------------------------ Model 2A board (type 1) */

static uint8_t m2a_read8(m2_snd *s, uint32_t a)
{
    if (a < 0x100000)
        return s->ram[a & (M2A_RAM_SIZE - 1)];
    if (a < 0x101000) {
        uint16_t w = scsp_read16(s->scsp, a & 0xffe);
        return (uint8_t)(a & 1 ? w : w >> 8);
    }
    if (a >= 0x600000 && a < 0x680000)
        return rom8(s->prog, s->prog_size, a - 0x600000);
    if (a >= 0x800000)
        return rom8(s->samples, s->samples_size, a - 0x800000);
    return 0;
}

static uint16_t m2a_read16(m2_snd *s, uint32_t a)
{
    if (a >= 0x100000 && a < 0x101000)
        return scsp_read16(s->scsp, a & 0xffe);
    return (uint16_t)(m2a_read8(s, a) << 8 | m2a_read8(s, a + 1));
}

static void m2a_write8(m2_snd *s, uint32_t a, uint8_t v)
{
    if (a < 0x100000)
        s->ram[a & (M2A_RAM_SIZE - 1)] = v;
    else if (a < 0x101000)
        scsp_write16(s->scsp, a & 0xffe, a & 1 ? v : (uint16_t)(v << 8), a & 1 ? 0x00ff : 0xff00);
    /* 0x400000: sample bank control, ignored by the original too */
}

static void m2a_write16(m2_snd *s, uint32_t a, uint16_t v)
{
    if (a >= 0x100000 && a < 0x101000) {
        scsp_write16(s->scsp, a & 0xffe, v, 0xffff);
        s->chip_writes++;
        return;
    }
    m2a_write8(s, a, (uint8_t)(v >> 8));
    m2a_write8(s, a + 1, (uint8_t)v);
}

/* ------------------------------------------------------------ Musashi callbacks */

unsigned int m68k_read_memory_8(unsigned int a)
{
    a &= 0xffffff;
    return cur->type == 0 ? m1_read8(cur, a) : m2a_read8(cur, a);
}

unsigned int m68k_read_memory_16(unsigned int a)
{
    a &= 0xffffff;
    if (cur->type == 0) {
        if (a < 0x80000 || (a >= 0x80000 && a < 0xa0000)) {   /* fast path: ROM */
            uint32_t o = a < 0x80000 ? a : a - 0x60000;
            if (o + 1 < cur->prog_size)
                return (unsigned)(cur->prog[o] | cur->prog[o + 1] << 8);
        }
        if (a >= 0xf00000 && a < 0xf10000)
            return (unsigned)(cur->ram[a - 0xf00000] << 8 | cur->ram[a - 0xf00000 + 1]);
        return (unsigned)(m1_read8(cur, a) << 8 | m1_read8(cur, a + 1));
    }
    if (a < 0x100000) {
        uint32_t o = a & (M2A_RAM_SIZE - 1);
        return (unsigned)(cur->ram[o] << 8 | cur->ram[o + 1]);
    }
    if (a >= 0x600000 && a < 0x680000 && a - 0x600000 + 1 < cur->prog_size)
        return (unsigned)(cur->prog[a - 0x600000] | cur->prog[a - 0x600000 + 1] << 8);
    return m2a_read16(cur, a);
}

unsigned int m68k_read_memory_32(unsigned int a)
{
    return m68k_read_memory_16(a) << 16 | m68k_read_memory_16(a + 2);
}

void m68k_write_memory_8(unsigned int a, unsigned int v)
{
    a &= 0xffffff;
    if (cur->type == 0) m1_write8(cur, a, (uint8_t)v);
    else m2a_write8(cur, a, (uint8_t)v);
}

void m68k_write_memory_16(unsigned int a, unsigned int v)
{
    a &= 0xffffff;
    if (cur->type == 0) {
        m1_write8(cur, a, (uint8_t)(v >> 8));
        m1_write8(cur, a + 1, (uint8_t)v);
    } else {
        m2a_write16(cur, a, (uint16_t)v);
    }
}

void m68k_write_memory_32(unsigned int a, unsigned int v)
{
    m68k_write_memory_16(a, v >> 16);
    m68k_write_memory_16(a + 2, v & 0xffff);
}

/* Interrupt lines are levels, held until the device drops them (UART FIFO
   empty, SCSP SCIRE). Musashi's default acknowledge clears the level. */
static int int_ack(int level) { (void)level; return M68K_INT_ACK_AUTOVECTOR; }

unsigned int m68k_read_disassembler_16(unsigned int a) { return m68k_read_memory_16(a); }
unsigned int m68k_read_disassembler_32(unsigned int a) { return m68k_read_memory_32(a); }

/* ------------------------------------------------------------ interface */

m2_snd *m2snd_create(const m2_board *b)
{
    m2_snd *s = calloc(1, sizeof *s);
    if (!s)
        return NULL;
    s->type = b->type == 0 ? 0 : 1;
    s->prog = b->rom.ptr[5];
    s->prog_size = b->rom.size[5];
    s->samples = b->rom.ptr[6];
    s->samples_size = b->rom.size[6];
    s->ram_size = s->type == 0 ? M1_RAM_SIZE : M2A_RAM_SIZE;
    s->ram = calloc(1, s->ram_size + 2);
    s->ctx = calloc(1, m68k_context_size());
    if (!s->ram || !s->prog || !s->ctx) {
        m2snd_destroy(s);
        return NULL;
    }
    if (cur)   /* keep the running board's CPU */
        m68k_get_context(cur->ctx);
    cur = s;
    if (s->type == 0) {
        s->cycles_per_sample = 10000000.0 / M2SND_RATE;
        s->pcm[0] = multipcm_create(M2SND_RATE, pcm_read, NULL);
        s->pcm[1] = multipcm_create(M2SND_RATE, pcm_read, (void *)1);
        s->ym = ym3438_create(8000000, M2SND_RATE);
        if (!s->pcm[0] || !s->pcm[1] || !s->ym) {
            m2snd_destroy(s);
            return NULL;
        }
    } else {
        s->cycles_per_sample = 11289600.0 / M2SND_RATE;
        s->scsp = scsp_create(s->ram, M2A_RAM_SIZE - 1, scsp_irq, s);
        if (!s->scsp) {
            m2snd_destroy(s);
            return NULL;
        }
    }
    m68k_init();
    m68k_set_cpu_type(M68K_CPU_TYPE_68000);
    m68k_set_int_ack_callback(int_ack);
    m2snd_reset(s);
    m68k_get_context(s->ctx);
    return s;
}

void m2snd_destroy(m2_snd *s)
{
    if (!s)
        return;
    if (s->pcm[0]) multipcm_destroy(s->pcm[0]);
    if (s->pcm[1]) multipcm_destroy(s->pcm[1]);
    if (s->ym) ym3438_destroy(s->ym);
    if (s->scsp) scsp_destroy(s->scsp);
    free(s->ram);
    free(s->ctx);
    if (cur == s)
        cur = NULL;
    free(s);
}

void m2snd_reset(m2_snd *s)
{
    enter(s);
    memset(s->ram, 0, s->ram_size);
    s->fifo_rd = s->fifo_wr = 0;
    atomic_store(&s->qr, atomic_load(&s->qw));   /* drop pending commands */
    s->irq_lines = 0;
    s->pcm_bank[0] = s->pcm_bank[1] = 0;
    s->cycle_debt = 0;
    if (s->type == 0) {
        multipcm_reset(s->pcm[0]);
        multipcm_reset(s->pcm[1]);
        ym3438_reset(s->ym);
    } else {
        scsp_reset(s->scsp);
        /* the vector table is copied from the program ROM into RAM */
        for (uint32_t i = 0; i < 16; i++)
            s->ram[i] = rom8(s->prog, s->prog_size, i);
    }
    m68k_pulse_reset();
    m68k_set_irq(0);
}

void m2snd_command(m2_snd *s, uint8_t cmd)
{
    unsigned w = atomic_load_explicit(&s->qw, memory_order_relaxed);
    if (w - atomic_load_explicit(&s->qr, memory_order_acquire) >= QSIZE)
        return;   /* the sound board is far behind: drop */
    s->q[w & (QSIZE - 1)] = cmd;
    atomic_store_explicit(&s->qw, w + 1, memory_order_release);
}

static void deliver(m2_snd *s, uint8_t cmd)
{
    s->commands++;
    if (s->type == 0) {
        unsigned next = (s->fifo_wr + 1) & 31;
        if (next != s->fifo_rd) {   /* the original overwrites; a full FIFO drops here */
            s->fifo[s->fifo_wr] = cmd;
            s->fifo_wr = next;
        }
        s->irq_lines |= 1u << 2;
        update_irq(s);
    } else {
        scsp_midi_in(s->scsp, cmd);
    }
}

static void drain(m2_snd *s)
{
    unsigned r = atomic_load_explicit(&s->qr, memory_order_relaxed);
    unsigned w = atomic_load_explicit(&s->qw, memory_order_acquire);
    if (r == w)
        return;
    for (; r != w; r++)
        deliver(s, s->q[r & (QSIZE - 1)]);
    atomic_store_explicit(&s->qr, r, memory_order_release);
}

void m2snd_render(m2_snd *s, int16_t *out, int n)
{
    enter(s);
    while (n > 0) {
        int k = n < SLICE ? n : SLICE;
        drain(s);
        s->cycle_debt += s->cycles_per_sample * k;
        int run = (int)s->cycle_debt;
        s->cycle_debt -= m68k_execute(run);

        if (s->type == 0) {
            memset(s->mix, 0, sizeof(int32_t) * (size_t)k * 2);
            multipcm_render(s->pcm[0], s->mix, k);
            multipcm_render(s->pcm[1], s->mix, k);
            for (int i = 0; i < k * 2; i++)
                s->mix[i] /= 2;   /* MAME: 0.5 per MultiPCM */
            ym3438_render(s->ym, s->mix, k, 0.30f);
        } else {
            scsp_render(s->scsp, s->mix, k);
        }
        for (int i = 0; i < k * 2; i++) {
            int32_t v = s->mix[i];
            out[i] = (int16_t)(v > 32767 ? 32767 : v < -32768 ? -32768 : v);
        }
        out += k * 2;
        n -= k;
    }
}

#include <stdio.h>
void m2snd_debug(m2_snd *s, char *buf, int size)
{
    enter(s);
    int a = 0, b = 0, mvol = 0;
    if (s->type == 0) {
        a = multipcm_voices_playing(s->pcm[0]);
        b = multipcm_voices_playing(s->pcm[1]);
    } else {
        a = scsp_slots_active(s->scsp, &mvol);
    }
    snprintf(buf, (size_t)size, "68k pc %06x sr %04x irq %02x | cmds %lu, cmd reads %lu, chip writes %lu | voices %d %d mvol %d",
             m68k_get_reg(NULL, M68K_REG_PC), m68k_get_reg(NULL, M68K_REG_SR), s->irq_lines,
             s->commands, s->cmd_reads, s->chip_writes, a, b, mvol);
}

