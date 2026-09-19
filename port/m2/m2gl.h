/*
 * OpenGL renderer for the Model 2 port: the OpenGL ES 2.0 feature set, on
 * desktop OpenGL 2.1+ (Windows, Linux) or OpenGL ES 2 (Android, Mali-400).
 * The GL context is created by the frontend. Draw order as in the original
 * (0x4cbae0): tile layers low pass, 3D polygons, tile layers high pass;
 * rendered at the native 496x384 and scaled to the window.
 */
#ifndef M2GL_H
#define M2GL_H

#include <stdint.h>

#include "m2geo.h"
#include "m2tile.h"
#include "m2wide.h"

#ifdef __cplusplus
extern "C" {
#endif

struct m2_board;
typedef struct m2_gl m2_gl;

/* With the functions loaded (m2glapi_load) and a context current: an
   OpenGL ES 2 context (es = 1) or desktop OpenGL 2.1 or later (es = 0). */
m2_gl *m2gl_create(int es);
void   m2gl_destroy(m2_gl *g);

/* change notifications, from the board hooks */
void   m2gl_texture_written(m2_gl *g, int bank, uint32_t offset);
void   m2gl_luma_written(m2_gl *g);
void   m2gl_palette_written(m2_gl *g, uint32_t offset);
void   m2gl_xlat_written(m2_gl *g);

typedef struct {
    int frame_w;     /* frame width in native pixels: 496 (4:3) up to 661 (16:9) */
    int scale;       /* render at frame_w*scale x 384*scale */
    int stretch;     /* m2wide.h M2_STRETCH_*: tile layers drawn over the full width */
    int smooth;      /* linear filtering when scaling to the window */
    int mesh_blend;  /* mesh polygons as 50% translucency (the original's
                        MeshTransparency=1) instead of the hardware checkerboard */
    float saturation;   /* 1.0 = as the hardware; applied in the final pass */
} m2_view;

/* Draws a frame into the current framebuffer, letterboxed to the frame's
   aspect (4:3 at 496 wide) in a `width` x `height` window. `geo` may be
   NULL (no 3D). The geometrizer must have run with
   wide_extra = (frame_w - 496) / 2. */
void   m2gl_draw(m2_gl *g, const struct m2_board *b, const m2_tilegen *t, const m2_geo *geo,
                 const m2_view *view, int width, int height);

#ifdef __cplusplus
}
#endif

#endif
