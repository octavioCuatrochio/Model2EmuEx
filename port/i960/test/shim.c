/* Flat state access for difftest.py (ctypes). */
#include "../i960.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t ip, r[32], cc, ac, pc, sat, prcb, icon;
    double fp[4];
    int32_t fc_index, fc_count;
    uint32_t fc_addr[I960_FRAME_CACHE];
    uint32_t fc_regs[I960_FRAME_CACHE][16];
    uint32_t in_interrupt, stop, multi_count, multi_addr;
    uint32_t iac[4];
    uint32_t lit[32][4];
} snap;

/* I/O pages: reads return a function of the address, writes are logged. */
typedef struct {
    uint32_t log[4096][3];   /* size, addr, value */
    uint32_t nlog;
} io_log;

static io_log iolog;

static uint8_t  io_r8(void *u, uint32_t a)  { (void)u; return (uint8_t)(a ^ 0xa5a5a5a5u); }
static uint16_t io_r16(void *u, uint32_t a) { (void)u; return (uint16_t)(a ^ 0xa5a5a5a5u); }
static uint32_t io_r32(void *u, uint32_t a) { (void)u; return a ^ 0xa5a5a5a5u; }

static void io_w(uint32_t size, uint32_t a, uint32_t v)
{
    if (iolog.nlog < 4096) {
        iolog.log[iolog.nlog][0] = size;
        iolog.log[iolog.nlog][1] = a;
        iolog.log[iolog.nlog][2] = v;
        iolog.nlog++;
    }
}
static void io_w8(void *u, uint32_t a, uint8_t v)   { (void)u; io_w(1, a, v); }
static void io_w16(void *u, uint32_t a, uint16_t v) { (void)u; io_w(2, a, v); }
static void io_w32(void *u, uint32_t a, uint32_t v) { (void)u; io_w(4, a, v); }

i960_state *t_new(uint8_t *ram, uint32_t size)
{
    i960_state *s = malloc(sizeof *s);
    i960_init(s, NULL);
    i960_map_read(s, 0x00000000, 0xffffffff, NULL, io_r8, io_r16, io_r32);
    i960_map_write(s, 0x00000000, 0xffffffff, NULL, io_w8, io_w16, io_w32);
    i960_map_read(s, 0, size - 1, ram, NULL, NULL, NULL);
    i960_map_write(s, 0, size - 1, ram, NULL, NULL, NULL);
    i960_set_code_region(s, 0, 0, size);
    s->region[0].valid = 1;
    s->region[1].base = 0xfff00000;
    s->region[2].base = 0xfff00000;
    s->region[1].valid = 1;
    s->region[2].valid = 1;
    s->cur_region = &s->region[0];
    return s;
}

void t_load(i960_state *s, const snap *n)
{
    s->ip = n->ip;
    memcpy(s->r, n->r, sizeof s->r);
    s->reg_pad = 0;
    s->cc = n->cc; s->ac = n->ac; s->pc = n->pc;
    s->sat = n->sat; s->prcb = n->prcb; s->icon = n->icon;
    memcpy(s->fp, n->fp, sizeof s->fp);
    s->fc_index = n->fc_index; s->fc_count = n->fc_count;
    memcpy(s->fc_addr, n->fc_addr, sizeof s->fc_addr);
    memcpy(s->fc_regs, n->fc_regs, sizeof s->fc_regs);
    s->in_interrupt = (uint8_t)n->in_interrupt;
    s->stop = n->stop;
    s->multi_count = (uint8_t)n->multi_count;
    s->multi_addr = n->multi_addr;
    memcpy(s->iac, n->iac, sizeof s->iac);
    memcpy(s->lit, n->lit, sizeof s->lit);
    s->prev_ip = n->ip;
    s->cur_region = &s->region[0];
    iolog.nlog = 0;
}

void t_save(const i960_state *s, snap *n)
{
    n->ip = s->ip;
    memcpy(n->r, s->r, sizeof s->r);
    n->cc = s->cc; n->ac = s->ac; n->pc = s->pc;
    n->sat = s->sat; n->prcb = s->prcb; n->icon = s->icon;
    memcpy(n->fp, s->fp, sizeof s->fp);
    n->fc_index = s->fc_index; n->fc_count = s->fc_count;
    memcpy(n->fc_addr, s->fc_addr, sizeof n->fc_addr);
    memcpy(n->fc_regs, s->fc_regs, sizeof n->fc_regs);
    n->in_interrupt = s->in_interrupt;
    n->stop = s->stop;
    n->multi_count = s->multi_count;
    n->multi_addr = s->multi_addr;
    memcpy(n->iac, s->iac, sizeof n->iac);
    memcpy(n->lit, s->lit, sizeof n->lit);
}

int t_run(i960_state *s, int cycles) { return i960_execute(s, cycles); }
void t_irq(i960_state *s, int line) { i960_interrupt(s, line); }
void t_clear(i960_state *s, uint32_t addr) { memset(&s->region[0].cache[addr >> 2], 0, sizeof(i960_insn)); }
uint32_t t_iolog(uint32_t *out, uint32_t max)
{
    uint32_t n = iolog.nlog < max ? iolog.nlog : max;
    memcpy(out, iolog.log, n * 3 * sizeof(uint32_t));
    return n;
}
int t_snap_size(void) { return (int)sizeof(snap); }
