/* Headless runner: boots a game and reports what the board is doing. */
#include "m2board.h"
#include "m2tile.h"
#include "m2input.h"
#include "m2geo.h"
#include "m2pipe.h"
#include "m2net.h"
#include "../snd/m2snd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#define NULL_DEVICE "NUL"
#else
#define NULL_DEVICE "/dev/null"
#endif

void m2_dump_tilemaps(const m2_board *b, const char *prefix, int nib_order);

static void logmsg(const char *m) { fprintf(stderr, "%s\n", m); }

static m2_tilegen tg;

static double now_ms(void)
{
    struct timespec t;
#ifdef _WIN32
    timespec_get(&t, TIME_UTC);
#else
    clock_gettime(CLOCK_MONOTONIC, &t);
#endif
    return t.tv_sec * 1e3 + t.tv_nsec / 1e6;
}

typedef struct {
    unsigned long tile, pal, tex, luma, xlat, cg, sound, invalid;
    uint32_t last_invalid_addr, last_invalid_raw;
} stats;

static void on_tile(void *u, uint32_t o) { ((stats *)u)->tile++; m2tile_tile_written(&tg, o); }
static void on_pal(void *u, uint32_t o) { ((stats *)u)->pal++; m2tile_palette_written(&tg, o); }
static void on_tex(void *u, int bank, uint32_t o) { (void)bank; (void)o; ((stats *)u)->tex++; }
static void on_luma(void *u) { ((stats *)u)->luma++; }
static void on_xlat(void *u) { ((stats *)u)->xlat++; m2tile_xlat_written(&tg); }
static void on_cg(void *u) { ((stats *)u)->cg++; m2tile_cg_written(&tg); }
static m2_snd *snd;
static void on_sound(void *u, uint8_t c) { ((stats *)u)->sound++; if (snd) m2snd_command(snd, c); }

static stats st;
static uint32_t io_addr[64], io_cnt[64], io_last[64];
static int io_tracing;
static void on_io(void *u, uint32_t a, uint32_t v)
{
    (void)u;
    if (!io_tracing) return;
    for (int i = 0; i < 64; i++)
        if (!io_cnt[i] || io_addr[i] == a) { io_addr[i] = a; io_cnt[i]++; io_last[i] = v; return; }
}
static m2_board *board;

/* M2_COOP=1: a second board, linked through the network ring */
static m2_tilegen *tg2;
static void on_tile2(void *u, uint32_t o) { (void)u; m2tile_tile_written(tg2, o); }
static void on_pal2(void *u, uint32_t o) { (void)u; m2tile_palette_written(tg2, o); }
static void on_xlat2(void *u) { (void)u; m2tile_xlat_written(tg2); }
static void on_cg2(void *u) { (void)u; m2tile_cg_written(tg2); }

static void write_shot(const char *path, m2_tilegen *t, const m2_board *b)
{
    m2tile_render(t, b);
    FILE *f = fopen(path, "wb");
    if (!f)
        return;
    fprintf(f, "P6\n%d %d\n255\n", M2_SCREEN_W, M2_SCREEN_H);
    for (int i = 0; i < M2_SCREEN_W * M2_SCREEN_H; i++) {
        uint16_t v = M2_TILE_PEN(t->layer[0][i]) ? t->layer[0][i] : t->layer[1][i];
        uint32_t c = M2_TILE_PEN(v) ? t->pal[M2_TILE_INDEX(v)] : 0;
        uint8_t rgb[3] = { (uint8_t)c, (uint8_t)(c >> 8), (uint8_t)(c >> 16) };
        fwrite(rgb, 1, 3, f);
    }
    fclose(f);
}
static uint32_t prof_ip[4096], prof_n[4096];
static int profiling;
/* M2_SLICES=1: where the CPU stands at the end of each of the 210 slices of
   a frame, as a checksum per frame (M2_SLICES=2 prints every slice). For
   comparing two CPU implementations: the slices must line up. */
static int slice_log;
static uint64_t slice_hash = 1469598103934665603ull;
static int slice_index;
static void on_slice(void *u)
{
    if (slice_log) {
        uint32_t ip = board->cpu->ip;
        for (int k = 0; k < 4; k++)
            slice_hash = (slice_hash ^ ((ip >> (k * 8)) & 0xff)) * 1099511628211ull;
        if (slice_log > 1)
            printf("  slice %3d: ip %08x stop %u left %d idle %d\n", slice_index, ip, board->cpu->stop, board->cpu->cycles_left, board->idle_count);
        slice_index++;
    }
    (void)u;
    if (!profiling || board->cpu->stop)
        return;
    uint32_t ip = board->cpu->ip, h = (ip >> 2) & 4095;
    while (prof_n[h] && prof_ip[h] != ip)
        h = (h + 1) & 4095;
    prof_ip[h] = ip;
    prof_n[h]++;
}

static void on_invalid(void *u, uint32_t addr, uint32_t raw)
{
    (void)u;
    st.invalid++;
    st.last_invalid_addr = addr;
    st.last_invalid_raw = raw;
}

static uint32_t nonzero(const uint8_t *p, size_t n)
{
    uint32_t c = 0;
    for (size_t i = 0; i < n; i++)
        c += p[i] != 0;
    return c;
}

/* M2_TGP_RECORD=file:from:to: the TGP's state at frame `from`, then every
   access to it until frame `to`, for tgp/test/replay.c (format there) */
static FILE *trec;
static void trec_event(int op, uint32_t a, uint32_t v)
{
    uint8_t e[9] = { (uint8_t)op, (uint8_t)a, (uint8_t)(a >> 8), (uint8_t)(a >> 16), (uint8_t)(a >> 24),
                     (uint8_t)v, (uint8_t)(v >> 8), (uint8_t)(v >> 16), (uint8_t)(v >> 24) };
    fwrite(e, 1, sizeof e, trec);
}

static void trec_blob(const void *p, uint32_t n)
{
    fwrite(&n, 4, 1, trec);
    if (n)
        fwrite(p, 1, n, trec);
}

static void trec_start(const m2_board *b, const char *path)
{
    trec = fopen(path, "wb");
    if (!trec) {
        fprintf(stderr, "cannot write %s\n", path);
        return;
    }
    fwrite("M2TR", 1, 4, trec);
    /* the state, field by field (the struct holds pointers) */
    const tgp *t = &b->tgp;
    uint32_t regs[] = { t->table_ptr_value, t->prog_ptr_value, t->prog_upload_ctrl, t->prog_upload_off,
                        t->data_upload_ctrl, t->data_upload_off, t->reg_98000c, t->ext_ptr, t->reg_803008,
                        t->reg_801008_read, (uint32_t)t->unused_573320 };
    trec_blob(t->blk, sizeof t->blk);
    trec_blob(&t->in, sizeof t->in);
    trec_blob(&t->out, sizeof t->out);
    trec_blob(regs, sizeof regs);
    trec_blob(b->tgp_prog, 0x40000);
    trec_blob(b->tgp_data, 0x40000);
    trec_blob(b->bufram, 0x80000);
    trec_blob(b->tgp.table_rom, b->tgp.table_rom_size);
    trec_blob(b->tgp.ext_rom, b->tgp.ext_rom_words * 4);
    m2_tgp_record = trec_event;
}

int main(int argc, char **argv)
{
    const char *game = argc > 1 ? argv[1] : "daytona";
    int frames = argc > 2 ? atoi(argv[2]) : 600;
    const char *dirs[] = { argc > 3 ? argv[3] : ".", NULL };

    const m2_game *g = m2_find_game(game);
    if (!g) { fprintf(stderr, "unknown game %s\n", game); return 1; }
    m2_board *b = m2_create(g, dirs, logmsg);
    if (!b) return 1;
    printf("%s (%s), board type %d, program ROM %u KB, data ROM %u KB, TGP tables %u KB\n",
           g->title, g->board, b->type, b->rom.size[0] >> 10, b->rom.size[1] >> 10, b->rom.size[10] >> 10);

    const char *nv = getenv("M2_NVRAM");
    if (nv && !m2_nvram_load(b, nv))
        printf("NVRAM loaded from %s\n", nv);
    else if (m2_nvram_defaults(b))
        printf("NVRAM: first-boot defaults applied\n");

    m2tile_init(&tg, 1.0f, 1.0f, 1.0f);
    b->hooks.user = &st;
    b->hooks.tilemap_written = on_tile;
    b->hooks.palette_written = on_pal;
    b->hooks.texture_written = on_tex;
    b->hooks.luma_written = on_luma;
    b->hooks.xlat_written = on_xlat;
    b->hooks.cg_written = on_cg;
    b->hooks.sound_command = on_sound;
    b->cpu->invalid_opcode = on_invalid;
    b->hooks.slice_done = on_slice;
    b->hooks.io_read = on_io;
    /* M2_PIPE=1: video goes through m2pipe (capture + apply every frame, on
       this thread), to check that the hand-over reproduces the output */
    m2_pipe *pipe = getenv("M2_PIPE") ? m2pipe_create() : NULL;
    m2_hooks replay = b->hooks;
    if (pipe)
        m2pipe_attach(pipe, b);
    const m2_board *vb = b;   /* what the renderers read */
    int reset_at = getenv("M2_RESET_AT") ? atoi(getenv("M2_RESET_AT")) : -1;

    m2_board *b2 = NULL;
    m2_net *net = NULL;
    if (getenv("M2_COOP")) {   /* board 1 master, board 2 slave, one ring */
        b2 = m2_create(g, dirs, logmsg);
        tg2 = malloc(sizeof *tg2);
        if (!b2 || !tg2) return 1;
        m2_nvram_defaults(b2);
        m2tile_init(tg2, 1.0f, 1.0f, 1.0f);
        b2->hooks.tilemap_written = on_tile2;
        b2->hooks.palette_written = on_pal2;
        b2->hooks.xlat_written = on_xlat2;
        b2->hooks.cg_written = on_cg2;
        m2_nvram_set_link(b, 1, 1);
        m2_nvram_set_link(b2, 2, 2);
        m2_board *ring[2] = { b, b2 };
        net = m2net_create(ring, 2);
    }
    int io_from = getenv("M2_IOTRACE") ? atoi(getenv("M2_IOTRACE")) : -1;
    board = b;
    int prof_from = getenv("M2_PROF") ? atoi(getenv("M2_PROF")) : -1;
    slice_log = getenv("M2_SLICES") ? atoi(getenv("M2_SLICES")) : 0;
    const char *keys = getenv("M2_KEYS");
    const char *wav_path = getenv("M2_WAV");
    int bench = getenv("M2_BENCH") != NULL;   /* full per-frame CPU work, timed */
    double tb[4] = { 0, 0, 0, 0 };
    if (bench) {
        m2tile_init(&tg, 1, 1, 1);
        if (!wav_path) wav_path = NULL_DEVICE;   /* sound runs, output discarded */
    }
    FILE *wav = NULL;
    uint32_t wav_samples = 0;
    double sample_debt = 0;
    if (wav_path) {   /* sound board output to a WAV file */
        snd = m2snd_create(b);
        wav = fopen(wav_path, "wb");
        if (wav) fwrite("RIFF\0\0\0\0WAVEfmt \x10\0\0\0\x01\0\x02\0\x44\xac\0\0\x10\xb1\x02\0\x04\0\x10\0data\0\0\0\0", 1, 44, wav);
    }
    int geohash = getenv("M2_GEOHASH") != NULL;   /* 3D output checksum, every frame */
    uint64_t gh = 1469598103934665603ull;
    m2_geo *geo = getenv("M2_GEO") || getenv("M2_BENCH") || geohash ? m2geo_create() : NULL;
    m2_input_state ins;
    m2input_init(&ins);

    const char *trec_spec = getenv("M2_TGP_RECORD");
    char trec_path[1024] = "";
    int trec_from = 0, trec_to = 0;
    if (trec_spec && sscanf(trec_spec, "%1023[^:]:%d:%d", trec_path, &trec_from, &trec_to) != 3)
        trec_spec = NULL;

    for (int f = 1; f <= frames; f++) {
        if (trec_spec && f == trec_from)
            trec_start(b, trec_path);
        if (trec && f == trec_to + 1) {
            m2_tgp_record = NULL;
            fclose(trec);
            trec = NULL;
        }
        if (snd) {
            static int16_t buf[4096];
            sample_debt += (double)M2SND_RATE / b->game->fps;
            int n = (int)sample_debt;
            sample_debt -= n;
            double ts = now_ms();
            m2snd_render(snd, buf, n);
            tb[3] += now_ms() - ts;
            if (wav) fwrite(buf, 4, (size_t)n, wav);
            wav_samples += (uint32_t)n;
        }
        profiling = prof_from >= 0 && f >= prof_from;
        io_tracing = io_from >= 0 && f >= io_from;
        double t0 = now_ms();
        if (keys) {   /* M2_KEYS=frame:dik:frames,... (dik in hex) */
            memset(ins.key, 0, sizeof ins.key);
            for (const char *k = keys; *k;) {
                int at = 0, len = 0;
                unsigned dik = 0;
                if (sscanf(k, "%d:%x:%d", &at, &dik, &len) == 3 && f >= at && f < at + len && dik < 256)
                    ins.key[dik] = 1;
                k = strchr(k, ',');
                if (!k) break;
                k++;
            }
            m2input_apply(&ins, b);
        }
        m2_run_frame(b);
        if (b2) {
            m2_run_frame(b2);
            m2net_frame(net);
            static int last[2] = { -1, -1 };
            for (int k = 0; k < 2; k++)
                if (m2net_state(net, k) != last[k]) {
                    last[k] = m2net_state(net, k);
                    printf("frame %d: board %d link state %d\n", f, k + 1, last[k]);
                }
        }
        if (reset_at == f) {   /* as the frontend's F3 */
            m2_reset(b);
            if (pipe)
                m2pipe_invalidate(pipe);
            else
                m2tile_init(&tg, 1.0f, 1.0f, 1.0f);
        }
        if (pipe) {
            m2pipe_capture(pipe, b);
            vb = m2pipe_apply(pipe, &replay, NULL);
        }
        if (bench) {
            double t1 = now_ms();
            m2tile_render(&tg, vb);
            double t2 = now_ms();
            m2geo_run(geo, vb);
            double t3 = now_ms();
            tb[0] += t1 - t0; tb[1] += t2 - t1; tb[2] += t3 - t2;
        }
        if (geohash) {   /* FNV-1a over the polygons in drawing order, then the tile output */
            m2geo_run(geo, vb);
            for (int i = 0; i < geo->npolys; i++) {
                const m2_gpoly *p = &geo->polys[geo->order[i]];
                uint8_t h[16];
                memcpy(h, p->th, 8);
                h[8] = p->luma; h[9] = p->nverts; h[10] = p->window;
                memcpy(h + 11, &p->zval, 2);
                h[13] = h[14] = h[15] = 0;
                for (int k = 0; k < 16; k++) gh = (gh ^ h[k]) * 1099511628211ull;
                const uint8_t *v = (const uint8_t *)p->v;
                for (size_t k = 0; k < sizeof(m2_gvert) * p->nverts; k++) gh = (gh ^ v[k]) * 1099511628211ull;
            }
            m2tile_render(&tg, vb);
            const uint8_t *tb8 = (const uint8_t *)tg.layer;
            for (size_t k = 0; k < sizeof tg.layer; k += 8) {   /* 8 bytes a step: speed over quality */
                uint64_t w8;
                memcpy(&w8, tb8 + k, 8);
                gh = (gh ^ w8) * 1099511628211ull;
            }
            const uint8_t *pb8 = (const uint8_t *)tg.pal;
            for (size_t k = 0; k < sizeof tg.pal; k += 8) {
                uint64_t w8;
                memcpy(&w8, pb8 + k, 8);
                gh = (gh ^ w8) * 1099511628211ull;
            }
            if (f % 300 == 0 || f == frames)
                printf("geohash %d: %016llx\n", f, (unsigned long long)gh);
        }
        if (getenv("M2_RAMHASH")) {   /* a checksum of main RAM every frame */
            uint64_t h = 1469598103934665603ull;
            for (size_t k = 0; k < 0x100000; k += 8) {
                uint64_t w;
                memcpy(&w, b->ram + k, 8);
                h = (h ^ w) * 1099511628211ull;
            }
            printf("frame %d ram %016llx\n", f, (unsigned long long)h);
        }
        if (slice_log) {
            printf("frame %d slices %016llx\n", f, (unsigned long long)slice_hash);
            slice_index = 0;
        }
        if (f == 1 || f % 60 == 0 || f == frames || (getenv("M2_FROM") && f >= atoi(getenv("M2_FROM")) && f <= atoi(getenv("M2_TO")))) {
            printf("frame %4d: ip=%08x busy=%3u%% irq en=%03x pend=%03x | tile %lu pal %lu tex %lu cg %lu snd %lu | "
                   "tgp prog %u B, pc %04x | bufram %u nz, ram %u nz | invalid %lu",
                   f, b->cpu->ip, b->busy_slices * 100 / (210u * (unsigned)f),
                   b->irq_enable, b->irq_pending, st.tile, st.pal, st.tex, st.cg, st.sound,
                   b->tgp.prog_upload_off, *(uint16_t *)(b->tgp.blk + TGP_O_PC),
                   nonzero(b->bufram, 0x80000), nonzero(b->ram, 0x100000), st.invalid);
            if (st.invalid)
                printf(" (last %08x at %08x)", st.last_invalid_raw, st.last_invalid_addr);
            printf("\n");
            if (snd) {
                char sb[256];
                m2snd_debug(snd, sb, sizeof sb);
                printf("    snd: %s\n", sb);
            }
            if (geo) {
                m2geo_run(geo, vb);
                printf("    geo: list at %05x, %d polys, %d windows, hsync %d vsync %d", b->tgp.reg_803008 & 0x7ffff,
                       geo->npolys, geo->cur_window, b->hsync, b->vsync);
                if (geo->npolys) {
                    const m2_gpoly *p = &geo->polys[geo->order[geo->npolys / 2]];
                    printf(" | mid poly: th %04x %04x %04x %04x luma %d z %04x v0 (%.1f,%.1f,%.2f uv %.1f,%.1f)",
                           p->th[0], p->th[1], p->th[2], p->th[3], p->luma, p->zval,
                           p->v[0].x, p->v[0].y, p->v[0].z, p->v[0].u, p->v[0].v);
                }
                printf("\n");
            }
        }
    }
    if (prof_from >= 0) {
        for (int k = 0; k < 20; k++) {
            int best = -1;
            for (int h = 0; h < 4096; h++)
                if (prof_n[h] && (best < 0 || prof_n[h] > prof_n[best]))
                    best = h;
            if (best < 0)
                break;
            printf("  ip %08x: %u\n", prof_ip[best], prof_n[best]);
            prof_n[best] = 0;
        }
    }
    for (int i = 0; i < 64 && io_cnt[i]; i++)
        printf("  io read %08x x%u last %02x\n", io_addr[i], io_cnt[i], io_last[i] & 0xff);
    if (getenv("M2_ROMS")) {
        char path[512];
        for (int r = 0; r < M2_REGIONS; r++) {
            if (!b->rom.ptr[r]) continue;
            snprintf(path, sizeof path, "%s-region%d.bin", getenv("M2_ROMS"), r);
            FILE *f = fopen(path, "wb");
            if (f) { fwrite(b->rom.ptr[r], 1, b->rom.size[r], f); fclose(f); }
        }
    }
    if (getenv("M2_GEODUMP")) {   /* the last frame's polygons, in drawing order */
        m2_geo *g = geo ? geo : m2geo_create();
        m2geo_run(g, vb);
        FILE *f = fopen(getenv("M2_GEODUMP"), "w");
        if (f) {
            for (int i = 0; i < g->npolys; i++) {
                const m2_gpoly *p = &g->polys[g->order[i]];
                fprintf(f, "%u %u", p->luma, p->nverts);
                for (int k = 0; k < p->nverts; k++)
                    fprintf(f, " %.2f,%.2f,%.3f", p->v[k].x, p->v[k].y, p->v[k].z);
                fprintf(f, "\n");
            }
            fclose(f);
        }
    }
    if (getenv("M2_RAM")) {
        FILE *f = fopen(getenv("M2_RAM"), "wb");
        if (f) { fwrite(b->ram, 1, 0x100000, f); fclose(f); }
    }
    if (getenv("M2_SHOT")) {   /* composited tile layers: B, then A on top */
        write_shot(getenv("M2_SHOT"), &tg, b);
        if (b2) {   /* the second board: <file>.2.ppm */
            char p2[1024];
            snprintf(p2, sizeof p2, "%s.2.ppm", getenv("M2_SHOT"));
            write_shot(p2, tg2, b2);
        }
    }
    if (getenv("M2_DUMP"))
        m2_dump_tilemaps(b, getenv("M2_DUMP"), getenv("M2_NIB") ? atoi(getenv("M2_NIB")) : 2);
    if (trec) {
        m2_tgp_record = NULL;
        fclose(trec);
        trec = NULL;
    }
    if (bench)
        printf("bench %d frames, ms/frame: emu %.3f  tiles %.3f  geo %.3f  sound %.3f  total %.3f\n", frames,
               tb[0] / frames, tb[1] / frames, tb[2] / frames, tb[3] / frames,
               (tb[0] + tb[1] + tb[2] + tb[3]) / frames);
    if (wav) {
        uint32_t data = wav_samples * 4, riff = data + 36;
        fseek(wav, 4, SEEK_SET); fwrite(&riff, 4, 1, wav);
        fseek(wav, 40, SEEK_SET); fwrite(&data, 4, 1, wav);
        fclose(wav);
    }
    if (snd) m2snd_destroy(snd);
    if (nv && m2_nvram_save(b, nv))
        fprintf(stderr, "cannot save NVRAM to %s\n", nv);
    m2_destroy(b);
    return 0;
}
