/*
 * Intel i960KB core, ported from Model 2 Emulator 1.1a. See i960.h.
 * Comments of the form "orig 0x4cXXXX" give the address of the function in
 * EMULATOR.EXE that a piece of code was ported from.
 *
 * Compile-time switches (all default to 0 = behave like the original):
 *   I960_ACCURATE          fix the emulator bugs listed in QUIRKS.md that
 *                          change what guest code sees
 *   I960_FP_SINGLE         round add/sub/mul/div/sqrt results to single
 *                          precision, as the original's x87 FPU was usually
 *                          left in 24-bit precision mode (QUIRKS.md)
 */
#include "i960.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef I960_ACCURATE
#define I960_ACCURATE 0
#endif
#ifndef I960_FP_SINGLE
#define I960_FP_SINGLE 0
#endif

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error "i960 core assumes a little-endian host"
#endif

/* ---------------------------------------------------------------- memory */

static inline uint16_t ld16(const uint8_t *p) { uint16_t v; memcpy(&v, p, 2); return v; }
static inline uint32_t ld32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }
static inline void st16(uint8_t *p, uint16_t v) { memcpy(p, &v, 2); }
static inline void st32(uint8_t *p, uint32_t v) { memcpy(p, &v, 4); }

static inline uint8_t rd8(i960_state *s, uint32_t a)
{
    const i960_read_page *p = &s->rpage[a >> 16];
    return p->host ? p->host[a & 0xffff] : p->r8(s->user, a);
}

static inline uint16_t rd16(i960_state *s, uint32_t a)
{
    const i960_read_page *p = &s->rpage[a >> 16];
    return p->host ? ld16(p->host + (a & 0xffff)) : p->r16(s->user, a);
}

static inline uint32_t rd32(i960_state *s, uint32_t a)
{
    const i960_read_page *p = &s->rpage[a >> 16];
    return p->host ? ld32(p->host + (a & 0xffff)) : p->r32(s->user, a);
}

static inline void wr8(i960_state *s, uint32_t a, uint8_t v)
{
    const i960_write_page *p = &s->wpage[a >> 16];
    if (p->host) p->host[a & 0xffff] = v; else p->w8(s->user, a, v);
}

static inline void wr16(i960_state *s, uint32_t a, uint16_t v)
{
    const i960_write_page *p = &s->wpage[a >> 16];
    if (p->host) st16(p->host + (a & 0xffff), v); else p->w16(s->user, a, v);
}

static inline void wr32(i960_state *s, uint32_t a, uint32_t v)
{
    const i960_write_page *p = &s->wpage[a >> 16];
    if (p->host) st32(p->host + (a & 0xffff), v); else p->w32(s->user, a, v);
}

uint8_t  i960_read8(i960_state *s, uint32_t a)  { return rd8(s, a); }
uint16_t i960_read16(i960_state *s, uint32_t a) { return rd16(s, a); }
uint32_t i960_read32(i960_state *s, uint32_t a) { return rd32(s, a); }
void i960_write8(i960_state *s, uint32_t a, uint8_t v)   { wr8(s, a, v); }
void i960_write16(i960_state *s, uint32_t a, uint16_t v) { wr16(s, a, v); }
void i960_write32(i960_state *s, uint32_t a, uint32_t v) { wr32(s, a, v); }

/* orig 0x4c0730: the original stores host - start in every page of the
   range; storing the host address of each page's first byte is equivalent
   and avoids forming out-of-range pointers. */
void i960_map_read(i960_state *s, uint32_t start, uint32_t end, uint8_t *host,
                   i960_read8_fn r8, i960_read16_fn r16, i960_read32_fn r32)
{
    for (uint32_t pg = start >> 16; pg <= end >> 16; pg++) {
        i960_read_page *p = &s->rpage[pg];
        p->host = host ? host + ((int64_t)pg << 16) - (int64_t)start : NULL;
        p->r8 = r8;
        p->r16 = r16;
        p->r32 = r32;
    }
}

void i960_map_write(i960_state *s, uint32_t start, uint32_t end, uint8_t *host,
                    i960_write8_fn w8, i960_write16_fn w16, i960_write32_fn w32)
{
    for (uint32_t pg = start >> 16; pg <= end >> 16; pg++) {
        i960_write_page *p = &s->wpage[pg];
        p->host = host ? host + ((int64_t)pg << 16) - (int64_t)start : NULL;
        p->w8 = w8;
        p->w16 = w16;
        p->w32 = w32;
    }
}

/* Unmapped pages read as zero and ignore writes instead of crashing. */
static uint8_t  unmapped_r8(void *u, uint32_t a)  { (void)u; (void)a; return 0; }
static uint16_t unmapped_r16(void *u, uint32_t a) { (void)u; (void)a; return 0; }
static uint32_t unmapped_r32(void *u, uint32_t a) { (void)u; (void)a; return 0; }
static void unmapped_w8(void *u, uint32_t a, uint8_t v)   { (void)u; (void)a; (void)v; }
static void unmapped_w16(void *u, uint32_t a, uint16_t v) { (void)u; (void)a; (void)v; }
static void unmapped_w32(void *u, uint32_t a, uint32_t v) { (void)u; (void)a; (void)v; }

/* ------------------------------------------------------------ operands */

#define U(p)  (*(uint32_t *)(p))
#define I(p)  (*(int32_t *)(p))
#define F(p)  (*(float *)(p))
#define D(p)  (*(double *)(p))

#define SRC1(in) U((in)->src1)
#define SRC2(in) U((in)->src2)
#define DST(in)  U((in)->dst)

/* Floating-point literals 0x10 (+0.0) and 0x16 (+1.0). */
static double fp_lit_zero = 0.0;    /* orig 0x10ed948 */
static double fp_lit_one = 1.0;     /* orig 0x573590 */

/* Where a write through an m3-literal destination goes. The original writes
   into the literal table itself. */
static void *lit_dst(i960_state *s, uint32_t reg)
{
#if I960_ACCURATE
    static uint32_t sink[4];
    (void)s; (void)reg;
    return sink;
#else
    return s->lit[reg];
#endif
}

/* orig 0x4c2a70 (src1), 0x4c2990 (src2): REG-format sources. */
static void *reg_src(i960_state *s, const i960_opdef *op, uint32_t reg, int literal)
{
    if (!literal)
        return &s->r[reg];
    if (!op->fp)
        return s->lit[reg];
    switch (reg) {
    case 0: case 1: case 2: case 3:
        return &s->fp[reg];
    case 0x10:
        return &fp_lit_zero;
    case 0x16:
        return &fp_lit_one;
    default:
        return s->lit[0];
    }
}

/* orig 0x4c2910: REG-format destination. */
static void *reg_dst(i960_state *s, const i960_opdef *op, uint32_t reg, int literal)
{
    if (!literal)
        return &s->r[reg];
    if (!op->fp)
        return lit_dst(s, reg);
    if (reg > 3)
        return lit_dst(s, 0);
    return &s->fp[reg];
}

/* MEM-format effective address routines, orig 0x4c27b0-0x4c28a0. Modes
   that carry a displacement word skip it by advancing ip. */
static uint32_t ea_offset(i960_state *s, i960_insn *in)           /* MEMA, offset */
{ (void)s; return in->mem_disp; }
static uint32_t ea_abase_offset(i960_state *s, i960_insn *in)     /* MEMA, abase+offset */
{ (void)s; return *in->abase + in->mem_disp; }
static uint32_t ea_abase(i960_state *s, i960_insn *in)            /* mode 4 */
{ (void)s; return *in->abase; }
static uint32_t ea_ip_disp(i960_state *s, i960_insn *in)          /* mode 5 */
{ s->ip += 4; return in->mem_disp; }
static uint32_t ea_abase_index(i960_state *s, i960_insn *in)      /* mode 7 */
{ (void)s; return (*in->index << in->scale) + *in->abase; }
static uint32_t ea_disp(i960_state *s, i960_insn *in)             /* mode 0xc */
{ s->ip += 4; return in->mem_disp; }
static uint32_t ea_abase_disp(i960_state *s, i960_insn *in)       /* mode 0xd */
{ s->ip += 4; return *in->abase + in->mem_disp; }
static uint32_t ea_index_disp(i960_state *s, i960_insn *in)       /* mode 0xe */
{ s->ip += 4; return (*in->index << in->scale) + in->mem_disp; }
static uint32_t ea_abase_index_disp(i960_state *s, i960_insn *in) /* mode 0xf */
{ s->ip += 4; return (*in->index << in->scale) + *in->abase + in->mem_disp; }

/* Reserved MEMB modes. The original leaves the entry's EA routine as it
   was (a null pointer for a fresh entry). */
static uint32_t ea_reserved(i960_state *s, i960_insn *in)
{
    if (s->invalid_opcode)
        s->invalid_opcode(s->user, s->ip - 4, in->raw);
    return 0;
}

/* orig 0x4c36c0. `addr` is the address of the instruction word. */
static void decode_mem(i960_state *s, i960_insn *in, uint32_t addr)
{
    uint32_t raw = in->raw;

    in->abase = &s->r[(raw >> 14) & 0x1f];
    if (!(raw & 0x1000)) {
        in->mem_disp = raw & 0x1fff;
        in->ea = (raw & 0x2000) ? ea_abase_offset : ea_offset;
        return;
    }
    in->index = &s->r[raw & 0x1f];
    in->scale = (raw >> 7) & 7;
    switch ((raw >> 10) & 0xf) {
    case 0x4: in->ea = ea_abase; break;
    case 0x5:
        /* disp + address of the displacement word + 8 (QUIRKS.md) */
        in->mem_disp = rd32(s, addr + 4) + (addr + 4) + 8;
        in->ea = ea_ip_disp;
        break;
    case 0x7: in->ea = ea_abase_index; break;
    case 0xc: in->mem_disp = rd32(s, addr + 4); in->ea = ea_disp; break;
    case 0xd: in->mem_disp = rd32(s, addr + 4); in->ea = ea_abase_disp; break;
    case 0xe: in->mem_disp = rd32(s, addr + 4); in->ea = ea_index_disp; break;
    case 0xf: in->mem_disp = rd32(s, addr + 4); in->ea = ea_abase_index_disp; break;
    default:  in->ea = ea_reserved; break;
    }
}

/* ------------------------------------------------------------- decoding */

static const i960_opdef *optable[0x1000];
static const i960_opdef invalid_opdef;  /* defined after the handlers */

static inline uint32_t opcode_key(uint32_t raw)   /* orig 0x4c2790 */
{
    uint32_t key = raw >> 24;
    if (key - 0x58 < 0x22)
        key = (key << 4) | ((raw >> 7) & 0xf);
    return key & 0xfff;
}

/* orig 0x4c4750 (single entry) and the loop body of 0x4c48f0. */
static void decode(i960_state *s, i960_insn *in, uint32_t addr)
{
    uint32_t raw = rd32(s, addr);
    const i960_opdef *op = optable[opcode_key(raw)];
    uint32_t m = op->operands;

    in->raw = raw;
    in->op = op;
    switch (op->format) {
    case 0: /* REG */
        if (m & 1) in->src1 = reg_src(s, op, raw & 0x1f, raw & 0x800);
        if (m & 2) in->src2 = reg_src(s, op, (raw >> 14) & 0x1f, raw & 0x1000);
        if (m & 4) in->dst = reg_dst(s, op, (raw >> 19) & 0x1f, raw & 0x2000);
        break;
    case 1: /* MEM */
        if (m & 4) in->dst = &s->r[(raw >> 19) & 0x1f];
        if (m & 8) decode_mem(s, in, addr);
        break;
    case 2: /* COBR */
        if (m & 1) {
            uint32_t reg = (raw >> 19) & 0x1f;
            in->src1 = (raw & 0x2000) ? (void *)s->lit[reg] : (void *)&s->r[reg];
        }
        if (m & 2) in->src2 = &s->r[(raw >> 14) & 0x1f];
        if (m & 8) {
            uint32_t d = raw & 0x1fff;
            if (raw & 0x1000) d |= 0xfffff000;
            in->disp = (int32_t)(d - 4);
        }
        break;
    case 3: /* CTRL */
        if (m & 8) {
            uint32_t d = raw & 0xffffff;
            if (raw & 0x800000) d |= 0xff000000;
            in->disp = (int32_t)(d - 4);
        }
        break;
    }
}

/* orig 0x4c48f0 */
static void predecode_region(i960_state *s, i960_code_region *r)
{
    uint32_t n = r->size >> 2;
    for (uint32_t i = 0; i < n; i++) {
        s->cur = &r->cache[i];
        decode(s, s->cur, r->base + i * 4);
    }
    r->valid = 1;
}

/* orig 0x4c4aa0 and the region switch in 0x4c4b00. */
static void select_region(i960_state *s)
{
    uint32_t hi = s->ip >> 20;
    if (hi < s->region[1].base >> 20)
        s->cur_region = &s->region[0];
    else if (hi < s->region[2].base >> 20)
        s->cur_region = &s->region[1];
    else
        s->cur_region = &s->region[2];
    if (!s->cur_region->valid)
        predecode_region(s, s->cur_region);
}

/* ------------------------------------------------ frame cache, call/ret */

static void frame_spill(i960_state *s, int slot)
{
    uint32_t a = s->fc_addr[slot];
    for (int k = 0; k < 16; k++, a += 4)
        wr32(s, a, s->fc_regs[slot][k]);
}

/* Saves r0-r15 in the on-chip frame cache, spilling the oldest frame to
   memory when all 128 slots are in use. */
static void frame_push(i960_state *s)
{
    if (s->fc_count == I960_FRAME_CACHE) {
        frame_spill(s, s->fc_index);
        s->fc_count--;
    }
    memcpy(s->fc_regs[s->fc_index], s->r, 16 * sizeof(uint32_t));
    s->fc_addr[s->fc_index] = s->r[31] & ~0x3fu;
    s->fc_index = (s->fc_index + 1) & (I960_FRAME_CACHE - 1);
    s->fc_count++;
}

/* Restores the frame at `target`, from the cache if it is there (spilling
   any newer cached frames on the way) or else from memory. */
static void frame_pop(i960_state *s, uint32_t target)
{
    while (s->fc_count > 0) {
        if (--s->fc_index < 0)
            s->fc_index = I960_FRAME_CACHE - 1;
        s->fc_count--;
        if (s->fc_addr[s->fc_index] == target) {
            memcpy(s->r, s->fc_regs[s->fc_index], 16 * sizeof(uint32_t));
            return;
        }
        frame_spill(s, s->fc_index);
    }
    for (int k = 0; k < 16; k++)
        s->r[k] = rd32(s, target + k * 4);
}

static void do_call(i960_state *s, uint32_t target)
{
    uint32_t sp = s->r[1];
    uint32_t fp = s->r[31];

    s->r[2] = s->ip;                /* rip */
    frame_push(s);
    s->ip = target;
    uint32_t nfp = (sp + 0x3f) & ~0x3fu;
    s->r[0] = fp & ~0x3fu;          /* pfp, return type 0 */
    s->r[31] = nfp;
    s->r[1] = nfp + 0x40;
}

static uint32_t ac_merged(const i960_state *s)
{
    return ((s->ac ^ s->cc) & 7) ^ s->ac;
}

/* ------------------------------------------------------------- handlers */

#if defined(__GNUC__)
#define MAYBE_UNUSED __attribute__((unused))
#else
#define MAYBE_UNUSED
#endif
#define OPFN(name) static void op_##name(i960_state *s MAYBE_UNUSED, i960_insn *in MAYBE_UNUSED)

static void set_cc_cmp_u(i960_state *s, uint32_t a, uint32_t b)
{
    s->cc = a < b ? 4 : a == b ? 2 : 1;
}

static void set_cc_cmp_i(i960_state *s, int32_t a, int32_t b)
{
    s->cc = a < b ? 4 : a == b ? 2 : 1;
}

/* CTRL branches */
OPFN(b)   { s->ip += in->disp; }
OPFN(bal) { s->r[30] = s->ip; s->ip += in->disp; }   /* g14 = return address */
OPFN(bno) { if ((s->cc & 7) == 0) s->ip += in->disp; }
OPFN(bg)  { if (s->cc & 1) s->ip += in->disp; }
OPFN(be)  { if (s->cc & 2) s->ip += in->disp; }
OPFN(bge) { if (s->cc & 3) s->ip += in->disp; }
OPFN(bl)  { if (s->cc & 4) s->ip += in->disp; }
OPFN(bne) { if (s->cc & 5) s->ip += in->disp; }
OPFN(ble) { if (s->cc & 6) s->ip += in->disp; }
OPFN(bo)  { if (s->cc & 7) s->ip += in->disp; }

OPFN(call) { do_call(s, s->ip + in->disp); }

/* orig 0x4c3080 */
OPFN(ret)
{
    switch (s->r[0] & 7) {
    case 0: {
        uint32_t target = s->r[0] & ~0x3fu;
        frame_pop(s, target);
        s->r[31] = target;
        s->ip = s->r[2];
        break;
    }
    case 7: {   /* return from interrupt */
        uint32_t saved_pc = rd32(s, s->r[31] - 0x10);
        uint32_t saved_ac = rd32(s, s->r[31] - 0x0c);
        uint32_t target = s->r[0] & ~7u;
        frame_pop(s, target);
        s->ip = s->r[2];
        s->r[31] = target;
        s->ac = saved_ac;
        s->cc = saved_ac & 7;
        if (s->pc & 2)
            s->pc = saved_pc;
        s->in_interrupt = 0;
        break;
    }
    default:
        /* Fault and supervisor returns are not implemented by the original;
           ret falls through to the next instruction. */
        break;
    }
}

/* COBR */
static void cobr_branch(i960_state *s, i960_insn *in, uint32_t mask)
{
    if (s->cc & mask)
        s->ip += in->disp;
}

OPFN(bbc)
{
    if (!(SRC2(in) & (1u << (SRC1(in) & 0x1f)))) {
#if I960_ACCURATE
        s->cc = 0;
#else
        s->cc = 2;
#endif
        s->ip += in->disp;
    } else {
#if I960_ACCURATE
        s->cc = 2;
#else
        s->cc = 0;
#endif
    }
}

OPFN(bbs)
{
    if (SRC2(in) & (1u << (SRC1(in) & 0x1f))) {
        s->cc = 2;
        s->ip += in->disp;
    } else {
        s->cc = 0;
    }
}

#define CMPOB(name, mask) OPFN(name) { set_cc_cmp_u(s, SRC1(in), SRC2(in)); cobr_branch(s, in, mask); }
#define CMPIB(name, mask) OPFN(name) { set_cc_cmp_i(s, I(in->src1), I(in->src2)); cobr_branch(s, in, mask); }
CMPOB(cmpobg, 1) CMPOB(cmpobe, 2) CMPOB(cmpobge, 3) CMPOB(cmpobl, 4)
CMPOB(cmpobne, 5) CMPOB(cmpoble, 6)
CMPIB(cmpibg, 1) CMPIB(cmpibe, 2) CMPIB(cmpibge, 3) CMPIB(cmpibl, 4)
CMPIB(cmpibne, 5) CMPIB(cmpible, 6)

/* test* write the result through the src1 field */
OPFN(testno) { SRC1(in) = s->cc == 0; }
OPFN(testg)  { SRC1(in) = s->cc & 1; }
OPFN(teste)  { SRC1(in) = (s->cc >> 1) & 1; }
OPFN(testge) { SRC1(in) = (s->cc & 3) != 0; }
OPFN(testl)  { SRC1(in) = (s->cc >> 2) & 1; }
OPFN(testne) { SRC1(in) = (s->cc & 5) != 0; }
OPFN(testle) { SRC1(in) = (s->cc & 6) != 0; }

/* MEM: loads and stores */
OPFN(ldob) { uint32_t a = in->ea(s, in); DST(in) = rd8(s, a); }
OPFN(ldib) { uint32_t a = in->ea(s, in); DST(in) = (uint32_t)(int32_t)(int8_t)rd8(s, a); }
OPFN(ldos) { uint32_t a = in->ea(s, in); DST(in) = rd16(s, a); }
OPFN(ldis) { uint32_t a = in->ea(s, in); DST(in) = (uint32_t)(int32_t)(int16_t)rd16(s, a); }
OPFN(ld)   { uint32_t a = in->ea(s, in); DST(in) = rd32(s, a); }
OPFN(stob) { uint32_t a = in->ea(s, in); wr8(s, a, (uint8_t)DST(in)); }
OPFN(stib) { uint32_t a = in->ea(s, in); wr8(s, a, (uint8_t)DST(in)); }
OPFN(stos) { uint32_t a = in->ea(s, in); wr16(s, a, (uint16_t)DST(in)); }
OPFN(stis) { uint32_t a = in->ea(s, in); wr16(s, a, (uint16_t)DST(in)); }
OPFN(st)   { uint32_t a = in->ea(s, in); wr32(s, a, DST(in)); }

static void load_multi(i960_state *s, i960_insn *in, int n)
{
    uint32_t a = in->ea(s, in);
    uint32_t *d = in->dst;
    s->multi_addr = a;
    s->multi_count = (uint8_t)n;
    for (int k = 0; k < n; k++)
        d[k] = rd32(s, a + k * 4);
}

static void store_multi(i960_state *s, i960_insn *in, int n)
{
    uint32_t a = in->ea(s, in);
    const uint32_t *d = in->dst;
    s->multi_addr = a;
    s->multi_count = (uint8_t)n;
    for (int k = 0; k < n; k++)
        wr32(s, a + k * 4, d[k]);
}

OPFN(ldl) { load_multi(s, in, 2); }
OPFN(ldt) { load_multi(s, in, 3); }
OPFN(ldq) { load_multi(s, in, 4); }
OPFN(stl) { store_multi(s, in, 2); }
OPFN(stt) { store_multi(s, in, 3); }
OPFN(stq) { store_multi(s, in, 4); }

OPFN(lda)   { DST(in) = in->ea(s, in); }
OPFN(bx)    { s->ip = in->ea(s, in); }
OPFN(balx)  { uint32_t t = in->ea(s, in); DST(in) = s->ip; s->ip = t; }
OPFN(callx) { uint32_t t = in->ea(s, in); do_call(s, t); }

/* REG: logic */
OPFN(and)    { DST(in) = SRC2(in) & SRC1(in); }
OPFN(andnot) { DST(in) = SRC2(in) & ~SRC1(in); }
OPFN(notand) { DST(in) = ~SRC2(in) & SRC1(in); }
OPFN(xor)    { DST(in) = SRC2(in) ^ SRC1(in); }
OPFN(or)     { DST(in) = SRC2(in) | SRC1(in); }
OPFN(nor)    { DST(in) = ~SRC2(in) & ~SRC1(in); }
OPFN(xnor)   { DST(in) = ~(SRC2(in) ^ SRC1(in)); }
OPFN(not)    { DST(in) = ~SRC1(in); }
OPFN(ornot)  { DST(in) = SRC2(in) | ~SRC1(in); }
OPFN(notor)  { DST(in) = ~SRC2(in) | SRC1(in); }
OPFN(nand)   { DST(in) = ~SRC2(in) | ~SRC1(in); }

OPFN(notbit)   { DST(in) = SRC2(in) ^ (1u << (SRC1(in) & 0x1f)); }
OPFN(setbit)   { DST(in) = SRC2(in) | (1u << (SRC1(in) & 0x1f)); }
OPFN(clrbit)   { DST(in) = SRC2(in) & ~(1u << (SRC1(in) & 0x1f)); }
OPFN(alterbit)
{
    uint32_t bit = 1u << (SRC1(in) & 0x1f);
    DST(in) = (s->cc & 2) ? SRC2(in) | bit : SRC2(in) & ~bit;
}
OPFN(chkbit) { s->cc = (SRC2(in) & (1u << (SRC1(in) & 0x1f))) ? 2 : 0; }

/* orig 0x4c0910 / 0x4c08b0: index of the most significant set / clear bit */
static int msb(uint32_t v)
{
    int n = 31;
    while (!(v >> n))
        n--;
    return n;
}

OPFN(scanbit)
{
    uint32_t v = SRC1(in);
    if (!v) { s->cc = 0; DST(in) = 0xffffffff; return; }
    s->cc = 2;
    DST(in) = (uint32_t)msb(v);
}

OPFN(spanbit)
{
    uint32_t v = ~SRC1(in);
    if (!v) { s->cc = 0; DST(in) = 0xffffffff; return; }
    s->cc = 2;
    DST(in) = (uint32_t)msb(v);
}

/* REG: shifts. The original masks the count to 5 bits like x86 does. */
#if I960_ACCURATE
OPFN(shlo) { uint32_t n = SRC1(in); DST(in) = n > 31 ? 0 : SRC2(in) << n; }
OPFN(shli) { uint32_t n = SRC1(in); DST(in) = n > 31 ? 0 : SRC2(in) << n; }
OPFN(shro) { uint32_t n = SRC1(in); DST(in) = n > 31 ? 0 : SRC2(in) >> n; }
OPFN(shri) { uint32_t n = SRC1(in); DST(in) = (uint32_t)(I(in->src2) >> (n > 31 ? 31 : n)); }
#else
OPFN(shlo) { DST(in) = SRC2(in) << (SRC1(in) & 0x1f); }
OPFN(shli) { DST(in) = SRC2(in) << (SRC1(in) & 0x1f); }
OPFN(shro) { DST(in) = SRC2(in) >> (SRC1(in) & 0x1f); }
OPFN(shri) { DST(in) = (uint32_t)(I(in->src2) >> (SRC1(in) & 0x1f)); }
#endif

OPFN(shrdi)   /* shift right, rounding toward zero */
{
    uint32_t v = SRC2(in);
    uint32_t n = SRC1(in) & 0x1f;
    int32_t adj = ((int32_t)v < 0 && (v & ((1u << n) - 1))) ? 1 : 0;
    DST(in) = (uint32_t)(((int32_t)v >> n) + adj);
}

OPFN(rotate)
{
    uint32_t n = SRC1(in) & 0x1f;
    uint32_t v = SRC2(in);
    DST(in) = n ? (v << n) | (v >> (32 - n)) : v;
}

/* REG: arithmetic */
OPFN(addo) { DST(in) = SRC2(in) + SRC1(in); }
OPFN(addi) { DST(in) = SRC2(in) + SRC1(in); }
OPFN(subo) { DST(in) = SRC2(in) - SRC1(in); }
OPFN(subi) { DST(in) = SRC2(in) - SRC1(in); }
OPFN(mulo) { DST(in) = SRC2(in) * SRC1(in); }
OPFN(muli) { DST(in) = SRC2(in) * SRC1(in); }

OPFN(addc)   /* orig 0x4c1b50 */
{
    uint32_t a = SRC1(in), b = SRC2(in);
#if I960_ACCURATE
    uint64_t r = (uint64_t)a + b + ((s->cc >> 1) & 1);
    uint32_t v = (~(a ^ b) & (a ^ (uint32_t)r)) >> 31;
    s->cc = (uint32_t)((r >> 31) & 2) | v;
    DST(in) = (uint32_t)r;
#else
    /* The original adds 1 whenever src2 + (cc & 2) is non-zero, instead of
       adding src2 and the carry (QUIRKS.md). */
    uint32_t c1 = ((uint64_t)b + (s->cc & 2)) != 0;
    uint64_t r = (uint64_t)a + c1;
    s->cc &= ~7u;
    if ((r >> 32) & 1)
        s->cc |= 2;
    DST(in) = (uint32_t)r;
#endif
}

OPFN(subc)   /* orig 0x4c1ae0, inline x86 assembly in the original */
{
    uint32_t a = SRC1(in), b = SRC2(in);
#if I960_ACCURATE
    /* i960: src2 - src1 - 1 + carry, carry = no borrow */
    uint64_t r = (uint64_t)b + (uint32_t)~a + ((s->cc >> 1) & 1);
    uint32_t v = ((b ^ a) & (b ^ (uint32_t)r)) >> 31;
    s->cc = (uint32_t)((r >> 31) & 2) | v;
    DST(in) = (uint32_t)r;
#else
    /* x86 sbb: carry = borrow, the opposite of the i960 convention. */
    uint32_t c = (s->cc >> 1) & 1;
    uint32_t r = b - a - c;
    uint32_t borrow = (uint64_t)b < (uint64_t)a + c;
    int64_t sr = (int64_t)(int32_t)b - (int32_t)a - c;
    uint32_t ov = sr < INT32_MIN || sr > INT32_MAX;
    s->cc = (borrow << 1) | ov;
    DST(in) = r;
#endif
}

/* Division. A zero divisor, or INT_MIN / -1, raises an x86 divide fault in
   the original and kills the emulator; here the result is 0 (QUIRKS.md). */
OPFN(divo) { uint32_t d = SRC1(in); DST(in) = d ? SRC2(in) / d : 0; }
OPFN(remo) { uint32_t d = SRC1(in); DST(in) = d ? SRC2(in) % d : 0; }

static int bad_idiv(int32_t n, int32_t d)
{
    return d == 0 || (n == INT32_MIN && d == -1);
}

OPFN(divi)
{
    int32_t n = I(in->src2), d = I(in->src1);
    DST(in) = bad_idiv(n, d) ? (d ? (uint32_t)n : 0) : (uint32_t)(n / d);
}

OPFN(remi)
{
    int32_t n = I(in->src2), d = I(in->src1);
    DST(in) = bad_idiv(n, d) ? 0 : (uint32_t)(n % d);
}

OPFN(modi)
{
    int32_t n = I(in->src2), d = I(in->src1);
    if (bad_idiv(n, d)) { DST(in) = 0; return; }
    int32_t r = n % d;
    if (((n ^ d) < 0) && r)
        r += d;
    DST(in) = (uint32_t)r;
}

OPFN(emul)
{
    uint64_t r = (uint64_t)SRC2(in) * SRC1(in);
    uint32_t *d = in->dst;
    d[0] = (uint32_t)r;
    d[1] = (uint32_t)(r >> 32);
}

OPFN(ediv)
{
    const uint32_t *n = in->src2;
    uint32_t *d = in->dst;
    uint64_t num = (uint64_t)n[1] << 32 | n[0];
    uint32_t div = SRC1(in);
    if (!div) { d[0] = 0; d[1] = 0; return; }
    d[0] = (uint32_t)(num % div);
    d[1] = (uint32_t)(num / div);
}

/* REG: compares */
OPFN(cmpo) { set_cc_cmp_u(s, SRC1(in), SRC2(in)); }
OPFN(cmpi) { set_cc_cmp_i(s, I(in->src1), I(in->src2)); }

OPFN(concmpo)
{
    if (!(s->cc & 4))
        s->cc = SRC1(in) <= SRC2(in) ? 2 : 1;
}

OPFN(concmpi)
{
    if (!(s->cc & 4))
        s->cc = I(in->src1) <= I(in->src2) ? 2 : 1;
}

OPFN(cmpinco) { uint32_t b = SRC2(in); set_cc_cmp_u(s, SRC1(in), b); DST(in) = b + 1; }
OPFN(cmpdeco) { uint32_t b = SRC2(in); set_cc_cmp_u(s, SRC1(in), b); DST(in) = b - 1; }
OPFN(cmpinci) { int32_t b = I(in->src2); set_cc_cmp_i(s, I(in->src1), b); DST(in) = (uint32_t)b + 1; }
OPFN(cmpdeci) { int32_t b = I(in->src2); set_cc_cmp_i(s, I(in->src1), b); DST(in) = (uint32_t)b - 1; }

/* REG: moves */
OPFN(mov) { DST(in) = SRC1(in); }

static void move_multi(i960_insn *in, int n)
{
    const uint32_t *a = in->src1;
    uint32_t *d = in->dst;
    uint32_t w0 = a[0];
    d[0] = w0;
    for (int k = 1; k < n; k++)
        d[k] = (in->raw & 0x800) ? w0 : a[k];   /* literal: same value */
}

OPFN(movl) { move_multi(in, 2); }
OPFN(movt) { move_multi(in, 3); }
OPFN(movq) { move_multi(in, 4); }

OPFN(dmovt)
{
    uint32_t v = SRC1(in);
    /* The original tests the whole word and sets cc=2 for a non-digit. */
    s->cc = (v - 0x30u > 9) ? 2 : 0;
    DST(in) = v;
}

/* REG: control registers */
OPFN(modac)  /* orig 0x4c3520 */
{
    uint32_t old = ac_merged(s);
    uint32_t mask = SRC1(in);
    uint32_t nac = (SRC2(in) & mask) | (old & ~mask);
    s->ac = nac;
    s->cc = nac & 7;
    DST(in) = old;
}

OPFN(modpc)
{
    uint32_t mask = SRC2(in);
#if I960_ACCURATE
    uint32_t old = s->pc;
    s->pc = (DST(in) & mask) | (old & ~mask);
    DST(in) = old;
#else
    s->pc = (DST(in) & mask) | (s->pc & ~mask);
#endif
}

OPFN(flushreg)
{
    while (s->fc_count > 0) {
        if (--s->fc_index < 0)
            s->fc_index = I960_FRAME_CACHE - 1;
        s->fc_count--;
        frame_spill(s, s->fc_index);
    }
}

static void iac_check(i960_state *s)   /* orig 0x4c2b40 */
{
    if ((s->iac[0] >> 24) == 0x93) {   /* reinitialize processor */
        s->sat = s->iac[1];
        s->prcb = s->iac[2];
        s->ip = s->iac[3];
    }
}

OPFN(synmov)   /* orig 0x4c16f0 */
{
    uint32_t dst = SRC1(in);
    uint32_t v = rd32(s, SRC2(in));
    if (dst == 0xff000004)
        s->icon = v;
    else
        wr32(s, dst, v);
    s->cc = 2;
}

OPFN(synmovq)  /* orig 0x4c3350 */
{
    uint32_t src = SRC2(in), dst = SRC1(in);
    for (int k = 0; k < 16; k += 4)
        wr32(s, dst + k, rd32(s, src + k));
    if (dst == 0xff000010)
        iac_check(s);
    s->cc = 2;
}

/* ------------------------------------------------------ floating point */

#if I960_FP_SINGLE
#define FPR(x) ((double)(float)(x))
#else
#define FPR(x) (x)
#endif

static inline double fsrc1(const i960_insn *in) { return (in->raw & 0x800) ? D(in->src1) : F(in->src1); }
static inline double fsrc2(const i960_insn *in) { return (in->raw & 0x1000) ? D(in->src2) : F(in->src2); }

static inline void fdst(const i960_insn *in, double v)
{
    if (in->raw & 0x2000)
        D(in->dst) = v;
    else
        F(in->dst) = (float)v;
}

/* x87 fistp with round-to-nearest: out-of-range and NaN give 0x80000000. */
static int32_t x87_round_i32(double v)
{
    if (!(v >= -2147483648.5 && v < 2147483647.5))
        return INT32_MIN;
    return (int32_t)nearbyint(v);
}

/* MSVC _ftol2: truncation, 0x80000000 when out of range. */
static int32_t ftol(double v)
{
    if (!(v > -2147483649.0 && v < 2147483648.0))
        return INT32_MIN;
    return (int32_t)v;
}

static int64_t ftol64(double v)
{
    if (!(v > -9223372036854775808.0 && v < 9223372036854775808.0))
        return INT64_MIN;
    return (int64_t)v;
}

/* Rounding by AC bits 30-31 as done by cvtri/roundr (orig 0x4c3f60).
   Mode 0 is floor(x + 0.5), not round-half-to-even. */
static int32_t round_ac(const i960_state *s, double v)
{
    switch (ac_merged(s) >> 30) {
    case 0: return ftol(floor(v + 0.5));
    case 1: return ftol(floor(v));
    case 2: return ftol(ceil(v));
    default: return ftol(v);
    }
}

OPFN(movr)  { fdst(in, fsrc1(in)); }
OPFN(movrl) { memcpy(in->dst, in->src1, 8); }
OPFN(movre)
{
    const uint32_t *a = in->src1;
    uint32_t *d = in->dst;
    d[0] = a[0];
    d[1] = a[1];
    d[2] = a[2] & 0xffff;
}

OPFN(cvtir)  { fdst(in, (double)I(in->src1)); }
OPFN(cvtilr) { int64_t v; memcpy(&v, in->src1, 8); D(in->dst) = (double)v; }
OPFN(cvtri)  { I(in->dst) = round_ac(s, fsrc1(in)); }
OPFN(cvtzri)
{
    /* orig: fistp(|2x| - 0.5) >> 1, a truncation trick */
    (void)s;
    double v = fsrc1(in);
    int32_t r = x87_round_i32(fabs(v + v) - 0.5) >> 1;
    I(in->dst) = v < 0.0 ? -r : r;
}
OPFN(cvtzril)
{
    int64_t r = ftol64(D(in->src1));
    memcpy(in->dst, &r, 8);
}

OPFN(roundr)  { fdst(in, (double)round_ac(s, fsrc1(in))); }
OPFN(roundrl) { D(in->dst) = (double)round_ac(s, D(in->src1)); }

OPFN(addr)  { fdst(in, FPR(fsrc2(in) + fsrc1(in))); }
OPFN(subr)  { fdst(in, FPR(fsrc2(in) - fsrc1(in))); }
OPFN(mulr)  { fdst(in, FPR(fsrc2(in) * fsrc1(in))); }
OPFN(divr)  { fdst(in, FPR(fsrc2(in) / fsrc1(in))); }
OPFN(addrl) { D(in->dst) = FPR(D(in->src2) + D(in->src1)); }
OPFN(subrl) { D(in->dst) = FPR(D(in->src2) - D(in->src1)); }
OPFN(mulrl) { D(in->dst) = FPR(D(in->src2) * D(in->src1)); }
OPFN(divrl) { D(in->dst) = FPR(D(in->src2) / D(in->src1)); }

OPFN(sqrtr)  { fdst(in, FPR(sqrt(fsrc1(in)))); }
OPFN(sqrtrl) { D(in->dst) = FPR(sqrt(D(in->src1))); }

static void fp_compare(i960_state *s, double a, double b)
{
    if (a < b) s->cc = 4;
    else if (a == b) s->cc = 2;
    else if (a > b) s->cc = 1;
    else s->cc = 0;   /* unordered */
}

OPFN(cmpr)  { fp_compare(s, fsrc1(in), fsrc2(in)); }
OPFN(cmprl) { fp_compare(s, D(in->src1), D(in->src2)); }

OPFN(atanr)  { fdst(in, atan2(fsrc2(in), fsrc1(in))); }
OPFN(atanrl) { D(in->dst) = atan2(D(in->src2), D(in->src1)); }
/* x87 fsin/fcos/fptan leave operands with |x| >= 2^63 unchanged and turn
   infinities into NaN. For fptan the original then stores whatever was
   below on the x87 stack, which cannot be reproduced; x is used instead. */
typedef double (*trig_fn)(double);

static double x87_trig(trig_fn f, double x)
{
    if (isinf(x))
        return -NAN;
    if (!(fabs(x) < 9223372036854775808.0))
        return x;
    return f(x);
}

OPFN(sinr)   { fdst(in, x87_trig(sin, fsrc1(in))); }
OPFN(sinrl)  { D(in->dst) = x87_trig(sin, D(in->src1)); }
OPFN(cosr)   { fdst(in, x87_trig(cos, fsrc1(in))); }
OPFN(cosrl)  { D(in->dst) = x87_trig(cos, D(in->src1)); }
OPFN(tanr)   { fdst(in, x87_trig(tan, fsrc1(in))); }
OPFN(tanrl)  { D(in->dst) = x87_trig(tan, D(in->src1)); }

/* logr: src2 * log2(src1); logepr: src2 * log2(src1 + 1);
   logbnr: log2(src1), not the unbiased exponent (QUIRKS.md). */
OPFN(logr)    { fdst(in, fsrc2(in) * log2(fsrc1(in))); }
OPFN(logrl)   { D(in->dst) = D(in->src2) * log2(D(in->src1)); }
OPFN(logepr)  { fdst(in, fsrc2(in) * log2(fsrc1(in) + 1.0)); }
OPFN(logeprl) { D(in->dst) = D(in->src2) * log2(D(in->src1) + 1.0); }
OPFN(logbnr)  { fdst(in, log2(fsrc1(in))); }

/* expr: 2^src1 - 1; scaler: src2 * 2^(int)src1 */
OPFN(expr)    { fdst(in, pow(2.0, fsrc1(in)) - 1.0); }
OPFN(exprl)   { D(in->dst) = pow(2.0, D(in->src1)) - 1.0; }
OPFN(scaler)  { fdst(in, pow(2.0, (double)I(in->src1)) * fsrc2(in)); }
OPFN(scalerl) { D(in->dst) = pow(2.0, (double)I(in->src1)) * D(in->src2); }

/* remr: fmod, not the IEEE remainder (QUIRKS.md) */
OPFN(remr)  { fdst(in, fmod(fsrc2(in), fsrc1(in))); }
OPFN(remrl) { D(in->dst) = fmod(D(in->src2), D(in->src1)); }

/* Copy sign; the comparisons reproduce the original's handling of zero
   and NaN. */
OPFN(cpysre)
{
    double m = fabs(D(in->src1)), g = D(in->src2);
    D(in->dst) = (isnan(g) || (g > 0.0) == (g == 0.0)) ? -m : m;
}

OPFN(cpyrsre)
{
    double m = fabs(D(in->src1)), g = D(in->src2);
    D(in->dst) = (!isnan(g) && (g > 0.0) != (g == 0.0)) ? -m : m;
}

/* --------------------------------------------------- special handlers */

static void op_invalid(i960_state *s, i960_insn *in)   /* orig 0x4c2530 */
{
    if (s->invalid_opcode)
        s->invalid_opcode(s->user, s->ip - 4, in->raw);
}

static void op_nop(i960_state *s, i960_insn *in) { (void)s; (void)in; }   /* orig 0x4c24d0 */

static void op_hook(i960_state *s, i960_insn *in)       /* orig 0x4c24e0 */
{
    in->hooked_op->exec(s, in);
    if (s->hook)
        s->hook(s->user);
}

static void op_stop_after(i960_state *s, i960_insn *in) /* orig 0x4c2510 */
{
    in->hooked_op->exec(s, in);
    s->stop = 1;
}

/* Special descriptors cost no cycles in the original. */
static const i960_opdef invalid_opdef = { op_invalid, 0, 0, 0, 0, 0, 0, 0, "invalid" };
static const i960_opdef nop_opdef = { op_nop, 0, 0, 0, 0, 0, 0, 0, "(nop)" };
static const i960_opdef hook_opdef = { op_hook, 0, 0, 0, 0, 0, 0, 0, "(hook)" };
static const i960_opdef stop_opdef = { op_stop_after, 0, 0, 0, 0, 0, 0, 0, "(stop)" };

const i960_opdef i960_opdefs[] = {
#define OP(fn, fmt, key, cyc, fp, f18, f1c, ops) { fn, fmt, key, cyc, fp, f18, f1c, ops, &#fn[3] },
#include "i960_optable.inc"
#undef OP
};
const int i960_opdef_count = (int)(sizeof i960_opdefs / sizeof i960_opdefs[0]);

static void build_optable(void)   /* orig 0x4c2740 */
{
    /* built once: other CPUs may be running (several boards, one process) */
    static int built;
    if (built)
        return;
    built = 1;
    for (int k = 0; k < 0x1000; k++)
        optable[k] = &invalid_opdef;
    for (int i = 0; i < i960_opdef_count; i++)
        optable[i960_opdefs[i].key] = &i960_opdefs[i];
}

/* --------------------------------------------------------------- setup */

void i960_init(i960_state *s, void *user)
{
    memset(s, 0, sizeof *s);
    build_optable();
    s->user = user;
    for (uint32_t n = 0; n < 32; n++)
        for (int k = 0; k < 4; k++)
            s->lit[n][k] = n;
    i960_map_read(s, 0, 0xffffffff, NULL, unmapped_r8, unmapped_r16, unmapped_r32);
    i960_map_write(s, 0, 0xffffffff, NULL, unmapped_w8, unmapped_w16, unmapped_w32);
    s->cur_region = &s->region[0];
    s->cur = &s->scratch;
}

void i960_free(i960_state *s)
{
    for (int n = 0; n < 3; n++) {
        free(s->region[n].cache);
        s->region[n].cache = NULL;
    }
}

/* orig 0x4c4870. The code must be in memory mapped with a host pointer. */
int i960_set_code_region(i960_state *s, int n, uint32_t base, uint32_t size)
{
    i960_code_region *r = &s->region[n];
    const i960_read_page *p = &s->rpage[base >> 16];

    if (n < 0 || n > 2)
        return -1;
    if (size == 0) {   /* boundary only: nothing executes from this region */
        free(r->cache);
        r->cache = NULL;
        r->words = NULL;
        r->size = 0;
        r->base = base;
        r->valid = 1;
        return 0;
    }
    if (!p->host)
        return -1;
    free(r->cache);
    r->cache = calloc(size / 4 ? size / 4 : 1, sizeof *r->cache);
    if (!r->cache)
        return -1;
    r->words = p->host + (base & 0xffff);
    r->size = size;
    r->base = base;
    r->valid = 0;
    return 0;
}

void i960_invalidate_code(i960_state *s, int n)
{
    s->region[n].valid = 0;
}

/* orig 0x4c3570 */
void i960_reset(i960_state *s)
{
    s->sat = rd32(s, 0);
    s->prcb = rd32(s, 4);
    s->ip = rd32(s, 0xc);
    s->ac = 0;
    s->cc = 0;
    s->pc = 0x1f2002;
    uint32_t isp = rd32(s, s->prcb + 0x18);   /* interrupt stack */
    s->r[31] = isp;                           /* fp */
    s->r[1] = isp + 0x40;                     /* sp */
    s->in_interrupt = 0;
    s->fc_count = 0;
    s->cur_region = &s->region[0];
    s->prev_ip = s->ip;
    select_region(s);
}

/* ----------------------------------------------------------- execution */

/* orig 0x4c4b00 */
int i960_execute(i960_state *s, int cycles)
{
    s->cycles_left = cycles;
    if (cycles <= 0)
        return cycles - s->cycles_left;

    while (!s->stop) {
        i960_code_region *r = s->cur_region;
        uint32_t idx = (s->ip - r->base) >> 2;
        i960_insn *in;

        if (idx < r->size >> 2) {
            in = &r->cache[idx];
            if (in->raw != ld32(r->words + idx * 4))
                decode(s, in, s->ip);
        } else {
            /* Outside every code region (the original reads out of bounds). */
            in = &s->scratch;
            decode(s, in, s->ip);
        }
        s->cur = in;
#ifdef I960_TRACE
        uint32_t at = s->ip;
#endif
        s->ip += 4;
        s->multi_count = 0;
        in->op->exec(s, in);

        if ((s->ip ^ s->prev_ip) & 0xffe00000)
            select_region(s);
        s->prev_ip = s->ip;
#ifdef I960_TRACE
        /* after the region switch: s->cur is what the cycles come from */
        if (s->trace)
            s->trace(s->user, at, in);
#endif

        /* Charged from s->cur, which a region predecode may have moved to
           another entry, exactly like the original's global pointer. */
        s->cycles_left -= s->cur->op->cycles;
        if (s->cycles_left < 1)
            return cycles - s->cycles_left;
    }
    s->cycles_left = 0;
    return cycles;
}

int i960_step(i960_state *s)
{
    i960_code_region *r = s->cur_region;
    uint32_t idx = (s->ip - r->base) >> 2;
    i960_insn *in;

    if (idx < r->size >> 2) {
        in = &r->cache[idx];
        if (in->raw != ld32(r->words + idx * 4))
            decode(s, in, s->ip);
    } else {
        in = &s->scratch;
        decode(s, in, s->ip);
    }
    s->cur = in;
    s->ip += 4;
    s->multi_count = 0;
    in->op->exec(s, in);
    if ((s->ip ^ s->prev_ip) & 0xffe00000)
        select_region(s);
    s->prev_ip = s->ip;
    s->cycles_left -= s->cur->op->cycles;
    return s->cur->op->cycles;
}

void i960_call(i960_state *s, uint32_t target) { do_call(s, target); }

int i960_ret(i960_state *s)
{
    uint32_t type = s->r[0] & 7;
    op_ret(s, NULL);
    return type == 0 || type == 7;
}

void i960_flushreg(i960_state *s) { op_flushreg(s, NULL); }

int i960_region_switch(i960_state *s, int cycles)
{
    int was_valid;
    uint32_t hi = s->ip >> 20;
    i960_code_region *r = hi < s->region[1].base >> 20 ? &s->region[0]
                        : hi < s->region[2].base >> 20 ? &s->region[1]
                        : &s->region[2];
    was_valid = r->valid;
    select_region(s);
    s->prev_ip = s->ip;
    return was_valid ? cycles : s->cur->op->cycles;
}

/* orig 0x4c4c60 */
void i960_interrupt(i960_state *s, int line)
{
    uint32_t itab = rd32(s, s->prcb + 0x14);
    uint32_t vec = (s->icon >> ((line * 8) & 0x1f)) & 0xff;

    if (!vec || ((s->pc >> 16) & 0x1f) > vec >> 3)
        return;

    uint32_t fp = s->r[31];
    s->r[2] = s->ip;
    frame_push(s);
    uint32_t handler = rd32(s, itab + 4 + vec * 4);

    /* The original reads the interrupt stack pointer here but never uses it:
       the interrupt frame goes on the current stack (QUIRKS.md). The read is
       kept only for its side effect on callback pages. */
    if (!(s->pc & 0x2000)) {
        uint32_t a = s->prcb + 0x18;
        const i960_read_page *p = &s->rpage[a >> 16];
        if (!p->host)
            (void)p->r32(s->user, a);
    }

    uint32_t nf = (s->r[1] + 0x3f) & ~0x3fu;
    wr32(s, nf + 0x30, s->pc);
    wr32(s, nf + 0x34, ac_merged(s));
    wr32(s, nf + 0x38, vec);
    s->r[31] = nf + 0x40;
    s->r[0] = (fp & ~0x3fu) | 7;
#if I960_ACCURATE
    s->pc = (s->pc & 0xffe0ffff) | ((vec >> 3) << 16) | 0x2002;
#else
    /* The original clears bits 8-12 instead of the priority field 16-20. */
    s->pc = (s->pc & 0xffffe0ff) | ((vec >> 3) << 16) | 0x2002;
#endif
    s->r[1] = nf + 0x80;
    s->ip = handler;
    select_region(s);
    s->stop = 0;
    s->in_interrupt = 1;
}

/* ------------------------------------------------------ board helpers */

/* orig 0x4c2700: the board's write handler for 0xff000010-0xff00001f. */
void i960_iac_write(i960_state *s, uint32_t addr, uint32_t val)
{
    uint32_t k = (addr - 0xff000010u) >> 2;
    if (k < 4)   /* the original does not check the index */
        s->iac[k] = val;
}

void i960_iac_process(i960_state *s) { iac_check(s); }

/* orig 0x4c25f0 / 0x4c2670 select the region by the current ip rather than
   by `addr`; this uses `addr` and ignores addresses outside the region. */
static i960_insn *insn_at(i960_state *s, uint32_t addr)
{
    uint32_t hi = addr >> 20;
    i960_code_region *r = hi < s->region[1].base >> 20 ? &s->region[0]
                        : hi < s->region[2].base >> 20 ? &s->region[1]
                        : &s->region[2];
    uint32_t idx = (addr - r->base) >> 2;
    if (!r->cache || idx >= r->size >> 2)
        return NULL;
    return &r->cache[idx];
}

static void patch_insn(i960_state *s, uint32_t addr, const i960_opdef *op)
{
    i960_insn *in = insn_at(s, addr);
    if (in && in->op != op) {
        in->hooked_op = in->op;
        in->op = op;
    }
}

void i960_hook_insn(i960_state *s, uint32_t addr)       { patch_insn(s, addr, &hook_opdef); }
void i960_stop_after_insn(i960_state *s, uint32_t addr) { patch_insn(s, addr, &stop_opdef); }

/* orig 0x4c25d0: region 0 only, and the original instruction is dropped. */
void i960_nop_insn(i960_state *s, uint32_t addr)
{
    i960_code_region *r = &s->region[0];
    uint32_t idx = (addr - r->base) >> 2;
    if (r->cache && idx < r->size >> 2)
        r->cache[idx].op = &nop_opdef;
}

void     i960_set_stop(i960_state *s, uint32_t stop) { s->stop = stop; }
uint32_t i960_get_ac(const i960_state *s) { return ac_merged(s); }
void     i960_set_ac(i960_state *s, uint32_t ac) { s->ac = ac; s->cc = ac & 7; }
