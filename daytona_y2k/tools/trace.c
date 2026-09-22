/*
 * Coverage tracer: runs Daytona USA on the port's interpreter (built with
 * -DI960_TRACE) and records what the i960 executes, for the recompiler.
 *
 *   trace <romdir> <frames> <out.txt> [keys]
 *
 * keys: frame:dik:frames,... as M2_KEYS (DirectInput codes in hex).
 * Output lines:
 *   X start end      executed instruction words [start, end)
 *   C from to        call / callx from `from` to `to`
 *   B from to        bal / balx (g14 or dst gets the return address)
 *   J from to        bx (indirect jump)
 *   E to             entry not reached by an instruction (interrupt, IAC)
 *   R from to        ret from `from` to `to`
 *   N from to        another instruction that moved ip (IAC reinitialisation)
 */
#include "../../port/m2/m2board.h"
#include "../../port/m2/m2input.h"
#include "../../port/m2/m2tile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { uint32_t a, b; char k; } edge;

static uint8_t *seen;          /* 1 bit per word, 4 GB space is too much: hash other ranges */
#define SEEN_LIMIT 0x2000000u  /* words below 32 MB tracked in the bitmap */
static edge *edges;
static size_t nedges, capedges;
static uint32_t expect_ip = 0xffffffff;

static int edge_add(char k, uint32_t a, uint32_t b)
{
    /* open addressing on (k, a, b) */
    uint64_t h = ((uint64_t)a * 0x9e3779b1u) ^ ((uint64_t)b * 0x85ebca77u) ^ (uint64_t)k;
    size_t m = capedges - 1, i = (size_t)(h & m);
    while (edges[i].k) {
        if (edges[i].k == k && edges[i].a == a && edges[i].b == b)
            return 0;
        i = (i + 1) & m;
    }
    edges[i].k = k; edges[i].a = a; edges[i].b = b;
    nedges++;
    if (nedges * 2 > capedges) {
        fprintf(stderr, "edge table full\n");
        exit(1);
    }
    return 1;
}

/* TRACE_WATCH=addr: report every change of that byte, with the instruction
   that did it (for chasing down a difference against the recompiled code) */
static uint32_t watch_addr;
static int watch_last = -1;
static int watch_frame;

static void watch_check(m2_board *b, uint32_t at)
{
    int v;
    if (!watch_addr)
        return;
    v = i960_read8(b->cpu, watch_addr);
    if (v != watch_last) {
        printf("watch %08x: %02x -> %02x at ip %08x, frame %d\n",
               watch_addr, watch_last & 0xff, v, at, watch_frame);
        watch_last = v;
    }
}

/* TRACE_CYCLES=frame: every instruction of that frame with the cycles the
   interpreter charges for it (which is not always the instruction's own
   cost: see the region-switch quirk in port/i960/QUIRKS.md) */
static int cycles_frame;

static void on_trace(void *u, uint32_t at, const i960_insn *in)
{
    m2_board *b = u;
    i960_state *s = b->cpu;
    if (cycles_frame && (cycles_frame < 0 || watch_frame == cycles_frame))
        printf("c %08x %s %d\n", at, in->op->name, s->cur->op->cycles);
    if (at != expect_ip && expect_ip != 0xffffffff)
        edge_add('E', 0, at);
    watch_check(b, at);
    if (at < SEEN_LIMIT)
        seen[at >> 5] |= (uint8_t)(1u << ((at >> 2) & 7));
    else
        edge_add('X', at, 0);
    /* an instruction patched by the board (idle hook) reports its own op */
    const i960_opdef *op = in->hooked_op && in->op->cycles == 0 && in->op->name[0] == '(' ? in->hooked_op : in->op;
    const char *n = op->name;
    if (!strcmp(n, "call") || !strcmp(n, "callx"))
        edge_add('C', at, s->ip);
    else if (!strcmp(n, "bal") || !strcmp(n, "balx"))
        edge_add('B', at, s->ip);
    else if (!strcmp(n, "bx"))
        edge_add('J', at, s->ip);
    else if (!strcmp(n, "ret"))
        edge_add('R', at, s->ip);
    else if (op->format < 2) {
        /* anything else that moved ip (synmovq's IAC reinitialisation) */
        uint32_t len = 4;
        if (op->format == 1 && (in->raw & 0x1000)) {
            uint32_t mode = (in->raw >> 10) & 0xf;
            if (mode == 5 || mode >= 0xc)
                len = 8;
        }
        if (s->ip != at + len)
            edge_add('N', at, s->ip);
    }
    expect_ip = s->ip;
}

/* TRACE_TGP=1: the geometry processor's command stream. The i960 sends a
   command by writing to 0x880000 + command * 16 and streams its parameters
   to 0x884000-0x887fff (port/tgp/tgp.c); here every packet is counted by
   command, with how many parameter words followed and where it was sent from. */
#define TGP_CMDS 256
typedef struct {
    unsigned long packets;
    unsigned long words_min, words_max, words_total;
    uint32_t site[8];         /* a few addresses that sent it */
} tgp_stat;
static tgp_stat tgp_cmd[TGP_CMDS];
static int tgp_cur = -1;
static unsigned long tgp_words;
static i960_write32_fn tgp_w32_orig;
static i960_write16_fn tgp_w16_orig;

/* TRACE_TGP_FROM/TO=frames: in that window, what each command did: which
   words of the DSP's RAM changed and how many words it wrote out to
   external memory (the display list) */
extern void (*tgp_trace_ext)(tgp *t, uint32_t bank, uint32_t addr, uint32_t v);
static int tgp_from = -1, tgp_to = -1;
static int tgp_in_window;
static uint32_t tgp_snap[0x400];
static unsigned long tgp_ext_count, tgp_ext_bank4;
static struct {
    unsigned long packets, ext, ext4, reads;
    uint32_t ram[0x400];
} tgp_eff[TGP_CMDS];
static unsigned long tgp_reads;
static unsigned long dl_writes;   /* the i960's own writes through 0x80xxxx */
static i960_read32_fn tgp_r32_orig;
static i960_read16_fn tgp_r16_orig;
static i960_write32_fn dl_w32_orig;
static FILE *tgp_raw;
static int tgp_raw_left;
static int raw_on(void) { return tgp_raw && tgp_raw_left > 0 && (tgp_from < 0 || tgp_in_window); }
static uint32_t tgp_r32(void *u, uint32_t a)
{
    uint32_t v = tgp_r32_orig(u, a);
    if (a - 0x884000u < 0x4000) {
        tgp_reads++;
        if (raw_on())
            fprintf(tgp_raw, " ->%08x", v);
    }
    return v;
}
static uint16_t tgp_r16(void *u, uint32_t a) { if (a - 0x884000u < 0x4000) tgp_reads++; return tgp_r16_orig(u, a); }
static void dl_w32(void *u, uint32_t a, uint32_t v) { if (tgp_in_window) dl_writes++; dl_w32_orig(u, a, v); }
static m2_board *tgp_board;

static void tgp_on_ext(tgp *t, uint32_t bank, uint32_t addr, uint32_t v)
{
    (void)t; (void)addr; (void)v;
    tgp_ext_count++;
    if ((bank & 0xf00000) == 0x400000)
        tgp_ext_bank4++;
}

static const uint32_t *tgp_ram(void)
{
    return (const uint32_t *)(tgp_board->tgp.blk + TGP_O_RAM);
}

static void tgp_effects(void)
{
    if (!tgp_in_window || tgp_cur < 0)
        return;
    const uint32_t *r = tgp_ram();
    for (int i = 0; i < 0x400; i++)
        if (r[i] != tgp_snap[i])
            tgp_eff[tgp_cur].ram[i]++;
    tgp_eff[tgp_cur].packets++;
    tgp_eff[tgp_cur].ext += tgp_ext_count;
    tgp_eff[tgp_cur].ext4 += tgp_ext_bank4;
    tgp_eff[tgp_cur].reads += tgp_reads;
}

static void tgp_close_packet(void)
{
    tgp_effects();
    if (tgp_cur < 0)
        return;
    tgp_stat *c = &tgp_cmd[tgp_cur];
    if (!c->packets || tgp_words < c->words_min) c->words_min = tgp_words;
    if (tgp_words > c->words_max) c->words_max = tgp_words;
    c->words_total += tgp_words;
    c->packets++;
    if (raw_on()) {
        fprintf(tgp_raw, "\n");
        tgp_raw_left--;
    }
}

static void tgp_note(void *u, uint32_t a, uint32_t v)
{
    m2_board *b = u;
    if (a - 0x880000u < 0x4000 && !(a & 0xf)) {
        tgp_close_packet();
        tgp_cur = (a >> 4) & 0xff;
        tgp_words = 0;
        tgp_in_window = tgp_from >= 0 && watch_frame >= tgp_from && watch_frame <= tgp_to;
        if (tgp_in_window) {
            memcpy(tgp_snap, tgp_ram(), sizeof tgp_snap);
            tgp_ext_count = tgp_ext_bank4 = 0;
            tgp_reads = 0;
        }
        uint32_t ip = b->cpu->ip;
        for (int k = 0; k < 8; k++) {
            if (tgp_cmd[tgp_cur].site[k] == ip) break;
            if (!tgp_cmd[tgp_cur].site[k]) { tgp_cmd[tgp_cur].site[k] = ip; break; }
        }
        if (raw_on())
            fprintf(tgp_raw, "cmd %02x @%08x:", tgp_cur, ip);
    } else if (a - 0x884000u < 0x4000) {
        tgp_words++;
        if (raw_on())
            fprintf(tgp_raw, " %08x", v);
    }
}

static void tgp_w32(void *u, uint32_t a, uint32_t v) { tgp_note(u, a, v); tgp_w32_orig(u, a, v); }
static void tgp_w16(void *u, uint32_t a, uint16_t v) { tgp_note(u, a, v); tgp_w16_orig(u, a, v); }

static void tgp_report(void)
{
    tgp_close_packet();
    if (tgp_from >= 0) {
        printf("TGP command effects, frames %d-%d (packets; words the DSP wrote out and the i960\n"
               "read back per packet; DSP RAM words changed by at least half the packets).\n"
               "The i960 wrote %lu words straight into the display list (0x80xxxx) in the window.\n",
               tgp_from, tgp_to, dl_writes);
        for (int k = 0; k < TGP_CMDS; k++) {
            unsigned long n = tgp_eff[k].packets;
            if (!n) continue;
            printf("  %02x %7lu  out %5.1f  read %4.1f  ram", k, n, (double)tgp_eff[k].ext4 / n,
                   (double)tgp_eff[k].reads / n);
            for (int i = 0; i < 0x400;) {
                if (tgp_eff[k].ram[i] * 2 < n) { i++; continue; }
                int j = i;
                while (j < 0x400 && tgp_eff[k].ram[j] * 2 >= n) j++;
                printf(j - i > 1 ? " %03x-%03x" : " %03x", i, j - 1);
                i = j;
            }
            printf("\n");
        }
    }
    printf("TGP commands (packets, parameter words min/avg/max, sent from):\n");
    for (int k = 0; k < TGP_CMDS; k++) {
        if (!tgp_cmd[k].packets) continue;
        printf("  %02x  %8lu  %3lu/%5.1f/%3lu ", k, tgp_cmd[k].packets, tgp_cmd[k].words_min,
               (double)tgp_cmd[k].words_total / tgp_cmd[k].packets, tgp_cmd[k].words_max);
        for (int j = 0; j < 8 && tgp_cmd[k].site[j]; j++)
            printf(" %06x", tgp_cmd[k].site[j]);
        printf("\n");
    }
}

static void on_slice(void *u)
{
    /* interrupts are entered between slices: the next instruction is an entry */
    m2_board *b = u;
    if (cycles_frame && (cycles_frame < 0 || watch_frame == cycles_frame))
        printf("s %08x\n", b->cpu->ip);
    expect_ip = b->cpu->ip;
}

static void logmsg(const char *m) { fprintf(stderr, "%s\n", m); }

static void shot(m2_board *b, const char *path)
{
    static m2_tilegen tg;
    m2tile_init(&tg, 1, 1, 1);
    m2tile_render(&tg, b);
    FILE *f = fopen(path, "wb");
    if (!f) return;
    fprintf(f, "P6\n%d %d\n255\n", M2_SCREEN_W, M2_SCREEN_H);
    for (int i = 0; i < M2_SCREEN_W * M2_SCREEN_H; i++) {
        uint16_t v = M2_TILE_PEN(tg.layer[0][i]) ? tg.layer[0][i] : tg.layer[1][i];
        uint32_t c = M2_TILE_PEN(v) ? tg.pal[M2_TILE_INDEX(v)] : 0;
        uint8_t rgb[3] = { (uint8_t)c, (uint8_t)(c >> 8), (uint8_t)(c >> 16) };
        fwrite(rgb, 1, 3, f);
    }
    fclose(f);
}

int main(int argc, char **argv)
{
    if (argc < 4) {
        fprintf(stderr, "usage: trace <romdir> <frames> <out.txt> [keys]\n");
        return 1;
    }
    const char *dirs[] = { argv[1], NULL };
    int frames = atoi(argv[2]);
    const char *keys = argc > 4 ? argv[4] : "";
    const char *shots = getenv("TRACE_SHOTS");
    if (getenv("TRACE_CYCLES"))
        cycles_frame = atoi(getenv("TRACE_CYCLES"));
    if (getenv("TRACE_WATCH"))
        watch_addr = (uint32_t)strtoul(getenv("TRACE_WATCH"), NULL, 16);   /* prefix: a picture every 600 frames */

    const m2_game *g = m2_find_game("daytona");
    m2_board *b = m2_create(g, dirs, logmsg);
    if (!b) return 1;
    m2_nvram_defaults(b);
    b->cpu->trace = on_trace;
    if (getenv("TRACE_TGP")) {   /* wrap the geometry FIFO page */
        tgp_board = b;
        tgp_trace_ext = tgp_on_ext;
        i960_read_page *rp = &b->cpu->rpage[0x88];
        tgp_r32_orig = rp->r32; tgp_r16_orig = rp->r16;
        rp->r32 = tgp_r32; rp->r16 = tgp_r16;
        i960_write_page *dp = &b->cpu->wpage[0x80];
        dl_w32_orig = dp->w32;
        dp->w32 = dl_w32;
        if (getenv("TRACE_TGP_FROM")) {
            tgp_from = atoi(getenv("TRACE_TGP_FROM"));
            tgp_to = getenv("TRACE_TGP_TO") ? atoi(getenv("TRACE_TGP_TO")) : tgp_from;
        }
        i960_write_page *p = &b->cpu->wpage[0x88];
        tgp_w32_orig = p->w32;
        tgp_w16_orig = p->w16;
        p->w32 = tgp_w32;
        p->w16 = tgp_w16;
        if (getenv("TRACE_TGP_RAW")) {
            tgp_raw = fopen(getenv("TRACE_TGP_RAW"), "w");
            tgp_raw_left = getenv("TRACE_TGP_RAW_MAX") ? atoi(getenv("TRACE_TGP_RAW_MAX")) : 4000;
        }
    }
    b->hooks.user = b;
    b->hooks.slice_done = on_slice;

    seen = calloc(SEEN_LIMIT / 32, 1);
    capedges = 1 << 20;
    edges = calloc(capedges, sizeof *edges);
    edge_add('E', 0, b->cpu->ip);   /* reset entry */

    m2_input_state ins;
    m2input_init(&ins);
    for (int f = 1; f <= frames; f++) {
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
        watch_frame = f;
        m2_run_frame(b);
        if (shots && f % 600 == 0) {
            char p[512];
            snprintf(p, sizeof p, "%s%05d.ppm", shots, f);
            shot(b, p);
        }
    }

    if (getenv("TRACE_TGP"))
        tgp_report();
    FILE *o = fopen(argv[3], "w");
    if (!o) return 1;
    uint32_t words = 0;
    for (uint32_t w = 0; w < SEEN_LIMIT / 4;) {
        if (!(seen[w >> 3] >> (w & 7) & 1)) { w++; continue; }
        uint32_t s = w;
        while (w < SEEN_LIMIT / 4 && (seen[w >> 3] >> (w & 7) & 1)) w++;
        fprintf(o, "X %08x %08x\n", s * 4, w * 4);
        words += w - s;
    }
    for (size_t i = 0; i < capedges; i++)
        if (edges[i].k)
            fprintf(o, "%c %08x %08x\n", edges[i].k, edges[i].a, edges[i].b);
    fclose(o);
    fprintf(stderr, "%u instruction words executed, %zu edges\n", words, nedges);
    m2_destroy(b);
    return 0;
}
