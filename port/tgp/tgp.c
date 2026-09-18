/*
 * MB86234 TGP core, ported from Model 2 Emulator 1.1a. See tgp.h.
 * "orig 0x4aXXXX" comments give the address of the ported code.
 */
#include "tgp.h"

#include <math.h>
#include <string.h>

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error "tgp core assumes a little-endian host"
#endif

/* ------------------------------------------------------------ state block */

static inline uint32_t g32(const tgp *t, uint32_t off)
{
    uint32_t v;
    memcpy(&v, t->blk + off, 4);
    return v;
}

static inline void s32(tgp *t, uint32_t off, uint32_t v) { memcpy(t->blk + off, &v, 4); }

static inline uint16_t g16(const tgp *t, uint32_t off)
{
    uint16_t v;
    memcpy(&v, t->blk + off, 2);
    return v;
}

static inline void s16(tgp *t, uint32_t off, uint16_t v) { memcpy(t->blk + off, &v, 2); }

/* bookkeeping for the embedder (not part of the original): 4 KB pages of
   ext_ram written, for copies that follow it */
static inline void ext_dirty(tgp *t, uint32_t byte_off, uint32_t n)
{
    if (t->ext_ram_dirty)
        for (uint32_t p = byte_off >> 12; p <= (byte_off + n - 1) >> 12; p++)
            t->ext_ram_dirty[p >> 5] |= 1u << (p & 31);
}

/* Computed accesses: anything leaving the block reads 0 / is dropped. */
static uint32_t b32(const tgp *t, uint32_t off)
{
    return off <= TGP_BLK_SIZE - 4 ? g32(t, off) : 0;
}

static void w32(tgp *t, uint32_t off, uint32_t v)
{
    if (off <= TGP_BLK_SIZE - 4)
        s32(t, off, v);
}

#define RAM_OFF(i)   ((uint32_t)TGP_O_RAM + (uint32_t)(i) * 4u)
#define RAM(i)       b32(t, RAM_OFF(i))
#define SET_RAM(i,v) w32(t, RAM_OFF(i), (v))
#define R(n)         g32(t, TGP_O_R + 4 * (n))
#define SET_R(n,v)   s32(t, TGP_O_R + 4 * (n), (v))
#define A()          g32(t, TGP_O_A)
#define B()          g32(t, TGP_O_B)
#define D()          g32(t, TGP_O_D)
#define P()          g32(t, TGP_O_P)
#define SR()         t->blk[TGP_O_SR]
#define PC()         g16(t, TGP_O_PC)
#define SET_PC(v)    s16(t, TGP_O_PC, (uint16_t)(v))

/* --------------------------------------------------------- x87 float ops */

static inline float    fl(uint32_t u) { float f; memcpy(&f, &u, 4); return f; }
static inline uint32_t bits(float f) { uint32_t u; memcpy(&u, &f, 4); return u; }

static inline int is_nan(uint32_t u) { return (u & 0x7f800000) == 0x7f800000 && (u & 0x7fffff); }

/* fld of a single: signalling NaNs become quiet. TGP_TEST_UNICORN_FLD
   reproduces Unicorn's x87, which keeps them signalling; it exists only so
   that test/difftest.py can compare bursts exactly. */
static inline uint32_t x87_load(uint32_t u)
{
#ifdef TGP_TEST_UNICORN_FLD
    return u;
#else
    return is_nan(u) ? u | 0x400000 : u;
#endif
}

static inline uint32_t quiet(uint32_t u) { return is_nan(u) ? u | 0x400000 : u; }

/* NaN result of an x87 arithmetic op (st0 op src), per Intel's rules:
   SNaN + QNaN gives the QNaN; otherwise the NaN with the larger significand
   wins, and on a tie the positive one; a single NaN operand propagates;
   no NaN operand gives the default NaN. The result is always quiet. */
static uint32_t x87_nan(uint32_t st0, uint32_t src)
{
    int n0 = is_nan(st0), n1 = is_nan(src);
    if (n0 && n1) {
        int s0 = !(st0 & 0x400000), s1 = !(src & 0x400000);
        uint32_t m0 = st0 & 0x7fffff, m1 = src & 0x7fffff;
        if (s0 != s1)
            return quiet(s0 ? src : st0);
        if (m0 != m1)
            return quiet(m0 > m1 ? st0 : src);
        return quiet(!(st0 & 0x80000000) && (src & 0x80000000) ? st0 : src);
    }
    if (n0)
        return quiet(st0);
    if (n1)
        return quiet(src);
    return 0xffc00000;
}

/* st0 op src, rounded to single (double rounding through the x87's 53-bit
   precision is innocuous for these operations). */
static uint32_t x87_fadd(uint32_t st0, uint32_t src)
{
    float r = fl(st0) + fl(src);
    return r != r ? x87_nan(st0, src) : bits(r);
}

static uint32_t x87_fsub(uint32_t st0, uint32_t src)
{
    float r = fl(st0) - fl(src);
    return r != r ? x87_nan(st0, src) : bits(r);
}

static uint32_t x87_fmul(uint32_t st0, uint32_t src)
{
    float r = fl(st0) * fl(src);
    return r != r ? x87_nan(st0, src) : bits(r);
}

static uint32_t x87_fdiv(uint32_t st0, uint32_t src)
{
    float r = fl(st0) / fl(src);
    return r != r ? x87_nan(st0, src) : bits(r);
}

/* MSVC _ftol2 (SSE2 path, cvttsd2si): truncates, 0x80000000 when out of range. */
static uint32_t ftol(double v)
{
    if (!(v > -2147483649.0 && v < 2147483648.0))
        return 0x80000000u;
    return (uint32_t)(int32_t)v;
}

/* --------------------------------------------------------------- flags */

static void flags_float(tgp *t, uint32_t v)   /* orig 0x4a7440 */
{
    float f = fl(v);
    uint8_t sr = SR() & 0xfc;
    if (f < 0.0f) sr |= 2;
    if (f == 0.0f) sr |= 1;
    SR() = sr;
}

static void flags_int(tgp *t, uint32_t v)     /* orig 0x4a7470 / 0x4a7490 */
{
    uint8_t sr = SR() & 0xfc;
    if ((int32_t)v < 0) sr |= 2;
    if (v == 0) sr |= 1;
    SR() = sr;
}

/* ----------------------------------------------------------------- ALU */

static void alu(tgp *t, uint32_t op)          /* orig 0x4a7e00 */
{
    uint32_t a = A(), b = B(), d = D(), p = P(), r;
    uint8_t sh = (uint8_t)(g32(t, TGP_O_SHIFT) & 31);

    switch (op) {
    case 0x01: r = d & a; s32(t, TGP_O_D, r); flags_int(t, r); break;
    case 0x02: r = d | a; s32(t, TGP_O_D, r); flags_int(t, r); break;
    case 0x03: r = d ^ a; s32(t, TGP_O_D, r); flags_int(t, r); break;
    case 0x04: r = ~d;    s32(t, TGP_O_D, r); flags_int(t, r); break;
    case 0x05: flags_float(t, x87_fsub(d, a)); break;                    /* compare */
    case 0x06: r = x87_fadd(a, d); s32(t, TGP_O_D, r); flags_float(t, r); break;
    case 0x07: r = x87_fsub(d, a); s32(t, TGP_O_D, r); flags_float(t, r); break;
    case 0x08: s32(t, TGP_O_P, x87_fmul(b, a)); break;
    case 0x09:
        r = x87_fadd(p, d); s32(t, TGP_O_D, r);
        s32(t, TGP_O_P, x87_fmul(b, a)); flags_float(t, r); break;
    case 0x0a:
        r = x87_fsub(d, p); s32(t, TGP_O_D, r);
        s32(t, TGP_O_P, x87_fmul(b, a)); flags_float(t, r); break;
    case 0x0b: r = x87_load(d) & 0x7fffffff; s32(t, TGP_O_D, r); flags_float(t, r); break;
    case 0x0c: r = x87_fadd(p, d); s32(t, TGP_O_D, r); flags_float(t, r); break;
    case 0x0d:
        r = x87_load(p); s32(t, TGP_O_D, r);
        s32(t, TGP_O_P, x87_fmul(b, a)); flags_float(t, r); break;
    case 0x0e: r = bits((float)(int32_t)d); s32(t, TGP_O_D, r); flags_float(t, r); break;
    case 0x0f: {                                                     /* to int */
        double v = fl(x87_load(d));
        switch ((t->blk[TGP_O_MODE] >> 1) & 3) {
        case 0: r = ftol(floor(v + 0.5)); break;
        case 1: r = ftol(ceil(v)); break;
        case 2: r = ftol(floor(v)); break;
        default: r = ftol(v); break;
        }
        s32(t, TGP_O_D, r); flags_int(t, r); break;
    }
    case 0x10: r = x87_fdiv(d, a); s32(t, TGP_O_D, r); flags_int(t, r); break;
    case 0x11: r = x87_load(d) ^ 0x80000000; s32(t, TGP_O_D, r); flags_float(t, r); break;
    case 0x13: r = x87_fadd(b, a); s32(t, TGP_O_D, r); flags_int(t, r); break;
    case 0x14: r = x87_fsub(b, a); s32(t, TGP_O_D, r); flags_float(t, r); break;
    case 0x16: r = d >> sh; s32(t, TGP_O_D, r); flags_int(t, r); break;
    case 0x17: r = d << sh; s32(t, TGP_O_D, r); flags_int(t, r); break;
    case 0x18: r = (uint32_t)((int32_t)d >> sh); s32(t, TGP_O_D, r); flags_int(t, r); break;
    case 0x1a: r = d + a; s32(t, TGP_O_D, r); flags_int(t, r); break;
    case 0x1b: r = d - a; s32(t, TGP_O_D, r); flags_int(t, r); break;
    default: break;
    }
}

/* ---------------------------------------------------------- addressing */

/* 9-bit operand address (inline in the original). Pair X uses r0 (base) and
   r2 (offset), pair Y r1 and r3. Modes: 0/1/3 direct, 2 base+offset+disp6,
   6/7 base+offset with signed 5-bit post-increment (7 wraps offset to 64),
   4/5 give 0. */
static uint32_t addr_calc(tgp *t, uint32_t code, int pair_y)
{
    uint32_t base_n = pair_y ? 1 : 0, off_n = pair_y ? 3 : 2;
    uint32_t mode = (code >> 6) & 7;

    switch (mode) {
    case 0: case 1: case 3:
        return code;
    case 2:
        return (code & 0x3f) + R(off_n) + R(base_n);
    case 6: case 7: {
        uint32_t a = ((code & 0x20) ? 0 : R(base_n));
        uint32_t off = R(off_n);
        a += off;
        off += (code & 0x10) ? (code & 0x1f) - 0x20 : (code & 0x1f);
        if (mode == 7)
            off &= 0x3f;
        SET_R(off_n, off);
        return a;
    }
    default:
        return 0;
    }
}

/* ---------------------------------------------------- register file */

static uint32_t mant(uint32_t v) { return ((v >> 8) & 0x800000) | (v & 0x7fffff); }

/* orig 0x4a77f0: register read, or an address for modes 2 and 6 (note:
   5-bit displacement, and mode 6 does not wrap; mode 7 gives 0). */
static uint32_t reg_read(tgp *t, uint32_t code, int pair_x)
{
    uint32_t mode = (code >> 6) & 7, n = code & 0x3f;

    if (mode == 2) {
        uint32_t a = code & 0x1f;
        if (pair_x) {
            if (!(code & 0x20)) a += R(0);
            return a + R(2);
        }
        if (!(code & 0x20)) a += R(1);
        return a + R(3);
    }
    if (mode == 6) {
        uint32_t off_n = pair_x ? 2 : 3;
        uint32_t a = (code & 0x20) ? 0 : R(pair_x ? 0 : 1);
        uint32_t off = R(off_n);
        a += off;
        off += (code & 0x10) ? (code & 0x1f) - 0x20 : (code & 0x1f);
        SET_R(off_n, off);
        return a;
    }
    if (mode != 0 && mode != 1 && mode != 3)
        return 0;
    if (n < 0x10)
        return R(n);
    switch (n) {
    case 0x10: return A();
    case 0x11: return (A() >> 23) & 0xff;
    case 0x12: return mant(A());
    case 0x13: return B();
    case 0x14: return (B() >> 23) & 0xff;
    case 0x15: return mant(B());
    case 0x16: return g32(t, TGP_O_C);
    case 0x17: return (g32(t, TGP_O_C) >> 23) & 0xff;
    case 0x18: return mant(g32(t, TGP_O_C));
    case 0x19: return D();
    case 0x1a: return (D() >> 23) & 0xff;
    case 0x1b: return mant(D());
    case 0x1c: return P();
    case 0x1d: return (P() >> 23) & 0xff;
    case 0x1e: return mant(P());
    case 0x1f: return g32(t, TGP_O_SHIFT);
    case 0x21: return tgp_fifo_pop(&t->in);
    case 0x23: return g32(t, TGP_O_BANK);
    case 0x34: return t->blk[TGP_O_R34];
    default: return 0;
    }
}

static void reg_write(tgp *t, uint32_t code, uint32_t v)   /* orig 0x4a76f0 */
{
    uint32_t mode = (code >> 6) & 7, n = code & 0x3f;

    if (mode != 0 && mode != 1 && mode != 3)
        return;
    if (n == 0xc || n == 0xd)
        v &= 0xff;
    if (n < 0x10) {
        SET_R(n, v);
        return;
    }
    switch (n) {
    case 0x10: s32(t, TGP_O_A, v); break;
    case 0x13: s32(t, TGP_O_B, v); break;
    case 0x16: s32(t, TGP_O_C, v); break;
    case 0x19: s32(t, TGP_O_D, v); break;
    case 0x1c: s32(t, TGP_O_P, v); break;
    case 0x1d: s32(t, TGP_O_P, P() ^ ((v << 23 ^ P()) & 0x7f800000)); break;
    case 0x1f: s32(t, TGP_O_SHIFT, v); break;
    case 0x22: tgp_fifo_push(&t->out, v); break;
    case 0x23: s32(t, TGP_O_BANK, v); break;
    case 0x34: t->blk[TGP_O_R34] = (uint8_t)v; break;
    default: break;
    }
}

/* ------------------------------------------- external memory, table unit */

static uint32_t rom32(const tgp *t, uint32_t off)
{
    uint32_t v;
    if (!t->table_rom || off > t->table_rom_size - 4 || t->table_rom_size < 4)
        return 0;
    memcpy(&v, t->table_rom + off, 4);
    return v;
}

static int16_t rom16(const tgp *t, uint32_t off)
{
    int16_t v;
    if (!t->table_rom || off > t->table_rom_size - 2 || t->table_rom_size < 2)
        return 0;
    memcpy(&v, t->table_rom + off, 2);
    return v;
}

#define TREG(n) g32(t, TGP_O_TREG + 4 * (n))

static uint32_t table_atan(tgp *t)            /* register 0x27, orig 0x4a7b16 */
{
    uint32_t r24 = TREG(0x24), r25 = TREG(0x25), r27 = TREG(0x27);
    uint32_t e = (r27 >> 23) & 0xff;
    uint32_t idx, v;
    int half = 0;

    if (e == 0) {
        if ((r24 & 0x7fffffff) > (r25 & 0x7fffffff))
            return (r24 >> 16) & 0x8000;
        return ((r25 & 0xc000ffff) | 0x40000000) >> 16 & 0xffff;
    }
    if ((int32_t)(r25 ^ r24) < 0)
        half = 2;
    if ((e & 0x70) == 0x70) {
        if (e - 0x70 <= 0xe) {
            int s = (int)e - 0x71;
            if (s < 0) s = 0;
            uint32_t one = 1u << s;
            idx = ((r27 >> (23 - s)) & (one - 1)) + one;
        } else {
            idx = 0x3fff;
        }
    } else {
        idx = 0;
    }
    v = (uint32_t)(int32_t)rom16(t, 0x10000 + idx * 4 + (uint32_t)half);
    if ((r24 & 0x7fffffff) <= (r25 & 0x7fffffff))
        v = 0x4000 - v;
    if (r24 & 0x80000000) {
        if ((int32_t)r25 < 0) v |= 0x8000;
        else v &= 0x7fff;
    } else if ((int32_t)r25 < 0) {
        v |= 0x8000;
    }
    return v & 0xffff;
}

static uint32_t recip_like(tgp *t, uint32_t x, uint32_t base, int keep_sign, int rsqrt)
{
    uint32_t idx, v, ex;
    if (rsqrt) {
        idx = ((x >> 11) ^ 0xfffff000) & 0x1fff;
        ex = (x >> 24) & 0x7f;
    } else {
        idx = (x >> 10) & 0x1fff;
        ex = (x >> 23) & 0xff;
    }
    v = rom32(t, base + idx * 8 + (keep_sign ? 4 : 0)) & 0x7fffffff;
    if (keep_sign && (int32_t)x < 0)
        v |= 0x80000000;
    return ((((v >> 23) & 0xff) - ex + (rsqrt ? 0x3f : 0x7f)) << 23) | (v & 0x807fffff);
}

/* orig 0x4a7a80 */
static uint32_t ext_read(tgp *t, uint32_t bank, uint32_t addr)
{
    if (bank) {
        uint32_t sel = (bank >> 20) & 0xf;
        uint32_t i = (bank & 0x3f0000) | (addr & 0xffff);
        if (sel >= 8) {
            if (!t->ext_rom)
                return 0xffffffff;
            return i < t->ext_rom_words ? t->ext_rom[i] : 0;
        }
        if (sel != 4)
            return 0;
        if (!t->ext_ram || (uint64_t)i * 4 + 4 > t->ext_ram_words * 4ull)
            return 0;
        uint32_t v;
        memcpy(&v, (uint8_t *)t->ext_ram + i * 4, 4);
        return v;
    }
    if (addr - 0x20 > 0xf)   /* program memory */
        return addr < t->prog_words ? t->prog[addr] : 0;
    if (addr < 0x24) {       /* sin/cos of a 16-bit angle */
        uint32_t ang = (addr - 0x20) * 0x4000 + TREG(0x20);
        uint32_t i = ang & 0x3fff, v;
        if ((ang & 0x7fff) == 0)
            v = 0;
        else if ((ang & 0x7fff) == 0x4000)
            v = 0x3f800000;
        else {
            if (ang & 0x4000)
                i = 0x4000 - i;
            v = rom32(t, i * 4);
        }
        if ((ang & 0x8000) && v)
            v |= 0x80000000;
        return v;
    }
    switch (addr) {
    case 0x27: return table_atan(t);
    case 0x28: return recip_like(t, TREG(0x28), 0x20000, 0, 0);
    case 0x29: return recip_like(t, TREG(0x28), 0x20000, 1, 0);
    case 0x2a: return recip_like(t, TREG(0x2a), 0x30000, 0, 1);
    case 0x2b: return recip_like(t, TREG(0x2a), 0x30000, 1, 1);
    default: return 0;
    }
}

/* orig 0x4a7690 */
static void ext_write(tgp *t, uint32_t bank, uint32_t addr, uint32_t v)
{
    if (bank == 0) {
        if (addr - 0x20 <= 0xf) {
            s32(t, TGP_O_TREG + 4 * addr, v);
            if (addr == 0x25) {
                if ((TREG(0x24) & 0x7fffffff) <= (TREG(0x25) & 0x7fffffff))
                    SR() |= 4;
                else
                    SR() &= 0xfb;
            }
        }
        return;
    }
    if ((bank & 0xf00000) == 0x400000) {
        uint32_t i = addr & 0xfffff;
        if (t->ext_ram && i < t->ext_ram_words) {
            memcpy((uint8_t *)t->ext_ram + i * 4, &v, 4);
            ext_dirty(t, i * 4, 4);
        }
    }
}

/* ---------------------------------------------------------- branches */

static int cond(tgp *t, uint32_t c)           /* orig 0x4a74b0 */
{
    uint8_t sr = SR();
    switch (c) {
    case 0x00: return sr & 1;
    case 0x01: return (sr & 1) || !(sr & 2);
    case 0x02: return (sr & 3) != 0;
    case 0x0a: return (sr >> 2) & 1;
    case 0x10: { uint32_t n = R(12) - 1; SET_R(12, n); return n != 0; }
    case 0x11: { uint32_t n = R(13) - 1; SET_R(13, n); return n != 0; }
    case 0x16: return 1;
    default: return 0;
    }
}

static void push_pc(tgp *t)
{
    uint8_t sp = t->blk[TGP_O_SP];
    s16(t, TGP_O_STACK + 2u * sp, PC());
    /* re-read: with sp == 4 the store just overwrote the sp byte itself */
    t->blk[TGP_O_SP] = (uint8_t)(t->blk[TGP_O_SP] + 1);
}

static void pop_pc(tgp *t)
{
    uint8_t sp = (uint8_t)(t->blk[TGP_O_SP] - 1);
    t->blk[TGP_O_SP] = sp;
    SET_PC(g16(t, TGP_O_STACK + 2u * sp));
}

static void branch(tgp *t, uint32_t w, int when_true)
{
    uint32_t tgt = w & 0xffff;

    if (cond(t, (w >> 20) & 0x1f) != when_true)
        return;
    switch ((w >> 16) & 0xf) {
    case 0x0: SET_PC(tgt - 1); break;
    case 0x2: SET_PC(reg_read(t, tgt & 0x7f, 0) - 1); break;
    case 0x4: push_pc(t); SET_PC(tgt - 1); break;
    case 0x6:
        if (when_true) {
            push_pc(t);
            if (tgt & 0x4000) {
                SET_PC(reg_read(t, tgt & 0x7f, 0) - 1);
                break;
            }
        }
        /* the "branch if false" form does not push (QUIRKS.md) */
        SET_PC((uint16_t)RAM(tgt) - 1);
        break;
    case 0xa: pop_pc(t); break;
    case 0xc: reg_write(t, (w >> 9) & 0x7f, RAM(w & 0x1ff)); break;
    default: break;
    }
}

/* ------------------------------------------------------- instructions */

/* Class 0: ALU op (bits 21-25) first, then move A and B. */
static void op_class0(tgp *t, uint32_t w)
{
    uint32_t s = w & 0x1ff, d = (w >> 9) & 0x7f;

    alu(t, (w >> 21) & 0x1f);
    switch ((w >> 16) & 0x1f) {
    case 0x01:
        s32(t, TGP_O_A, RAM(addr_calc(t, s, 0)));
        s32(t, TGP_O_B, ext_read(t, g32(t, TGP_O_BANK), addr_calc(t, d | 0x80, 1)));
        break;
    case 0x04:
        s32(t, TGP_O_A, RAM(s));
        s32(t, TGP_O_B, ext_read(t, g32(t, TGP_O_BANK), d));
        break;
    case 0x0c:
        s32(t, TGP_O_A, RAM(addr_calc(t, s, 0)));
        s32(t, TGP_O_B, RAM(d | 0x200));
        break;
    case 0x0d:
        s32(t, TGP_O_A, RAM(addr_calc(t, s, 0)));
        s32(t, TGP_O_B, RAM(addr_calc(t, d | 0x80, 1) | 0x200));
        break;
    case 0x0f:
        s32(t, TGP_O_A, RAM(s));
        s32(t, TGP_O_B, RAM(addr_calc(t, d | 0x180, 1) | 0x200));
        break;
    case 0x10:
        s32(t, TGP_O_A, RAM(addr_calc(t, s, 0) | 0x200));
        s32(t, TGP_O_B, RAM(d));
        break;
    case 0x11:
        s32(t, TGP_O_A, RAM(addr_calc(t, s, 0) | 0x200));
        s32(t, TGP_O_B, RAM(addr_calc(t, d | 0x80, 1)));
        break;
    default:
        break;
    }
}

/* Class 7: loads and stores; the ALU op (bits 21-25) runs between the read
   and the write, except where noted. */
static void op_class7(tgp *t, uint32_t w)
{
    uint32_t s = w & 0x1ff, d = (w >> 9) & 0x7f, op = (w >> 21) & 0x1f, v;
    uint32_t bank;

    switch ((w >> 16) & 0x1f) {
    case 0x03: /* RAM A[S] -> ext[Y++] */
    case 0x07: /* RAM A[X(S)] -> ext[Y++] */
        v = RAM(((w >> 16) & 0x1f) == 7 ? addr_calc(t, s, 0) : s);
        alu(t, op);
        {
            uint32_t a = addr_calc(t, d | 0x180, 1);
            ext_write(t, g32(t, TGP_O_BANK), a, v);
        }
        break;
    case 0x04: /* RAM A[S] -> ext[D], ALU after */
        ext_write(t, g32(t, TGP_O_BANK), d, RAM(s));
        alu(t, op);
        break;
    case 0x08: /* ext[X(S)] -> RAM A[D] */
    case 0x14: /* program / table unit [X(S)] -> RAM A[D] */
        {
            uint32_t a = addr_calc(t, s, 0);
            bank = ((w >> 16) & 0x1f) == 0x14 ? 0 : g32(t, TGP_O_BANK);
            v = ext_read(t, bank, a);
        }
        alu(t, op);
        SET_RAM(d, v);
        break;
    case 0x0b: /* ext[X(S)] -> RAM A[Y++] */
    case 0x17:
        v = ext_read(t, g32(t, TGP_O_BANK), addr_calc(t, s, 0));
        alu(t, op);
        SET_RAM(addr_calc(t, d | 0x180, 1), v);
        break;
    case 0x0c: /* RAM A[S] -> RAM B[D], ALU after */
        SET_RAM(d | 0x200, RAM(s));
        alu(t, op);
        break;
    case 0x0d: /* RAM A[S] -> RAM B[Y(D|0x80)] */
        v = RAM(s);
        alu(t, op);
        SET_RAM(addr_calc(t, d | 0x80, 1) | 0x200, v);
        break;
    case 0x0f: /* RAM A[X(S)] -> RAM B[Y++] */
        v = RAM(addr_calc(t, s, 0));
        alu(t, op);
        SET_RAM(addr_calc(t, d | 0x180, 1) | 0x200, v);
        break;
    case 0x10: /* RAM B[X(S)] -> RAM A[D] */
        v = RAM(addr_calc(t, s, 0) | 0x200);
        alu(t, op);
        SET_RAM(d, v);
        break;
    case 0x13: /* RAM B[X(S)] -> RAM A[Y++] */
        v = RAM(addr_calc(t, s, 0) | 0x200);
        alu(t, op);
        SET_RAM(addr_calc(t, d | 0x180, 1), v);
        break;
    case 0x1c: /* register D -> RAM A[Y(S)] or ext[Y(S)] */
        v = reg_read(t, d, 1);
        alu(t, op);
        {
            uint32_t a = addr_calc(t, s, 1);
            if (d & 0x40)
                ext_write(t, g32(t, TGP_O_BANK), a, v);
            else
                SET_RAM(a, v);
        }
        break;
    case 0x1d: /* RAM A[S or address from reg_read] -> register D */
        if (s & 0x180)
            v = RAM(reg_read(t, s, 0));
        else
            v = RAM(s);
        alu(t, op);
        reg_write(t, d, v);
        break;
    case 0x1e: /* ext / program / table [addr] -> register D */
        if (d & 0x40) {
            uint32_t a = addr_calc(t, s, 0);
            v = ext_read(t, 0, a);
        } else {
            uint32_t a = addr_calc(t, s, 1);
            v = ext_read(t, g32(t, TGP_O_BANK), a);
        }
        alu(t, op);
        reg_write(t, d, v);
        break;
    case 0x1f: /* register S -> register D (S=0x10, D=0xf: ALU only) */
        if (s == 0x10 && d == 0xf) {
            alu(t, op);
            break;
        }
        v = reg_read(t, s, 1);
        alu(t, op);
        reg_write(t, d, v);
        break;
    default:
        break;
    }
}

static uint32_t sext24(uint32_t w) { return (uint32_t)((int32_t)(w << 8) >> 8); }

static uint32_t set_exp(uint32_t r, uint32_t imm) { return r ^ ((imm << 23 ^ r) & 0x7f800000); }

static uint32_t set_sign_mant(uint32_t r, uint32_t imm)
{
    return ((imm & 0xff800000) << 8) | (imm & 0x7fffff) | (r & 0x7f800000);
}

/* orig 0x4a8245: replaces the instruction by atan2(B, A) as a 16-bit angle */
static void hle_atan(tgp *t)
{
    float a = fl(A()), b = fl(B());
    uint32_t r;
    /* the "< 0" tests are fcompp + test ah,1, which is also true for NaN */
    if (b == 0.0f)
        r = (a < 0.0f || a != a) ? 0xffff8000 : 0;
    else if (a == 0.0f)
        r = (b < 0.0f || b != b) ? 0xffffc000 : 0x4000;
    else
        r = ftol(atan2((double)b, (double)a) * 32768.0 * 0.3183098861837907);
    s32(t, TGP_O_TREG + 4 * 0x27, r);
}

static void step(tgp *t, uint32_t w, uint32_t pflags)
{
    if (pflags & TGP_FLAG_HLE_ATAN) {
        hle_atan(t);
        return;
    }
    switch (w >> 26) {
    case 0x00: op_class0(t, w); break;
    case 0x07: op_class7(t, w); break;
    case 0x0d:
        if ((w & 0xff0000) == 0xa0000)
            t->blk[TGP_O_MODE] = (uint8_t)w;
        break;
    case 0x0e: {
        static const uint16_t dst[4] = { TGP_O_P, TGP_O_A, TGP_O_B, TGP_O_D };
        s32(t, dst[(w >> 24) & 3], sext24(w));
        break;
    }
    case 0x0f: {
        uint32_t sub = (w >> 16) & 0xf, sub2 = (w >> 12) & 0xf;
        alu(t, (w >> 20) & 0x1f);
        if (sub == 4) {
            if (sub2 == 0) {
                uint16_t n = (uint16_t)(w & 0xfff);
                s16(t, TGP_O_REPEAT, n ? n : 0x100);
                SET_PC(PC() + 1);
            } else if (sub2 == 8) {
                uint32_t n = reg_read(t, w & 0xfff, 0);
                SET_PC(PC() + 1);
                s16(t, TGP_O_REPEAT, (uint16_t)n);
            }
        } else if (sub == 2) {
            SR() &= (uint8_t)~w;
        } else if (sub == 6) {
            SR() |= (uint8_t)w;
        } else if (sub == 0) {
            switch (w & 0x1f) {
            case 0x04: s32(t, TGP_O_A, 0); break;
            case 0x08: s32(t, TGP_O_B, 0); break;
            case 0x10: s32(t, TGP_O_D, 0); break;
            default: break;
            }
        }
        break;
    }
    case 0x10: {
        uint32_t n = (w >> 24) & 0xf;
        if (n <= 3)
            SET_R(n, w & 0xffff);
        break;
    }
    case 0x13:
        if (((w >> 24) & 3) < 2)
            SET_R(12 + ((w >> 24) & 3), w & 0xffffff);
        break;
    case 0x14:
        switch ((w >> 24) & 3) {
        case 0: s32(t, TGP_O_A, w & 0xffffff); break;
        case 1: s32(t, TGP_O_A, set_exp(A(), w & 0xffffff)); break;
        case 2: s32(t, TGP_O_A, set_sign_mant(A(), w & 0xffffff)); break;
        default: break;
        }
        break;
    case 0x15:
        switch ((w >> 24) & 3) {
        case 0: case 1: s32(t, TGP_O_B, set_exp(B(), w & 0xffffff)); break;
        case 2: s32(t, TGP_O_B, set_sign_mant(B(), w & 0xffffff)); break;
        default: break;
        }
        break;
    case 0x16:
        switch ((w >> 24) & 3) {
        case 0: s32(t, TGP_O_C, (w & 0xff) << 23); break;
        case 1: s32(t, TGP_O_D, set_sign_mant(0, w & 0xffffff)); break;
        case 2: s32(t, TGP_O_D, set_exp(D(), w & 0xffffff)); break;
        default:
            t->blk[TGP_O_FLAGS2] &= 0xef;
            s32(t, TGP_O_D, set_sign_mant(D(), w & 0xffffff));
            break;
        }
        break;
    case 0x17:
        if ((w & 0x3000000) == 0x3000000)
            s32(t, TGP_O_SHIFT, w & 0xffffff);
        break;
    case 0x18:
        if ((w & 0x3000000) == 0x3000000)
            s32(t, TGP_O_BANK, w & 0xffffff);
        break;
    case 0x1d:
        if (!(w & 0x3000000))
            t->blk[TGP_O_R34] = (uint8_t)w;
        break;
    case 0x2f: branch(t, w, 1); break;
    case 0x3f: branch(t, w, 0); break;
    default: break;
    }
}

/* orig 0x4a81c0 */
void tgp_run(tgp *t)
{
    tgp_run_count(t, 1000);
}

void tgp_run_count(tgp *t, int count)
{
    for (int n = count; n > 0; n--) {
        if (t->blk[TGP_O_STOP])
            return;

        uint32_t pc = PC();
        uint32_t w = pc < t->prog_words ? t->prog[pc] : 0;
        uint32_t pflags = b32(t, TGP_O_PFLAGS + pc * 8);

        if (((pflags & TGP_FLAG_FIFO_IN) && t->in.free == 32) ||
            ((pflags & TGP_FLAG_FIFO_OUT) && t->out.free == 0)) {
            t->blk[TGP_O_STOP] = 1;
            return;
        }
        step(t, w, pflags);

        uint16_t rep = g16(t, TGP_O_REPEAT);
        if (rep)
            s16(t, TGP_O_REPEAT, (uint16_t)(rep - 1));
        else
            SET_PC(PC() + 1);
    }
}

void tgp_run_until_stop(tgp *t)
{
    while (!t->blk[TGP_O_STOP])
        tgp_run(t);
}

/* orig 0x4a9a80 */
void tgp_reset(tgp *t, uint32_t prog_bytes)
{
    memset(t->blk + TGP_O_PFLAGS, 0, 0x8000);
    memset(t->blk + TGP_O_A, 0, 0x1072);
    t->blk[TGP_O_STOP] = 0;
    t->blk[TGP_O_SP] = 0;
    s32(t, TGP_O_PROGPTR, t->prog_ptr_value);

    uint32_t n = prog_bytes >> 2;
    for (uint32_t i = 0; i < n && i < t->prog_words; i++) {
        uint32_t w = t->prog[i], f = 0;
        if ((w & 0xfc000000) != 0x1c000000)
            continue;
        switch ((w >> 16) & 0x1f) {
        case 0x1f:
            if ((w & 0xff) == 0x21) f |= TGP_FLAG_FIFO_IN;
            if ((w & 0x7e00) == 0x4400) f |= TGP_FLAG_FIFO_OUT;
            break;
        case 0x1c:
            if ((w & 0x7e00) == 0x4200) f |= TGP_FLAG_FIFO_IN;
            break;
        case 0x1d:
            if ((w & 0x7e00) == 0x4400) f |= TGP_FLAG_FIFO_OUT;
            break;
        default:
            break;
        }
        /* no bounds check in the original: past 0x1000 words this ORs into
           the registers that follow the table */
        w32(t, TGP_O_PFLAGS + i * 8, b32(t, TGP_O_PFLAGS + i * 8) | f);
    }
}

void tgp_init(tgp *t)
{
    memset(t, 0, sizeof *t);
    tgp_fifo_reset(&t->in);
    tgp_fifo_reset(&t->out);
    t->blk[TGP_O_STOP] = 1;
}

/* ---------------------------------------------------------------- FIFOs */

void tgp_fifo_reset(tgp_fifo *f)
{
    memset(f, 0, sizeof *f);
    f->free = 32;
}

void tgp_fifo_push(tgp_fifo *f, uint32_t v)
{
    f->buf[f->wr & 31] = v;
    f->free--;
    f->wr = (f->wr + 1) & 31;
}

uint32_t tgp_fifo_pop(tgp_fifo *f)
{
    uint32_t v = f->buf[f->rd & 31];
    f->free++;
    f->rd = (f->rd + 1) & 31;
    return v;
}

/* --------------------------------------------- Model 2/2A copro driver */

void tgp_m2_board_reset(tgp *t)   /* orig 0x4c54d0, TGP part */
{
    tgp_reset(t, 0x100);
    s32(t, TGP_O_TABLEPTR, t->table_ptr_value);
    t->blk[TGP_O_STOP] = 1;
}

static int uploading(uint32_t ctrl) { return (ctrl & 0x80000000u) != 0; }

void tgp_m2_port_write32(tgp *t, uint32_t addr, uint32_t v)   /* orig 0x4c5620 */
{
    if (addr - 0x880000u <= 0x3fff && !(addr & 0xf)) {
        /* command: the value comes from the address, the data is ignored */
        uint32_t b = (addr >> 4) & 0xff;
        tgp_fifo_push(&t->in, b << 23 | b << 8 | b);
        t->blk[TGP_O_STOP] = 0;
        do tgp_run(t); while (!t->blk[TGP_O_STOP]);
        return;
    }
    if (uploading(t->prog_upload_ctrl)) {
        uint32_t i = t->prog_upload_off / 4;
        if (t->prog && i < t->prog_words && !(t->prog_upload_off & 3))
            t->prog[i] = v;
        else if (t->prog && t->prog_upload_off + 4 <= t->prog_words * 4)
            memcpy((uint8_t *)t->prog + t->prog_upload_off, &v, 4);
        t->prog_upload_off += 4;
        return;
    }
    if (t->prog_upload_off && addr - 0x884000u <= 0x3fff) {
        tgp_fifo_push(&t->in, v);
        t->blk[TGP_O_STOP] = 0;
        tgp_run_until_stop(t);
    }
}

void tgp_m2_port_write16(tgp *t, uint32_t addr, uint16_t v)   /* orig 0x4c5700 */
{
    if (uploading(t->prog_upload_ctrl)) {
        if (t->prog && t->prog_upload_off + 2 <= t->prog_words * 4)
            memcpy((uint8_t *)t->prog + t->prog_upload_off, &v, 2);
        t->prog_upload_off += 2;
        return;
    }
    tgp_m2_port_write32(t, addr, v);
}

static uint32_t port_pop(tgp *t)
{
    uint32_t v = tgp_fifo_pop(&t->out);
    if (t->out.free != 0) {
        t->blk[TGP_O_STOP] = 0;
        do tgp_run(t); while (!t->blk[TGP_O_STOP]);
    }
    return v;
}

uint32_t tgp_m2_port_read32(tgp *t, uint32_t addr)   /* orig 0x4c5730 */
{
    return addr - 0x884000u <= 0x3fff ? port_pop(t) : 0;
}

uint16_t tgp_m2_port_read16(tgp *t, uint32_t addr)   /* orig 0x4c5790 */
{
    return addr - 0x884000u <= 0x3fff ? (uint16_t)port_pop(t) : 0xffff;
}

uint8_t tgp_m2_port_read8(tgp *t, uint32_t addr)     /* orig 0x4c57f0 */
{
    return addr - 0x884000u <= 0x3fff ? (uint8_t)port_pop(t) : 0xff;
}

static void ext_ram_store(tgp *t, uint32_t byte_off, uint32_t v)
{
    if (t->ext_ram && (uint64_t)byte_off + 4 <= t->ext_ram_words * 4ull) {
        memcpy((uint8_t *)t->ext_ram + byte_off, &v, 4);
        ext_dirty(t, byte_off, 4);
    }
}

void tgp_m2_ctrl_write32(tgp *t, uint32_t addr, uint32_t v)   /* orig 0x4c5310 */
{
    if (addr == 0x801008) {
        t->ext_ptr = v;
        return;
    }
    if (addr == 0x803008) {
        t->reg_803008 = v;
        return;
    }
    if ((addr & 0xfffff000) == 0x804000 && uploading(t->data_upload_ctrl)) {
        if (t->data_buf && t->data_upload_off + 4 <= t->data_buf_words * 4)
            memcpy((uint8_t *)t->data_buf + t->data_upload_off, &v, 4);
        t->data_upload_off += 4;
        return;
    }

    uint32_t a = addr & 0xffff;
    if (a < 0x3ff0 && !(a & 0xf)) {
        /* copro RAM write with the exponent taken from the address */
        uint32_t b = a >> 4, e = b & 0x3f, val = (v & 0xe07fffff) | e << 23;
        if ((b & 0xc0) && e == 1)
            val |= (a & 0xc00) << 19;
        uint32_t p = t->ext_ptr & 0x7ffff;
        ext_ram_store(t, p, val);
        t->ext_ptr = (p + 4) & 0x7ffff;
    }
    if (a - 0x4000u <= 0x3fff) {
        uint32_t p = t->ext_ptr & 0x7ffff;
        ext_ram_store(t, p, v);
        t->ext_ptr = (p + 4) & 0x7ffff;
    }
}

/* orig 0x4c5400: masks the address to 12 bits and then compares it with
   0x804000, so it never does anything (QUIRKS.md). */
void tgp_m2_ctrl_write16(tgp *t, uint32_t addr, uint16_t v)
{
    (void)t; (void)addr; (void)v;
}

uint32_t tgp_m2_ctrl_read32(tgp *t, uint32_t addr)   /* orig 0x4c5450 */
{
    switch (addr) {
    case 0x801008: return t->reg_801008_read;
    case 0x802008: return t->ext_ptr;
    case 0x803008: return t->reg_803008;
    default: return 0;
    }
}

void tgp_m2_upload_write(tgp *t, uint32_t addr, uint32_t v)   /* orig 0x4c79c0 */
{
    if (addr == 0x980000) {
        if ((t->prog_upload_ctrl ^ v) == 0x80000000u) {
            if (v & 0x80000000u) {
                t->prog_upload_ctrl = v;
                t->prog_upload_off = 0;
                return;
            }
            tgp_reset(t, t->prog_upload_off);   /* driver entry 15 */
        }
        t->prog_upload_ctrl = v;
    } else if (addr == 0x980008) {
        if ((t->data_upload_ctrl ^ v) == 0x80000000u && (v & 0x80000000u)) {
            t->data_upload_ctrl = v;
            t->data_upload_off = 0;
            return;
        }
        t->data_upload_ctrl = v;   /* driver entry 16 does nothing on Model 2/2A */
    } else if (addr == 0x98000c) {
        t->reg_98000c = v;
    }
}
