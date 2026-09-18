/* Emulation -> renderer frame hand-over; see m2pipe.h. */
#include "m2pipe.h"

#include <stdlib.h>
#include <string.h>

/* sizes the renderers read (the board's buffers may be larger) */
#define TILE_SIZE   0x10000
#define PAL_SIZE    0x4000     /* writes are masked to 0x3fff */
#define XLAT_SIZE   (0x10000 + 4)
#define LUMA_SIZE   (0x20000 + 4)
#define CG_SIZE     0x80000    /* writes are masked to 0x7ffff */
#define TEX_SIZE    (0x100000 + 4)
#define BUF_SIZE    0x80000
#define BAND_SHIFT  14         /* texture RAM in 16 KB bands (32 atlas rows, m2gl.c) */
#define PAL_SHIFT   9          /* palette in 0x200-byte blocks */

typedef struct {               /* notifications of one frame */
    uint8_t  tile_words[0x8000 / 2 / 8];   /* maps area, bit per 16-bit word */
    int      tile_ram;         /* any tile RAM write */
    int      cg, cg_mirror, xlat, luma;
    uint32_t pal_blocks;
    uint64_t tex[2];
} events;

typedef struct {
    events   ev;
    int      all;              /* everything copied and reported */
    uint8_t *tile, *pal, *xlat, *luma, *cg, *bufram;   /* traded with the mirrors */
    uint8_t *tex[2];           /* only the written bands are valid */
    int16_t  hsync, vsync;
    uint32_t reg_803008;
    m2_pipe_frame info;
} slot;

struct m2_pipe {
    /* emulation side */
    m2_hooks next;             /* the board's hooks before attach */
    events   ev;
    int      invalid;
    unsigned wr;
    /* render side */
    unsigned rd;
    m2_board shadow;           /* video memories = the mirrors */
    slot     s[2];
};

/* ------------------------------------------------------------ recording */

static void rec_tile(void *u, uint32_t o)
{
    m2_pipe *p = u;
    p->ev.tile_ram = 1;
    if (o < 0x8000) {   /* direct write to the maps (bit 16 = mirror: no decode) */
        uint32_t w = o >> 1;
        p->ev.tile_words[w >> 3] |= (uint8_t)(1u << (w & 7));
    }
}
static void rec_pal(void *u, uint32_t o) { ((m2_pipe *)u)->ev.pal_blocks |= 1u << ((o & 0x3fff) >> PAL_SHIFT); }
static void rec_xlat(void *u) { ((m2_pipe *)u)->ev.xlat = 1; }
static void rec_tex(void *u, int bank, uint32_t o) { ((m2_pipe *)u)->ev.tex[bank & 1] |= 1ull << ((o >> BAND_SHIFT) & 63); }
static void rec_luma(void *u) { ((m2_pipe *)u)->ev.luma = 1; }
static void rec_cg(void *u) { ((m2_pipe *)u)->ev.cg = 1; }
static void rec_cg_mirror(void *u) { ((m2_pipe *)u)->ev.cg_mirror = 1; }

/* the rest go straight through, on the emulation thread */
static void fwd_sound(void *u, uint8_t c)
{
    m2_pipe *p = u;
    if (p->next.sound_command) p->next.sound_command(p->next.user, c);
}
static void fwd_drive(void *u, int kind, int value)
{
    m2_pipe *p = u;
    if (p->next.drive_command) p->next.drive_command(p->next.user, kind, value);
}
static void fwd_io(void *u, uint32_t a, uint32_t v)
{
    m2_pipe *p = u;
    if (p->next.io_read) p->next.io_read(p->next.user, a, v);
}
static void fwd_slice(void *u)
{
    m2_pipe *p = u;
    if (p->next.slice_done) p->next.slice_done(p->next.user);
}

/* ------------------------------------------------------------ setup */

m2_pipe *m2pipe_create(void)
{
    m2_pipe *p = calloc(1, sizeof *p);
    if (!p)
        return NULL;
    int ok = 1;
#define ALLOC(ptr, size) do { (ptr) = calloc(1, (size)); ok &= (ptr) != NULL; } while (0)
    ALLOC(p->shadow.tile, TILE_SIZE);
    ALLOC(p->shadow.pal, PAL_SIZE);
    ALLOC(p->shadow.xlat, XLAT_SIZE);
    ALLOC(p->shadow.luma, LUMA_SIZE);
    ALLOC(p->shadow.cg, CG_SIZE);
    ALLOC(p->shadow.bufram, BUF_SIZE);
    ALLOC(p->shadow.tex0, TEX_SIZE);
    ALLOC(p->shadow.tex1, TEX_SIZE);
    for (int i = 0; i < 2; i++) {
        slot *s = &p->s[i];
        ALLOC(s->tile, TILE_SIZE);
        ALLOC(s->pal, PAL_SIZE);
        ALLOC(s->xlat, XLAT_SIZE);
        ALLOC(s->luma, LUMA_SIZE);
        ALLOC(s->cg, CG_SIZE);
        ALLOC(s->bufram, BUF_SIZE);
        ALLOC(s->tex[0], TEX_SIZE);
        ALLOC(s->tex[1], TEX_SIZE);
    }
#undef ALLOC
    if (!ok) {
        m2pipe_destroy(p);
        return NULL;
    }
    p->invalid = 1;
    return p;
}

void m2pipe_destroy(m2_pipe *p)
{
    if (!p)
        return;
    free(p->shadow.tile); free(p->shadow.pal); free(p->shadow.xlat); free(p->shadow.luma);
    free(p->shadow.cg); free(p->shadow.bufram); free(p->shadow.tex0); free(p->shadow.tex1);
    for (int i = 0; i < 2; i++) {
        slot *s = &p->s[i];
        free(s->tile); free(s->pal); free(s->xlat); free(s->luma);
        free(s->cg); free(s->bufram); free(s->tex[0]); free(s->tex[1]);
    }
    free(p);
}

void m2pipe_attach(m2_pipe *p, m2_board *b)
{
    p->next = b->hooks;
    m2_hooks h;
    memset(&h, 0, sizeof h);
    h.user = p;
    h.tilemap_written = rec_tile;
    h.palette_written = rec_pal;
    h.xlat_written = rec_xlat;
    h.texture_written = rec_tex;
    h.luma_written = rec_luma;
    h.cg_written = rec_cg;
    h.cg_mirror_written = rec_cg_mirror;
    h.sound_command = fwd_sound;
    h.drive_command = fwd_drive;
    h.io_read = fwd_io;
    h.slice_done = fwd_slice;
    b->hooks = h;

    /* what the renderers read besides the video memories (fixed) */
    p->shadow.game = b->game;
    p->shadow.type = b->type;
    p->shadow.rom = b->rom;
}

void m2pipe_invalidate(m2_pipe *p) { p->invalid = 1; }

/* ------------------------------------------------------------ capture */

void m2pipe_capture(m2_pipe *p, const m2_board *b)
{
    slot *s = &p->s[p->wr];
    p->wr ^= 1;
    s->ev = p->ev;
    memset(&p->ev, 0, sizeof p->ev);
    s->all = p->invalid;
    p->invalid = 0;
    const events *e = &s->ev;

    if (s->all || e->tile_ram) memcpy(s->tile, b->tile, TILE_SIZE);
    if (s->all || e->pal_blocks) memcpy(s->pal, b->pal, PAL_SIZE);
    if (s->all || e->xlat) memcpy(s->xlat, b->xlat, XLAT_SIZE);
    if (s->all || e->luma) memcpy(s->luma, b->luma, LUMA_SIZE);
    if (s->all || e->cg || e->cg_mirror) memcpy(s->cg, b->cg, CG_SIZE);
    memcpy(s->bufram, b->bufram, BUF_SIZE);
    const uint8_t *tex[2] = { b->tex0, b->tex1 };
    for (int k = 0; k < 2; k++) {
        if (s->all) {
            memcpy(s->tex[k], tex[k], TEX_SIZE);
            continue;
        }
        for (uint64_t m = e->tex[k]; m; m &= m - 1) {
            size_t off = (size_t)__builtin_ctzll(m) << BAND_SHIFT;
            memcpy(s->tex[k] + off, tex[k] + off, (size_t)1 << BAND_SHIFT);
        }
    }
    s->hsync = b->hsync;
    s->vsync = b->vsync;
    s->reg_803008 = b->tgp.reg_803008;
    s->info.frame = b->frame;
    m2wide_update(b, &s->info.wide);
}

/* ------------------------------------------------------------ apply */

static void trade(uint8_t **a, uint8_t **b) { uint8_t *t = *a; *a = *b; *b = t; }

const m2_board *m2pipe_apply(m2_pipe *p, const m2_hooks *r, m2_pipe_frame *info)
{
    slot *s = &p->s[p->rd];
    p->rd ^= 1;
    const events *e = &s->ev;
    m2_board *sh = &p->shadow;
    int all = s->all;

    if (all || e->tile_ram) trade(&sh->tile, &s->tile);
    if (all || e->pal_blocks) trade(&sh->pal, &s->pal);
    if (all || e->xlat) trade(&sh->xlat, &s->xlat);
    if (all || e->luma) trade(&sh->luma, &s->luma);
    if (all || e->cg || e->cg_mirror) trade(&sh->cg, &s->cg);
    trade(&sh->bufram, &s->bufram);
    uint8_t *tex[2] = { sh->tex0, sh->tex1 };
    for (int k = 0; k < 2; k++) {
        if (all) {
            memcpy(tex[k], s->tex[k], TEX_SIZE);
            continue;
        }
        for (uint64_t m = e->tex[k]; m; m &= m - 1) {
            size_t off = (size_t)__builtin_ctzll(m) << BAND_SHIFT;
            memcpy(tex[k] + off, s->tex[k] + off, (size_t)1 << BAND_SHIFT);
        }
    }
    sh->hsync = s->hsync;
    sh->vsync = s->vsync;
    sh->tgp.reg_803008 = s->reg_803008;
    sh->frame = s->info.frame;
    if (info)
        *info = s->info;

    /* the frame's notifications, in the consumers' own terms */
    void *u = r->user;
    if (r->tilemap_written) {
        for (uint32_t i = 0; i < sizeof e->tile_words; i++)
            for (unsigned m = e->tile_words[i]; m; m &= m - 1)
                r->tilemap_written(u, (i * 8 + (uint32_t)__builtin_ctz(m)) * 2);
        if (all || e->tile_ram)   /* the rest of tile RAM: no decode needed */
            r->tilemap_written(u, 0x1fffe);
    }
    if (r->cg_written && (all || e->cg)) r->cg_written(u);
    if (r->cg_mirror_written && e->cg_mirror) r->cg_mirror_written(u);
    if (r->xlat_written && (all || e->xlat)) r->xlat_written(u);
    if (r->luma_written && (all || e->luma)) r->luma_written(u);
    if (r->palette_written)
        for (uint32_t m = all ? 0xffffffffu : e->pal_blocks; m; m &= m - 1)
            r->palette_written(u, (uint32_t)__builtin_ctz(m) << PAL_SHIFT);
    if (r->texture_written)
        for (int k = 0; k < 2; k++)
            for (uint64_t m = all ? ~0ull : e->tex[k]; m; m &= m - 1)
                r->texture_written(u, k, (uint32_t)__builtin_ctzll(m) << BAND_SHIFT);
    return sh;
}
