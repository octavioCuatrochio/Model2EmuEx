/*
 * Model 2 / 2A geometrizer and rasterizer front end (high level): walks the
 * display list the i960 builds in buffer RAM, transforms, lights, culls,
 * clips and projects the polygons, and returns them in drawing order.
 *
 * Structure follows EMULATOR.EXE's renderer (command tables 0x5733e8 and
 * 0x573340, parser 0x4baa60, object processor 0x4bb400, window 0x4bac20);
 * the field-level details follow MAME's model2 video code (src/mame/sega/
 * model2_v.cpp, BSD-3-Clause, by R. Belmont, Olivier Galibert, ElSemi,
 * Angelo Salese, Matthew Daniels), which documents the same hardware from
 * the 2B manual and ElSemi's Direct3D implementation. See QUIRKS.md.
 */
#ifndef M2GEO_H
#define M2GEO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct m2_board;

typedef struct {
    float x, y;      /* frame pixels, 0..496 (+2 * wide_extra) x 0..384, y down */
    float z;         /* view depth (> 0) */
    float u, v;      /* texel coordinates inside one copy of the texture,
                        0..width x 0..height (repeat and mirror already applied) */
} m2_gvert;

#define M2_POLY_VERTS 12

typedef struct {
    uint16_t th[4];  /* texture header: see m2gl.c */
    uint8_t  luma;   /* polygon luminance 0..255 */
    uint8_t  nverts; /* 3..12 after clipping and texture splitting */
    uint8_t  window;
    uint16_t zval;   /* 4.12 sort key */
    uint32_t seq;
    m2_gvert v[M2_POLY_VERTS];
} m2_gpoly;

#define M2_MAX_POLYS 32768

typedef struct { float x, y, z; } m2_vec3;

struct m2_geo;
/* Dump mode (m2dump.c): every polygon the display list draws, whole, in
   camera space before the perspective divide: x right, y up, z depth, in
   the game's units (the focus scaling taken out); u, v texels inside one
   copy of the texture, as m2_gvert. obj/obj_addr in the m2_geo say which
   object it belongs to. */
typedef void (*m2_geo_dump_fn)(void *user, const struct m2_geo *g, const m2_gvert *v, int n,
                               const uint16_t th[4], uint8_t luma);

typedef struct m2_geo {
    /* geometrizer state */
    float    matrix[12];
    float    focus_x, focus_y;
    m2_vec3  light;
    uint32_t mode;
    float    lod;
    struct { float diffuse, ambient, spec_scale; uint32_t spec_ctrl; } texparam[32];
    float    coef[32];
    uint32_t poly_ram[2][0x8000];   /* slow (0) and fast (1) polygon RAM */
    uint16_t tex_ram[0x10000];      /* texture point/header RAM (command 4) */
    uint8_t  log_ram[0x8000];

    /* rasterizer front end */
    int32_t  viewport[4];
    int32_t  center[4][2];
    m2_vec3  clip_n[4][4];
    int      center_sel;
    int      cur_window;
    float    xoff, yoff;             /* CRTC offsets: 84 + hsync, 130 + vsync */
    /* widescreen, set by the caller before m2geo_run: pixels added on each
       side of the 496-wide frame (0 = 4:3), which shifts everything right by
       this much; with wide_fov, full-width windows also see the extra area */
    float    wide_extra;
    int      wide_fov;
    float    polygon_z;
    uint32_t z_adjust;

    /* output, in drawing order after m2geo_run */
    m2_gpoly *polys;
    uint32_t *order;
    uint32_t *order_tmp;             /* sort scratch */
    int       npolys;
    uint32_t  seq;
    /* the previous run's output: each run swaps these with polys and order,
       so a frame stays drawable while the next one is made */
    m2_gpoly *polys_prev;
    uint32_t *order_prev;

    /* dump mode: with `dump` set, m2geo_run neither clips nor culls (only
       what the list marks as not drawn is left out), outputs nothing to
       draw, and hands every polygon to dump */
    m2_geo_dump_fn dump;
    void     *dump_user;
    uint32_t  obj;        /* objects so far this run (the current one's number) */
    uint32_t  obj_addr;   /* its polygon data address; 0xffffffff: direct data */
    /* the current polygon's normal (camera space, as the list gives it), if
       it has one (not direct data) */
    m2_vec3   dump_nrm;
    int       dump_has_nrm;
} m2_geo;

/* One run's output, back to front: polys[order[i]] for i < npolys. Stays
   valid through the next m2geo_run (not the one after). */
typedef struct {
    const m2_gpoly *polys;
    const uint32_t *order;
    int             npolys;
} m2_geo_frame;

static inline m2_geo_frame m2geo_frame(const m2_geo *g)
{
    m2_geo_frame f = { g->polys, g->order, g->npolys };
    return f;
}

m2_geo *m2geo_create(void);
void    m2geo_destroy(m2_geo *g);
/* Processes this frame's display list; the result is g->polys[g->order[i]]
   for i < g->npolys, back to front. */
void    m2geo_run(m2_geo *g, const struct m2_board *b);
/* dst takes src's geometrizer state (matrices, polygon and texture RAM,
   windows...), keeping its own output buffers and dump settings: to run a
   frame's list again in dump mode without disturbing src. */
void    m2geo_copy_state(m2_geo *dst, const m2_geo *src);

#ifdef __cplusplus
}
#endif

#endif
