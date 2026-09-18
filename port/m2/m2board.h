/*
 * Sega Model 2 / Model 2A board (i960 + TGP), ported from Model 2 Emulator
 * 1.1a: board init 0x4cd450, frame loop 0x4cc7a0, I/O handlers
 * 0x4c6780-0x4c8840 and 0x4cb480-0x4cc710 of EMULATOR.EXE.
 *
 * Video and sound are not part of this module: memory the renderer reads is
 * exposed as buffers, and the places where the original notifies its
 * renderer or sound CPU call the optional hooks in m2_hooks.
 */
#ifndef M2BOARD_H
#define M2BOARD_H

#include "../i960/i960.h"
#include "../tgp/tgp.h"
#include "m2rom.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *user;
    /* renderer notifications (original: copro driver table entries) */
    void (*tilemap_written)(void *user, uint32_t offset);
    void (*palette_written)(void *user, uint32_t offset);
    void (*xlat_written)(void *user);
    void (*texture_written)(void *user, int bank, uint32_t offset);   /* byte offset in tex0/tex1 */
    void (*luma_written)(void *user);
    void (*cg_written)(void *user);
    /* sound board: command byte from the i960 (Model 2A: SCSP MIDI in) */
    void (*sound_command)(void *user, uint8_t cmd);
    /* drive board (force feedback) commands */
    void (*drive_command)(void *user, int kind, int value);
    /* debugging: every I/O board read (0x1c00000) with the value returned */
    void (*io_read)(void *user, uint32_t addr, uint32_t value);
    /* debugging: called after each of the 210 i960 slices of a frame */
    void (*slice_done)(void *user);
} m2_hooks;

/* 93C46-style serial EEPROM, as in old MAME's eeprom.c (orig 0x4ece50) */
typedef struct {
    int      serial_count;
    char     serial_buffer[40];
    uint32_t data_bits;
    uint32_t read_address;
    int      clock_count;
    int      latch;
    int      reset_line;
    int      clock_line;
    int      sending;
    int      locked;
    int      reset_delay;
    int      wait_start;
    uint8_t  data[128];     /* 64 x 16 bits */
} m2_eeprom;

typedef struct m2_board {
    const m2_game *game;
    int type;                  /* 0 Model 2, 1 Model 2A */
    m2_regions rom;

    /* board memory (original: board+0x203850 ...) */
    uint8_t *ram, *fb, *ram2, *pal, *cg, *tile, *back, *xlat, *luma;
    uint8_t *bufram, *tgp_prog, *tgp_data, *coproram, *tex0, *tex1, *zshared, *scsp;

    i960_state *cpu;
    tgp tgp;
    m2_eeprom eeprom;
    m2_hooks hooks;

    uint32_t irq_pending, irq_enable;   /* 0xe80000 / 0xe80004 */
    uint32_t timer[64];                 /* 0xf00000; 0-3 are the timers */
    uint32_t display_ctrl;              /* 0x10000000 */
    int16_t  hsync, vsync;              /* CRTC sync registers 0x1040000 / 0x1060000 (MAME) */
    uint32_t frame_flags;               /* bit 2 toggles each displayed frame */
    int      idle_count;                /* reads of 0x98000c / hooked insn this frame */
    int      sound_irqs;                /* pending sound IRQs (bit 10) */
    uint8_t  sound_fifo[32];
    uint8_t  sound_rd, sound_wr;
    int      sound_count;
    uint32_t sound_word;
    uint8_t  net_reg0, net_reg2;
    int      net_enabled;
    uint32_t prev_fps;

    /* inputs, updated by the frontend */
    uint8_t  port[8];                   /* orig 0x5aa874 */
    uint8_t  analog[16];                /* orig 0x5aa70c */
    uint8_t  analog_index;              /* orig 0x10ed8fe */
    uint8_t  gun_mux;                   /* orig 0x10ed8ff */
    int      eeprom_select;             /* orig 0x10ed900 */
    uint32_t gun_xy[4];
    uint32_t gun_word;                  /* orig 0x5aa734, read inverted at mux 9 */
    uint32_t reg_574cf0, reg_574cf4, drive_last;
    uint32_t tile_state[4];

    /* the 0x240000 status counter of the original Model 2 */
    uint32_t snd_status_count;

    uint64_t frame;
    uint32_t busy_slices;
} m2_board;

m2_board *m2_create(const m2_game *game, const char *const *rom_dirs,
                    void (*log)(const char *msg));
void m2_destroy(m2_board *b);
void m2_reset(m2_board *b);
void m2_run_frame(m2_board *b);

/* Non-volatile data, in the original's NVDATA\<game>.DAT layout: 64 KB of
   backup RAM followed by the 128-byte EEPROM. Load after m2_reset. */
#define M2_NVRAM_SIZE (0x10000 + 128)
int  m2_nvram_load(m2_board *b, const char *path);
int  m2_nvram_save(const m2_board *b, const char *path);
/* First-boot settings for games whose factory defaults don't boot on a
   standalone machine (Daytona USA: linked cabinet). Returns 1 if applied. */
int  m2_nvram_defaults(m2_board *b);

#ifdef __cplusplus
}
#endif

#endif
