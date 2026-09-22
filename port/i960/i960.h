/*
 * Intel i960KB CPU core, ported from ElSemi's Sega Model 2 Emulator 1.1a
 * (EMULATOR.EXE, code at 0x4c0000-0x4c5900) to portable C99.
 *
 * The design follows the original: a per-word predecoded instruction cache
 * (three code regions), operands resolved to pointers at decode time, a
 * 128-entry on-chip local register frame cache, and 64 KB page tables for
 * memory with either a direct host pointer or read/write callbacks.
 *
 * Behavioural differences from the original are listed in QUIRKS.md.
 * Nothing here depends on x86, the x87 FPU, Windows or pointer width.
 */
#ifndef I960_H
#define I960_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct i960_state i960_state;
typedef struct i960_insn i960_insn;

typedef uint8_t  (*i960_read8_fn)(void *user, uint32_t addr);
typedef uint16_t (*i960_read16_fn)(void *user, uint32_t addr);
typedef uint32_t (*i960_read32_fn)(void *user, uint32_t addr);
typedef void (*i960_write8_fn)(void *user, uint32_t addr, uint8_t val);
typedef void (*i960_write16_fn)(void *user, uint32_t addr, uint16_t val);
typedef void (*i960_write32_fn)(void *user, uint32_t addr, uint32_t val);

/* One 64 KB page. If `host` is non-NULL the page is plain memory and
   `host` points at the byte for the page's first address; otherwise the
   callbacks are used. */
typedef struct {
    uint8_t *host;
    i960_read8_fn  r8;
    i960_read16_fn r16;
    i960_read32_fn r32;
} i960_read_page;

typedef struct {
    uint8_t *host;
    i960_write8_fn  w8;
    i960_write16_fn w16;
    i960_write32_fn w32;
} i960_write_page;

/* Opcode descriptor (original: 36-byte records at 0x5735e0). */
typedef struct {
    void (*exec)(i960_state *s, i960_insn *in);
    uint8_t  format;    /* 0 REG, 1 MEM, 2 COBR, 3 CTRL */
    uint16_t key;       /* opcode, REG opcodes are (op << 4) | op2 */
    int32_t  cycles;
    uint8_t  fp;        /* operands may name fp0-fp3 / fp literals */
    uint8_t  flags18;   /* unused by the core, kept from the original table */
    uint8_t  flags1c;
    uint8_t  operands;  /* bit0 src1, bit1 src2, bit2 dst, bit3 disp/EA */
    const char *name;
} i960_opdef;

/* Predecoded instruction (original: 0x30-byte cache entries). */
struct i960_insn {
    uint32_t raw;
    int32_t  disp;      /* branch displacement, already minus 4 */
    void    *src1;
    void    *src2;
    void    *dst;       /* also the src register of stores */
    uint32_t (*ea)(i960_state *s, i960_insn *in);
    const i960_opdef *op;
    const i960_opdef *hooked_op;  /* original op while a hook is installed */
    uint32_t mem_disp;
    uint32_t *abase;
    uint32_t *index;
    uint32_t scale;
};

typedef struct {
    i960_insn *cache;
    const uint8_t *words;   /* host memory the region's code is read from */
    uint8_t  valid;
    uint32_t size;          /* bytes */
    uint32_t base;          /* guest address */
} i960_code_region;

#define I960_FRAME_CACHE 128

struct i960_state {
    i960_read_page  rpage[0x10000];
    i960_write_page wpage[0x10000];

    i960_code_region region[3];
    i960_code_region *cur_region;

    /* Register file. The layout from `ip` to `icon` mirrors the original
       (0x200040-0x2000ff) so that multi-word register operands that run
       past g15 land in the same places. */
    uint32_t ip;
    uint32_t r[32];         /* r0-r15 (local), g0-g15 (global) */
    uint32_t reg_pad;
    double   fp[4];         /* fp0-fp3, held as double like the original */
    uint32_t cc;            /* condition code, kept apart from AC */
    uint32_t ac;            /* arithmetic controls (cc bits stale) */
    uint32_t pc;            /* process controls */
    uint32_t sat;           /* system address table */
    uint32_t prcb;          /* processor control block */
    uint32_t icon;          /* interrupt control word (4 vectors) */
    uint32_t iac[4];        /* last IAC message written to 0xff000010 */
    uint8_t  multi_count;   /* words moved by the last ldl/ldt/ldq/stl/stt/stq */
    uint32_t multi_addr;

    /* On-chip local register frame cache. */
    int32_t  fc_index;
    int32_t  fc_count;
    uint32_t fc_addr[I960_FRAME_CACHE];
    uint32_t fc_regs[I960_FRAME_CACHE][16];

    /* Literal operands 0-31, each replicated four times so that
       long/triple/quad instructions read the same value (original 0x5aa8f8). */
    uint32_t lit[32][4];

    int32_t  cycles_left;
    uint32_t stop;
    uint8_t  in_interrupt;
    uint32_t prev_ip;
    i960_insn *cur;         /* entry whose cycle cost is charged (see QUIRKS.md) */

    void *user;                                  /* passed to all callbacks */
    void (*hook)(void *user);                    /* run after hooked insns */
    void (*invalid_opcode)(void *user, uint32_t addr, uint32_t raw);
    /* built with -DI960_TRACE only: called after each instruction, with the
       instruction's address (s->ip is already the next one) */
    void (*trace)(void *user, uint32_t addr, const i960_insn *in);

    i960_insn scratch;      /* used when ip is outside every code region */
};

/* Setup. i960_init clears the state and the page tables; map memory, then
   define code regions, then call i960_reset. */
void i960_init(i960_state *s, void *user);
void i960_free(i960_state *s);

void i960_map_read(i960_state *s, uint32_t start, uint32_t end, uint8_t *host,
                   i960_read8_fn r8, i960_read16_fn r16, i960_read32_fn r32);
void i960_map_write(i960_state *s, uint32_t start, uint32_t end, uint8_t *host,
                    i960_write8_fn w8, i960_write16_fn w16, i960_write32_fn w32);

/* Region 0 must start at the lowest address. Regions are selected by
   ip >> 20 against the bases of regions 1 and 2. The code must be in memory
   mapped with a host pointer. size 0 only sets the region's base, for
   boards that use fewer than three regions. Returns 0 on success. */
int  i960_set_code_region(i960_state *s, int n, uint32_t base, uint32_t size);
void i960_invalidate_code(i960_state *s, int n);

/* Reads the initial boot record at address 0 (SAT, PRCB, first IP). */
void i960_reset(i960_state *s);

/* Runs for up to `cycles` cycles; returns the number actually executed. */
int  i960_execute(i960_state *s, int cycles);

/* For code that runs the guest outside i960_execute (the recompiled
   Daytona USA in ../../daytona_y2k); each does what the interpreter does:
   i960_step       one instruction at ip, as one turn of i960_execute's loop
                   (without the stop check); returns the cycles charged
   i960_call       the call/callx transfer (s->ip = return address on entry)
   i960_ret        the ret instruction; 0 for the return types it ignores
   i960_flushreg   the flushreg instruction
   i960_region_switch  bookkeeping after an instruction moved ip to another
                   2 MB area: returns the cycles to charge (`cycles`, unless a
                   code region was predecoded, see QUIRKS.md) */
int  i960_step(i960_state *s);
void i960_call(i960_state *s, uint32_t target);
int  i960_ret(i960_state *s);
void i960_flushreg(i960_state *s);
int  i960_region_switch(i960_state *s, int cycles);

/* Signals interrupt pin `line` (0-3); the vector comes from ICON. */
void i960_interrupt(i960_state *s, int line);

/* Board-side helpers exposed by the original core. */
void     i960_iac_write(i960_state *s, uint32_t addr, uint32_t val);
void     i960_iac_process(i960_state *s);
void     i960_hook_insn(i960_state *s, uint32_t addr);       /* run op, then s->hook */
void     i960_stop_after_insn(i960_state *s, uint32_t addr); /* run op, then stop */
void     i960_nop_insn(i960_state *s, uint32_t addr);        /* region 0 only */
void     i960_set_stop(i960_state *s, uint32_t stop);
uint32_t i960_get_ac(const i960_state *s);
void     i960_set_ac(i960_state *s, uint32_t ac);

/* Memory access through the page tables, as the CPU sees it. */
uint8_t  i960_read8(i960_state *s, uint32_t addr);
uint16_t i960_read16(i960_state *s, uint32_t addr);
uint32_t i960_read32(i960_state *s, uint32_t addr);
void     i960_write8(i960_state *s, uint32_t addr, uint8_t val);
void     i960_write16(i960_state *s, uint32_t addr, uint16_t val);
void     i960_write32(i960_state *s, uint32_t addr, uint32_t val);

extern const i960_opdef i960_opdefs[];
extern const int i960_opdef_count;

#ifdef __cplusplus
}
#endif

#endif
