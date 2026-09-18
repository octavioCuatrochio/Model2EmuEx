/* Debug: renders the four System 24-style tilemaps (64x64 tiles of 8x8,
   4 bpp) to PPM files, without scroll or priority. Tile word as in MAME's
   segaic24: code = w & 0x3fff, colour = (w >> 7) & 0xff (overlapping). */
#include "m2board.h"

#include <stdio.h>
#include <stdlib.h>

static uint16_t ld16(const uint8_t *p) { return (uint16_t)(p[0] | p[1] << 8); }

void m2_dump_tilemaps(const m2_board *b, const char *prefix, int nib_order)
{
    static uint8_t img[512 * 512 * 3];
    char path[512];

    for (int layer = 0; layer < 4; layer++) {
        for (int ty = 0; ty < 64; ty++)
            for (int tx = 0; tx < 64; tx++) {
                uint16_t w = ld16(b->tile + 2 * (layer * 0x1000 + ty * 64 + tx));
                uint32_t code = w & 0x3fff, colour = (w >> 7) & 0xff;
                const uint8_t *ch = b->cg + code * 32;
                for (int y = 0; y < 8; y++)
                    for (int x = 0; x < 8; x++) {
                        uint8_t byte = ch[y * 4 + (nib_order & 2 ? (x >> 1) ^ 1 : x >> 1)];
                        int pix = (nib_order & 1) ? (x & 1 ? byte >> 4 : byte & 15)
                                                  : (x & 1 ? byte & 15 : byte >> 4);
                        uint16_t c = pix ? ld16(b->pal + 2 * ((colour * 16 + pix) & 0x1fff)) : 0;
                        uint8_t *o = img + 3 * ((ty * 8 + y) * 512 + tx * 8 + x);
                        o[0] = (uint8_t)((c & 31) << 3);
                        o[1] = (uint8_t)(((c >> 5) & 31) << 3);
                        o[2] = (uint8_t)(((c >> 10) & 31) << 3);
                        if (pix && !c)   /* visible pixel with black colour: show grey */
                            o[0] = o[1] = o[2] = 0x40;
                    }
            }
        snprintf(path, sizeof path, "%s-layer%d.ppm", prefix, layer);
        FILE *f = fopen(path, "wb");
        if (!f)
            continue;
        fprintf(f, "P6\n512 512\n255\n");
        fwrite(img, 1, sizeof img, f);
        fclose(f);
    }
}
