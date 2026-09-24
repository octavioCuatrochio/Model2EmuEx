/* A frame's 3D scene as a model; see m2dump.h.

   The geometrizer runs the frame's display list in dump mode (m2geo.h):
   nothing clipped or culled, every polygon whole, in camera space. Each
   object of the list becomes an OBJ object ("o obj012_00801234": its
   number in the list and its polygon data address, so the same model can
   be found again in another frame).

   Textures: each distinct texture (place in texture RAM, size, luma table
   row, colour table row) is decoded once into the atlas, as the renderer
   colours it (m2gl.c's poly_fs: texel -> luma table -> colour table), at
   the brightest polygon luma it's used with in the frame: the model gets
   the textures lit, not each face's shading. Untextured polygons get a
   small swatch of their colour. Translucent texels (15, translucent
   polygons) have alpha 0, mesh polygons (the hardware's checkerboard, as
   shadows) 50%.

   Decals (later polygons lying on earlier ones, which the game keeps on
   top by drawing them after) are lifted a little off what they lie on.

   Materials: opaque, cutout (translucent texels: alpha) and mesh (50%),
   so that only what has alpha gets it: exporters (glTF...) blend a
   material with alpha, drawn without depth, and in the wrong order. Each
   face has its normal, from the list, and the matching winding. */
#include "m2dump.h"
#include "m2board.h"
#include "m2geo.h"

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <zlib.h>
#ifdef _WIN32
#include <direct.h>
#define mkdir_one(p) _mkdir(p)
#else
#define mkdir_one(p) mkdir(p, 0755)
#endif

typedef struct {
    int      textured, translucent, mesh;
    int      ax, ay, w, h;           /* texture: atlas-style place (m2gl.c), size in texels */
    int      luma_row, colour_row;
    int      luma;                   /* brightest polygon luma seen with it */
    int      px, py, pw, ph;         /* its place in our atlas (with a 1-texel border) */
} material;

typedef struct {
    /* welded positions */
    float    *pos;                   /* x, y, z */
    int       npos, cap_pos;
    int32_t  *hash;                  /* open addressing: position index + 1, 0 empty */
    int       hash_cap;
    /* faces, in list order */
    int      *face_n, *face_mat;
    uint32_t *face_obj, *face_addr;
    float    *face_nrm;              /* the list's normal (OBJ axes), 0 if none */
    int       nface, cap_face;
    int      *corner_v;              /* position index */
    float    *corner_uv;             /* texels */
    int       ncorner, cap_corner;
    material *mat;
    int       nmat, cap_mat;
    int       failed;
} scene;

static int grow(void **p, int *cap, int need, size_t size)
{
    if (need <= *cap)
        return 1;
    int n = *cap ? *cap : 256;
    while (n < need)
        n *= 2;
    void *q = realloc(*p, (size_t)n * size);
    if (!q)
        return 0;
    *p = q;
    *cap = n;
    return 1;
}

static uint32_t hash3(const float *v)
{
    uint32_t h = 2166136261u, w[3];
    memcpy(w, v, sizeof w);
    for (int i = 0; i < 3; i++)
        h = (h ^ w[i]) * 16777619u;
    return h ^ (h >> 15);
}

/* the index of a position, added if new */
static int weld(scene *s, float x, float y, float z)
{
    if (s->npos * 2 >= s->hash_cap) {   /* keep the table at most half full */
        int cap = s->hash_cap ? s->hash_cap * 2 : 4096;
        int32_t *h = calloc((size_t)cap, sizeof *h);
        if (!h)
            return -1;
        for (int i = 0; i < s->npos; i++) {
            uint32_t k = hash3(s->pos + i * 3) & (uint32_t)(cap - 1);
            while (h[k])
                k = (k + 1) & (uint32_t)(cap - 1);
            h[k] = i + 1;
        }
        free(s->hash);
        s->hash = h;
        s->hash_cap = cap;
    }
    float v[3] = { x, y, z };
    uint32_t k = hash3(v) & (uint32_t)(s->hash_cap - 1);
    while (s->hash[k]) {
        const float *p = s->pos + (s->hash[k] - 1) * 3;
        if (!memcmp(p, v, sizeof v))
            return s->hash[k] - 1;
        k = (k + 1) & (uint32_t)(s->hash_cap - 1);
    }
    if (!grow((void **)&s->pos, &s->cap_pos, (s->npos + 1) * 3, sizeof(float)))
        return -1;
    memcpy(s->pos + s->npos * 3, v, sizeof v);
    s->hash[k] = ++s->npos;
    return s->npos - 1;
}

static int find_material(scene *s, const material *m)
{
    for (int i = 0; i < s->nmat; i++) {
        const material *o = &s->mat[i];
        if (o->textured == m->textured && o->translucent == m->translucent && o->mesh == m->mesh &&
            o->colour_row == m->colour_row &&
            (!m->textured || (o->ax == m->ax && o->ay == m->ay && o->w == m->w && o->h == m->h &&
                              o->luma_row == m->luma_row)))
            return i;
    }
    if (!grow((void **)&s->mat, &s->cap_mat, s->nmat + 1, sizeof *s->mat))
        return -1;
    s->mat[s->nmat] = *m;
    return s->nmat++;
}

/* space for one more face of n corners, in every array */
static int room(scene *s, int n)
{
    if (s->nface + 1 > s->cap_face) {
        int cap = s->cap_face ? s->cap_face * 2 : 1024;
        void *a = realloc(s->face_n, (size_t)cap * sizeof(int));
        if (a) s->face_n = a;
        void *b = a ? realloc(s->face_mat, (size_t)cap * sizeof(int)) : NULL;
        if (b) s->face_mat = b;
        void *c = b ? realloc(s->face_obj, (size_t)cap * 4) : NULL;
        if (c) s->face_obj = c;
        void *d = c ? realloc(s->face_addr, (size_t)cap * 4) : NULL;
        if (d) s->face_addr = d;
        void *e = d ? realloc(s->face_nrm, (size_t)cap * 3 * sizeof(float)) : NULL;
        if (e) s->face_nrm = e;
        if (!e)
            return 0;
        s->cap_face = cap;
    }
    if (s->ncorner + n > s->cap_corner) {
        int cap = s->cap_corner ? s->cap_corner * 2 : 4096;
        while (cap < s->ncorner + n)
            cap *= 2;
        void *a = realloc(s->corner_v, (size_t)cap * sizeof(int));
        if (a) s->corner_v = a;
        void *b = a ? realloc(s->corner_uv, (size_t)cap * 2 * sizeof(float)) : NULL;
        if (b) s->corner_uv = b;
        if (!b)
            return 0;
        s->cap_corner = cap;
    }
    return 1;
}

static void on_poly(void *user, const m2_geo *g, const m2_gvert *v, int n, const uint16_t th[4], uint8_t luma)
{
    scene *s = user;
    if (s->failed)
        return;
    /* the texture header as m2gl_mesh_build reads it */
    material m;
    memset(&m, 0, sizeof m);
    m.textured = (th[0] >> 14) & 1;
    m.translucent = (th[0] >> 13) & 1;
    m.mesh = (th[0] >> 15) & 1;
    m.colour_row = (th[3] >> 6) & 0x3ff;
    if (m.textured) {
        m.ax = 32 * (th[2] & 0x3f);
        m.ay = 32 * ((th[2] >> 6) & 0x1f) + (th[2] & 0x1000 ? 1024 : 0);
        m.w = 32 << (th[0] & 7);
        m.h = 32 << ((th[0] >> 3) & 7);
        m.luma_row = th[1] & 0xff;
    }
    int mi = find_material(s, &m);
    if (mi < 0 || !room(s, n)) {
        s->failed = 1;
        return;
    }
    if (luma > s->mat[mi].luma)
        s->mat[mi].luma = luma;
    for (int i = 0; i < n; i++) {
        /* OBJ: y up, looking down -z */
        int k = weld(s, v[i].x, v[i].y, -v[i].z);
        if (k < 0) {
            s->failed = 1;
            return;
        }
        s->corner_v[s->ncorner + i] = k;
        s->corner_uv[(s->ncorner + i) * 2] = m.textured ? v[i].u : 0;
        s->corner_uv[(s->ncorner + i) * 2 + 1] = m.textured ? v[i].v : 0;
    }
    s->face_n[s->nface] = n;
    s->face_mat[s->nface] = mi;
    s->face_obj[s->nface] = g->obj;
    s->face_addr[s->nface] = g->obj_addr;
    s->face_nrm[s->nface * 3] = g->dump_has_nrm ? g->dump_nrm.x : 0;
    s->face_nrm[s->nface * 3 + 1] = g->dump_has_nrm ? g->dump_nrm.y : 0;
    s->face_nrm[s->nface * 3 + 2] = g->dump_has_nrm ? -g->dump_nrm.z : 0;
    s->nface++;
    s->ncorner += n;
}

/* ------------------------------------------------------------ decals */

typedef struct { float c[3], n[3], d, size; int first, layer; } face_info;

static void sub3(float *o, const float *a, const float *b) { o[0] = a[0] - b[0]; o[1] = a[1] - b[1]; o[2] = a[2] - b[2]; }
static float dot(const float *a, const float *b) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; }
static void cross(float *o, const float *a, const float *b)
{
    o[0] = a[1] * b[2] - a[2] * b[1];
    o[1] = a[2] * b[0] - a[0] * b[2];
    o[2] = a[0] * b[1] - a[1] * b[0];
}

/* p (in the plane of face f) inside it: the polygons are convex */
static int inside(const scene *s, const face_info *f, int nf, const float *p)
{
    int pos = 0, neg = 0;
    for (int k = 0; k < nf; k++) {
        const float *a = s->pos + s->corner_v[f->first + k] * 3;
        const float *b = s->pos + s->corner_v[f->first + (k + 1) % nf] * 3;
        float e[3], q[3], x[3];
        sub3(e, b, a);
        sub3(q, p, a);
        cross(x, e, q);
        float t = dot(x, f->n);
        if (t > 1e-9f) pos++;
        else if (t < -1e-9f) neg++;
    }
    return !(pos && neg);
}

/* The game draws a decal after what it lies on, and its sorting (depth,
   then drawing order) keeps it on top; a model has only the depth. A
   polygon in the plane of an earlier one (in any object: a car's window
   bars are an object of their own on the glass) and overlapping it goes a
   layer above it, lifted by 0.2% of its object's size a layer, towards the
   side the list's normal says it's seen from. */
static int lift_decals(scene *s)
{
    face_info *fi = malloc(sizeof *fi * (size_t)(s->nface ? s->nface : 1));
    if (!fi)
        return -1;
    for (int i = 0, c = 0; i < s->nface; c += s->face_n[i], i++) {
        face_info *f = &fi[i];
        int n = s->face_n[i];
        memset(f, 0, sizeof *f);
        f->first = c;
        for (int k = 0; k < n; k++) {   /* centroid, Newell normal */
            const float *a = s->pos + s->corner_v[c + k] * 3, *b = s->pos + s->corner_v[c + (k + 1) % n] * 3;
            for (int j = 0; j < 3; j++) f->c[j] += a[j] / (float)n;
            f->n[0] += (a[1] - b[1]) * (a[2] + b[2]);
            f->n[1] += (a[2] - b[2]) * (a[0] + b[0]);
            f->n[2] += (a[0] - b[0]) * (a[1] + b[1]);
        }
        float l = sqrtf(dot(f->n, f->n));
        if (l > 0)
            for (int j = 0; j < 3; j++) f->n[j] /= l;
        f->d = dot(f->n, f->c);
        for (int k = 0; k < n; k++) {
            float e[3];
            sub3(e, s->pos + s->corner_v[c + k] * 3, f->c);
            float r = sqrtf(dot(e, e));
            if (r > f->size) f->size = r;
        }
    }
    int lifted = 0;
    float *step = malloc(sizeof *step * (size_t)(s->nface ? s->nface : 1));
    if (!step) {
        free(fi);
        return -1;
    }
    for (int a = 0; a < s->nface;) {   /* each object's size */
        int b = a;
        while (b < s->nface && s->face_obj[b] == s->face_obj[a])
            b++;
        float lo[3] = { 1e30f, 1e30f, 1e30f }, hi[3] = { -1e30f, -1e30f, -1e30f };
        for (int i = a; i < b; i++)
            for (int k = 0; k < s->face_n[i]; k++)
                for (int j = 0; j < 3; j++) {
                    float v = s->pos[s->corner_v[fi[i].first + k] * 3 + j];
                    if (v < lo[j]) lo[j] = v;
                    if (v > hi[j]) hi[j] = v;
                }
        float diag[3];
        sub3(diag, hi, lo);
        for (int i = a; i < b; i++)
            step[i] = 0.002f * sqrtf(dot(diag, diag));
        a = b;
    }
    {
        for (int j = 1; j < s->nface; j++) {
            face_info *fj = &fi[j];
            if (fj->size == 0)
                continue;
            for (int i = 0; i < j; i++) {
                const face_info *f = &fi[i];
                float e[3];
                if (f->size == 0 || fabsf(dot(f->n, fj->n)) < 0.99f || fi[i].layer + 1 <= fj->layer)
                    continue;
                sub3(e, fj->c, f->c);
                if (dot(e, e) > (f->size + fj->size) * (f->size + fj->size))   /* too far apart */
                    continue;
                float eps = 0.02f * (f->size > fj->size ? f->size : fj->size);
                int flat = 1;
                for (int k = 0; k < s->face_n[j] && flat; k++)
                    flat = fabsf(dot(f->n, s->pos + s->corner_v[fj->first + k] * 3) - f->d) <= eps;
                if (flat && (inside(s, f, s->face_n[i], fj->c) || inside(s, fj, s->face_n[j], f->c)))
                    fj->layer = f->layer + 1;
            }
            if (!fj->layer)
                continue;
            /* seen from the side the list's normal points away from; direct
               data (no normal): towards the camera */
            const float *ln = s->face_nrm + j * 3;
            float dir[3] = { -ln[0], -ln[1], -ln[2] };
            if (dot(ln, ln) == 0) {
                float sgn = dot(fj->n, fj->c) < 0 ? 1.0f : -1.0f;   /* camera at the origin */
                for (int k = 0; k < 3; k++) dir[k] = fj->n[k] * sgn;
            }
            float l = sqrtf(dot(dir, dir));
            if (l == 0)
                continue;
            float k = step[j] * (float)(fj->layer > 8 ? 8 : fj->layer) / l;
            for (int m = 0; m < s->face_n[j]; m++) {
                const float *p = s->pos + s->corner_v[fj->first + m] * 3;
                int v = weld(s, p[0] + dir[0] * k, p[1] + dir[1] * k, p[2] + dir[2] * k);
                if (v < 0) {
                    free(fi);
                    free(step);
                    return -1;
                }
                s->corner_v[fj->first + m] = v;
            }
            lifted++;
        }
    }
    free(fi);
    free(step);
    return lifted;
}

/* ------------------------------------------------------------ colours */

static uint16_t ld16(const uint8_t *p) { uint16_t v; memcpy(&v, p, 2); return v; }

/* the 4-bit texel at atlas-style coordinates (m2gl.c upload_band): sheet
   s = rows 1024 s.., stored as 1024 x 2048 with the right half below */
static int texel(const m2_board *b, int x, int y)
{
    int bank = (y >> 10) & 1, sx = x & 1023, sy = (y & 1023) + (x & 1024);
    const uint8_t *ram = bank ? b->tex1 : b->tex0;
    uint16_t w = ld16(ram + (size_t)(sy >> 1) * 1024 + (sx & ~1));
    int shy = (sy & 1) ? 0 : 8;
    return sx & 1 ? (w >> shy) & 15 : (w >> (shy + 4)) & 15;
}

/* colour table row `row`, column l (0..63), as m2gl.c upload_tables */
static void colour(const m2_board *b, const uint8_t remap[256], const uint8_t gamma[3][256], int row, int l,
                   uint8_t *o)
{
    uint16_t c = ld16(b->pal + (0x1000 + row) * 2) & 0x7fff;
    o[0] = gamma[0][remap[b->xlat[0x0000 + (c & 0x1f) * 0x200 + l * 2]]];
    o[1] = gamma[1][remap[b->xlat[0x4000 + ((c >> 5) & 0x1f) * 0x200 + l * 2]]];
    o[2] = gamma[2][remap[b->xlat[0x8000 + ((c >> 10) & 0x1f) * 0x200 + l * 2]]];
}

/* ------------------------------------------------------------ files */

static void be32(uint8_t *p, uint32_t v) { p[0] = (uint8_t)(v >> 24); p[1] = (uint8_t)(v >> 16); p[2] = (uint8_t)(v >> 8); p[3] = (uint8_t)v; }

static int png_chunk(FILE *f, const char *type, const uint8_t *data, uint32_t n)
{
    uint8_t h[8];
    be32(h, n);
    memcpy(h + 4, type, 4);
    uLong crc = crc32(0, (const Bytef *)type, 4);
    if (n)
        crc = crc32(crc, data, n);
    uint8_t c[4];
    be32(c, (uint32_t)crc);
    return fwrite(h, 1, 8, f) == 8 && (!n || fwrite(data, 1, n, f) == n) && fwrite(c, 1, 4, f) == 4;
}

static int write_png(const char *path, const uint8_t *rgba, int w, int h)
{
    size_t raw_n = (size_t)(w * 4 + 1) * h;
    uint8_t *raw = malloc(raw_n);
    uLongf z_n = compressBound((uLong)raw_n);
    uint8_t *z = malloc(z_n);
    FILE *f = fopen(path, "wb");
    int ok = raw && z && f;
    if (ok) {
        for (int y = 0; y < h; y++) {
            raw[(size_t)y * (w * 4 + 1)] = 0;   /* filter: none */
            memcpy(raw + (size_t)y * (w * 4 + 1) + 1, rgba + (size_t)y * w * 4, (size_t)w * 4);
        }
        ok = compress2(z, &z_n, raw, (uLong)raw_n, 6) == Z_OK;
    }
    if (ok) {
        uint8_t ihdr[13];
        be32(ihdr, (uint32_t)w);
        be32(ihdr + 4, (uint32_t)h);
        ihdr[8] = 8; ihdr[9] = 6; ihdr[10] = ihdr[11] = ihdr[12] = 0;   /* 8-bit RGBA */
        ok = fwrite("\x89PNG\r\n\x1a\n", 1, 8, f) == 8 && png_chunk(f, "IHDR", ihdr, 13) &&
             png_chunk(f, "IDAT", z, (uint32_t)z_n) && png_chunk(f, "IEND", NULL, 0);
    }
    if (f && fclose(f))
        ok = 0;
    free(raw);
    free(z);
    return ok ? 0 : -1;
}

static void make_dirs(const char *dir)
{
    char p[1024];
    snprintf(p, sizeof p, "%s", dir);
    for (char *c = p + 1; *c; c++)
        if (*c == '/') {
            *c = 0;
            mkdir_one(p);
            *c = '/';
        }
    mkdir_one(p);
}

static int by_height(const void *a, const void *b)
{
    const material *const *x = a, *const *y = b;
    return (*y)->ph - (*x)->ph;
}

int m2dump_scene(const m2_geo *live, const m2_board *b, const uint8_t remap[256], const uint8_t gamma[3][256],
                 const char *dir, char *msg, int msg_size)
{
    scene s;
    memset(&s, 0, sizeof s);
    uint8_t *atlas = NULL;
    material **order = NULL;
    FILE *f = NULL;
    int ret = -1;
    char path[1200];

    /* the list again, in dump mode, on a copy of the geometrizer state */
    m2_geo *g = calloc(1, sizeof *g);
    if (!g) {
        snprintf(msg, (size_t)msg_size, "out of memory");
        return -1;
    }
    g->dump = on_poly;
    g->dump_user = &s;
    m2geo_copy_state(g, live);
    float focus_x = live->focus_x, focus_y = live->focus_y;
    /* where m2geo puts the camera's axis on screen (emit_piece, 4:3) */
    float centre_x = live->xoff + (float)live->center[live->center_sel][0];
    float centre_y = 384.0f - (float)live->center[live->center_sel][1] + live->yoff;
    m2geo_run(g, b);
    free(g);
    if (s.failed) {
        snprintf(msg, (size_t)msg_size, "out of memory");
        goto out;
    }
    if (!s.nface) {
        snprintf(msg, (size_t)msg_size, "no 3D in this frame");
        goto out;
    }
    if (lift_decals(&s) < 0)
        goto oom;

    /* the atlas: materials in shelves, tallest first, each with a border
       of its edge texels (so filtering doesn't bleed the neighbours in) */
    long area = 0;
    int max_w = 0;
    order = malloc(sizeof *order * (size_t)s.nmat);
    if (!order)
        goto oom;
    for (int i = 0; i < s.nmat; i++) {
        material *m = &s.mat[i];
        m->pw = (m->textured ? m->w : 4) + 2;
        m->ph = (m->textured ? m->h : 4) + 2;
        area += (long)m->pw * m->ph;
        if (m->pw > max_w) max_w = m->pw;
        order[i] = m;
    }
    qsort(order, (size_t)s.nmat, sizeof *order, by_height);
    int aw = 256;
    while ((long)aw * aw < area * 5 / 4 || aw < max_w)
        aw *= 2;
    int x = 0, y = 0, shelf = 0;
    for (int i = 0; i < s.nmat; i++) {
        material *m = order[i];
        if (x + m->pw > aw) {
            x = 0;
            y += shelf;
            shelf = 0;
        }
        m->px = x;
        m->py = y;
        x += m->pw;
        if (m->ph > shelf) shelf = m->ph;
    }
    int ah = (y + shelf + 3) & ~3;
    atlas = calloc((size_t)aw * ah, 4);
    if (!atlas)
        goto oom;
    for (int i = 0; i < s.nmat; i++) {
        const material *m = &s.mat[i];
        int iw = m->pw - 2, ih = m->ph - 2;
        for (int ty = -1; ty <= ih; ty++)
            for (int tx = -1; tx <= iw; tx++) {
                int cx = tx < 0 ? 0 : tx >= iw ? iw - 1 : tx, cy = ty < 0 ? 0 : ty >= ih ? ih - 1 : ty;
                uint8_t *o = atlas + ((size_t)(m->py + 1 + ty) * aw + (m->px + 1 + tx)) * 4;
                if (!m->textured) {   /* poly_fs, untextured: colour column luma / 4 */
                    colour(b, remap, gamma, m->colour_row, m->luma >> 2, o);
                    o[3] = m->mesh ? 128 : 255;
                    continue;
                }
                int ay = m->ay & 1024, t = texel(b, (m->ax + cx) & 2047, ay | ((m->ay + cy) & 1023));
                int l = b->luma[(size_t)(m->luma_row * 128 + t * 8) * 2];
                l = (int)floorf((float)l * ((float)m->luma / 256.0f));
                if (l > 63) l = 63;
                colour(b, remap, gamma, m->colour_row, l, o);
                o[3] = m->translucent && t == 15 ? 0 : m->mesh ? 128 : 255;
            }
    }

    make_dirs(dir);
    snprintf(path, sizeof path, "%s/scene.png", dir);
    if (write_png(path, atlas, aw, ah)) {
        snprintf(msg, (size_t)msg_size, "cannot write %s: %s", path, strerror(errno));
        goto out;
    }
    snprintf(path, sizeof path, "%s/scene.mtl", dir);
    f = fopen(path, "w");
    if (!f)
        goto cant;
    /* no alpha where there's none to have: an exporter would make a
       material with alpha blended, drawn without depth, in the wrong order */
    fprintf(f, "newmtl opaque\nKa 1 1 1\nKd 1 1 1\nKs 0 0 0\nillum 1\nmap_Kd scene.png\n\n"
               "newmtl cutout\nKa 1 1 1\nKd 1 1 1\nKs 0 0 0\nillum 1\nmap_Kd scene.png\nmap_d scene.png\n\n"
               "newmtl mesh\nKa 1 1 1\nKd 1 1 1\nKs 0 0 0\nillum 1\nd 0.5\nmap_Kd scene.png\n");
    if (fclose(f)) { f = NULL; goto cant; }
    snprintf(path, sizeof path, "%s/scene.obj", dir);
    f = fopen(path, "w");
    if (!f)
        goto cant;
    fprintf(f, "# Model 2 frame, %d polygons, in camera space: the game's camera at the origin looking down -z,\n"
               "# y up. Its lens: a point lands focus * x / depth pixels from the picture's centre\n"
               "# (496 x 384 pixels), focus %g x %g, the centre at pixel %g, %g.\n"
               "mtllib scene.mtl\n", s.nface, focus_x, focus_y, centre_x, centre_y);
    for (int i = 0; i < s.npos; i++)
        fprintf(f, "v %.5g %.5g %.5g\n", s.pos[i * 3], s.pos[i * 3 + 1], s.pos[i * 3 + 2]);
    for (int i = 0, c = 0; i < s.nface; i++) {
        const material *m = &s.mat[s.face_mat[i]];
        for (int k = 0; k < s.face_n[i]; k++, c++) {
            float u, v;
            if (m->textured) {
                u = (float)m->px + 1 + s.corner_uv[c * 2];
                v = (float)m->py + 1 + s.corner_uv[c * 2 + 1];
            } else {
                u = (float)m->px + 3;
                v = (float)m->py + 3;
            }
            fprintf(f, "vt %.6f %.6f\n", u / (float)aw, 1.0f - v / (float)ah);
        }
    }
    /* a normal per face: the list's, which points into the model (the
       polygon is seen from the side it points away from), turned outwards;
       direct data has none: towards the camera. The corners go round it
       anticlockwise, as OBJ expects; the lists have them clockwise (every
       polygon of Daytona USA, Sega Rally and VF2 frames tried), so nearly
       all are turned round. */
    int *flip = calloc((size_t)s.nface, sizeof *flip);
    if (!flip)
        goto oom;
    for (int i = 0, c = 0; i < s.nface; c += s.face_n[i], i++) {
        float nw[3] = { 0, 0, 0 }, ct[3] = { 0, 0, 0 }, out[3];
        int n = s.face_n[i];
        for (int k = 0; k < n; k++) {
            const float *a = s.pos + s.corner_v[c + k] * 3, *b2 = s.pos + s.corner_v[c + (k + 1) % n] * 3;
            nw[0] += (a[1] - b2[1]) * (a[2] + b2[2]);
            nw[1] += (a[2] - b2[2]) * (a[0] + b2[0]);
            nw[2] += (a[0] - b2[0]) * (a[1] + b2[1]);
            for (int j = 0; j < 3; j++) ct[j] += a[j];
        }
        const float *ln = s.face_nrm + i * 3;
        if (dot(ln, ln) > 0) {
            for (int j = 0; j < 3; j++) out[j] = -ln[j];
        } else {
            float sg = dot(nw, ct) < 0 ? 1.0f : -1.0f;   /* camera at the origin */
            for (int j = 0; j < 3; j++) out[j] = nw[j] * sg;
        }
        float l = sqrtf(dot(out, out));
        if (l == 0) {   /* degenerate */
            out[0] = 0; out[1] = 1; out[2] = 0;
        } else {
            for (int j = 0; j < 3; j++) out[j] /= l;
        }
        flip[i] = dot(nw, out) < 0;
        fprintf(f, "vn %.4f %.4f %.4f\n", out[0], out[1], out[2]);
    }
    uint32_t cur_obj = 0;
    int nobj = 0, cur_mtl = -1, flipped = 0;
    static const char *const mtl_name[3] = { "opaque", "cutout", "mesh" };
    for (int i = 0, c = 0; i < s.nface; c += s.face_n[i], i++) {
        const material *m = &s.mat[s.face_mat[i]];
        int mtl = m->mesh ? 2 : m->translucent ? 1 : 0;
        if (i == 0 || s.face_obj[i] != cur_obj) {
            cur_obj = s.face_obj[i];
            nobj++;
            if (s.face_addr[i] == 0xffffffffu)
                fprintf(f, "o obj%03u_direct\n", cur_obj);
            else
                fprintf(f, "o obj%03u_%08x\n", cur_obj, s.face_addr[i]);
            cur_mtl = -1;
        }
        if (mtl != cur_mtl) {
            fprintf(f, "usemtl %s\n", mtl_name[mtl]);
            cur_mtl = mtl;
        }
        fprintf(f, "f");
        int n = s.face_n[i];
        flipped += flip[i];
        for (int k = 0; k < n; k++) {
            int q = flip[i] ? c + (n - k) % n : c + k;   /* reversed, from the same first corner */
            fprintf(f, " %d/%d/%d", s.corner_v[q] + 1, q + 1, i + 1);
        }
        fprintf(f, "\n");
    }
    free(flip);
    if (fclose(f)) { f = NULL; goto cant; }
    f = NULL;
    snprintf(msg, (size_t)msg_size, "3D scene: %d objects, %d polygons (%d turned round), %d textures -> %s/scene.obj",
             nobj, s.nface, flipped, s.nmat, dir);
    ret = 0;
    goto out;
cant:
    snprintf(msg, (size_t)msg_size, "cannot write %s: %s", path, strerror(errno));
    goto out;
oom:
    snprintf(msg, (size_t)msg_size, "out of memory");
out:
    if (f) fclose(f);
    free(order);
    free(atlas);
    free(s.pos); free(s.hash);
    free(s.face_n); free(s.face_mat); free(s.face_obj); free(s.face_addr); free(s.face_nrm);
    free(s.corner_v); free(s.corner_uv);
    free(s.mat);
    return ret;
}
