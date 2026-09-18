/* ROM loading, ported from EMULATOR.EXE 0x4f7c40 (list walker) and
   0x4f7980 (file lookup). Zip access is a small reader on top of zlib. */
#include "m2rom.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <zlib.h>

#include "gamedb.inc"

const m2_game *m2_game_list(int *count)
{
    *count = (int)(sizeof games / sizeof games[0]);
    return games;
}

const m2_game *m2_find_game(const char *name)
{
    for (size_t i = 0; i < sizeof games / sizeof games[0]; i++)
        if (!strcasecmp(games[i].name, name))
            return &games[i];
    return NULL;
}

/* ------------------------------------------------------------ zip reading */

typedef struct {
    FILE *f;
    uint8_t *cd;      /* central directory */
    uint32_t cd_size;
    uint32_t entries;
} zipf;

static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | p[1] << 8); }
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }

static int zip_open(zipf *z, const char *path)
{
    uint8_t tail[65536 + 22];
    long size, n, i;

    memset(z, 0, sizeof *z);
    z->f = fopen(path, "rb");
    if (!z->f)
        return -1;
    fseek(z->f, 0, SEEK_END);
    size = ftell(z->f);
    n = size < (long)sizeof tail ? size : (long)sizeof tail;
    fseek(z->f, size - n, SEEK_SET);
    if (fread(tail, 1, (size_t)n, z->f) != (size_t)n)
        goto fail;
    for (i = n - 22; i >= 0; i--)
        if (rd32(tail + i) == 0x06054b50)
            break;
    if (i < 0)
        goto fail;
    z->entries = rd16(tail + i + 10);
    z->cd_size = rd32(tail + i + 12);
    z->cd = malloc(z->cd_size);
    if (!z->cd)
        goto fail;
    fseek(z->f, (long)rd32(tail + i + 16), SEEK_SET);
    if (fread(z->cd, 1, z->cd_size, z->f) != z->cd_size)
        goto fail;
    return 0;
fail:
    free(z->cd);
    fclose(z->f);
    z->f = NULL;
    return -1;
}

static void zip_close(zipf *z)
{
    if (z->f)
        fclose(z->f);
    free(z->cd);
}

/* Finds an entry by name (case-insensitive, path ignored), else by CRC.
   Returns the offset of its central directory record or -1. */
static long zip_find(const zipf *z, const char *name, uint32_t crc)
{
    uint32_t pos = 0;
    long by_crc = -1;
    for (uint32_t e = 0; e < z->entries && pos + 46 <= z->cd_size; e++) {
        const uint8_t *c = z->cd + pos;
        uint16_t nl = rd16(c + 28), xl = rd16(c + 30), cl = rd16(c + 32);
        char fn[256];
        size_t k = nl < 255 ? nl : 255;
        memcpy(fn, c + 46, k);
        fn[k] = 0;
        const char *base = strrchr(fn, '/');
        base = base ? base + 1 : fn;
        if (!strcasecmp(base, name))
            return (long)pos;
        if (by_crc < 0 && crc && rd32(c + 16) == crc)
            by_crc = (long)pos;
        pos += 46u + nl + xl + cl;
    }
    return by_crc;
}

static int zip_read(zipf *z, long cdpos, uint8_t *dst, uint32_t size)
{
    const uint8_t *c = z->cd + cdpos;
    uint16_t method = rd16(c + 10);
    uint32_t csize = rd32(c + 20), usize = rd32(c + 24), loc = rd32(c + 42);
    uint8_t lh[30];
    int ok = -1;

    if (usize != size)
        return -1;
    fseek(z->f, (long)loc, SEEK_SET);
    if (fread(lh, 1, 30, z->f) != 30 || rd32(lh) != 0x04034b50)
        return -1;
    fseek(z->f, (long)loc + 30 + rd16(lh + 26) + rd16(lh + 28), SEEK_SET);
    if (method == 0)
        return fread(dst, 1, size, z->f) == size ? 0 : -1;
    if (method != 8)
        return -1;

    uint8_t *src = malloc(csize);
    if (!src)
        return -1;
    if (fread(src, 1, csize, z->f) == csize) {
        z_stream st;
        memset(&st, 0, sizeof st);
        if (inflateInit2(&st, -MAX_WBITS) == Z_OK) {
            st.next_in = src;
            st.avail_in = csize;
            st.next_out = dst;
            st.avail_out = size;
            if (inflate(&st, Z_FINISH) == Z_STREAM_END && st.total_out == size)
                ok = 0;
            inflateEnd(&st);
        }
    }
    free(src);
    return ok;
}

/* orig 0x4f7980: game zip, loose directory, then the parent set */
static int find_file(const m2_game *g, const char *const *dirs, const char *file,
                     uint32_t crc, uint32_t size, uint8_t *dst)
{
    const char *sets[3] = { g->name, g->parent, g->shared };
    char path[1024];

    for (int s = 0; s < 3; s++) {
        if (!sets[s] || (s > 0 && !strcmp(sets[s], sets[s - 1])))
            continue;
        for (int d = 0; dirs[d]; d++) {
            zipf z;
            snprintf(path, sizeof path, "%s/%s.zip", dirs[d], sets[s]);
            if (!zip_open(&z, path)) {
                long e = zip_find(&z, file, crc);
                int ok = -1;
                if (e >= 0) {
                    const uint8_t *c = z.cd + e;
                    if (rd32(c + 24) == size && (!crc || rd32(c + 16) == crc))
                        ok = zip_read(&z, e, dst, size);
                }
                zip_close(&z);
                if (!ok)
                    return 0;
            }
            snprintf(path, sizeof path, "%s/%s/%s", dirs[d], sets[s], file);
            FILE *f = fopen(path, "rb");
            if (f) {
                size_t n = fread(dst, 1, size, f);
                fclose(f);
                if (n == size)
                    return 0;
            }
        }
    }
    return -1;
}

int m2_load_roms(const m2_game *g, const char *const *dirs, m2_regions *r,
                 void (*log)(const char *msg))
{
    int missing = 0;
    unsigned cur = 0;
    char msg[256];

    memset(r, 0, sizeof *r);
    for (const m2_rom_entry *e = g->roms; e->type >= 0; e++) {
        switch (e->type) {
        case 0:
            cur = e->arg & 0xff;
            free(r->ptr[cur]);
            r->size[cur] = e->offset;
            r->ptr[cur] = calloc(1, e->offset ? e->offset : 1);
            if (!r->ptr[cur])
                return -1;
            break;
        case 1: {
            uint8_t *buf = malloc(e->size);
            if (!buf)
                return -1;
            if (find_file(g, dirs, e->file, e->crc, e->size, buf)) {
                missing = 1;
                if (log) {
                    snprintf(msg, sizeof msg, "Unable to load. FileName: %s Size: %u CRC: %X",
                             e->file, e->size, e->crc);
                    log(msg);
                }
            } else {
                uint32_t chunk = e->flags & 0xff, skip = (e->flags >> 8) & 0xff;
                uint64_t dst = e->offset;
                if (!chunk)
                    chunk = 1;
                for (uint32_t done = 0; done < e->size; done += chunk) {
                    uint32_t n = e->size - done < chunk ? e->size - done : chunk;
                    if (dst + n <= r->size[cur])
                        memcpy(r->ptr[cur] + dst, buf + done, n);
                    dst += chunk + skip;
                }
            }
            free(buf);
            break;
        }
        case 2:
            if ((uint64_t)e->arg + e->offset <= r->size[cur])
                memset(r->ptr[cur] + e->arg, (int)(e->size & 0xff), e->offset);
            break;
        case 3:
            if ((uint64_t)e->offset + e->size <= r->size[cur] &&
                (uint64_t)e->arg + e->size <= r->size[cur])
                memmove(r->ptr[cur] + e->offset, r->ptr[cur] + e->arg, e->size);
            break;
        case 4:
            r->option[e->arg & 0xff & (M2_OPTIONS - 1)] = e->offset;
            break;
        default:
            break;
        }
    }
    if (missing && log)
        log("There are missing files. Load aborted");
    return missing ? -1 : 0;
}

void m2_free_roms(m2_regions *r)
{
    for (int i = 0; i < M2_REGIONS; i++)
        free(r->ptr[i]);
    memset(r, 0, sizeof *r);
}
