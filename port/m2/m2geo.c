/* Model 2 geometrizer and rasterizer front end; see m2geo.h and QUIRKS.md. */
#include "m2geo.h"
#include "m2board.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

static inline float u2f(uint32_t u) { float f; memcpy(&f, &u, 4); return f; }
static inline uint32_t f2u(float f) { uint32_t u; memcpy(&u, &f, 4); return u; }

/* ---------------------------------------------------------- data sources */

typedef struct {            /* 32-bit words: display list, polygon RAM/ROM */
    const uint8_t *base;
    uint32_t words;         /* size in words; reads past the end give 0 */
    uint32_t pos;
} wstream;

static inline uint32_t rd(wstream *s)
{
    uint32_t v = 0;
    if (s->pos < s->words)
        memcpy(&v, s->base + (size_t)s->pos * 4, 4);
    s->pos++;
    return v;
}

static inline float rdf(wstream *s) { return u2f(rd(s)); }

/* texture point / header data: texture RAM (address bit 23) or texture ROM (region 4) */
static uint16_t tex16(const m2_geo *g, const m2_board *b, uint32_t addr)
{
    if (addr & 0x800000)
        return g->tex_ram[addr & 0xffff];
    uint32_t n = b->rom.size[4] / 2;
    if (!n)
        return 0;
    uint16_t v;
    memcpy(&v, b->rom.ptr[4] + (size_t)(addr % n) * 2, 2);
    return v;
}

/* ---------------------------------------------------------- math */

typedef struct { float x, y, z, u, v; } pvert;

static inline void xform_point(const m2_geo *g, pvert *p)
{
    const float *m = g->matrix;
    float x = p->x * m[0] + p->y * m[3] + p->z * m[6] + m[9];
    float y = p->x * m[1] + p->y * m[4] + p->z * m[7] + m[10];
    float z = p->x * m[2] + p->y * m[5] + p->z * m[8] + m[11];
    p->x = x; p->y = y; p->z = z;
}

static inline m2_vec3 xform_vec(const m2_geo *g, m2_vec3 v)
{
    const float *m = g->matrix;
    m2_vec3 r = { v.x * m[0] + v.y * m[3] + v.z * m[6],
                  v.x * m[1] + v.y * m[4] + v.z * m[7],
                  v.x * m[2] + v.y * m[5] + v.z * m[8] };
    return r;
}

static inline float dot3(m2_vec3 a, float x, float y, float z) { return a.x * x + a.y * y + a.z * z; }

/* 1.8.23 float to 4.12 float (MAME, by Aaron Giles) */
static uint16_t float_to_zval(float f, uint32_t z_adjust)
{
    int32_t fpint = (int32_t)f2u(f);
    int32_t exponent = ((fpint >> 23) & 0xff) - (int32_t)((z_adjust >> 23) & 0xff);
    uint32_t mantissa = (uint32_t)fpint & 0x7fffff;
    mantissa += 0x400;
    if (mantissa > 0x7fffff) {
        exponent++;
        mantissa = (mantissa & 0x7fffff) >> 1;
    }
    mantissa >>= 11;
    if (fpint < 0) return 0;
    if (exponent < -12) return 0;
    if (exponent < 0) return (uint16_t)((mantissa | 0x1000) >> -exponent);
    if (exponent < 15) return (uint16_t)(((uint32_t)(exponent + 1) << 12) | mantissa);
    return 0xffff;
}

static int clip_poly(const pvert *in, int n, pvert *out, m2_vec3 pl)
{
    int k = 0;
    float cd = dot3(pl, in[0].x, in[0].y, in[0].z);
    int ci = cd >= 0;
    for (int i = 0; i < n; i++) {
        const pvert *c = &in[i], *nx = &in[(i + 1) % n];
        if (ci)
            out[k++] = *c;
        float nd = dot3(pl, nx->x, nx->y, nx->z);
        int ni = nd >= 0;
        if (ci != ni && !isnan(cd) && !isnan(nd)) {
            float s = (0 - cd) / (nd - cd);
            out[k].x = c->x + (nx->x - c->x) * s;
            out[k].y = c->y + (nx->y - c->y) * s;
            out[k].z = c->z + (nx->z - c->z) * s;
            out[k].u = c->u + (nx->u - c->u) * s;
            out[k].v = c->v + (nx->v - c->v) * s;
            k++;
        }
        cd = nd;
        ci = ni;
    }
    return k;
}

/* Keeps the part of a polygon where a*u + bb*v + c >= 0 */
static int clip_uv(const pvert *in, int n, pvert *out, float a, float bb, float c)
{
    int k = 0;
    float cd = a * in[0].u + bb * in[0].v + c;
    for (int i = 0; i < n; i++) {
        const pvert *cu = &in[i], *nx = &in[(i + 1) % n];
        float nd = a * nx->u + bb * nx->v + c;
        if (cd >= 0)
            out[k++] = *cu;
        if ((cd >= 0) != (nd >= 0)) {
            float s = cd / (cd - nd);
            out[k].x = cu->x + (nx->x - cu->x) * s;
            out[k].y = cu->y + (nx->y - cu->y) * s;
            out[k].z = cu->z + (nx->z - cu->z) * s;
            out[k].u = cu->u + (nx->u - cu->u) * s;
            out[k].v = cu->v + (nx->v - cu->v) * s;
            k++;
        }
        cd = nd;
    }
    return k;
}

/* ---------------------------------------------------------- rasterizer front end */

typedef struct {            /* one object's strip state (MAME: command buffer) */
    uint32_t tpa, tha;      /* texture point / header addresses */
    pvert p0, p1;           /* P0(n-1), P1(n-1) */
} strip;

/* Projects one piece and appends it to the output */
static void emit_piece(m2_geo *g, const pvert *a, int n, const uint16_t th[4], uint8_t luma,
                       uint16_t zval, uint32_t seq)
{
    if (n < 3 || g->npolys >= M2_MAX_POLYS)
        return;
    m2_gpoly *p = &g->polys[g->npolys++];
    float cx = (float)g->center[g->center_sel][0], cy = (float)g->center[g->center_sel][1];
    memcpy(p->th, th, sizeof p->th);
    p->luma = luma;
    p->nverts = (uint8_t)n;
    p->window = (uint8_t)g->cur_window;
    p->zval = zval;
    p->seq = seq;
    for (int i = 0; i < n; i++) {
        float z = a[i].z + 1e-30f;
        /* as MAME model2_3d_project: the CRTC sync registers place the image
           (Daytona, VF2, Sega Rally: x 0 and y 128 for a 0..496 x 128..512 window) */
        p->v[i].x = g->wide_extra + g->xoff + cx + a[i].x / z;
        p->v[i].y = 384.0f - cy + g->yoff - a[i].y / z;
        p->v[i].z = z;
        p->v[i].u = a[i].u;
        p->v[i].v = a[i].v;
    }
}

/* Textured polygons are cut where their texture repeats, so every piece maps
   into one copy of the texture (mirrored copies flipped); the renderer can
   then sample a texture atlas without per-pixel wrapping. u, v in texels. */
#define MAX_SPLIT 256
static void emit(m2_geo *g, const pvert *a, int n, const uint16_t th[4], uint8_t luma, uint16_t zval)
{
    uint32_t seq = g->seq++;
    if (!(th[0] & 0x4000)) {   /* untextured */
        emit_piece(g, a, n, th, luma, zval, seq);
        return;
    }
    float w = (float)(32 << (th[0] & 7)), h = (float)(32 << ((th[0] >> 3) & 7));
    int mx = (th[0] >> 8) & 1, my = (th[0] >> 9) & 1;
    float u0 = a[0].u, u1 = a[0].u, v0 = a[0].v, v1 = a[0].v;
    for (int i = 1; i < n; i++) {
        if (a[i].u < u0) u0 = a[i].u;
        if (a[i].u > u1) u1 = a[i].u;
        if (a[i].v < v0) v0 = a[i].v;
        if (a[i].v > v1) v1 = a[i].v;
    }
    int tu0 = (int)floorf(u0 / w), tu1 = (int)floorf(u1 / w);
    int tv0 = (int)floorf(v0 / h), tv1 = (int)floorf(v1 / h);
    if (u1 == tu1 * w && tu1 > tu0) tu1--;   /* ends exactly on a boundary */
    if (v1 == tv1 * h && tv1 > tv0) tv1--;
    if ((int64_t)(tu1 - tu0 + 1) * (tv1 - tv0 + 1) > MAX_SPLIT) {   /* give up: clamp */
        tu1 = tu0;
        tv1 = tv0;
    }
    pvert col[M2_POLY_VERTS + 4], tmp[M2_POLY_VERTS + 4], piece[M2_POLY_VERTS + 4];
    for (int tu = tu0; tu <= tu1; tu++) {
        int nc = n;
        memcpy(col, a, sizeof(pvert) * (size_t)n);
        if (tu > tu0) nc = clip_uv(col, nc, tmp, 1, 0, -tu * w), memcpy(col, tmp, sizeof(pvert) * (size_t)nc);
        if (tu < tu1) nc = clip_uv(col, nc, tmp, -1, 0, (tu + 1) * w), memcpy(col, tmp, sizeof(pvert) * (size_t)nc);
        if (nc < 3) continue;
        for (int tv = tv0; tv <= tv1; tv++) {
            int np = nc;
            memcpy(piece, col, sizeof(pvert) * (size_t)nc);
            if (tv > tv0) np = clip_uv(piece, np, tmp, 0, 1, -tv * h), memcpy(piece, tmp, sizeof(pvert) * (size_t)np);
            if (tv < tv1) np = clip_uv(piece, np, tmp, 0, -1, (tv + 1) * h), memcpy(piece, tmp, sizeof(pvert) * (size_t)np);
            if (np < 3 || np > M2_POLY_VERTS) continue;
            for (int i = 0; i < np; i++) {
                float lu = piece[i].u - tu * w, lv = piece[i].v - tv * h;
                if (mx && (tu & 1)) lu = w - lu;
                if (my && (tv & 1)) lv = h - lv;
                piece[i].u = lu;
                piece[i].v = lv;
            }
            emit_piece(g, piece, np, th, luma, zval, seq);
        }
    }
}

/* A polygon from the strip: P1(n-1), P0(n-1), P0(n) [, P1(n)] (orig 0x4bb400,
   MAME model2_3d_process_polygon). `lumaword` carries luma << 15 and the
   back-face bit (0x800000). */
static void raster_poly(m2_geo *g, const m2_board *b, strip *s, uint32_t attr, int nv,
                        pvert p2, pvert p3, uint32_t lumaword)
{
    pvert v[4] = { s->p1, s->p0, p2, p3 };
    if (nv == 3)
        p3 = p2;

    float min_z = v[0].z, max_z = v[0].z;
    for (int i = 1; i < nv; i++) {
        if (v[i].z < min_z) min_z = v[i].z;
        if (v[i].z > max_z) max_z = v[i].z;
    }

    for (int i = 0; i < nv; i++) {
        v[i].v = tex16(g, b, s->tpa + i * 2) * 0.125f;       /* 13.3 fixed point */
        v[i].u = tex16(g, b, s->tpa + i * 2 + 1) * 0.125f;
    }
    s->tpa += (uint32_t)nv * 2;

    uint16_t th[4];
    for (int i = 0; i < 4; i++)
        th[i] = tex16(g, b, s->tha + i);
    int32_t tho = (attr >> 12) & 0x1f;
    if (tho & 0x10)
        tho |= -16;
    s->tha += (uint32_t)(tho * 4);

    int cull = 0;
    if (!((attr >> 17) & 1) && (lumaword & 0x800000))   /* single sided, back face */
        cull = 1;
    if (((attr >> 8) & 3) == 0)                          /* link type 0: not drawn */
        cull = 1;
    if (max_z < 0)
        cull = 1;

    float zvalue;
    switch ((attr >> 10) & 3) {
    case 0: zvalue = g->polygon_z; break;
    case 1: zvalue = min_z; break;
    case 2: zvalue = max_z; break;
    default: zvalue = 1e10f; break;
    }
    g->polygon_z = zvalue;

    if (!cull && g->npolys < M2_MAX_POLYS) {
        pvert a[M2_POLY_VERTS], c[M2_POLY_VERTS];
        int n = nv;
        memcpy(a, v, sizeof(pvert) * (size_t)nv);
        for (int k = 0; k < 4 && n > 2; k++) {
            n = clip_poly(a, n, c, g->clip_n[g->center_sel][k]);
            memcpy(a, c, sizeof(pvert) * (size_t)n);
        }
        if (n > 2)
            emit(g, a, n, th, (uint8_t)((lumaword >> 15) & 0xff), float_to_zval(zvalue, g->z_adjust));
    }

    switch ((attr >> 8) & 3) {   /* strip linking */
    case 0: case 2: s->p0 = p2; s->p1 = p3; break;
    case 1: s->p1 = p2; break;
    case 3: s->p0 = p3; break;
    }
}

/* ---------------------------------------------------------- geometrizer */

/* Object data polygons (MAME geo_parse_{np,nn}_{ns,s}; orig 0x4bb400) */
static void parse_object(m2_geo *g, const m2_board *b, wstream *in, uint32_t count, strip *s)
{
    int normals = !(g->mode & 2), specular = g->mode & 1;
    pvert pt[4];   /* untransformed-by-focus copies for normal calculation */

    for (int k = 0; k < 2; k++) {
        pvert p = { rdf(in), rdf(in), rdf(in), 0, 0 };
        xform_point(g, &p);
        pt[k] = p;
        p.x *= g->focus_x;
        p.y *= g->focus_y;
        if (k == 0) s->p0 = p; else s->p1 = p;
    }

    for (uint32_t i = 0; i < count; i++) {
        uint32_t attr = rd(in);
        m2_vec3 nrm = { rdf(in), rdf(in), rdf(in) };
        if ((attr & 3) == 0)
            break;
        nrm = xform_vec(g, nrm);

        pvert p = { rdf(in), rdf(in), rdf(in), 0, 0 };
        xform_point(g, &p);
        pt[2] = p;
        if (!normals) {   /* cross product of (P1-P0) x (P2-P0), normalized */
            float ax = pt[1].x - pt[0].x, ay = pt[1].y - pt[0].y, az = pt[1].z - pt[0].z;
            float bx = pt[2].x - pt[0].x, by = pt[2].y - pt[0].y, bz = pt[2].z - pt[0].z;
            nrm.x = ay * bz - az * by;
            nrm.y = az * bx - ax * bz;
            nrm.z = ax * by - ay * bx;
            float l = sqrtf(nrm.x * nrm.x + nrm.y * nrm.y + nrm.z * nrm.z);
            if (l != 0) { nrm.x /= l; nrm.y /= l; nrm.z /= l; }
        }
        float dotl = nrm.x * g->light.x + nrm.y * g->light.y + nrm.z * g->light.z;
        float dotp = dot3(nrm, p.x, p.y, p.z);
        p.x *= g->focus_x;
        p.y *= g->focus_y;

        uint32_t face = dotp >= 0 ? 0 : 0x100;
        int tp = (attr >> 18) & 0x1f;
        float lum = (dotl * dotp) < 0 ? 0 : fabsf(dotl);
        float spec = 0;
        if (specular) {
            spec = 2 * dotl * nrm.z - g->light.z;
            uint32_t sc = g->texparam[tp].spec_ctrl;
            if (spec < 0 || sc == 0) spec = 0;
            if (sc >> 1) spec *= spec;
            if (sc >> 2) spec *= spec;
            if ((sc + 1) >> 3) spec *= spec;
            spec *= g->texparam[tp].spec_scale;
        }
        lum = lum * g->texparam[tp].diffuse + g->texparam[tp].ambient + spec;
        if (lum < 0) lum = 0;
        if (lum > 255) lum = 255;
        uint32_t luma = (uint32_t)lum + face;

        pvert p3 = p;
        if (attr & 1) {
            pvert q = { rdf(in), rdf(in), rdf(in), 0, 0 };
            xform_point(g, &q);
            pt[3] = q;
            q.x *= g->focus_x;
            q.y *= g->focus_y;
            p3 = q;
        } else {
            in->pos += 3;
            pt[3] = pt[2];
        }
        raster_poly(g, b, s, attr & 0x3ffff, (attr & 1) ? 4 : 3, p, p3, luma << 15);

        if (!normals) {   /* keep the unfocused points linked like the raster's */
            switch ((attr >> 8) & 3) {
            case 0: case 2: pt[0] = pt[2]; pt[1] = pt[3]; break;
            case 1: pt[1] = pt[2]; break;
            case 3: pt[0] = pt[3]; break;
            }
        }
    }
}

static void cmd_object(m2_geo *g, const m2_board *b, uint32_t op, wstream *list)
{
    strip s;
    s.tpa = rd(list);
    s.tha = rd(list);
    uint32_t oba = rd(list), obc = rd(list);
    g->center_sel = (op >> 29) & 3;

    wstream in;
    if (oba & 0x01000000) {
        in.base = (const uint8_t *)g->poly_ram[1]; in.words = 0x8000; in.pos = oba & 0x7fff;
    } else if (oba & 0x00800000) {
        in.base = b->rom.ptr[3]; in.words = b->rom.size[3] / 4;
        in.pos = in.words ? (oba & 0x7fffff) % in.words : 0;
    } else {
        in.base = (const uint8_t *)g->poly_ram[0]; in.words = 0x8000; in.pos = oba & 0x7fff;
    }
    if (obc == 0)
        obc = 0xfffff;
    parse_object(g, b, &in, obc, &s);
}

/* Direct data: already transformed points in the list itself */
static void cmd_direct(m2_geo *g, const m2_board *b, uint32_t op, wstream *in)
{
    strip s;
    s.tpa = rd(in);
    s.tha = rd(in);
    g->center_sel = ((((op >> 23) - 1) >> 6) & 3);
    pvert p = { 0, 0, 0, 0, 0 };
    p.x = u2f(rd(in) & ~0xffu); p.y = u2f(rd(in) & ~0xffu); p.z = u2f(rd(in) & ~0xffu);
    s.p0 = p;
    p.x = u2f(rd(in) & ~0xffu); p.y = u2f(rd(in) & ~0xffu); p.z = u2f(rd(in) & ~0xffu);
    s.p1 = p;
    for (int guard = 0; guard < 0x10000; guard++) {
        uint32_t attr = rd(in);
        if ((attr & 3) == 0)
            break;
        uint32_t lw = rd(in);
        rd(in);   /* distance */
        pvert a = { 0, 0, 0, 0, 0 }, c;
        a.x = u2f(rd(in) & ~0xffu); a.y = u2f(rd(in) & ~0xffu); a.z = u2f(rd(in) & ~0xffu);
        c = a;
        if (attr & 1) {
            c.x = u2f(rd(in) & ~0xffu); c.y = u2f(rd(in) & ~0xffu); c.z = u2f(rd(in) & ~0xffu);
        }
        raster_poly(g, b, &s, attr & 0xffffff, (attr & 1) ? 4 : 3, a, c, lw >> 8);
    }
}

static int32_t sext12(uint32_t v) { v &= 0xfff; return v & 0x800 ? (int32_t)v - 0x1000 : (int32_t)v; }

/* Window: viewport and the four centres (orig 0x4bac20); 12-bit fields
   x = bits 16-27, y = bits 0-11 */
static void cmd_window(m2_geo *g, wstream *in)
{
    uint32_t w[6];
    for (int i = 0; i < 6; i++)
        w[i] = rd(in);
    g->cur_window++;
    g->viewport[0] = sext12(w[0] >> 16);
    g->viewport[1] = sext12(w[0]);
    g->viewport[2] = sext12(w[1] >> 16);
    g->viewport[3] = sext12(w[1]);
    for (int i = 0; i < 4; i++) {
        g->center[i][0] = sext12(w[2 + i] >> 16);
        g->center[i][1] = sext12(w[2 + i]);
        float l = (float)(g->center[i][0] - g->viewport[0]);
        float r = (float)(g->viewport[2] - g->center[i][0]);
        /* widescreen (the original's Model2_SetWideScreen): a window that
           spans the whole screen width sees the extra field of view */
        if (g->wide_fov && g->wide_extra > 0 && g->xoff + g->viewport[0] <= 1 && g->xoff + g->viewport[2] >= 495) {
            l += g->wide_extra;
            r += g->wide_extra;
        }
        float t = (float)(g->viewport[3] - g->center[i][1]);
        float bt = (float)(g->center[i][1] - g->viewport[1]);
        float hl = hypotf(1, l), hr = hypotf(1, r), ht = hypotf(1, t), hb = hypotf(1, bt);
        g->clip_n[i][0] = (m2_vec3){ 1 / hl, 0, l / hl };
        g->clip_n[i][1] = (m2_vec3){ -1 / hr, 0, r / hr };
        g->clip_n[i][2] = (m2_vec3){ 0, -1 / ht, t / ht };
        g->clip_n[i][3] = (m2_vec3){ 0, 1 / hb, bt / hb };
    }
}

static void write_texram(m2_geo *g, uint32_t addr, uint32_t v)
{
    if (addr & 0x800000)
        g->tex_ram[addr & 0xffff] = (uint16_t)v;
    else
        g->log_ram[addr & 0x7fff] = (uint8_t)v;
}

m2_geo *m2geo_create(void)
{
    m2_geo *g = calloc(1, sizeof *g);
    if (!g)
        return NULL;
    g->polys = malloc(sizeof(m2_gpoly) * M2_MAX_POLYS);
    g->order = malloc(sizeof(uint32_t) * M2_MAX_POLYS);
    if (!g->polys || !g->order) {
        m2geo_destroy(g);
        return NULL;
    }
    return g;
}

void m2geo_destroy(m2_geo *g)
{
    if (!g)
        return;
    free(g->polys);
    free(g->order);
    free(g);
}

static const m2_gpoly *sort_base;
static int cmp_draw(const void *a, const void *b)
{
    const m2_gpoly *p = &sort_base[*(const uint32_t *)a], *q = &sort_base[*(const uint32_t *)b];
    if (p->window != q->window) return p->window < q->window ? -1 : 1;   /* later windows on top */
    if (p->zval != q->zval) return p->zval > q->zval ? -1 : 1;           /* far first */
    return p->seq < q->seq ? -1 : p->seq > q->seq;                       /* older first */
}

/* orig 0x4baa60 (and the pre-pass 0x4b9f70) */
void m2geo_run(m2_geo *g, const m2_board *b)
{
    wstream list = { b->bufram, 0x80000 / 4, (b->tgp.reg_803008 & 0x7ffff) / 4 };

    g->npolys = 0;
    g->seq = 0;
    g->xoff = 84.0f + b->hsync;
    g->yoff = 130.0f + b->vsync;
    g->cur_window = 0;
    g->polygon_z = 1e10f;

    for (int ops = 0; ops < 0x8000 && list.pos < list.words; ops++) {
        uint32_t op = rd(&list);
        if (op & 0x80000000) {   /* jump */
            list.pos = (op & 0x7ffff) / 4;
            continue;
        }
        uint32_t cmd = (op >> 23) & 0x1f, n, a;
        switch (cmd) {
        case 0x00: break;
        case 0x01: case 0x11: cmd_object(g, b, op, &list); break;
        case 0x02: case 0x12: cmd_direct(g, b, op, &list); break;
        case 0x03: case 0x13: cmd_window(g, &list); break;
        case 0x04:   /* texture / log data */
            a = rd(&list); n = rd(&list);
            for (uint32_t i = 0; i < n && list.pos < list.words; i++)
                write_texram(g, a++, rd(&list));
            break;
        case 0x05: case 0x15:   /* polygon data */
            a = rd(&list); n = rd(&list);
            for (uint32_t i = 0; i < n && list.pos < list.words; i++, a++)
                g->poly_ram[(a >> 24) & 1][a & 0x7fff] = rd(&list);
            break;
        case 0x06:   /* texture parameters */
            a = (rd(&list) >> 2) & 0x1f; n = rd(&list);
            for (uint32_t i = 0; i < n && list.pos < list.words; i++) {
                uint32_t p = rd(&list);
                g->texparam[a].diffuse = (float)(p & 0xff);
                g->texparam[a].ambient = (float)((p >> 8) & 0xff);
                g->texparam[a].spec_scale = (float)((p >> 16) & 0xff);
                g->texparam[a].spec_ctrl = (p >> 24) & 0xff;
                g->coef[a] = rdf(&list);
                a = (a + 1) & 0x1f;
            }
            break;
        case 0x07: case 0x17: g->mode = rd(&list); break;
        case 0x08: case 0x18: g->z_adjust = (rd(&list) >> 8) << 8; break;
        case 0x09: case 0x19: g->focus_x = rdf(&list); g->focus_y = rdf(&list); break;
        case 0x0a: case 0x1a:
            g->light.x = rdf(&list); g->light.y = rdf(&list); g->light.z = rdf(&list);
            break;
        case 0x0b: case 0x1b:
            for (int i = 0; i < 12; i++) g->matrix[i] = rdf(&list);
            break;
        case 0x0c: case 0x1c:
            for (int i = 9; i < 12; i++) g->matrix[i] = rdf(&list);
            break;
        case 0x0d: rd(&list); rd(&list); break;   /* data memory push: unsupported */
        case 0x0e:                                   /* self test */
            list.pos += 32;
            n = rd(&list);
            list.pos += 3 * n;
            break;
        case 0x0f: case 0x1f: goto done;
        case 0x10: rd(&list); break;
        case 0x14:   /* log data, 4 bytes per word */
            a = rd(&list); n = rd(&list);
            for (uint32_t i = 0; i < n && list.pos < list.words; i++) {
                uint32_t d = rd(&list);
                for (int k = 0; k < 4; k++)
                    write_texram(g, a++, (d >> (8 * k)) & 0xff);
            }
            break;
        case 0x16: g->lod = rdf(&list); break;
        case 0x1d:   /* code upload */
            rd(&list); n = rd(&list);
            list.pos += 3 * n;
            break;
        case 0x1e: rd(&list); break;
        default: break;
        }
    }
done:
    for (int i = 0; i < g->npolys; i++)
        g->order[i] = (uint32_t)i;
    sort_base = g->polys;
    qsort(g->order, (size_t)g->npolys, sizeof(uint32_t), cmp_draw);
}
