/*
 * Model 2 text/tile layers (System 24-style tilemap chip), ported from
 * Model 2 Emulator 1.1a: colour tables 0x4cd450 (end) and 0x4c6b10, tile
 * palette 0x4cbae0, tile decode 0x4c0410/0x452d90, line composer 0x451a90
 * and its line functions 0x451e90-0x4528b0.
 *
 * Pure CPU code: produces two 496x384 layers of palette indices that a
 * renderer draws below (all visible pixels) and above (high-priority pixels)
 * the 3D scene, looking the colours up in `pal`. Work is only redone when
 * the inputs changed (see the version counters).
 */
#ifndef M2TILE_H
#define M2TILE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct m2_board;

#define M2_SCREEN_W 496
#define M2_SCREEN_H 384

/* layer pixel: bits 0-3 pen (0 = transparent), 4-11 colour, 12 priority
   (1 = drawn above the 3D as well) */
#define M2_TILE_PEN(v)      ((v) & 15)
#define M2_TILE_INDEX(v)    ((v) & 0xfff)
#define M2_TILE_PRIORITY(v) (((v) >> 12) & 1)

typedef struct {
    /* the four tilemaps decoded to 512x512 pixels, same format as the layers */
    uint16_t map[4][512 * 512];
    uint32_t col32[0x8000];      /* RGB555 -> 0xAABBGGRR through translation RAM */
    uint32_t pal[0x1000];        /* tile palette as RGBA (memory order R, G, B, A) */
    uint8_t  remap[256];         /* translation RAM value -> intensity */
    uint8_t  gamma[3][256];      /* R, G, B */
    int      maps_dirty;         /* full re-decode needed (CG written) */
    uint8_t  tile_dirty[4][4096 / 8];
    int      any_tile_dirty;
    int      ram_dirty;          /* tile RAM written: layers must be recomposed */
    int      colours_dirty;      /* translation RAM written */
    int      pal_dirty;          /* tile palette written */
    int      window_enable;      /* original: tilegen +0x11, always set */
    uint32_t layer_version;      /* incremented when `layer` changes */
    uint32_t pal_version;        /* incremented when `pal` changes */
    /* output; [0] = A (maps 0/1), [1] = B (maps 2/3). Draw order is B then A. */
    uint16_t layer[2][M2_SCREEN_W * M2_SCREEN_H];
} m2_tilegen;

void m2tile_init(m2_tilegen *t, float gamma_r, float gamma_g, float gamma_b);
/* notifications, to be called from the board hooks */
void m2tile_tile_written(m2_tilegen *t, uint32_t offset);
void m2tile_cg_written(m2_tilegen *t);
void m2tile_xlat_written(m2_tilegen *t);
void m2tile_palette_written(m2_tilegen *t, uint32_t offset);
/* brings the palette and both layers up to date */
void m2tile_render(m2_tilegen *t, const struct m2_board *b);

#ifdef __cplusplus
}
#endif

#endif
