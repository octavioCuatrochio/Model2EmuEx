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

enum { M2_TEX_NEAREST, M2_TEX_BILINEAR, M2_TEX_TRILINEAR };

typedef struct {
    int frame_w;     /* frame width in native pixels: 496 (4:3), 661 (16:9), wider with
                        fill; narrower crops the sides (layers not stretched are cut) */
    int scale;       /* render at frame_w*scale x 384*scale */
    int stretch;     /* m2wide.h M2_STRETCH_*: tile layers drawn over the full width */
    int smooth;      /* linear filtering when scaling to the window */
    int fill;        /* stretch the frame over the whole rectangle (distorts) */
    int tex_filter;  /* 3D textures: M2_TEX_NEAREST (hardware), BILINEAR, TRILINEAR */
    int mesh_blend;  /* mesh polygons as 50% translucency (the original's
                        MeshTransparency=1) instead of the hardware checkerboard */
    float saturation;   /* 1.0 = as the hardware; applied in the final pass */
} m2_view;

/* Sends what changed in the frame's tables (the 3D ones too with `tables`)
   and tile layers to the GPU, for the next m2gl_draw. Clears t's row_dirty. */
void   m2gl_upload(m2_gl *g, const struct m2_board *b, m2_tilegen *t, int tables);
/* A frame's polygons ready to draw: vertex and index arrays built from the
   geometrizer's output (m2gl_mesh_build: no GL, any thread). frame_w: the
   frame's width, as m2_view.frame_w. 0 when out of memory. */
typedef struct m2_mesh m2_mesh;
m2_mesh *m2gl_mesh_create(void);
void     m2gl_mesh_destroy(m2_mesh *m);
int      m2gl_mesh_build(m2_mesh *m, const m2_geo_frame *geo, int frame_w);
/* Draws the uploaded frame into the current framebuffer, letterboxed to the
   frame's aspect (4:3 at 496 wide) in a `width` x `height` window. `mesh`
   may be NULL (no 3D); it must be built for view->frame_w, from a
   geometrizer run with wide_extra = (frame_w - 496) / 2. */
void   m2gl_draw(m2_gl *g, const m2_mesh *mesh, const m2_view *view, int width, int height);
/* The same into the window rectangle x, y, width, height (GL window
   coordinates, origin bottom left); only that rectangle is cleared. For
   several screens in one window (split screen). */
void   m2gl_draw_rect(m2_gl *g, const m2_mesh *mesh, const m2_view *view,
                      int x, int y, int width, int height);

#ifdef __cplusplus
}
#endif

#endif
