/*
 * Model 2 text/tile layers (System 24-style tilemap chip), ported from
 * Model 2 Emulator 1.1a: colour tables 0x4cd450 (end) and 0x4c6b10, tile
 * palette 0x4cbae0, tile decode 0x4c0410/0x452d90, line composer 0x451a90
 * and its line functions 0x451e90-0x4528b0.
 *
 * Pure CPU code: produces two 496x384 RGBA layers that a renderer draws
 * below (all visible pixels) and above (high-priority pixels) the 3D scene.
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

/* layer pixel alpha: 0 = transparent (pen 0), else priority */
#define M2_TILE_LOW  0x80
#define M2_TILE_HIGH 0xff

typedef struct {
    /* the four tilemaps decoded to 512x512 pixels: bit 12 = priority
       (tile bit 15), bits 4-11 = colour (tile bits 7-14), bits 0-3 = pen */
    uint16_t map[4][512 * 512];
    uint32_t col32[0x8000];      /* RGB555 -> 0xAABBGGRR through translation RAM */
    uint32_t pal[0x1000];        /* tile palette, converted each frame */
    uint8_t  remap[256];         /* translation RAM value -> intensity */
    uint8_t  gamma[3][256];      /* R, G, B */
    int      maps_dirty;         /* full re-decode needed (CG written) */
    uint8_t  tile_dirty[4][4096 / 8];
    int      any_tile_dirty;
    int      colours_dirty;      /* translation RAM written */
    int      window_enable;      /* original: tilegen +0x11, always set */
    uint32_t flags;              /* 1/2: layer A has low/high pixels; 4/8: layer B */
    /* output, RGBA in memory order (R, G, B, A); [0] = A (maps 0/1), [1] = B (maps 2/3).
       Draw order is B then A. */
    uint32_t layer[2][M2_SCREEN_W * M2_SCREEN_H];
} m2_tilegen;

void m2tile_init(m2_tilegen *t, float gamma_r, float gamma_g, float gamma_b);
/* notifications, to be called from the board hooks */
void m2tile_tile_written(m2_tilegen *t, uint32_t offset);
void m2tile_cg_written(m2_tilegen *t);
void m2tile_xlat_written(m2_tilegen *t);
/* builds both layers from the board's tile RAM, CG, palette and translation RAM */
void m2tile_render(m2_tilegen *t, const struct m2_board *b);

#ifdef __cplusplus
}
#endif

#endif
