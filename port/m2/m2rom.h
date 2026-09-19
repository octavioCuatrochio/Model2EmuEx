/*
 * Game database and ROM loading for the Model 2 port.
 * The database (gamedb.inc) is generated from the game definitions that
 * EMULATOR.EXE builds at startup (tools/dump_games.py, tools/gen_gamedb.py).
 */
#ifndef M2ROM_H
#define M2ROM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* One ROM-list entry, as in the original (loader at 0x4f7c40):
   type 0: start region arg&0xff, size = offset
   type 1: load file into the current region at offset; size, crc; flags
           byte 0 = bytes per chunk, byte 1 = bytes skipped after each chunk
   type 2: fill: region[arg .. arg+offset) = size & 0xff
   type 3: copy: region[offset .. +size) = region[arg .. +size)
   type 4: option[arg&0xff] = offset
   type 5: call function (not used by any game) */
typedef struct {
    int type;
    const char *file;
    uint32_t arg, offset, size, crc, flags;
} m2_rom_entry;

typedef enum {
    M2_HOOK_NONE,
    M2_HOOK_IDLE_COUNT,   /* after the insn has run 10 times in a frame, stop the i960 */
    M2_HOOK_STOP_AFTER,   /* stop the i960 right after the insn */
    M2_HOOK_UNKNOWN
} m2_hook_kind;

/* One input definition from the original's per-game input list.
   kind: 0 = emulator hotkey, 1 = button, 3 = left/right, 5 = up/down,
         8 = analog (port = analog channel)
   key:  DirectInput scan code; 0x1xx = joystick button, 0xff0001xx = joystick axis */
typedef enum { M2_IN_HOTKEY = 0, M2_IN_BUTTON = 1, M2_IN_LEFTRIGHT = 3, M2_IN_UPDOWN = 5,
               M2_IN_ANALOG = 8 } m2_input_kind;
typedef struct {
    int kind;             /* m2_input_kind, -1 ends the list */
    uint8_t port, mask;   /* active low: pressing clears `mask` in port[port] */
    uint32_t key;
    const char *name;
} m2_input;

typedef struct {
    const char *name;     /* zip name */
    const char *title;
    const char *parent;   /* parent set (clones) or the shared set */
    const char *shared;   /* "model2" */
    const char *board;    /* "Model 2", "Model 2A", ... */
    int width, height;
    float fps;
    m2_hook_kind hook;
    uint32_t hook_addr;
    const m2_rom_entry *roms;
    const m2_input *inputs;
    uint8_t port_defaults[8];   /* input port values with nothing pressed */
    uint32_t input_fn;          /* original's per-game input routine (0 = none):
                                   0x4c9080 Daytona, 0x4cb870 Sega Rally, ... */
} m2_game;

#define M2_REGIONS 32
#define M2_OPTIONS 32

typedef struct {
    uint8_t *ptr[M2_REGIONS];
    uint32_t size[M2_REGIONS];
    uint32_t option[M2_OPTIONS];   /* 0: board type (0=2, 1=2A, 2=2B, 3=2C), 1: SCSP IRQ, 2: drive board */
} m2_regions;

const m2_game *m2_find_game(const char *name);
const m2_game *m2_game_list(int *count);

/* Loads all ROMs of `game` from the zip files in `dirs` (NULL-terminated).
   Missing or bad files are reported through `log`. Returns 0 on success. */
/* How many of the game's ROM files can't be found (0: the set is complete);
   *total gets the number of files. Only looks, reads nothing. */
int  m2_check_roms(const m2_game *game, const char *const *dirs, int *total);
int  m2_load_roms(const m2_game *game, const char *const *dirs, m2_regions *out,
                  void (*log)(const char *msg));
void m2_free_roms(m2_regions *r);

#ifdef __cplusplus
}
#endif

#endif
