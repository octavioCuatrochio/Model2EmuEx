/* Replays a recording of the TGP's work in a real game: its state at the
   first recorded frame, then every access the i960 made to it, in order
   (M2_TGP_RECORD=file:from:to in ../../m2/m2run.c). Every value the TGP
   returns is checked against the recording, and the run is timed: the TGP
   alone, on real work, the same every time.

     replay <file> [runs]

   prints the time of the fastest run and a checksum of the final state
   (state block, FIFOs, buffer RAM), which two versions of tgp.c must agree on.

   File: "M2TR", then blobs (u32 size, bytes): state block, input FIFO,
   output FIFO, 11 driver registers, program RAM, data RAM, buffer RAM, table
   ROM, bank ROM; then 9-byte events: op (M2_TREC_* in ../../m2/m2board.h),
   address, value, little-endian. */
#include "../tgp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum {
    PW32 = 1, PW16, PR32, PR16, PR8, CW32, CW16, CR32, UW32, BW8, BW16, BW32
};

static double now(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static uint32_t le32(const uint8_t *p) { return p[0] | p[1] << 8 | p[2] << 16 | (uint32_t)p[3] << 24; }

typedef struct { const uint8_t *p; uint32_t n; } blob;

static int next_blob(const uint8_t **p, const uint8_t *end, blob *b)
{
    if (end - *p < 4) return 0;
    b->n = le32(*p);
    b->p = *p + 4;
    if ((size_t)(end - b->p) < b->n) return 0;
    *p = b->p + b->n;
    return 1;
}

static uint64_t fnv(uint64_t h, const void *p, size_t n)
{
    const uint8_t *c = p;
    while (n--) h = (h ^ *c++) * 1099511628211ull;
    return h;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: replay <file> [runs]\n");
        return 1;
    }
    int runs = argc > 2 ? atoi(argv[2]) : 1;
    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror(argv[1]); return 1; }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *file = malloc((size_t)size);
    if (!file || fread(file, 1, (size_t)size, f) != (size_t)size) { fprintf(stderr, "read failed\n"); return 1; }
    fclose(f);

    const uint8_t *p = file + 4, *end = file + size;
    blob s_blk, s_in, s_out, s_regs, s_prog, s_data, s_buf, s_table, s_rom;
    if (memcmp(file, "M2TR", 4) || !next_blob(&p, end, &s_blk) || !next_blob(&p, end, &s_in) ||
        !next_blob(&p, end, &s_out) || !next_blob(&p, end, &s_regs) || !next_blob(&p, end, &s_prog) ||
        !next_blob(&p, end, &s_data) || !next_blob(&p, end, &s_buf) || !next_blob(&p, end, &s_table) ||
        !next_blob(&p, end, &s_rom) || s_blk.n != TGP_BLK_SIZE || s_in.n != sizeof(tgp_fifo) ||
        s_out.n != sizeof(tgp_fifo) || s_regs.n != 11 * 4 || s_prog.n != 0x40000 || s_data.n != 0x40000 ||
        s_buf.n != 0x80000) {
        fprintf(stderr, "%s: not a TGP recording\n", argv[1]);
        return 1;
    }
    const uint8_t *events = p;
    long nevents = (end - p) / 9;

    uint32_t *prog = malloc(0x40000), *data = malloc(0x40000), *buf = malloc(0x80000);
    uint32_t dirty[4];
    tgp *t = malloc(sizeof *t);
    double best = 1e9;
    long mismatches = 0;
    uint64_t sum = 0;

    for (int r = 0; r < runs; r++) {
        /* the recorded state, as m2board.c sets the TGP up */
        memcpy(prog, s_prog.p, 0x40000);
        memcpy(data, s_data.p, 0x40000);
        memcpy(buf, s_buf.p, 0x80000);
        tgp_init(t);
        memcpy(t->blk, s_blk.p, TGP_BLK_SIZE);
        memcpy(&t->in, s_in.p, sizeof t->in);
        memcpy(&t->out, s_out.p, sizeof t->out);
        const uint8_t *g = s_regs.p;
        t->table_ptr_value = le32(g);       t->prog_ptr_value = le32(g + 4);
        t->prog_upload_ctrl = le32(g + 8);  t->prog_upload_off = le32(g + 12);
        t->data_upload_ctrl = le32(g + 16); t->data_upload_off = le32(g + 20);
        t->reg_98000c = le32(g + 24);       t->ext_ptr = le32(g + 28);
        t->reg_803008 = le32(g + 32);       t->reg_801008_read = le32(g + 36);
        t->unused_573320 = (int)le32(g + 40);
        t->prog = prog;
        t->prog_words = 0x10000;
        t->table_rom = s_table.n ? s_table.p : NULL;
        t->table_rom_size = s_table.n;
        t->ext_ram = buf;
        t->ext_ram_words = 0x20000;
        t->ext_ram_dirty = dirty;
        t->ext_rom = s_rom.n ? (const uint32_t *)s_rom.p : NULL;
        t->ext_rom_words = s_rom.n / 4;
        t->data_buf = data;
        t->data_buf_words = 0x10000;

        uint8_t *bb = (uint8_t *)buf;
        mismatches = 0;
        double t0 = now();
        for (long i = 0; i < nevents; i++) {
            const uint8_t *e = events + i * 9;
            uint32_t a = le32(e + 1), v = le32(e + 5), got;
            switch (e[0]) {
            case PW32: tgp_m2_port_write32(t, a, v); break;
            case PW16: tgp_m2_port_write16(t, a, (uint16_t)v); break;
            case PR32: got = tgp_m2_port_read32(t, a); mismatches += got != v; break;
            case PR16: got = tgp_m2_port_read16(t, a); mismatches += got != v; break;
            case PR8:  got = tgp_m2_port_read8(t, a);  mismatches += got != v; break;
            case CW32: tgp_m2_ctrl_write32(t, a, v); break;
            case CW16: tgp_m2_ctrl_write16(t, a, (uint16_t)v); break;
            case CR32: got = tgp_m2_ctrl_read32(t, a); mismatches += got != v; break;
            case UW32: tgp_m2_upload_write(t, a, v); break;
            /* buffer RAM, as m2board.c's handlers write it */
            case BW8:  bb[a] = (uint8_t)v; break;
            case BW16:
                if (a <= 0x7fffe) { uint16_t h = (uint16_t)v; memcpy(bb + a, &h, 2); } else bb[a] = (uint8_t)v;
                break;
            case BW32:
                if (a <= 0x7fffc) memcpy(bb + a, &v, 4); else memcpy(bb + a, &v, 0x80000 - a);
                break;
            default:
                fprintf(stderr, "bad event %ld\n", i);
                return 1;
            }
        }
        double dt = now() - t0;
        if (dt < best) best = dt;
        sum = fnv(1469598103934665603ull, t->blk, TGP_BLK_SIZE);
        sum = fnv(sum, &t->in, sizeof t->in);
        sum = fnv(sum, &t->out, sizeof t->out);
        sum = fnv(sum, buf, 0x80000);
    }
    printf("%ld events, %ld mismatches, %.3f s (best of %d), state %016llx\n",
           nevents, mismatches, best, runs, (unsigned long long)sum);
    return mismatches != 0;
}
