/* OpenGL (ES) 2 renderer; see m2gl.h. Polygon colour pipeline as in the hardware
   (and the original's pixel shader, string at 0x51bd58):
     texel (4 bit) -> luma RAM[lumabase + texel*8] * polygon luma / 256
     -> colour translation RAM row of each channel of palette[0x1000 + colorbase]
   Tables live in textures so a frame is one draw call; everything the
   fragment shader computes fits mediump (Mali-400 has no highp there). */
#include "m2gl.h"
#include "m2board.h"

#include "m2glapi.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ATLAS 2048

struct m2_gl {
    GLuint tile_prog, post_prog;
    GLint  u_post_tex, u_post_sat, u_post_x;
    struct {                   /* polygon programs, by texture filter (M2_TEX_*) */
        GLuint prog;
        GLint  u_atlas, u_luma, u_colour, u_scale, u_meshblend;
    } poly[3];
    GLint  u_tile_tex, u_tile_pal, u_tile_hipass, u_tile_x;
    GLuint tile_pal;
    uint32_t layer_version, pal_version;
    int    tiles_valid;
    int    fb_w, fb_h, fb_smooth;
    /* Tile layers and polygon buffers in rings of RING: the CPU writes one
       the GPU has finished with. Mali-400: updating a texture or buffer a
       queued frame still uses makes the driver copy all of it first. */
#define RING 3
    GLuint tex_tiles[RING][2], atlas, luma, colour, fb_tex, fbo;
    uint8_t tile_pending[RING][2][M2_SCREEN_H];   /* rows each copy lacks */
    int     tile_cur;
    GLuint vbo_quad, vbo_poly[RING], ibo_poly[RING];
    GLsizeiptr vbo_cap[RING], ibo_cap[RING];
    int     buf_cur;
    uint8_t band_dirty[2][64];
    int     luma_dirty, colour_dirty, atlas_ready;
    uint8_t *scratch;          /* 1024 x 32 texels */
    uint8_t *table;            /* colour table build: 64 x 1024 RGBA */
};

/* A polygon vertex: 32 bytes. The per-polygon parameters travel as small
   integers; the vertex shader turns them into the same values the fragment
   shader always got. */
typedef struct pvtx {
    float    x, y, z, w;      /* clip space, w = view depth (perspective-correct uv) */
    float    u, v;            /* atlas coordinates */
    uint16_t par[4];          /* luma row, colour row, polygon luma, flags (PF_*) */
    uint16_t rect[4];         /* texture in the atlas, for filtering: x, y (+ 0x8000
                                 when mirrored on that axis), width, height */
} pvtx;
#define PF_TRANSLUCENT 1
#define PF_MESH        2
#define PF_UNTEXTURED  4
#define BATCH_VERTS 65535     /* 16-bit indices */

/* ------------------------------------------------------------ shaders */

static const char *tile_vs =
    "attribute vec2 pos;\n"
    "attribute vec2 uv;\n"
    "uniform vec2 xform;\n"      /* x scale, x offset: where the 496-wide layer lands */
    "varying vec2 v_uv;\n"
    "void main() { v_uv = uv; gl_Position = vec4(pos.x * xform.x + xform.y, pos.y, 0.0, 1.0); }\n";

/* Tile layers hold palette indices (m2tile.h), uploaded as luminance (low
   byte) + alpha (high byte); the palette is a 256x16 texture. As the
   original's alpha test: below the 3D every visible pixel (pen != 0), above
   it only high-priority ones. Everything stays in exact small integers, fine
   for mediump. */
static const char *tile_fs =
    "uniform sampler2D tex;\n"
    "uniform sampler2D pal;\n"
    "uniform float hipass;\n"
    "varying vec2 v_uv;\n"
    "void main() {\n"
    "    vec4 c = texture2D(tex, v_uv);\n"
    "    float lo = floor(c.r * 255.0 + 0.5);\n"
    "    float hi = floor(c.a * 255.0 + 0.5);\n"
    "    if (mod(lo, 16.0) < 0.5) discard;\n"
    "    if (hipass > 0.5 && hi < 15.5) discard;\n"
    "    gl_FragColor = vec4(texture2D(pal, vec2((lo + 0.5) / 256.0, (mod(hi, 16.0) + 0.5) / 16.0)).rgb, 1.0);\n"
    "}\n";

/* final pass to the window: optional saturation boost (not in the original) */
static const char *post_fs =
    "uniform sampler2D tex;\n"
    "uniform float sat;\n"
    "varying vec2 v_uv;\n"
    "void main() {\n"
    "    vec3 c = texture2D(tex, v_uv).rgb;\n"
    "    float l = dot(c, vec3(0.299, 0.587, 0.114));\n"
    "    gl_FragColor = vec4(clamp(mix(vec3(l), c, sat), 0.0, 1.0), 1.0);\n"
    "}\n";

/* par: luma row (texture header 1, low byte), colour row (header 3 bits
   6-15), polygon luma, flags 1 translucent / 2 mesh / 4 untextured, all
   exact integers. Out: v_par = luma table row, colour table row, luma scale
   (textured) or colour column (untextured), translucent; v_par2 = mesh,
   untextured. Vertex shaders are highp, so these are exact. */
static const char *poly_vs =
    "attribute vec4 pos;\n"
    "attribute vec2 uv;\n"
    "attribute vec4 par;\n"
    "varying vec2 v_uv;\n"
    "varying vec4 v_par;\n"
    "varying vec2 v_par2;\n"
    "void main() {\n"
    "    float f = par.w;\n"
    "    float untex = step(3.5, f); f -= 4.0 * untex;\n"
    "    float mesh = step(1.5, f); f -= 2.0 * mesh;\n"
    "    float lz = untex > 0.5 ? (floor(par.z / 4.0) + 0.5) / 64.0 : par.z / 256.0;\n"
    "    v_uv = uv;\n"
    "    v_par = vec4((par.x + 0.5) / 256.0, (par.y + 0.5) / 1024.0, lz, f);\n"
    "    v_par2 = vec2(mesh, untex);\n"
    "    gl_Position = pos;\n"
    "}\n";

static const char *poly_fs =
    "uniform sampler2D atlas;\n"
    "uniform sampler2D lumat;\n"
    "uniform sampler2D colt;\n"
    "uniform float scale;\n"    /* render scale: the mesh stays at native pixels */
    "uniform float meshblend;\n" /* 1: mesh polygons 50% translucent (orig MeshTransparency) */
    "varying vec2 v_uv;\n"
    "varying vec4 v_par;\n"
    "varying vec2 v_par2;\n"
    "void main() {\n"
    "    float alpha = 1.0;\n"
    "    if (v_par2.x > 0.5) {\n"   /* mesh: every other pixel (hardware checker) */
    "        if (meshblend > 0.5) {\n"
    "            alpha = 0.5;\n"
    "        } else {\n"
    "            vec2 f = floor(gl_FragCoord.xy / scale);\n"
    "            if (mod(f.x + f.y, 2.0) > 0.5) discard;\n"
    "        }\n"
    "    }\n"
    "    float col;\n"
    "    if (v_par2.y > 0.5) {\n"
    "        col = v_par.z;\n"
    "    } else {\n"
    "        float t = floor(texture2D(atlas, v_uv).r * 15.0 + 0.5);\n"
    "        if (v_par.w > 0.5 && t > 14.5) discard;\n"
    "        float l = texture2D(lumat, vec2((t * 8.0 + 0.5) / 128.0, v_par.x)).r * 255.0;\n"
    "        l = min(floor(l * v_par.z), 63.0);\n"
    "        col = (l + 0.5) / 64.0;\n"
    "    }\n"
    "    gl_FragColor = vec4(texture2D(colt, vec2(col, v_par.y)).rgb, alpha);\n"
    "}\n";

/* Filtered texturing (bilinear, trilinear). The atlas holds 4-bit texel
   indices, which only become colours through the luma and colour tables, so
   the GPU can't filter them: the shader turns each neighbouring texel into
   its colour and blends those. Texels outside the texture wrap (the texture
   repeats) or clamp (mirrored axes: the next copy starts with the same
   texel), so neighbours in the atlas never bleed in. Translucent texels (15)
   count as transparent; the pixel is dropped when under half is opaque.
   Trilinear does what mipmaps are for (true mipmaps can't be built: the
   colours depend on each polygon's tables): where a pixel covers more than
   one texel (distant or slanted surfaces, from the screen-space derivatives)
   it averages four bilinear samples spread over the pixel's footprint,
   blended in as the footprint grows, so distant textures stop shimmering.
   The texture coordinates need highp: fine on desktop GL, and asked for on
   GLES where the GPU has it (not the Mali-400). */
static const char *poly_filter_vs =
    "attribute vec4 pos;\n"
    "attribute vec2 uv;\n"
    "attribute vec4 par;\n"
    "attribute vec4 rect;\n"
    "varying vec2 v_uv;\n"
    "varying vec4 v_par;\n"
    "varying vec2 v_par2;\n"
    "varying vec4 v_rect;\n"      /* texture origin in the atlas, size (texels) */
    "varying vec2 v_mirror;\n"
    "void main() {\n"
    "    float f = par.w;\n"
    "    float untex = step(3.5, f); f -= 4.0 * untex;\n"
    "    float mesh = step(1.5, f); f -= 2.0 * mesh;\n"
    "    float lz = untex > 0.5 ? (floor(par.z / 4.0) + 0.5) / 64.0 : par.z / 256.0;\n"
    "    v_uv = uv;\n"
    "    v_par = vec4((par.x + 0.5) / 256.0, (par.y + 0.5) / 1024.0, lz, f);\n"
    "    v_par2 = vec2(mesh, untex);\n"
    "    v_mirror = step(32767.5, rect.xy);\n"
    "    v_rect = vec4(rect.xy - 32768.0 * v_mirror, rect.zw);\n"
    "    gl_Position = pos;\n"
    "}\n";

static const char *poly_filter_fs =
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
    "precision highp float;\n"
    "#endif\n"
    "uniform sampler2D atlas;\n"
    "uniform sampler2D lumat;\n"
    "uniform sampler2D colt;\n"
    "uniform float scale;\n"
    "uniform float meshblend;\n"
    "varying vec2 v_uv;\n"
    "varying vec4 v_par;\n"
    "varying vec2 v_par2;\n"
    "varying vec4 v_rect;\n"
    "varying vec2 v_mirror;\n"
    /* one texel (texture-local, whole numbers) as premultiplied colour, coverage */
    "vec4 texel(vec2 p) {\n"
    "    vec2 q = mix(mod(p, v_rect.zw), clamp(p, vec2(0.0), v_rect.zw - 1.0), v_mirror);\n"
    "    float t = floor(texture2D(atlas, (v_rect.xy + q + 0.5) / 2048.0).r * 15.0 + 0.5);\n"
    "    float keep = (v_par.w > 0.5 && t > 14.5) ? 0.0 : 1.0;\n"
    "    float l = texture2D(lumat, vec2((t * 8.0 + 0.5) / 128.0, v_par.x)).r * 255.0;\n"
    "    l = min(floor(l * v_par.z), 63.0);\n"
    "    return vec4(texture2D(colt, vec2((l + 0.5) / 64.0, v_par.y)).rgb * keep, keep);\n"
    "}\n"
    /* bilinear at texture-local position tl (texels) */
    "vec4 bilinear(vec2 tl) {\n"
    "    vec2 p = tl - 0.5;\n"
    "    vec2 i = floor(p);\n"
    "    vec2 f = p - i;\n"
    "    return mix(mix(texel(i), texel(i + vec2(1.0, 0.0)), f.x),\n"
    "               mix(texel(i + vec2(0.0, 1.0)), texel(i + 1.0), f.x), f.y);\n"
    "}\n"
    "void main() {\n"
    "    float alpha = 1.0;\n"
    "    if (v_par2.x > 0.5) {\n"
    "        if (meshblend > 0.5) {\n"
    "            alpha = 0.5;\n"
    "        } else {\n"
    "            vec2 f = floor(gl_FragCoord.xy / scale);\n"
    "            if (mod(f.x + f.y, 2.0) > 0.5) discard;\n"
    "        }\n"
    "    }\n"
    "    vec3 rgb;\n"
    "    if (v_par2.y > 0.5) {\n"
    "        rgb = texture2D(colt, vec2(v_par.z, v_par.y)).rgb;\n"
    "    } else {\n"
    "        vec2 tl = v_uv * 2048.0 - v_rect.xy;\n"
    "        vec4 c = bilinear(tl);\n"
    "#if TRILINEAR\n"
    /* where a pixel covers more than a texel, average four bilinear samples
       over its footprint (dx, dy: how far one pixel steps in the texture) */
    "        vec2 dx = dFdx(tl) * 0.25, dy = dFdy(tl) * 0.25;\n"
    "        float rho = max(length(dx), length(dy)) * 4.0;\n"
    "        if (rho > 1.0) {\n"
    "            vec4 m = 0.25 * (bilinear(tl + dx + dy) + bilinear(tl + dx - dy) +\n"
    "                             bilinear(tl - dx + dy) + bilinear(tl - dx - dy));\n"
    "            c = mix(c, m, clamp(rho - 1.0, 0.0, 1.0));\n"
    "        }\n"
    "#endif\n"
    "        if (c.a < 0.5) discard;\n"
    "        rgb = c.rgb / c.a;\n"
    "    }\n"
    "    gl_FragColor = vec4(rgb, alpha);\n"
    "}\n";

/* The shaders are GLSL ES 1.00 without a version line. OpenGL ES gets
   mediump floats in fragment shaders (Mali-400 has no highp there; vertex
   shaders keep their default highp); desktop OpenGL compiles them as GLSL
   1.20, which has the same language but no precision qualifiers. */
static GLuint compile(int es, GLenum type, const char *extra, const char *src)
{
    /* extra: defines, and #extension lines, which must come before the
       precision statement */
    const char *version = es ? "#version 100\n" : "#version 120\n";
    const char *prec = es && type == GL_FRAGMENT_SHADER ? "precision mediump float;\n" : "";
    const char *parts[4] = { version, extra ? extra : "", prec, src };
    GLuint s = glCreateShader(type);
    GLint ok;
    glShaderSource(s, 4, parts, NULL);
    glCompileShader(s);
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(s, sizeof log, NULL, log);
        fprintf(stderr, "m2gl: shader: %s\n", log);
    }
    return s;
}

static GLuint program_ex(int es, const char *fs_extra, const char *vs, const char *fs,
                         const char *const *attrs, int nattrs)
{
    GLuint p = glCreateProgram();
    GLint ok;
    glAttachShader(p, compile(es, GL_VERTEX_SHADER, NULL, vs));
    glAttachShader(p, compile(es, GL_FRAGMENT_SHADER, fs_extra, fs));
    for (int i = 0; i < nattrs; i++)
        glBindAttribLocation(p, (GLuint)i, attrs[i]);
    glLinkProgram(p);
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(p, sizeof log, NULL, log);
        fprintf(stderr, "m2gl: link: %s\n", log);
        return 0;
    }
    return p;
}

static GLuint program(int es, const char *vs, const char *fs, const char *const *attrs, int nattrs)
{
    return program_ex(es, NULL, vs, fs, attrs, nattrs);
}

static GLuint texture(int w, int h, GLenum fmt, GLenum filter)
{
    GLuint t;
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, (GLint)fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, NULL);
    return t;
}

m2_gl *m2gl_create(int es)
{
    static const char *tile_attrs[] = { "pos", "uv" };
    static const char *poly_attrs[] = { "pos", "uv", "par" };
    m2_gl *g = calloc(1, sizeof *g);
    if (!g)
        return NULL;
    g->scratch = malloc(1024 * 32);
    g->table = malloc(64 * 1024 * 4);
    g->tile_prog = program(es, tile_vs, tile_fs, tile_attrs, 2);
    g->post_prog = program(es, tile_vs, post_fs, tile_attrs, 2);
    g->poly[M2_TEX_NEAREST].prog = program(es, poly_vs, poly_fs, poly_attrs, 3);
    if (!g->scratch || !g->table || !g->tile_prog || !g->poly[0].prog || !g->post_prog) {
        m2gl_destroy(g);
        return NULL;
    }
    g->u_tile_tex = glGetUniformLocation(g->tile_prog, "tex");
    g->u_tile_pal = glGetUniformLocation(g->tile_prog, "pal");
    g->u_tile_hipass = glGetUniformLocation(g->tile_prog, "hipass");
    g->u_tile_x = glGetUniformLocation(g->tile_prog, "xform");
    g->u_post_tex = glGetUniformLocation(g->post_prog, "tex");
    g->u_post_sat = glGetUniformLocation(g->post_prog, "sat");
    g->u_post_x = glGetUniformLocation(g->post_prog, "xform");
    /* filtered texturing; without it (or without derivatives, for trilinear)
       the next simpler filter is used */
    static const char *filter_attrs[] = { "pos", "uv", "par", "rect" };
    g->poly[M2_TEX_BILINEAR].prog = program_ex(es, "#define TRILINEAR 0\n", poly_filter_vs, poly_filter_fs,
                                               filter_attrs, 4);
    g->poly[M2_TEX_TRILINEAR].prog = program_ex(es, es ? "#extension GL_OES_standard_derivatives : enable\n"
                                                        "#define TRILINEAR 1\n" : "#define TRILINEAR 1\n",
                                                poly_filter_vs, poly_filter_fs, filter_attrs, 4);
    for (int f = 0; f < 3; f++) {
        GLuint pr = g->poly[f].prog;
        if (!pr)
            continue;
        g->poly[f].u_scale = glGetUniformLocation(pr, "scale");
        g->poly[f].u_meshblend = glGetUniformLocation(pr, "meshblend");
        g->poly[f].u_atlas = glGetUniformLocation(pr, "atlas");
        g->poly[f].u_luma = glGetUniformLocation(pr, "lumat");
        g->poly[f].u_colour = glGetUniformLocation(pr, "colt");
    }

    for (int i = 0; i < 2; i++)
        for (int r = 0; r < RING; r++)
            g->tex_tiles[r][i] = texture(M2_SCREEN_W, M2_SCREEN_H, GL_LUMINANCE_ALPHA, GL_NEAREST);
    g->tile_pal = texture(256, 16, GL_RGBA, GL_NEAREST);
    g->atlas = texture(ATLAS, ATLAS, GL_LUMINANCE, GL_NEAREST);
    g->luma = texture(128, 256, GL_LUMINANCE, GL_NEAREST);
    g->colour = texture(64, 1024, GL_RGBA, GL_NEAREST);
    glGenTextures(1, &g->fb_tex);
    glGenFramebuffers(1, &g->fbo);

    /* full-screen quad, triangle strip; v = 0 is the top line of the tile layers */
    static const float quad[] = { -1, -1, 0, 1,   1, -1, 1, 1,   -1, 1, 0, 0,   1, 1, 1, 0,
                                  /* blit: GL orientation */
                                  -1, -1, 0, 0,   1, -1, 1, 0,   -1, 1, 0, 1,   1, 1, 1, 1 };
    glGenBuffers(1, &g->vbo_quad);
    glBindBuffer(GL_ARRAY_BUFFER, g->vbo_quad);
    glBufferData(GL_ARRAY_BUFFER, sizeof quad, quad, GL_STATIC_DRAW);
    glGenBuffers(RING, g->vbo_poly);
    glGenBuffers(RING, g->ibo_poly);

    memset(g->band_dirty, 1, sizeof g->band_dirty);
    g->luma_dirty = g->colour_dirty = 1;
    return g;
}

void m2gl_destroy(m2_gl *g)
{
    if (!g)
        return;
    GLuint tex[] = { g->atlas, g->luma, g->colour, g->fb_tex, g->tile_pal };
    glDeleteTextures(5, tex);
    glDeleteTextures(RING * 2, &g->tex_tiles[0][0]);
    glDeleteFramebuffers(1, &g->fbo);
    glDeleteBuffers(1, &g->vbo_quad);
    glDeleteBuffers(RING, g->vbo_poly);
    glDeleteBuffers(RING, g->ibo_poly);
    glDeleteProgram(g->tile_prog);
    for (int f = 0; f < 3; f++)
        if (g->poly[f].prog)
            glDeleteProgram(g->poly[f].prog);
    glDeleteProgram(g->post_prog);
    free(g->scratch);
    free(g->table);
    free(g);
}

/* ------------------------------------------------------------ notifications */

/* Texture RAM holds 16-bit words of 2x2 texels; word k covers stored texels
   x = (k % 512) * 2, y = (k / 512) * 2 (1024 x 2048 stored). */
void m2gl_texture_written(m2_gl *g, int bank, uint32_t offset)
{
    uint32_t sy = ((offset >> 1) / 512) * 2;
    g->band_dirty[bank & 1][(sy >> 5) & 63] = 1;
}

void m2gl_luma_written(m2_gl *g) { g->luma_dirty = 1; }
void m2gl_xlat_written(m2_gl *g) { g->colour_dirty = 1; }
void m2gl_palette_written(m2_gl *g, uint32_t offset)
{
    if (offset >= 0x2000 && offset < 0x2800)   /* entries 0x1000-0x13ff */
        g->colour_dirty = 1;
}

/* ------------------------------------------------------------ table uploads */

static uint16_t ld16(const uint8_t *p) { uint16_t v; memcpy(&v, p, 2); return v; }

/* One band of 32 stored rows. The texture sheets are 2048 x 1024 texels but
   stored as 1024 x 2048: stored rows 1024-2047 are the sheet's right half
   (MAME get_texel). Atlas: sheet s at rows 1024*s. */
static void upload_band(m2_gl *g, const uint8_t *ram, int bank, int band)
{
    int sy0 = band * 32;
    for (int r = 0; r < 32; r++) {
        int sy = sy0 + r;
        uint8_t *out = g->scratch + r * 1024;
        const uint8_t *row = ram + (size_t)(sy >> 1) * 1024;
        int shy = (sy & 1) ? 0 : 8;
        for (int sx = 0; sx < 1024; sx += 2) {
            uint16_t w = ld16(row + sx);
            out[sx] = (uint8_t)(((w >> (shy + 4)) & 15) * 17);
            out[sx + 1] = (uint8_t)(((w >> shy) & 15) * 17);
        }
    }
    int x = sy0 >= 1024 ? 1024 : 0, y = (sy0 & 1023) + 1024 * bank;
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 1024, 32, GL_LUMINANCE, GL_UNSIGNED_BYTE, g->scratch);
}

static void upload_tables(m2_gl *g, const m2_board *b, const m2_tilegen *t)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g->atlas);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (int bank = 0; bank < 2; bank++)
        for (int band = 0; band < 64; band++)
            if (g->band_dirty[bank][band]) {
                upload_band(g, bank ? b->tex1 : b->tex0, bank, band);
                g->band_dirty[bank][band] = 0;
            }

    if (g->luma_dirty) {   /* luma RAM: byte lane 0 of each 32-bit word (0x12800000) */
        for (int i = 0; i < 0x8000; i++)
            g->scratch[i] = b->luma[i * 2];
        glBindTexture(GL_TEXTURE_2D, g->luma);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 128, 256, GL_LUMINANCE, GL_UNSIGNED_BYTE, g->scratch);
        g->luma_dirty = 0;
    }

    if (g->colour_dirty) {
        for (int cb = 0; cb < 1024; cb++) {
            uint16_t c = ld16(b->pal + (0x1000 + cb) * 2) & 0x7fff;
            const uint8_t *xr = b->xlat + 0x0000 + (c & 0x1f) * 0x200;
            const uint8_t *xg = b->xlat + 0x4000 + ((c >> 5) & 0x1f) * 0x200;
            const uint8_t *xb = b->xlat + 0x8000 + ((c >> 10) & 0x1f) * 0x200;
            uint8_t *o = g->table + cb * 64 * 4;
            for (int l = 0; l < 64; l++, o += 4) {
                o[0] = t->gamma[0][t->remap[xr[l * 2]]];
                o[1] = t->gamma[1][t->remap[xg[l * 2]]];
                o[2] = t->gamma[2][t->remap[xb[l * 2]]];
                o[3] = 255;
            }
        }
        glBindTexture(GL_TEXTURE_2D, g->colour);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 64, 1024, GL_RGBA, GL_UNSIGNED_BYTE, g->table);
        g->colour_dirty = 0;
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

/* ------------------------------------------------------------ drawing */

static void quad_attribs(m2_gl *g, int blit)
{
    glBindBuffer(GL_ARRAY_BUFFER, g->vbo_quad);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    const size_t base = blit ? 64 : 0;
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (const void *)base);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, (const void *)(base + 8));
}

/* the offscreen frame, (re)created when its size or filter changes */
static void frame_buffer(m2_gl *g, int w, int h, int smooth)
{
    if (w == g->fb_w && h == g->fb_h && smooth == g->fb_smooth)
        return;
    glBindTexture(GL_TEXTURE_2D, g->fb_tex);
    GLint f = smooth ? GL_LINEAR : GL_NEAREST;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindFramebuffer(GL_FRAMEBUFFER, g->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g->fb_tex, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "m2gl: %dx%d framebuffer incomplete\n", w, h);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    g->fb_w = w;
    g->fb_h = h;
    g->fb_smooth = smooth;
}

/* The tile layers are 496 wide: centred in a wider frame (4:3 in the middle),
   or stretched over all of it when the game's rule says so */
static void draw_layers(m2_gl *g, int hipass, int frame_w, int stretch_a, int stretch_b)
{
    glUseProgram(g->tile_prog);
    glUniform1i(g->u_tile_tex, 0);
    glUniform1i(g->u_tile_pal, 1);
    glUniform1f(g->u_tile_hipass, hipass ? 1.0f : 0.0f);
    quad_attribs(g, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g->tile_pal);
    glActiveTexture(GL_TEXTURE0);
    for (int i = 1; i >= 0; i--) {   /* layer B, then A */
        int stretch = i ? stretch_b : stretch_a;
        glUniform2f(g->u_tile_x, stretch ? 1.0f : (float)M2_SCREEN_W / (float)frame_w, 0.0f);
        glBindTexture(GL_TEXTURE_2D, g->tex_tiles[g->tile_cur][i]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

/* ------------------------------------------------------------ mesh */

/* A frame's polygons as vertices and indices, in batches of up to 65535
   vertices (16-bit indices, relative to the batch). Built without GL, so
   on any thread; drawn by draw_polys. */
struct m2_mesh {
    pvtx     *pv;
    uint16_t *idx;
    int       cap_v, cap_i, cap_b;
    int       nbatch;
    struct { int v0, nv, i0, ni; } *batch;
};

m2_mesh *m2gl_mesh_create(void)
{
    return calloc(1, sizeof(m2_mesh));
}

void m2gl_mesh_destroy(m2_mesh *m)
{
    if (!m)
        return;
    free(m->pv);
    free(m->idx);
    free(m->batch);
    free(m);
}

static int grow(void **p, int *cap, int need, size_t size)
{
    if (need <= *cap)
        return 1;
    int n = need + need / 2;
    void *q = realloc(*p, (size_t)n * size);
    if (!q)
        return 0;
    *p = q;
    *cap = n;
    return 1;
}

int m2gl_mesh_build(m2_mesh *m, const m2_geo_frame *geo, int frame_w)
{
    m->nbatch = 0;
    if (!geo || !geo->npolys)
        return 1;
    /* narrower than 496: a cropped frame, as m2gl_draw_rect */
    if (frame_w < 64)
        frame_w = 64;
    const float half_w = frame_w * 0.5f;

    int tv = 0, ti = 0;
    for (int i = 0; i < geo->npolys; i++) {
        int n = geo->polys[geo->order[i]].nverts;
        tv += n;
        ti += n > 2 ? (n - 2) * 3 : 0;
    }
    if (!grow((void **)&m->pv, &m->cap_v, tv, sizeof(pvtx)) ||
        !grow((void **)&m->idx, &m->cap_i, ti, sizeof(uint16_t)) ||
        !grow((void **)&m->batch, &m->cap_b, tv / BATCH_VERTS + 1, sizeof m->batch[0]))
        return 0;

    /* each polygon a fan of indexed triangles */
    int v0 = 0, i0 = 0, nv = 0, ni = 0;
    for (int i = 0; i < geo->npolys; i++) {
        const m2_gpoly *p = &geo->polys[geo->order[i]];
        if (nv + p->nverts > BATCH_VERTS) {
            m->batch[m->nbatch].v0 = v0; m->batch[m->nbatch].nv = nv;
            m->batch[m->nbatch].i0 = i0; m->batch[m->nbatch].ni = ni;
            m->nbatch++;
            v0 += nv;
            i0 += ni;
            nv = ni = 0;
        }
        uint16_t t0 = p->th[0], t1 = p->th[1], t2 = p->th[2], t3 = p->th[3];
        int textured = (t0 >> 14) & 1;
        float ax = (float)(32 * (t2 & 0x3f)), ay = (float)(32 * ((t2 >> 6) & 0x1f));
        if (t2 & 0x1000)
            ay += 1024.0f;   /* sheet 1 */
        uint16_t par[4] = {
            (uint16_t)(t1 & 0xff), (uint16_t)((t3 >> 6) & 0x3ff), p->luma,
            (uint16_t)(((t0 >> 13) & 1 ? PF_TRANSLUCENT : 0) | ((t0 >> 15) & 1 ? PF_MESH : 0) |
                       (textured ? 0 : PF_UNTEXTURED))
        };
        /* the texture's place, size and mirroring, as m2geo cut the pieces */
        uint16_t rect[4] = {
            (uint16_t)((uint16_t)ax | ((t0 >> 8) & 1 ? 0x8000 : 0)),
            (uint16_t)((uint16_t)ay | ((t0 >> 9) & 1 ? 0x8000 : 0)),
            (uint16_t)(32 << (t0 & 7)), (uint16_t)(32 << ((t0 >> 3) & 7))
        };
        pvtx *pv = m->pv + v0;
        uint16_t *idx = m->idx + i0;
        for (int k = 0; k < p->nverts; k++) {
            const m2_gvert *v = &p->v[k];
            pvtx *o = &pv[nv + k];
            o->x = (v->x / half_w - 1.0f) * v->z;
            o->y = (1.0f - v->y / 192.0f) * v->z;
            o->z = 0;
            o->w = v->z;
            o->u = (ax + v->u) / ATLAS;
            o->v = (ay + v->v) / ATLAS;
            memcpy(o->par, par, sizeof par);
            memcpy(o->rect, rect, sizeof rect);
        }
        for (int k = 1; k + 1 < p->nverts; k++) {
            idx[ni++] = (uint16_t)nv;
            idx[ni++] = (uint16_t)(nv + k);
            idx[ni++] = (uint16_t)(nv + k + 1);
        }
        nv += p->nverts;
    }
    m->batch[m->nbatch].v0 = v0; m->batch[m->nbatch].nv = nv;
    m->batch[m->nbatch].i0 = i0; m->batch[m->nbatch].ni = ni;
    m->nbatch++;
    return 1;
}

/* ------------------------------------------------------------ drawing */

static void draw_batch(m2_gl *g, const pvtx *pv, int nv, const uint16_t *idx, int ni)
{
    if (!ni)
        return;
    /* the next buffers in the ring, grown when too small (then kept) */
    int r = g->buf_cur = (g->buf_cur + 1) % RING;
    GLsizeiptr vs = (GLsizeiptr)(nv * sizeof(pvtx)), is = (GLsizeiptr)(ni * sizeof(uint16_t));
    glBindBuffer(GL_ARRAY_BUFFER, g->vbo_poly[r]);
    if (vs > g->vbo_cap[r]) {
        g->vbo_cap[r] = vs + vs / 2;
        glBufferData(GL_ARRAY_BUFFER, g->vbo_cap[r], NULL, GL_DYNAMIC_DRAW);
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, vs, pv);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g->ibo_poly[r]);
    if (is > g->ibo_cap[r]) {
        g->ibo_cap[r] = is + is / 2;
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, g->ibo_cap[r], NULL, GL_DYNAMIC_DRAW);
    }
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, is, idx);
    const GLsizei st = sizeof(pvtx);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, st, (const void *)offsetof(pvtx, x));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, st, (const void *)offsetof(pvtx, u));
    glVertexAttribPointer(2, 4, GL_UNSIGNED_SHORT, GL_FALSE, st, (const void *)offsetof(pvtx, par));
    glVertexAttribPointer(3, 4, GL_UNSIGNED_SHORT, GL_FALSE, st, (const void *)offsetof(pvtx, rect));
    glDrawElements(GL_TRIANGLES, ni, GL_UNSIGNED_SHORT, NULL);
}

static void draw_polys(m2_gl *g, const m2_mesh *m, int scale, int meshblend, int filter)
{
    if (!m->nbatch)
        return;
    if (filter < 0 || filter > M2_TEX_TRILINEAR)
        filter = M2_TEX_NEAREST;
    while (filter > 0 && !g->poly[filter].prog)
        filter--;
    glUseProgram(g->poly[filter].prog);
    glUniform1i(g->poly[filter].u_atlas, 0);
    glUniform1i(g->poly[filter].u_luma, 1);
    glUniform1i(g->poly[filter].u_colour, 2);
    glUniform1f(g->poly[filter].u_scale, (float)scale);
    glUniform1f(g->poly[filter].u_meshblend, meshblend ? 1.0f : 0.0f);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, g->atlas);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, g->luma);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, g->colour);
    glActiveTexture(GL_TEXTURE0);
    for (int a = 0; a < 4; a++)
        glEnableVertexAttribArray((GLuint)a);
    if (meshblend) {   /* back to front already, so plain alpha blending is right */
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    for (int b = 0; b < m->nbatch; b++)
        draw_batch(g, m->pv + m->batch[b].v0, m->batch[b].nv, m->idx + m->batch[b].i0, m->batch[b].ni);
    glDisable(GL_BLEND);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void m2gl_upload(m2_gl *g, const m2_board *b, m2_tilegen *t, int tables)
{
    if (tables)
        upload_tables(g, b, t);
    /* tile layers and palette: uploaded only when they changed */
    glActiveTexture(GL_TEXTURE0);
    if (!g->tiles_valid || t->layer_version != g->layer_version) {
        /* the next copy in the ring gets the rows it lacks, in runs (the
           Mali-400's driver re-tiles every uploaded texel on the CPU) */
        g->tile_cur = (g->tile_cur + 1) % RING;
        for (int i = 0; i < 2; i++) {
            for (int r = 0; r < RING; r++)
                for (int y = 0; y < M2_SCREEN_H; y++)
                    g->tile_pending[r][i][y] |= !g->tiles_valid || t->row_dirty[i][y];
            memset(t->row_dirty[i], 0, M2_SCREEN_H);
            uint8_t *pend = g->tile_pending[g->tile_cur][i];
            glBindTexture(GL_TEXTURE_2D, g->tex_tiles[g->tile_cur][i]);
            for (int y = 0; y < M2_SCREEN_H;) {
                if (!pend[y]) { y++; continue; }
                int y1 = y;
                while (y1 < M2_SCREEN_H && pend[y1])
                    y1++;
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, y, M2_SCREEN_W, y1 - y, GL_LUMINANCE_ALPHA,
                                GL_UNSIGNED_BYTE, t->layer[i] + y * M2_SCREEN_W);
                y = y1;
            }
            memset(pend, 0, M2_SCREEN_H);
        }
        g->layer_version = t->layer_version;
    }
    if (!g->tiles_valid || t->pal_version != g->pal_version) {
        glBindTexture(GL_TEXTURE_2D, g->tile_pal);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 16, GL_RGBA, GL_UNSIGNED_BYTE, t->pal);
        g->pal_version = t->pal_version;
    }
    g->tiles_valid = 1;
}

void m2gl_draw(m2_gl *g, const m2_mesh *mesh, const m2_view *view, int width, int height)
{
    m2gl_draw_rect(g, mesh, view, 0, 0, width, height);
}

void m2gl_draw_rect(m2_gl *g, const m2_mesh *mesh, const m2_view *view,
                    int x, int y, int width, int height)
{
    /* narrower than 496: a cropped frame (the centre of the picture) */
    int frame_w = view->frame_w < 64 ? 64 : view->frame_w;
    int scale = view->scale < 1 ? 1 : view->scale;

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);


    /* the frame, at frame_w x 384 native pixels times the render scale */
    frame_buffer(g, frame_w * scale, M2_SCREEN_H * scale, view->smooth);
    glBindFramebuffer(GL_FRAMEBUFFER, g->fbo);
    glViewport(0, 0, g->fb_w, g->fb_h);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    draw_layers(g, 0, frame_w, view->stretch & M2_STRETCH_A_LOW, view->stretch & M2_STRETCH_B_LOW);
    if (mesh)
        draw_polys(g, mesh, scale, view->mesh_blend, view->tex_filter);
    draw_layers(g, 1, frame_w, view->stretch & M2_STRETCH_A_HIGH, view->stretch & M2_STRETCH_B_HIGH);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* scale to the window: 496 native pixels are 4:3, a wider frame is
       proportionally wider */
    double aspect = (4.0 / 3.0) * frame_w / M2_SCREEN_W;
    int vw = width, vh = (int)(width / aspect + 0.5);
    if (view->fill)
        vh = height;
    else if (vh > height) {
        vh = height;
        vw = (int)(height * aspect + 0.5);
    }
    glViewport(x, y, width, height);
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
    glViewport(x + (width - vw) / 2, y + (height - vh) / 2, vw, vh);
    glUseProgram(g->post_prog);
    glUniform1i(g->u_post_tex, 0);
    glUniform1f(g->u_post_sat, view->saturation > 0 ? view->saturation : 1.0f);
    glUniform2f(g->u_post_x, 1.0f, 0.0f);
    quad_attribs(g, 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g->fb_tex);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
