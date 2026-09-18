/* Model 2 text/tile layers; see m2tile.h and QUIRKS.md ("Tile layers").
   The layers hold palette indices: composing a line is a copy with
   wrap-around, and the colour lookup happens in the renderer. */
#include "m2tile.h"
#include "m2board.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static uint16_t ld16(const uint8_t *p) { uint16_t v; memcpy(&v, p, 2); return v; }
static uint32_t ld32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }

/* orig end of 0x4cd450: gamma tables from the INI and the translation remap */
void m2tile_init(m2_tilegen *t, float gamma_r, float gamma_g, float gamma_b)
{
    const float g[3] = { gamma_r, gamma_g, gamma_b };
    memset(t, 0, sizeof *t);
    for (int i = 0; i < 256; i++) {
        for (int c = 0; c < 3; c++)
            t->gamma[c][i] = (uint8_t)lrint(pow(i / 256.0, 1.0 / g[c]) * 256.0);
        int v = (255 * i - 0x3fc0) / 0xbf;   /* 64..255 -> 0..255 */
        t->remap[i] = (uint8_t)(v < 1 ? 0 : v);
    }
    t->maps_dirty = 1;
    t->ram_dirty = 1;
    t->colours_dirty = 1;
    t->pal_dirty = 1;
    t->window_enable = 1;
}

/* offset: tile RAM offset; bit 16 = write through the 0x1100000 mirror,
   which the original doesn't re-decode (QUIRKS.md) */
void m2tile_tile_written(m2_tilegen *t, uint32_t offset)
{
    t->ram_dirty = 1;
    if (offset < 0x8000) {   /* the four maps; other areas need no decode */
        uint32_t m = offset >> 13, i = (offset >> 1) & 0xfff;
        t->tile_dirty[m][i >> 3] |= (uint8_t)(1u << (i & 7));
        t->any_tile_dirty = 1;
    }
}

void m2tile_cg_written(m2_tilegen *t) { t->maps_dirty = 1; }
void m2tile_xlat_written(m2_tilegen *t) { t->colours_dirty = 1; }
void m2tile_palette_written(m2_tilegen *t, uint32_t offset)
{
    if ((offset & 0x3fff) < 0x2000)   /* the 4096 tile colours */
        t->pal_dirty = 1;
}

/* orig 0x4c6b10, but keeping 8 bits per channel (the original also keeps a
   4-bit copy for its A4R4G4B4 tile textures, see QUIRKS.md) */
static void build_colours(m2_tilegen *t, const uint8_t *xlat)
{
    for (uint32_t i = 0; i < 0x8000; i++) {
        uint8_t r = t->gamma[0][t->remap[xlat[0x0080 + (i & 0x1f) * 0x200]]];
        uint8_t g = t->gamma[1][t->remap[xlat[0x4080 + ((i >> 5) & 0x1f) * 0x200]]];
        uint8_t b = t->gamma[2][t->remap[xlat[0x8080 + ((i >> 10) & 0x1f) * 0x200]]];
        t->col32[i] = 0xff000000u | (uint32_t)b << 16 | (uint32_t)g << 8 | r;
    }
}

/* orig 0x452d90 for one tile of map m */
static void decode_tile(m2_tilegen *t, const uint8_t *tile, const uint8_t *cg, int m, int i)
{
    uint16_t w = ld16(tile + m * 0x2000 + i * 2);
    uint16_t attr = (uint16_t)((w >> 3) & 0x1ff0);
    const uint8_t *ch = cg + (w & 0x3fff) * 32;
    uint16_t *d = t->map[m] + (i >> 6) * 8 * 512 + (i & 63) * 8;
    for (int y = 0; y < 8; y++, d += 512) {
        uint32_t u = ld32(ch + y * 4);
        d[0] = (uint16_t)(((u >> 12) & 15) | attr);
        d[1] = (uint16_t)(((u >> 8) & 15) | attr);
        d[2] = (uint16_t)(((u >> 4) & 15) | attr);
        d[3] = (uint16_t)((u & 15) | attr);
        d[4] = (uint16_t)(((u >> 28) & 15) | attr);
        d[5] = (uint16_t)(((u >> 24) & 15) | attr);
        d[6] = (uint16_t)(((u >> 20) & 15) | attr);
        d[7] = (uint16_t)(((u >> 16) & 15) | attr);
    }
}

/* orig 0x4c0410; returns 1 if any map pixel may have changed */
static int decode_maps(m2_tilegen *t, const uint8_t *tile, const uint8_t *cg)
{
    int changed = t->maps_dirty || t->any_tile_dirty;
    if (t->maps_dirty) {
        for (int m = 0; m < 4; m++)
            for (int i = 0; i < 4096; i++)
                decode_tile(t, tile, cg, m, i);
    } else if (t->any_tile_dirty) {
        for (int m = 0; m < 4; m++)
            for (int k = 0; k < 4096 / 8; k++)
                if (t->tile_dirty[m][k])
                    for (int j = 0; j < 8; j++)
                        if (t->tile_dirty[m][k] & (1u << j))
                            decode_tile(t, tile, cg, m, k * 8 + j);
    }
    if (t->any_tile_dirty)
        memset(t->tile_dirty, 0, sizeof t->tile_dirty);
    t->any_tile_dirty = 0;
    t->maps_dirty = 0;
    return changed;
}

/* orig 0x452000 / 0x452740: one map, wrapping at 512 */
static void line_copy(uint16_t *out, const uint16_t *row, uint32_t x)
{
    int n1 = 512 - (int)x;   /* pixels before the wrap */
    if (n1 >= M2_SCREEN_W) {
        memcpy(out, row + x, M2_SCREEN_W * 2);
    } else {
        memcpy(out, row + x, (size_t)n1 * 2);
        memcpy(out + n1, row, (size_t)(M2_SCREEN_W - n1) * 2);
    }
}

/* orig 0x451e90 / 0x4525c0: two maps side by side (1024 wide) */
static void line_wide(uint16_t *out, const uint16_t *row, const uint16_t *other, uint32_t x)
{
    if (x & 0x200) {
        const uint16_t *s = row;
        row = other;
        other = s;
        x &= 0x1ff;
    }
    int n1 = 512 - (int)x;
    if (n1 >= M2_SCREEN_W) {
        memcpy(out, row + x, M2_SCREEN_W * 2);
    } else {
        memcpy(out, row + x, (size_t)n1 * 2);
        memcpy(out + n1, other, (size_t)(M2_SCREEN_W - n1) * 2);
    }
}

/* copies n pixels from a 512-wide row starting at x, with wrap */
static inline void copy_wrap(uint16_t *out, const uint16_t *row, uint32_t x, int n)
{
    int n1 = 512 - (int)x;
    if (n1 >= n) {
        memcpy(out, row + x, (size_t)n * 2);
    } else {
        memcpy(out, row + x, (size_t)n1 * 2);
        memcpy(out + n1, row, (size_t)(n - n1) * 2);
    }
}

/* orig 0x452150 / 0x4528b0: per 8-pixel column, a mask bit picks the map;
   both scroll positions advance either way */
static void line_window(uint16_t *out, const uint16_t *row1, uint32_t x1,
                        const uint16_t *row2, uint32_t x2, const uint8_t *mask)
{
    int col = 0;
    while (col < M2_SCREEN_W / 8) {
        /* a run of columns from the same map is one copy */
        int second = (ld16(mask + (col >> 4) * 2) >> (15 - (col & 15))) & 1;
        int end = col + 1;
        while (end < M2_SCREEN_W / 8 &&
               ((ld16(mask + (end >> 4) * 2) >> (15 - (end & 15))) & 1) == second)
            end++;
        int n = (end - col) * 8;
        uint32_t off = (uint32_t)col * 8;
        if (second)
            copy_wrap(out + off, row2, (x2 + off) & 0x1ff, n);
        else
            copy_wrap(out + off, row1, (x1 + off) & 0x1ff, n);
        col = end;
    }
}

/* Row `line` of a map. Lines 512-1023 (1024-tall mode) come from the pair's
   other map; the original indexes past the end of its first map there. */
static const uint16_t *map_row(const m2_tilegen *t, int m, int other, uint32_t line)
{
    return line & 0x200 ? t->map[other] + (line & 0x1ff) * 512 : t->map[m] + line * 512;
}

/* orig 0x451a90 */
static void compose(m2_tilegen *t, const uint8_t *tile)
{
    const uint8_t *r = tile + 0xa000;
    uint16_t R0 = ld16(r), R1 = ld16(r + 2), R2 = ld16(r + 4), R3 = ld16(r + 6);
    uint16_t R4 = ld16(r + 8), R5 = ld16(r + 10), R6 = ld16(r + 12), R7 = ld16(r + 14);

    uint32_t hmaskA = ((R4 & 0x4000u) | 0x3fe0u) >> 5, hmaskB = ((R6 & 0x4000u) | 0x3fe0u) >> 5;
    uint32_t lmaskA = ((R4 & 0x2000u) | 0x1ff0u) >> 4, lmaskB = ((R6 & 0x2000u) | 0x1ff0u) >> 4;
    uint32_t hA = -(uint32_t)R0 & hmaskA, hA2 = -(uint32_t)R1 & 0x1ff;
    uint32_t hB = -(uint32_t)R2 & hmaskB, hB2 = -(uint32_t)R3 & 0x1ff;
    uint32_t lA = R4 & lmaskA, lA2 = R5 & 0x1ff, lB = R6 & lmaskB, lB2 = R7 & 0x1ff;
    int oneA = (R5 & 0x8000) || !t->window_enable;   /* no window: primary map only */
    int oneB = (R7 & 0x8000) || !t->window_enable;
    int offA = R4 & 0x8000, offB = R6 & 0x8000;       /* primary map off */
    int wideA = hmaskA & 0x200, wideB = hmaskB & 0x200;

    /* 1024x1024 is not handled: the original draws nothing at all */
    if ((wideB && (lmaskB & 0x200)) || (wideA && (lmaskA & 0x200))) {
        memset(t->layer, 0, sizeof t->layer);
        return;
    }

    for (uint32_t L = 0; L < M2_SCREEN_H; L++) {
        uint16_t *outA = t->layer[0] + L * M2_SCREEN_W, *outB = t->layer[1] + L * M2_SCREEN_W;

        if (R0 & 0x8000) hA = -(uint32_t)ld16(tile + 0x8000 + 2 * L) & hmaskA;
        if (R2 & 0x8000) hB = -(uint32_t)ld16(tile + 0x8800 + 2 * L) & hmaskB;
        if (R1 & 0x8000) hA2 = -(uint32_t)ld16(tile + 0x8400 + 2 * L) & 0x1ff;
        if (R3 & 0x8000) hB2 = -(uint32_t)ld16(tile + 0x8c00 + 2 * L) & 0x1ff;

        /* layer B: maps 2 (primary) and 3 */
        if (!offB && wideB)
            line_wide(outB, t->map[2] + lB * 512, t->map[3] + lB * 512, hB);
        else if (oneB) {
            if (!offB)
                line_copy(outB, map_row(t, 2, 3, lB), hB);
            else
                memset(outB, 0, M2_SCREEN_W * 2);
        } else if (offB)
            line_copy(outB, t->map[3] + lB2 * 512, hB2);
        else
            line_window(outB, map_row(t, 2, 3, lB), hB, t->map[3] + lB2 * 512, hB2,
                        tile + 0xc000 + (L + 0x200) * 8);

        /* layer A: maps 0 (primary) and 1 */
        if (!offA && wideA)
            line_wide(outA, t->map[0] + lA * 512, t->map[1] + lA * 512, hA);
        else if (oneA) {
            if (!offA)
                line_copy(outA, map_row(t, 0, 1, lA), hA);
            else
                memset(outA, 0, M2_SCREEN_W * 2);
        } else if (offA)
            line_copy(outA, t->map[1] + lA2 * 512, hA2);
        else
            line_window(outA, map_row(t, 0, 1, lA), hA, t->map[1] + lA2 * 512, hA2,
                        tile + 0xc000 + L * 8);

        lB = (lB + 1) & lmaskB;
        lA = (lA + 1) & lmaskA;
        lA2 = (lA2 + 1) & 0x1ff;
        lB2 = (lB2 + 1) & 0x1ff;
    }
}

void m2tile_render(m2_tilegen *t, const m2_board *b)
{
    if (t->colours_dirty) {
        build_colours(t, b->xlat);
        t->colours_dirty = 0;
        t->pal_dirty = 1;
    }
    if (t->pal_dirty) {   /* orig 0x4cbae0 does this every frame */
        for (int i = 0; i < 0x1000; i++)
            t->pal[i] = t->col32[ld16(b->pal + i * 2) & 0x7fff];
        t->pal_dirty = 0;
        t->pal_version++;
    }
    int decoded = decode_maps(t, b->tile, b->cg);
    if (decoded || t->ram_dirty) {
        compose(t, b->tile);
        t->ram_dirty = 0;
        t->layer_version++;
    }
}
