/* Headless runner: boots a game and reports what the board is doing. */
#include "m2board.h"
#include "m2tile.h"
#include "m2input.h"
#include "m2geo.h"
#include "../snd/m2snd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void m2_dump_tilemaps(const m2_board *b, const char *prefix, int nib_order);

static void logmsg(const char *m) { fprintf(stderr, "%s\n", m); }

static m2_tilegen tg;

typedef struct {
    unsigned long tile, pal, tex, luma, xlat, cg, sound, invalid;
    uint32_t last_invalid_addr, last_invalid_raw;
} stats;

static void on_tile(void *u, uint32_t o) { ((stats *)u)->tile++; m2tile_tile_written(&tg, o); }
static void on_pal(void *u, uint32_t o) { (void)o; ((stats *)u)->pal++; }
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
static uint32_t prof_ip[4096], prof_n[4096];
static int profiling;
static void on_slice(void *u)
{
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
    int io_from = getenv("M2_IOTRACE") ? atoi(getenv("M2_IOTRACE")) : -1;
    board = b;
    int prof_from = getenv("M2_PROF") ? atoi(getenv("M2_PROF")) : -1;
    const char *keys = getenv("M2_KEYS");
    FILE *wav = NULL;
    uint32_t wav_samples = 0;
    double sample_debt = 0;
    if (getenv("M2_WAV")) {   /* sound board output to a WAV file */
        snd = m2snd_create(b);
        wav = fopen(getenv("M2_WAV"), "wb");
        if (wav) fwrite("RIFF\0\0\0\0WAVEfmt \x10\0\0\0\x01\0\x02\0\x44\xac\0\0\x10\xb1\x02\0\x04\0\x10\0data\0\0\0\0", 1, 44, wav);
    }
    m2_geo *geo = getenv("M2_GEO") ? m2geo_create() : NULL;
    m2_input_state ins;
    m2input_init(&ins);

    for (int f = 1; f <= frames; f++) {
        if (snd) {
            static int16_t buf[4096];
            sample_debt += (double)M2SND_RATE / b->game->fps;
            int n = (int)sample_debt;
            sample_debt -= n;
            m2snd_render(snd, buf, n);
            if (wav) fwrite(buf, 4, (size_t)n, wav);
            wav_samples += (uint32_t)n;
        }
        profiling = prof_from >= 0 && f >= prof_from;
        io_tracing = io_from >= 0 && f >= io_from;
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
                m2geo_run(geo, b);
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
    if (getenv("M2_RAM")) {
        FILE *f = fopen(getenv("M2_RAM"), "wb");
        if (f) { fwrite(b->ram, 1, 0x100000, f); fclose(f); }
    }
    if (getenv("M2_SHOT")) {   /* composited tile layers: B, then A on top */
        m2tile_render(&tg, b);
        FILE *f = fopen(getenv("M2_SHOT"), "wb");
        if (f) {
            fprintf(f, "P6\n%d %d\n255\n", M2_SCREEN_W, M2_SCREEN_H);
            for (int i = 0; i < M2_SCREEN_W * M2_SCREEN_H; i++) {
                uint32_t c = tg.layer[0][i] >> 24 ? tg.layer[0][i] : tg.layer[1][i];
                uint8_t rgb[3] = { (uint8_t)c, (uint8_t)(c >> 8), (uint8_t)(c >> 16) };
                fwrite(rgb, 1, 3, f);
            }
            fclose(f);
        }
    }
    if (getenv("M2_DUMP"))
        m2_dump_tilemaps(b, getenv("M2_DUMP"), getenv("M2_NIB") ? atoi(getenv("M2_NIB")) : 2);
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
