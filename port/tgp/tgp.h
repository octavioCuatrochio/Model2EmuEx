/*
 * Fujitsu MB86234 "TGP" geometry DSP (Sega Model 2 / 2A), ported from
 * ElSemi's Model 2 Emulator 1.1a (EMULATOR.EXE, interpreter at 0x4a81c0,
 * Model 2/2A copro driver at 0x4c5290-0x4c5850) to portable C99.
 *
 * ElSemi reverse-engineered this instruction set himself; mnemonics are
 * unknown, so the code names things by what they do.
 *
 * The DSP state lives in a byte block laid out exactly like the original's
 * globals (0x10e3ff0-0x10ed0b0). The original never bounds-checks RAM, stack
 * or flag-table indices, so out-of-range accesses land on neighbouring
 * registers; keeping the layout reproduces that. Accesses that would leave
 * the block read 0 and are dropped (QUIRKS.md).
 */
#ifndef TGP_H
#define TGP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Offsets into tgp.blk (original address - 0x10e3ff0). */
enum {
    TGP_BLK_BASE    = 0x10e3ff0,
    TGP_O_PFLAGS    = 0x0000,  /* per-instruction flags, 8 bytes per PC */
    TGP_O_TABLEPTR  = 0x8000,  /* original: pointer to the table ROM (opaque here) */
    TGP_O_A         = 0x8008,  /* multiplier/ALU input A */
    TGP_O_B         = 0x800c,  /* multiplier input B */
    TGP_O_C         = 0x8010,
    TGP_O_D         = 0x8014,  /* accumulator */
    TGP_O_P         = 0x8018,  /* product */
    TGP_O_PC        = 0x801c,  /* u16 */
    TGP_O_STOP      = 0x801e,  /* u8: stalled / stopped */
    TGP_O_REPEAT    = 0x801f,  /* u16: repeat counter for the next instruction */
    TGP_O_STACK     = 0x8021,  /* u16[4] call stack */
    TGP_O_SP        = 0x8029,  /* u8 */
    TGP_O_FLAGS2    = 0x802a,  /* u8 */
    TGP_O_BANK      = 0x802b,  /* u32: external memory bank (register 0x23) */
    TGP_O_SHIFT     = 0x802f,  /* u32: shift count (register 0x1f) */
    TGP_O_R34       = 0x8033,  /* u8 (register 0x34) */
    TGP_O_SR        = 0x8034,  /* u8 status: bit0 Z, bit1 N, bit2 |t24|<=|t25| */
    TGP_O_R         = 0x8035,  /* u32[16]: r0-r3 address registers, r12/r13 loop counters */
    TGP_O_PROGPTR   = 0x8075,  /* original: pointer to program memory (opaque here) */
    TGP_O_RAM       = 0x8079,  /* u32[0x400]: bank A 0-0x1ff, bank B 0x200-0x3ff */
    TGP_O_MODE      = 0x9079,  /* u8: bits 1-2 rounding mode */
    TGP_O_TREG      = 0x9000,  /* table unit register n (0x20-0x2f) is at TREG + 4n */
    TGP_BLK_SIZE    = 0x90c0,
};

#define TGP_FLAG_FIFO_IN   1u    /* instruction reads the input FIFO */
#define TGP_FLAG_FIFO_OUT  2u    /* instruction writes the output FIFO */
#define TGP_FLAG_HLE_ATAN  0x10u /* replace instruction by atan2(B, A) (never set in 1.1a) */

/* FIFO layout matches the original (rd at +0x80, wr at +0x84, free at +0x88). */
typedef struct {
    uint32_t buf[32];
    uint32_t rd;
    uint32_t wr;
    int32_t  free;       /* 32 = empty, 0 = full */
} tgp_fifo;

typedef struct {
    uint8_t blk[TGP_BLK_SIZE];

    uint32_t *prog;             /* program memory (writable: the i960 uploads it) */
    uint32_t  prog_words;
    const uint8_t *table_rom;   /* sin/atan/reciprocal/rsqrt tables, 256 KB */
    uint32_t  table_rom_size;
    uint32_t *ext_ram;          /* shared copro RAM, bank 4 (original 0x5aa700) */
    uint32_t  ext_ram_words;
    const uint32_t *ext_rom;    /* banks 8-15 (original 0x5aa8dc), may be NULL */
    uint32_t  ext_rom_words;
    uint32_t *data_buf;         /* data upload buffer (original 0x5aa8b0) */
    uint32_t  data_buf_words;

    /* Values written into the original's pointer slots (TABLEPTR, PROGPTR)
       by the resets. The port never dereferences them; out-of-range RAM
       reads can see them. The original stored host addresses there. */
    uint32_t table_ptr_value;
    uint32_t prog_ptr_value;

    tgp_fifo in;
    tgp_fifo out;

    /* Model 2/2A copro board registers */
    uint32_t prog_upload_ctrl;  /* 0x980000, bit 31 = uploading */
    uint32_t prog_upload_off;   /* bytes */
    uint32_t data_upload_ctrl;  /* 0x980008 */
    uint32_t data_upload_off;
    uint32_t reg_98000c;
    uint32_t ext_ptr;           /* 0x801008 w / 0x802008 r: byte offset into ext_ram */
    uint32_t reg_803008;
    uint32_t reg_801008_read;
    int      unused_573320;     /* set by driver entry 25, never read */
} tgp;

/* Core */
void tgp_init(tgp *t);
void tgp_reset(tgp *t, uint32_t prog_bytes);     /* orig 0x4a9a80 */
void tgp_run(tgp *t);                             /* orig 0x4a81c0: up to 1000 insns */
void tgp_run_until_stop(tgp *t);                  /* orig 0x4a9bc0 */
void tgp_run_count(tgp *t, int count);            /* tgp_run with another budget */

/* FIFO helpers */
void     tgp_fifo_reset(tgp_fifo *f);
void     tgp_fifo_push(tgp_fifo *f, uint32_t v);  /* orig 0x452d70 */
uint32_t tgp_fifo_pop(tgp_fifo *f);

/* Model 2/2A copro driver (i960 side), orig table at 0x574b88 */
void     tgp_m2_board_reset(tgp *t);                               /* entry 13, TGP part */
void     tgp_m2_port_write32(tgp *t, uint32_t addr, uint32_t v);  /* entry 3: 0x880000-0x887fff */
void     tgp_m2_port_write16(tgp *t, uint32_t addr, uint16_t v);  /* entry 2 */
uint32_t tgp_m2_port_read32(tgp *t, uint32_t addr);               /* entry 6: 0x884000-0x887fff */
uint16_t tgp_m2_port_read16(tgp *t, uint32_t addr);               /* entry 5 */
uint8_t  tgp_m2_port_read8(tgp *t, uint32_t addr);                /* entry 4 */
void     tgp_m2_ctrl_write32(tgp *t, uint32_t addr, uint32_t v);  /* entry 9: 0x80xxxx */
void     tgp_m2_ctrl_write16(tgp *t, uint32_t addr, uint16_t v);  /* entry 8 */
uint32_t tgp_m2_ctrl_read32(tgp *t, uint32_t addr);               /* entry 12 */
void     tgp_m2_upload_write(tgp *t, uint32_t addr, uint32_t v);  /* 0x980000-0x98000c, orig 0x4c79c0 */

#ifdef __cplusplus
}
#endif

#endif
