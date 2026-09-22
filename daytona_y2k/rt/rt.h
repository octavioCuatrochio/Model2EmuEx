/*
 * Runtime for the recompiled Daytona USA program (see ../README.md).
 *
 * Generated functions work on the interpreter's own state (i960_state):
 * registers, frame cache, condition codes and memory map are the same
 * objects, so the interpreter can take over at any instruction, and the
 * board's interrupts need nothing new. Every helper here reproduces what
 * port/i960/i960.c does for the same instruction, quirks included.
 */
#ifndef RT_H
#define RT_H

#include "../../port/i960/i960.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

/* A recompiled function: entered with the guest at its entry and the frame
   set up by the caller; runs the guest until it is back at ret_ip with frame
   pointer ret_fp (the caller's return address and frame). It returns the
   function to continue with (a tail jump into another function's code), or
   RT_DONE; rt_call runs that chain. */
typedef struct rt_cont rt_cont;
typedef rt_cont (*rt_fn)(i960_state *s, uint32_t ret_ip, uint32_t ret_fp);
struct rt_cont { rt_fn fn; };
typedef struct { uint32_t addr; rt_fn fn; } rt_entry;

#define RT_FN(name) rt_cont name(i960_state *s, uint32_t ret_ip, uint32_t ret_fp)
#define RT_DONE ((rt_cont){ 0 })
#define RT_TAIL(f) ((rt_cont){ f })

static inline void rt_call(rt_fn fn, i960_state *s, uint32_t ret_ip, uint32_t ret_fp)
{
    rt_cont k = fn(s, ret_ip, ret_fp);
    while (k.fn)
        k = k.fn(s, ret_ip, ret_fp);
}

extern const rt_entry rt_funcs[];
extern const int rt_nfuncs;
extern const uint32_t rt_program_crc;   /* CRC-32 of the 256 KB program it was made from */

#define RT_UNLIKELY(x) __builtin_expect(!!(x), 0)

/* Registers past g15, as the interpreter's multi-word operands reach them:
   the register file continues with reg_pad and fp0-fp3 (i960.h). */
#define RX(n) (*(uint32_t *)((char *)s + offsetof(i960_state, r) + 4 * (n)))

/* ------------------------------------------------------------ slices */

/* Ends the slice at `next` (the ip the guest continues at); runs an
   interrupt handler if the board entered one. Nonzero if the guest is not
   at `next` afterwards. */
int  rt_yield(i960_state *s, uint32_t next);
/* The interpreter runs the instruction at s->ip (s->prev_ip set by the
   caller); nonzero if a slice ended and the guest isn't where it was. */
int  rt_step(i960_state *s);
/* Calls the function at s->ip == t (the interpreter if there is none). */
void rt_run(i960_state *s, uint32_t t, uint32_t ret_ip, uint32_t ret_fp);
/* The interpreter runs the guest until it is at ret_ip with ret_fp,
   handing over to recompiled code at each function entry it reaches. */
void rt_interp(i960_state *s, uint32_t ret_ip, uint32_t ret_fp);
/* A function's code found the guest where it didn't expect (s->ip): done
   if that is the return, else on with the function there, or the
   interpreter. */
rt_cont rt_resume(i960_state *s, uint32_t ret_ip, uint32_t ret_fp);
/* The recompiled function whose entry is `addr`, or NULL. */
rt_fn rt_lookup(uint32_t addr);

/* For functions rewritten by hand (../src, through src/decomp.h).

   Charge one instruction's cycles and let the board in if the slice ends
   there; `ip` is where the guest stands. Nonzero if an interrupt left the
   guest somewhere else. */
int rt_charge(i960_state *s, int cycles, uint32_t ip);
/* The cost of each instruction of the program, by word, from the
   recompiler (build/gen/cycles.c); bit 15 marks an 8-byte instruction. */
extern const uint16_t rt_cycle_word[];
static inline uint16_t rt_insn_word(uint32_t a)
{
    return rt_cycle_word[(a >= 0x220000u ? a - 0x200000u : a) >> 2];
}
/* Charge the instructions from `a` up to `b` one at a time, as the
   interpreter would, so a slice ends after the same instruction; `last`
   is where the final one goes (a taken branch), or 0 for the next one.
   Nonzero if an interrupt left the guest somewhere else. */
int rt_pay(i960_state *s, uint32_t a, uint32_t b, uint32_t last);
/* The ret at `at` of a rewritten function: the frame and ip it gives,
   then its cycles, as the recompiled ret charges them. */
rt_cont rt_return_at(i960_state *s, uint32_t at);
/* The bx at `at` that ends a bal procedure, to `target`. */
rt_cont rt_return_bx(i960_state *s, uint32_t at, uint32_t target, uint32_t ret_ip, uint32_t ret_fp);
/* The cycles of a call/bal whose callee is `entry`, charged after the
   transfer, with the region bookkeeping of a jump across 2 MB areas. */
int rt_charge_call(i960_state *s, int cycles, uint32_t site, uint32_t entry);

/* Built with -DRT_LOG_CYCLES: every charge is written out (the ip the guest
   moves to, what it cost, and the cycles left), to compare the stream with
   the interpreter's (tools/trace.c, TRACE_CYCLES=-1). Debugging only, and
   very slow; see ../README.md, "When the two disagree". */
#ifdef RT_LOG_CYCLES
void rt_log_cycle(uint32_t next, int cost, int left);
#define RT_LOG(next, cost) rt_log_cycle((next), (cost), s->cycles_left)
#else
#define RT_LOG(next, cost) ((void)0)
#endif

/* Charge an instruction's cycles; the slice ends where the interpreter's
   would, after the instruction (also when the board stopped the CPU). */
#define CY(cost, next) do { \
        int cy_cost_ = (cost);       /* i960_region_switch: evaluate once */ \
        uint32_t cy_next_ = (next); \
        s->cycles_left -= cy_cost_; \
        RT_LOG(cy_next_, cy_cost_); \
        if (RT_UNLIKELY(s->cycles_left < 1 || s->stop) && rt_yield(s, cy_next_)) \
            goto resync; \
    } while (0)
/* ... for an instruction at `at` whose target t (== s->ip) is known at run time */
#define CYD(at, cost, t) do { \
        int c_ = (cost); \
        if (((t) ^ (at)) & 0xffe00000u) \
            c_ = i960_region_switch(s, c_); \
        CY(c_, (t)); \
    } while (0)
#define STEP(next) do { if (rt_step(s) || s->ip != (next)) goto resync; } while (0)

/* ------------------------------------------------------------ memory */

static inline uint8_t rd8(i960_state *s, uint32_t a)
{
    const i960_read_page *p = &s->rpage[a >> 16];
    return p->host ? p->host[a & 0xffff] : p->r8(s->user, a);
}

static inline uint16_t rd16(i960_state *s, uint32_t a)
{
    const i960_read_page *p = &s->rpage[a >> 16];
    uint16_t v;
    if (!p->host)
        return p->r16(s->user, a);
    memcpy(&v, p->host + (a & 0xffff), 2);
    return v;
}

static inline uint32_t rd32(i960_state *s, uint32_t a)
{
    const i960_read_page *p = &s->rpage[a >> 16];
    uint32_t v;
    if (!p->host)
        return p->r32(s->user, a);
    memcpy(&v, p->host + (a & 0xffff), 4);
    return v;
}

static inline void wr8(i960_state *s, uint32_t a, uint8_t v)
{
    const i960_write_page *p = &s->wpage[a >> 16];
    if (p->host) p->host[a & 0xffff] = v; else p->w8(s->user, a, v);
}

static inline void wr16(i960_state *s, uint32_t a, uint16_t v)
{
    const i960_write_page *p = &s->wpage[a >> 16];
    if (p->host) memcpy(p->host + (a & 0xffff), &v, 2); else p->w16(s->user, a, v);
}

static inline void wr32(i960_state *s, uint32_t a, uint32_t v)
{
    const i960_write_page *p = &s->wpage[a >> 16];
    if (p->host) memcpy(p->host + (a & 0xffff), &v, 4); else p->w32(s->user, a, v);
}

/* ------------------------------------------------------ integer ops */

static inline void rt_cc_u(i960_state *s, uint32_t a, uint32_t b)
{
    s->cc = a < b ? 4 : a == b ? 2 : 1;
}

static inline void rt_cc_i(i960_state *s, uint32_t a, uint32_t b)
{
    s->cc = (int32_t)a < (int32_t)b ? 4 : a == b ? 2 : 1;
}

static inline uint32_t rt_shrdi(uint32_t a, uint32_t b)
{
    uint32_t n = a & 0x1f;
    int32_t adj = ((int32_t)b < 0 && (b & ((1u << n) - 1))) ? 1 : 0;
    return (uint32_t)(((int32_t)b >> n) + adj);
}

static inline uint32_t rt_rotate(uint32_t a, uint32_t b)
{
    uint32_t n = a & 0x1f;
    return n ? (b << n) | (b >> (32 - n)) : b;
}

/* division by zero and INT_MIN / -1 give 0 (divi by -1: INT_MIN), i960.c */
static inline int rt_bad_idiv(int32_t n, int32_t d) { return d == 0 || (n == INT32_MIN && d == -1); }

static inline uint32_t rt_divi(uint32_t a, uint32_t b)
{
    int32_t n = (int32_t)b, d = (int32_t)a;
    return rt_bad_idiv(n, d) ? (d ? (uint32_t)n : 0) : (uint32_t)(n / d);
}

static inline uint32_t rt_remi(uint32_t a, uint32_t b)
{
    int32_t n = (int32_t)b, d = (int32_t)a;
    return rt_bad_idiv(n, d) ? 0 : (uint32_t)(n % d);
}

static inline uint32_t rt_modi(uint32_t a, uint32_t b)
{
    int32_t n = (int32_t)b, d = (int32_t)a;
    if (rt_bad_idiv(n, d))
        return 0;
    int32_t r = n % d;
    if (((n ^ d) < 0) && r)
        r += d;
    return (uint32_t)r;
}

/* addc/subc as the original emulator computes them (QUIRKS.md #1, #2) */
static inline uint32_t rt_addc(i960_state *s, uint32_t a, uint32_t b)
{
    uint32_t c1 = ((uint64_t)b + (s->cc & 2)) != 0;
    uint64_t r = (uint64_t)a + c1;
    s->cc &= ~7u;
    if ((r >> 32) & 1)
        s->cc |= 2;
    return (uint32_t)r;
}

static inline uint32_t rt_subc(i960_state *s, uint32_t a, uint32_t b)
{
    uint32_t c = (s->cc >> 1) & 1;
    uint32_t r = b - a - c;
    uint32_t borrow = (uint64_t)b < (uint64_t)a + c;
    int64_t sr = (int64_t)(int32_t)b - (int32_t)a - c;
    uint32_t ov = sr < INT32_MIN || sr > INT32_MAX;
    s->cc = (borrow << 1) | ov;
    return r;
}

static inline int rt_msb(uint32_t v)
{
    int n = 31;
    while (!(v >> n))
        n--;
    return n;
}

static inline uint32_t rt_scanbit(i960_state *s, uint32_t v)
{
    if (!v) { s->cc = 0; return 0xffffffff; }
    s->cc = 2;
    return (uint32_t)rt_msb(v);
}

static inline uint32_t rt_spanbit(i960_state *s, uint32_t v)
{
    return rt_scanbit(s, ~v);
}

static inline uint32_t rt_ac(const i960_state *s) { return ((s->ac ^ s->cc) & 7) ^ s->ac; }

static inline uint32_t rt_modac(i960_state *s, uint32_t mask, uint32_t v)
{
    uint32_t old = rt_ac(s);
    uint32_t nac = (v & mask) | (old & ~mask);
    s->ac = nac;
    s->cc = nac & 7;
    return old;
}

/* ---------------------------------------------------- floating point */

/* operands are read and written through pointers, as the interpreter does:
   a register holds a float, fp0-fp3 a double, a register pair a double */
static inline float    rdF(const void *p) { float v; memcpy(&v, p, 4); return v; }
static inline double   rdD(const void *p) { double v; memcpy(&v, p, 8); return v; }
static inline uint32_t rdU(const void *p) { uint32_t v; memcpy(&v, p, 4); return v; }
static inline void wrF(void *p, float v)   { memcpy(p, &v, 4); }
static inline void wrD(void *p, double v)  { memcpy(p, &v, 8); }
static inline void wrI(void *p, int32_t v) { memcpy(p, &v, 4); }

extern double rt_fp_zero, rt_fp_one;   /* FP literals 0x10 and 0x16 */

/* a float in guest memory */
static inline double rdF_guest(i960_state *s, uint32_t a)
{
    uint32_t w = rd32(s, a);
    float f;
    memcpy(&f, &w, 4);
    return f;
}

#define FPR(x) (x)   /* the port's default build: results kept in double */

static inline void rt_fp_compare(i960_state *s, double a, double b)
{
    if (a < b) s->cc = 4;
    else if (a == b) s->cc = 2;
    else if (a > b) s->cc = 1;
    else s->cc = 0;
}

static inline int32_t rt_ftol(double v)
{
    if (!(v > -2147483649.0 && v < 2147483648.0))
        return INT32_MIN;
    return (int32_t)v;
}

static inline int32_t rt_round_ac(i960_state *s, double v)
{
    switch (rt_ac(s) >> 30) {
    case 0: return rt_ftol(floor(v + 0.5));
    case 1: return rt_ftol(floor(v));
    case 2: return rt_ftol(ceil(v));
    default: return rt_ftol(v);
    }
}

static inline int32_t rt_cvtzri(i960_state *s, double v)
{
    (void)s;
    double x = fabs(v + v) - 0.5;
    int32_t r = !(x >= -2147483648.5 && x < 2147483647.5) ? INT32_MIN : (int32_t)nearbyint(x);
    r >>= 1;
    return v < 0.0 ? -r : r;
}

#endif
