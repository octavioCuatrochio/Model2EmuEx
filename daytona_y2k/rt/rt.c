/* Runtime of the recompiled program: slices, dispatch, interpreter
   fallback, and the board's CPU backend. See rt.h. */
#include "rt.h"
#include "coro.h"
#include "../../port/m2/m2board.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

double rt_fp_zero = 0.0, rt_fp_one = 1.0;

#ifdef RT_LOG_CYCLES
void rt_log_slice(uint32_t next);
void rt_log_yield(uint32_t next, int left, unsigned stop, int idle);
void rt_log_exec(int cycles, int left);
#define RT_LOG_EXEC(c, l) rt_log_exec((c), (l))
#else
#define RT_LOG_EXEC(c, l) ((void)0)
#endif

#define STACK_SIZE (8u << 20)

typedef struct {
    m2_board *b;
    i960_state *s;
    coro *co;
    /* statistics */
    uint64_t interp_steps;
    uint32_t bail_addr[256];
    uint32_t bail_count[256];
} rt_ctx;

static rt_ctx *ctx_of(i960_state *s) { return ((m2_board *)s->user)->cpu_ctx; }

int rt_verbose = -1;

static int verbose(void)
{
    if (rt_verbose < 0)
        rt_verbose = getenv("DAYTONA_RT_LOG") != NULL;
    return rt_verbose;
}

/* ------------------------------------------------------------ slices */

int rt_yield(i960_state *s, uint32_t next)
{
    rt_ctx *c = ctx_of(s);
    uint32_t fp = s->r[31];

#ifdef RT_LOG_CYCLES
    rt_log_yield(next, s->cycles_left, s->stop, ((m2_board *)s->user)->idle_count);
#endif
    s->ip = next;
    coro_yield(c->co);
    if (s->ip == next)
        return 0;
    /* the board entered an interrupt (i960_interrupt pushed our frame, with
       rip = next): run the handler, it returns to next */
    rt_run(s, s->ip, next, fp & ~0x3fu);
    return s->ip != next;
}

int rt_step(i960_state *s)
{
    ctx_of(s)->interp_steps++;
    int cost = i960_step(s);
    RT_LOG(s->ip, cost);
    (void)cost;
    if (s->cycles_left < 1 || s->stop)
        return rt_yield(s, s->ip);
    return 0;
}

static void note_bail(rt_ctx *c, uint32_t ip, const char *why)
{
    for (int i = 0; i < 256; i++) {
        if (c->bail_count[i] && c->bail_addr[i] == ip) {
            c->bail_count[i]++;
            return;
        }
        if (!c->bail_count[i]) {
            c->bail_addr[i] = ip;
            c->bail_count[i] = 1;
            if (verbose())
                fprintf(stderr, "rt: interpreter at %08x (%s)\n", ip, why);
            return;
        }
    }
}

/* entries by address: program ROM and its mirror at 0x220000 */
#define ROM_WORDS (0x40000 / 4)
static rt_fn entry_rom[ROM_WORDS], entry_mirror[0x20000 / 4];

static void build_lookup(void)
{
    static int built;
    if (built)
        return;
    built = 1;
    for (int i = 0; i < rt_nfuncs; i++) {
        uint32_t a = rt_funcs[i].addr;
        if (a < 0x40000)
            entry_rom[a >> 2] = rt_funcs[i].fn;
        else if (a - 0x220000 < 0x20000)
            entry_mirror[(a - 0x220000) >> 2] = rt_funcs[i].fn;
    }
}

rt_fn rt_lookup(uint32_t a)
{
    if (a & 3)
        return NULL;
    if (a < 0x40000)
        return entry_rom[a >> 2];
    if (a - 0x220000 < 0x20000)
        return entry_mirror[(a - 0x220000) >> 2];
    return NULL;
}

void rt_interp(i960_state *s, uint32_t ret_ip, uint32_t ret_fp)
{
    rt_ctx *c = ctx_of(s);
    note_bail(c, s->ip, "no recompiled path");
    i960_region_switch(s, 0);   /* the interpreter's region follows ip */
    while (!(s->ip == ret_ip && s->r[31] == ret_fp)) {
        rt_fn fn = rt_lookup(s->ip);
        if (fn) {
            rt_call(fn, s, ret_ip, ret_fp);
            i960_region_switch(s, 0);
            continue;
        }
        c->interp_steps++;
        i960_step(s);
        if (s->cycles_left < 1 || s->stop)
            rt_yield(s, s->ip);
    }
}

rt_cont rt_resume(i960_state *s, uint32_t ret_ip, uint32_t ret_fp)
{
    if (s->ip == ret_ip && s->r[31] == ret_fp)
        return RT_DONE;
    rt_fn fn = rt_lookup(s->ip);
    if (fn)
        return RT_TAIL(fn);
    rt_interp(s, ret_ip, ret_fp);
    return RT_DONE;
}

#ifdef RT_LOG_CYCLES
static FILE *rt_logf(void);
void rt_log_slice(uint32_t next)
{
    i960_state *s = NULL;
    (void)s;
    fprintf(rt_logf(), "s %08x\n", next);
}
extern long long rt_logged_total;
void rt_log_yield(uint32_t next, int left, unsigned stop, int idle)
{
    fprintf(rt_logf(), "y %08x left %d stop %u idle %d logged %lld\n", next, left, stop, idle, rt_logged_total);
}
void rt_log_exec(int cycles, int left) { fprintf(rt_logf(), "e %d %d\n", cycles, left); }
static FILE *rt_logf(void)
{
    static FILE *f;
    if (!f)
        f = fopen(getenv("RT_LOG_FILE") ? getenv("RT_LOG_FILE") : "rt-cycles.log", "w");
    return f;
}

long long rt_logged_total;

void rt_log_cycle(uint32_t next, int cost, int left)
{
    rt_logged_total += cost;
    fprintf(rt_logf(), "c %08x %d %d\n", next, cost, left);
}
#endif

int rt_charge(i960_state *s, int cycles, uint32_t ip)
{
    s->cycles_left -= cycles;
    RT_LOG(ip, cycles);
    if (s->cycles_left < 1 || s->stop)
        return rt_yield(s, ip);
    return 0;
}

int rt_pay(i960_state *s, uint32_t a, uint32_t b, uint32_t last)
{
    while (a < b) {
        uint16_t w = rt_insn_word(a);
        uint32_t next = a + ((w & 0x8000) ? 8 : 4);
        int cost = w & 0x7fff;
        uint32_t to = next >= b && last ? last : next;

        s->cycles_left -= cost;
        RT_LOG(to, cost);
        if ((s->cycles_left < 1 || s->stop) && rt_yield(s, to))
            return 1;
        a = next;
    }
    return 0;
}

rt_cont rt_return_at(i960_state *s, uint32_t at)
{
    int cost = rt_insn_word(at) & 0x7fff;
    i960_ret(s);
    if ((s->ip ^ at) & 0xffe00000u)
        cost = i960_region_switch(s, cost);
    rt_charge(s, cost, s->ip);
    return RT_DONE;
}

rt_cont rt_return_bx(i960_state *s, uint32_t at, uint32_t target, uint32_t ret_ip, uint32_t ret_fp)
{
    int cost = rt_insn_word(at) & 0x7fff;
    s->ip = target;
    if ((target ^ at) & 0xffe00000u)
        cost = i960_region_switch(s, cost);
    rt_charge(s, cost, target);
    return rt_resume(s, ret_ip, ret_fp);
}

int rt_charge_call(i960_state *s, int cycles, uint32_t site, uint32_t entry)
{
    if ((entry ^ site) & 0xffe00000u) {
        s->ip = entry;
        cycles = i960_region_switch(s, cycles);
    }
    return rt_charge(s, cycles, entry);
}

void rt_run(i960_state *s, uint32_t t, uint32_t ret_ip, uint32_t ret_fp)
{
    rt_fn fn = rt_lookup(t);
    if (fn)
        rt_call(fn, s, ret_ip, ret_fp);
    else
        rt_interp(s, ret_ip, ret_fp);
}

/* ------------------------------------------------------ board backend */

static void co_main(void *p)
{
    rt_ctx *c = p;
    i960_state *s = c->s;
    /* the reset entry never returns; if it did, the interpreter goes on */
    rt_run(s, s->ip, 0xffffffffu, 0xffffffffu);
    for (;;)
        rt_interp(s, 0xffffffffu, 0xffffffffu);
}

/* i960_execute's contract, with the program running in the coroutine */
static int rec_execute(void *p, int cycles)
{
    rt_ctx *c = p;
    i960_state *s = c->s;

    RT_LOG_EXEC(cycles, s->cycles_left);
    s->cycles_left = cycles;
    if (cycles <= 0)
        return 0;
    if (s->stop) {
        s->cycles_left = 0;
        return cycles;
    }
    if (!c->co) {
        c->co = coro_create(co_main, c, STACK_SIZE);
        if (!c->co) {
            fprintf(stderr, "rt: no memory for the coroutine stack\n");
            abort();
        }
    }
    coro_resume(c->co);
    if (s->stop) {
        s->cycles_left = 0;
        return cycles;
    }
    return cycles - s->cycles_left;
}

static void rec_reset(void *p)
{
    /* m2_reset rebuilt the CPU state: start the program over */
    rt_ctx *c = p;
    coro_free(c->co);
    c->co = NULL;
}

static void rec_free(void *p)
{
    rt_ctx *c = p;
    if (verbose()) {
        fprintf(stderr, "rt: %llu instructions interpreted\n", (unsigned long long)c->interp_steps);
        for (int i = 0; i < 256 && c->bail_count[i]; i++)
            fprintf(stderr, "rt:   entered the interpreter at %08x x%u\n", c->bail_addr[i], c->bail_count[i]);
    }
    coro_free(c->co);
    free(c);
}

/* m2_board_created hook: Daytona USA with the program this was built from
   runs recompiled; everything else stays on the interpreter. */
void rt_board_created(m2_board *b)
{
    if (getenv("DAYTONA_INTERP"))
        return;
    if (strcmp(b->game->name, "daytona") || b->rom.size[0] < 0x40000 ||
        (uint32_t)crc32(0, b->rom.ptr[0], 0x40000) != rt_program_crc)
        return;
    rt_ctx *c = calloc(1, sizeof *c);
    if (!c)
        return;
    build_lookup();
    c->b = b;
    c->s = b->cpu;
    b->cpu_ctx = c;
    b->cpu_execute = rec_execute;
    b->cpu_reset = rec_reset;
    b->cpu_free = rec_free;
    if (verbose())
        fprintf(stderr, "rt: Daytona USA runs recompiled (%d functions)\n", rt_nfuncs);
}

__attribute__((constructor)) static void rt_install(void)
{
    m2_board_created = rt_board_created;
}
