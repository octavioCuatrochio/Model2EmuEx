/* Model 2 / 2A board, ported from Model 2 Emulator 1.1a. See m2board.h and
   QUIRKS.md; "orig 0x4cXXXX" gives the address of the ported code. */
#include "m2board.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define B(u) ((m2_board *)(u))

static inline uint16_t ld16(const uint8_t *p) { uint16_t v; memcpy(&v, p, 2); return v; }
static inline uint32_t ld32(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v; }
static inline void st16(uint8_t *p, uint16_t v) { memcpy(p, &v, 2); }
static inline void st32(uint8_t *p, uint32_t v) { memcpy(p, &v, 4); }

/* Handlers of the IRQ and timer pages use the start address of the current
   multi-word access (ldq/stq...) for every word, like the original. */
static uint32_t eff_addr(m2_board *b, uint32_t a)
{
    return b->cpu->multi_count ? b->cpu->multi_addr : a;
}

/* ------------------------------------------------------------- EEPROM */

static const struct {
    int addr_bits, data_bits;
    const char *cmd_read, *cmd_write, *cmd_erase, *cmd_lock, *cmd_unlock;
} ee_intf = { 6, 16, "110", "101", "10001", "10000", "10011" };   /* orig 0x574dac */

static void ee_init(m2_eeprom *e)       /* orig 0x4ecde0 */
{
    memset(e, 0, sizeof *e);
    memset(e->data, 0xff, sizeof e->data);
    e->reset_line = 1;
    e->clock_line = 1;
    e->locked = 1;
}

static uint32_t ee_word(const m2_eeprom *e, uint32_t addr)
{
    return ld16(e->data + (addr & 63) * 2);
}

static uint32_t ee_bits(const m2_eeprom *e, int from, int n)
{
    uint32_t v = 0;
    for (int i = 0; i < n; i++)
        v = v << 1 | (e->serial_buffer[from + i] == '1');
    return v;
}

static int ee_match(const m2_eeprom *e, const char *cmd, int extra)
{
    int n = (int)strlen(cmd);
    return e->serial_count == n + extra && !strncmp(e->serial_buffer, cmd, (size_t)n);
}

static void ee_write_bit(m2_eeprom *e, int bit)   /* orig 0x4eca60 */
{
    if (e->serial_count >= 0x27)
        return;
    e->serial_buffer[e->serial_count] = bit ? '1' : '0';
    if (!e->wait_start || e->serial_buffer[0] == '1')
        e->serial_count++;
    e->serial_buffer[e->serial_count] = 0;

    int ab = ee_intf.addr_bits, db = ee_intf.data_bits;
    if (ee_match(e, ee_intf.cmd_read, ab)) {
        uint32_t a = ee_bits(e, (int)strlen(ee_intf.cmd_read), ab);
        e->data_bits = ee_word(e, a);
        e->read_address = a;
        e->clock_count = 0;
        e->sending = 1;
        e->serial_count = 0;
        return;
    }
    if (ee_match(e, ee_intf.cmd_erase, ab)) {
        uint32_t a = ee_bits(e, (int)strlen(ee_intf.cmd_erase), ab);
        if (!e->locked) {
            st16(e->data + (a & 63) * 2, 0);   /* no count reset (QUIRKS.md) */
            return;
        }
        e->serial_count = 0;
        return;
    }
    if (ee_match(e, ee_intf.cmd_write, ab + db)) {
        int n = (int)strlen(ee_intf.cmd_write);
        uint32_t a = ee_bits(e, n, ab), v = ee_bits(e, n + ab, db);
        if (!e->locked) {
            st16(e->data + (a & 63) * 2, (uint16_t)v);
            return;
        }
        e->serial_count = 0;
        return;
    }
    if (ee_match(e, ee_intf.cmd_lock, 0)) {
        e->serial_count = 0;
        e->locked = 1;
        return;
    }
    if (ee_match(e, ee_intf.cmd_unlock, 0)) {
        e->locked = 0;
        e->serial_count = 0;
    }
}

static void ee_set_clock(m2_eeprom *e, int state)   /* orig 0x4ece50 */
{
    if ((state == 3 || (!e->clock_line && state)) && !e->reset_line) {
        if (e->sending) {
            e->data_bits = e->data_bits << 1 | 1;
            e->clock_count++;
            e->clock_line = state;
            return;
        }
        ee_write_bit(e, e->latch);
    }
    e->clock_line = state;
}

static int ee_read_bit(m2_eeprom *e)   /* orig 0x4eca10 */
{
    if (e->sending)
        return (int)((e->data_bits >> ee_intf.data_bits) & 1);
    if (e->reset_delay > 0) {
        e->reset_delay--;
        return 0;
    }
    return 1;
}

/* ------------------------------------------------------ default handlers */

static uint8_t  r8_ff(void *u, uint32_t a)  { (void)u; (void)a; return 0xff; }
static uint16_t r16_ffff(void *u, uint32_t a) { (void)u; (void)a; return 0xffff; }
static uint32_t r32_ffffffff(void *u, uint32_t a) { (void)u; (void)a; return 0xffffffff; }
static uint8_t  r8_0(void *u, uint32_t a)  { (void)u; (void)a; return 0; }
static uint16_t r16_0(void *u, uint32_t a) { (void)u; (void)a; return 0; }
static void w8_nop(void *u, uint32_t a, uint8_t v)   { (void)u; (void)a; (void)v; }
static void w16_nop(void *u, uint32_t a, uint16_t v) { (void)u; (void)a; (void)v; }
static void w32_nop(void *u, uint32_t a, uint32_t v) { (void)u; (void)a; (void)v; }

/* ------------------------------------------- 0x240000 (Model 2 original) */

/* orig 0x4c85b0 / 0x4c8560 / 0x4c8410: program ROM, with a fake status */
static uint8_t r8_240(void *u, uint32_t a)
{
    m2_board *b = B(u);
    uint32_t o = a & 0xfffff;
    return o < b->rom.size[0] ? b->rom.ptr[0][o] : 0;
}

static uint16_t r16_240(void *u, uint32_t a)
{
    m2_board *b = B(u);
    uint32_t o = a & 0xfffff;
    if (o - 0x40000 > 0x1f)
        return o + 2 <= b->rom.size[0] ? ld16(b->rom.ptr[0] + o) : 0;
    uint32_t n = b->snd_status_count + 1;
    b->snd_status_count = n & 0xf;
    if (!b->snd_status_count)
        return 7;
    return (n & 2) ? 0 : 4;
}

static uint32_t r32_240(void *u, uint32_t a)
{
    m2_board *b = B(u);
    uint32_t o = a & 0xfffff;
    if (o == 0x40022)
        return 0xff;
    return o + 4 <= b->rom.size[0] ? ld32(b->rom.ptr[0] + o) : 0;
}

/* ------------------------------------------------ copro (TGP) ports */

static uint32_t r32_copro_ctrl(void *u, uint32_t a) { return tgp_m2_ctrl_read32(&B(u)->tgp, a); }
static void w16_copro_ctrl(void *u, uint32_t a, uint16_t v) { tgp_m2_ctrl_write16(&B(u)->tgp, a, v); }
static void w32_copro_ctrl(void *u, uint32_t a, uint32_t v) { tgp_m2_ctrl_write32(&B(u)->tgp, a, v); }
static uint8_t  r8_copro_fifo(void *u, uint32_t a)  { return tgp_m2_port_read8(&B(u)->tgp, a); }
static uint16_t r16_copro_fifo(void *u, uint32_t a) { return tgp_m2_port_read16(&B(u)->tgp, a); }
static uint32_t r32_copro_fifo(void *u, uint32_t a) { return tgp_m2_port_read32(&B(u)->tgp, a); }
static void w16_copro_fifo(void *u, uint32_t a, uint16_t v) { tgp_m2_port_write16(&B(u)->tgp, a, v); }
static void w32_copro_fifo(void *u, uint32_t a, uint32_t v) { tgp_m2_port_write32(&B(u)->tgp, a, v); }

static void w32_8c0000(void *u, uint32_t a, uint32_t v)   /* orig 0x4c7310 */
{
    if (a == 0x8c0010)
        tgp_m2_port_write32(&B(u)->tgp, a, v);
}

/* 0x980000: copro status and upload control */
static uint32_t status_98000c(m2_board *b, int count_idle)
{
    if (count_idle && ++b->idle_count > 9)
        b->cpu->stop = 1;
    return (b->tgp.reg_98000c & 3) | (b->frame_flags & 4);
}

static uint8_t r8_980(void *u, uint32_t a)    /* orig 0x4c8830 */
{
    m2_board *b = B(u);
    if (a == 0x98000c) return (uint8_t)status_98000c(b, 1);
    if (a == 0x980014) return b->tgp.prog_upload_off ? 0 : 0xff;
    if (a == 0x980004) return b->tgp.out.free == 32;
    return 0;
}

static uint16_t r16_980(void *u, uint32_t a)  /* orig 0x4c8800 */
{
    m2_board *b = B(u);
    if (a != 0x980004) return 0xffff;
    return b->tgp.out.free == 32;
}

static uint32_t r32_980_m2(void *u, uint32_t a)   /* orig 0x4c7a80, Model 2 */
{
    m2_board *b = B(u);
    if (a == 0x980000) return b->tgp.prog_upload_ctrl;
    if (a == 0x980004) return 0xffffffff;
    if (a == 0x900008) return b->tgp.data_upload_ctrl;   /* typo in the original */
    if (a == 0x98000c) return status_98000c(b, 0);
    return a == 0x9c0004 ? 5 : 0;
}

static const uint32_t copro_ident[13] = {   /* orig 0x574d08 */
    0x48415400, 0x4f4b4100, 0x4b415a00, 0x4b544d00, 0x11546643, 0x8eccfe99, 0x891187dd,
    0x0944df22, 0x1e1b4e7f, 0x49f548a8, 0x094a3231, 0x41c02989, 0x2485493a };

static uint32_t r32_980_m2a(void *u, uint32_t a)  /* orig 0x4c8770, Model 2A */
{
    m2_board *b = B(u);
    if (a == 0x980000) return b->tgp.prog_upload_ctrl;
    if (a == 0x980004) return b->tgp.out.free == 32;
    if (a == 0x900008) return b->tgp.data_upload_ctrl;
    if (a == 0x98000c) return status_98000c(b, 1);
    if (a - 0x980030 < 0x31) {
        uint32_t o = a - 0x980030;   /* byte offset into the table */
        uint8_t tab[sizeof copro_ident + 4] = { 0 };
        memcpy(tab, copro_ident, sizeof copro_ident);
        return o + 4 <= sizeof tab ? ld32(tab + o) : 0;
    }
    return 0;
}

static void w32_980(void *u, uint32_t a, uint32_t v) { tgp_m2_upload_write(&B(u)->tgp, a, v); }

/* ------------------------------------------------ sound board ports */

static uint8_t r_sound(m2_board *b, uint32_t a)   /* orig 0x4c7650 */
{
    uint8_t lo = (uint8_t)a;
    if (lo == 4 || lo == 2)
        return 7;
    if (b->sound_count)
        b->sound_count--;
    return 0xff;
}

static uint8_t  r8_sound(void *u, uint32_t a)  { return r_sound(B(u), a); }
static uint16_t r16_sound(void *u, uint32_t a) { return r_sound(B(u), a); }
static uint32_t r32_sound(void *u, uint32_t a) { return r_sound(B(u), a); }

static void w_sound(m2_board *b, uint32_t a, uint8_t v)   /* orig 0x4cb570 */
{
    if ((uint8_t)a)
        return;
    b->sound_irqs++;
    if (b->type == 0) {
        b->sound_count++;
        b->sound_fifo[b->sound_wr] = v;
        b->sound_wr = (b->sound_wr + 1) & 31;
    } else {
        b->sound_count++;
    }
    if (b->hooks.sound_command)
        b->hooks.sound_command(b->hooks.user, v);
}

static void w8_sound(void *u, uint32_t a, uint8_t v)   { w_sound(B(u), a, v); }
static void w16_sound(void *u, uint32_t a, uint16_t v) { w_sound(B(u), a, (uint8_t)v); }
static void w32_sound(void *u, uint32_t a, uint32_t v) { B(u)->sound_word = v; w_sound(B(u), a, (uint8_t)v); }

/* ---------------------------------------------- IRQ controller, timers */

static uint32_t r32_irq(void *u, uint32_t a)   /* orig 0x4c7c70 */
{
    m2_board *b = B(u);
    return eff_addr(b, a) == 0xe80000 ? b->irq_pending : b->irq_enable;
}

static void w32_irq(void *u, uint32_t a, uint32_t v)   /* orig 0x4c7c10 */
{
    m2_board *b = B(u);
    a = eff_addr(b, a);
    if (a == 0xe80000)
        b->irq_pending &= v;
    else if (a == 0xe80004)
        b->irq_enable = v;
}

static uint32_t r32_timer(void *u, uint32_t a)   /* orig 0x4c7db0 */
{
    m2_board *b = B(u);
    return b->timer[(eff_addr(b, a) >> 2) & 0x3f] & 0xffffff;
}

static void w32_timer(void *u, uint32_t a, uint32_t v)   /* orig 0x4c7d80 */
{
    m2_board *b = B(u);
    b->timer[(eff_addr(b, a) >> 2) & 0x3f] = v;
}

static void run_timers(m2_board *b, int32_t dec)   /* orig 0x4c8390 */
{
    for (int n = 0; n < 4; n++) {
        int32_t t = (int32_t)b->timer[n];
        if (t >= 0) {
            t -= dec;
            if (t < 0) {
                b->irq_pending |= 4u << n;
                t = -1;
            }
            b->timer[n] = (uint32_t)t;
        }
    }
}

static void irq_dispatch(m2_board *b)   /* orig 0x4cc670 */
{
    static const int line[12] = { 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3 };
    if (b->cpu->in_interrupt)
        return;
    for (int i = 0; i < 12; i++)
        if ((b->irq_enable & b->irq_pending) >> i & 1) {
            i960_interrupt(b->cpu, line[i]);
            return;
        }
}

/* --------------------------------------------------- video memory */

#define HOOK(name, ...) do { if (b->hooks.name) b->hooks.name(b->hooks.user, ##__VA_ARGS__); } while (0)

/* tilemaps 0x1000000-0x102ffff, orig 0x4cc3f0 / 0x4cc390 / 0x4cc300 */
/* every tile RAM write is reported (the tile generator skips work when
   nothing changed); the original only re-decodes for maps and window masks */
static void tile_note(m2_board *b, uint32_t a)
{
    HOOK(tilemap_written, a & 0xffff);
}

/* the 0x1100000 mirror is plain memory in the original: no re-decode. It is
   reported with bit 16 set, so a renderer can still see the change. */
static void w8_tilem(void *u, uint32_t a, uint8_t v)   { m2_board *b = B(u); b->tile[a & 0xffff] = v; HOOK(tilemap_written, (a & 0xffff) | 0x10000); }
static void w16_tilem(void *u, uint32_t a, uint16_t v) { m2_board *b = B(u); if ((a & 0xffff) <= 0xfffe) st16(b->tile + (a & 0xffff), v); HOOK(tilemap_written, (a & 0xffff) | 0x10000); }
static void w32_tilem(void *u, uint32_t a, uint32_t v) { m2_board *b = B(u); if ((a & 0xffff) <= 0xfffc) st32(b->tile + (a & 0xffff), v); HOOK(tilemap_written, (a & 0xffff) | 0x10000); }
static void w8_tile(void *u, uint32_t a, uint8_t v)   { m2_board *b = B(u); b->tile[a & 0xffff] = v; tile_note(b, a); }
static void w16_tile(void *u, uint32_t a, uint16_t v) { m2_board *b = B(u); if ((a & 0xffff) <= 0xfffe) st16(b->tile + (a & 0xffff), v); tile_note(b, a); }
static void w32_tile(void *u, uint32_t a, uint32_t v)
{
    m2_board *b = B(u);
    if ((a & 0xffff) <= 0xfffc) st32(b->tile + (a & 0xffff), v);
    tile_note(b, a);
    tile_note(b, a + 2);
}

/* character generator 0x1080000, orig 0x4c76c0 / 0x4c76a0 / 0x4c7680 */
static void w8_cg(void *u, uint32_t a, uint8_t v)   { m2_board *b = B(u); b->cg[a & 0x7ffff] = v; HOOK(cg_written); }
static void w16_cg(void *u, uint32_t a, uint16_t v) { m2_board *b = B(u); st16(b->cg + (a & 0x7ffff), v); HOOK(cg_written); }
static void w32_cg(void *u, uint32_t a, uint32_t v) { m2_board *b = B(u); st32(b->cg + (a & 0x7ffff), v); HOOK(cg_written); }

/* palette 0x1800000, orig 0x4c7020 / 0x4c6fe0 / 0x4c6fa0 */
static void w8_pal(void *u, uint32_t a, uint8_t v)   { m2_board *b = B(u); b->pal[a & 0x3fff] = v; HOOK(palette_written, a & 0x3fff); }
static void w16_pal(void *u, uint32_t a, uint16_t v) { m2_board *b = B(u); st16(b->pal + (a & 0x3fff), v); HOOK(palette_written, a & 0x3fff); }
static void w32_pal(void *u, uint32_t a, uint32_t v) { m2_board *b = B(u); st32(b->pal + (a & 0x3fff), v); HOOK(palette_written, a & 0x3fff); }

/* colour translation 0x1810000, orig 0x4c7080... / 0x4c70e0... */
static uint8_t  r8_xlat(void *u, uint32_t a)  { return B(u)->xlat[a & 0xffff]; }
static uint16_t r16_xlat(void *u, uint32_t a) { return ld16(B(u)->xlat + (a & 0xffff)); }
static uint32_t r32_xlat(void *u, uint32_t a) { return ld32(B(u)->xlat + (a & 0xffff)); }
static void w8_xlat(void *u, uint32_t a, uint8_t v)   { m2_board *b = B(u); st16(b->xlat + (a & 0xffff), v); HOOK(xlat_written); }  /* 16-bit store */
static void w16_xlat(void *u, uint32_t a, uint16_t v) { m2_board *b = B(u); st16(b->xlat + (a & 0xffff), v); HOOK(xlat_written); }
static void w32_xlat(void *u, uint32_t a, uint32_t v) { m2_board *b = B(u); st32(b->xlat + (a & 0xffff), v); HOOK(xlat_written); }

/* textures: 0x11000000/0x11200000 byte-addressed, 0x12000000/0x12400000 at half
   the address; orig 0x4c72a0... 0x4c7230... 0x4c71c0... 0x4c7150... */
#define TEX_W(name, buf, shift, bank) \
    static void w8_##name(void *u, uint32_t a, uint8_t v)   { m2_board *b = B(u); uint32_t o = (a >> shift) & 0xfffff; b->buf[o] = v; HOOK(texture_written, bank, o); } \
    static void w16_##name(void *u, uint32_t a, uint16_t v) { m2_board *b = B(u); uint32_t o = (a >> shift) & 0xfffff; st16(b->buf + o, v); HOOK(texture_written, bank, o); } \
    static void w32_##name(void *u, uint32_t a, uint32_t v) { m2_board *b = B(u); uint32_t o = (a >> shift) & 0xfffff; st32(b->buf + o, v); HOOK(texture_written, bank, o); }
TEX_W(tex0, tex0, 0, 0)
TEX_W(tex1, tex1, 0, 1)
TEX_W(tex0h, tex0, 1, 0)
TEX_W(tex1h, tex1, 1, 1)

/* luminance 0x11400000 (byte-addressed) and 0x12800000 (half address) */
static void w8_luma(void *u, uint32_t a, uint8_t v)   { m2_board *b = B(u); b->luma[a & 0x1ffff] = v; HOOK(luma_written); }
static void w16_luma(void *u, uint32_t a, uint16_t v) { m2_board *b = B(u); st16(b->luma + (a & 0x1ffff), v); HOOK(luma_written); }
static void w32_luma(void *u, uint32_t a, uint32_t v) { m2_board *b = B(u); st32(b->luma + (a & 0x1ffff), v); HOOK(luma_written); }
static void w8_lumah(void *u, uint32_t a, uint8_t v)   { m2_board *b = B(u); b->luma[(a >> 1) & 0x1ffff] = v; HOOK(luma_written); }
static void w16_lumah(void *u, uint32_t a, uint16_t v) { m2_board *b = B(u); st16(b->luma + ((a >> 1) & 0x1ffff), v); HOOK(luma_written); }
static void w32_lumah(void *u, uint32_t a, uint32_t v)   /* orig 0x4c85d0: two 16-bit stores */
{
    m2_board *b = B(u);
    uint32_t o = (a & 0xfffff) >> 1;
    st16(b->luma + (o & 0x1ffff), (uint16_t)v);
    HOOK(luma_written);
    st16(b->luma + ((o + 2) & 0x1ffff), (uint16_t)(v >> 16));
    HOOK(luma_written);
}

/* CRTC sync registers (tilemap chip xhout/xvout); they set where the 3D
   image lands on screen (MAME model2_v.cpp) */
static void w16_hsync(void *u, uint32_t a, uint16_t v) { (void)a; B(u)->hsync = (int16_t)v; }
static void w32_hsync(void *u, uint32_t a, uint32_t v) { (void)a; B(u)->hsync = (int16_t)v; }
static void w16_vsync(void *u, uint32_t a, uint16_t v) { (void)a; B(u)->vsync = (int16_t)v; }
static void w32_vsync(void *u, uint32_t a, uint32_t v) { (void)a; B(u)->vsync = (int16_t)v; }

/* 0x10000000: display control; 0x10800000: status read from RAM2 */
static void w32_disp(void *u, uint32_t a, uint32_t v) { if (!(a & 0xfffff)) B(u)->display_ctrl = v; }
static void w16_disp(void *u, uint32_t a, uint16_t v) { if (!(a & 0xfffff)) B(u)->display_ctrl = v; }
static void w8_disp(void *u, uint32_t a, uint8_t v)   { if (!(a & 0xfffff)) B(u)->display_ctrl = v; }
static uint16_t r16_1080(void *u, uint32_t a) { (void)a; return ld16(B(u)->ram2 + 0xb940); }   /* orig 0x4c6e80 */

/* network board 0x1a00000 (orig 0x4c7ba0 ... 0x4c7ae0); the shared RAM view
   starts at M2ZSHARED + 0x8000 */
static uint8_t *zsh(m2_board *b) { return b->zshared + 0x8000; }
static uint8_t r8_net(void *u, uint32_t a)
{
    m2_board *b = B(u);
    if (!b->net_enabled) return 0xff;
    if ((a & 0xffff) < 0x4000) return zsh(b)[a & 0xffff];
    if ((a & 3) == 0) return b->net_reg0;
    if ((a & 3) == 2) return b->net_reg2;
    return 0xff;
}
static uint16_t r16_net(void *u, uint32_t a) { return ld16(zsh(B(u)) + (a & 0xffff)); }
static uint32_t r32_net(void *u, uint32_t a) { return ld32(zsh(B(u)) + (a & 0xffff)); }
static void w8_net(void *u, uint32_t a, uint8_t v)
{
    m2_board *b = B(u);
    if ((a & 0xffff) < 0x4000) { zsh(b)[a & 0xffff] = v; return; }
    if ((a & 3) == 0) b->net_reg0 = v;
    else if ((a & 3) == 2) b->net_reg2 = (uint8_t)((b->net_reg2 & 0xfe) | (v & 1));
}
static void w16_net(void *u, uint32_t a, uint16_t v) { if ((a & 0xffff) < 0x4000) st16(zsh(B(u)) + (a & 0xffff), v); }
static void w32_net(void *u, uint32_t a, uint32_t v) { if ((a & 0xffff) < 0x4000) st32(zsh(B(u)) + (a & 0xffff), v); }

/* IAC messages to the i960 (0xff000010), orig 0x4c7be0 */
static void w32_iac(void *u, uint32_t a, uint32_t v)
{
    if ((a & 0xfffffff0) == 0xff000010)
        i960_iac_write(B(u)->cpu, a, v);
}

/* ----------------------------------------------------------- I/O 0x1c00000 */

static uint32_t io_read(m2_board *b, uint32_t a)   /* orig 0x4c88a0 */
{
    uint32_t lo;
    a &= 0xffff;
    switch (a) {
    case 0: case 6: case 0x22: return b->port[2];
    case 2:
        if (b->eeprom_select)
            return (uint32_t)(ee_read_bit(&b->eeprom) << 5) | (b->port[0] & 0xdf) | 0xd0;
        return b->port[0];
    case 4:    return b->port[1];
    case 8:    return b->port[3];
    case 0xc:  return b->port[4];
    case 0x10: return b->port[5];
    case 0x12: return b->port[6];
    case 0x14: return b->port[2] ? b->port[2] : 0xff;
    case 0x16: return 0xff;
    case 0x18:
        switch (b->gun_mux - 1) {
        case 0: return b->gun_xy[1] + 0x56;
        case 1: return (b->gun_xy[1] + 0x56) >> 8;
        case 2: return b->gun_xy[0] - 0x53;
        case 3: return (b->gun_xy[0] + 0xad) >> 8;
        case 4: return b->gun_xy[3] + 0x56;
        case 5: return (b->gun_xy[3] + 0x56) >> 8;
        case 6: return b->gun_xy[2] - 0x53;
        case 7: return (b->gun_xy[2] + 0xad) >> 8;
        case 8: return ~b->gun_word;
        default: return 0xff;
        }
    case 0x1c: return b->type > 0 ? 0x1a : 0xff;
    case 0x1e: {
        uint8_t i = b->analog_index++;
        return i < sizeof b->analog ? b->analog[i] : 0;
    }
    case 0x24: case 0x40: return 0;
    case 0x42: return 0x40;
    default:
        break;
    }
    if (a < 0x200) {
        if (b->type != 0)
            return 0xff;
        lo = a & 0xff;
        if (lo <= 0x10 && !(lo & 1))
            return b->analog[lo / 2];
        return 0;
    }
    if (b->type < 1)
        return b->back[0x7e00 + a];
    return 0xff;
}

static uint32_t io_read_t(m2_board *b, uint32_t a)
{
    uint32_t v = io_read(b, a);
    if (b->hooks.io_read)
        b->hooks.io_read(b->hooks.user, a, v);
    return v;
}
static uint8_t  r8_io(void *u, uint32_t a)  { return (uint8_t)io_read_t(B(u), a); }
static uint16_t r16_io(void *u, uint32_t a) { return (uint8_t)io_read_t(B(u), a); }
/* the 32-bit handler returns the byte in al over a leftover code address */
static uint32_t r32_io(void *u, uint32_t a) { return 0x4c5200u | (uint8_t)io_read_t(B(u), a); }

static void drive_board(m2_board *b, uint8_t v)   /* orig 0x4cb67f... */
{
    uint32_t kind = b->rom.option[2];
    if (v == 0x83 || v == 0 || kind == 0 || b->drive_last == v)
        return;
    b->drive_last = v;
    if (b->hooks.drive_command)
        b->hooks.drive_command(b->hooks.user, (int)(v & 0xf0), v & 0xf);
}

static void io_write8(m2_board *b, uint32_t a, uint8_t v)   /* orig 0x4cb5e0 */
{
    a &= 0xffff;
    if (a == 0) {
        b->eeprom_select = v & 1;
        b->eeprom.latch = (v >> 5) & 1;
        ee_set_clock(&b->eeprom, v >> 7);
        b->eeprom.reset_line = ~(v >> 6) & 1;
        if (b->eeprom.reset_line) {
            b->eeprom.serial_count = 0;
            b->eeprom.sending = 0;
            b->eeprom.reset_delay = 5;
        }
        return;
    }
    if (a == 0x1e) { b->analog_index = v & 7; return; }
    if (a == 0x12) { if (v == 0x87) b->gun_mux++; return; }
    if (a == 0x14) { b->gun_mux = v; return; }
    if (a == 8) {
        b->reg_574cf4 = v;
    } else if (a == 10) {
        b->reg_574cf0 = v;
        return;
    } else if (a != 0x22) {
        if (a >= 0x200 && b->type <= 0)
            b->back[0x7e00 + a] = v;   /* can run past M2BACK (QUIRKS.md) */
        return;
    }
    drive_board(b, v);
}

static void io_write16(m2_board *b, uint32_t a, uint16_t v)   /* orig 0x4c86c0 */
{
    uint8_t lo = (uint8_t)v;
    a &= 0xffff;
    if (a == 0) { b->eeprom_select = v & 1; return; }
    if (a == 0x1e) { b->analog_index = lo & 7; return; }
    if (a == 8) {
        b->reg_574cf4 = v;
    } else if (a == 10) {
        b->reg_574cf0 = v;
        return;
    } else if (a != 0x22) {
        if (a > 0x1ff && b->type < 1) {
            if (a != 0x202 || v != 0x4d)
                b->back[0x7e00 + a] = lo;
        }
        return;
    }
    drive_board(b, lo);
}

static void w8_io(void *u, uint32_t a, uint8_t v)   { io_write8(B(u), a, v); }
static void w16_io(void *u, uint32_t a, uint16_t v) { io_write16(B(u), a, v); }

/* ------------------------------------------------------------ memory map */

static void map_r(m2_board *b, uint32_t page, uint32_t n, uint8_t *host,
                  i960_read8_fn r8, i960_read16_fn r16, i960_read32_fn r32)
{
    for (uint32_t i = 0; i < n; i++) {
        i960_read_page *p = &b->cpu->rpage[page + i];
        p->host = host ? host + ((size_t)i << 16) : NULL;
        p->r8 = r8; p->r16 = r16; p->r32 = r32;
    }
}

static void map_w(m2_board *b, uint32_t page, uint32_t n, uint8_t *host,
                  i960_write8_fn w8, i960_write16_fn w16, i960_write32_fn w32)
{
    for (uint32_t i = 0; i < n; i++) {
        i960_write_page *p = &b->cpu->wpage[page + i];
        p->host = host ? host + ((size_t)i << 16) : NULL;
        p->w8 = w8; p->w16 = w16; p->w32 = w32;
    }
}

/* host pointer for a page whose first address maps to buf[off] */
#define MIRROR(buf, off) ((buf) + (off))

static void setup_map(m2_board *b)   /* orig 0x4cd450 */
{
    m2_regions *r = &b->rom;

    map_r(b, 0, 0x10000, NULL, r8_ff, r16_ffff, r32_ffffffff);
    map_w(b, 0, 0x10000, NULL, w8_nop, w16_nop, w32_nop);

    /* program ROM from 0 */
    map_r(b, 0, (r->size[0] >> 16) + 1, r->ptr[0], NULL, NULL, NULL);
    if (b->type == 0) {
        map_r(b, 0x20, 2, b->ram2, NULL, NULL, NULL);
        map_r(b, 0x22, 2, r->ptr[0] + 0x20000, NULL, NULL, NULL);
        map_r(b, 0x24, 1, NULL, r8_240, r16_240, r32_240);
    } else {
        map_r(b, 0x20, 4, b->ram2, NULL, NULL, NULL);
    }
    map_w(b, 0x20, 4, b->ram2, NULL, NULL, NULL);
    map_r(b, 0x50, 0x10, b->ram, NULL, NULL, NULL);
    map_w(b, 0x50, 0x10, b->ram, NULL, NULL, NULL);

    /* copro */
    map_r(b, 0x80, 8, NULL, r8_0, r16_0, r32_copro_ctrl);
    map_w(b, 0x80, 1, NULL, w8_nop, w16_copro_ctrl, w32_copro_ctrl);
    map_r(b, 0x88, 1, NULL, r8_copro_fifo, r16_copro_fifo, r32_copro_fifo);
    map_w(b, 0x88, 1, NULL, w8_nop, w16_copro_fifo, w32_copro_fifo);
    map_w(b, 0x8c, 1, NULL, w8_nop, w16_nop, w32_8c0000);
    map_r(b, 0x90, 8, b->bufram, NULL, NULL, NULL);
    map_w(b, 0x90, 8, b->bufram, NULL, NULL, NULL);
    map_r(b, 0x98, 1, NULL, r8_980, r16_980, b->type == 0 ? r32_980_m2 : r32_980_m2a);
    map_w(b, 0x98, 1, NULL, w8_nop, w16_nop, w32_980);

    /* sound, IRQ, timers */
    map_r(b, 0x9c, 1, NULL, r8_sound, r16_sound, r32_sound);
    map_w(b, 0x9c, 1, NULL, w8_sound, w16_sound, w32_sound);
    map_r(b, 0x1c8, 1, NULL, r8_sound, r16_sound, r32_sound);
    map_w(b, 0x1c8, 1, NULL, w8_sound, w16_sound, w32_sound);
    map_r(b, 0xe8, 1, NULL, r8_0, r16_0, r32_irq);
    map_w(b, 0xe8, 1, NULL, w8_nop, w16_nop, w32_irq);
    map_r(b, 0xf0, 1, NULL, r8_ff, r16_ffff, r32_timer);
    map_w(b, 0xf0, 1, NULL, w8_nop, w16_nop, w32_timer);

    /* data ROM at 0x2000000 (and its upper half at 0x6000000) */
    map_r(b, 0x200, (r->size[1] >> 16) + 1, r->ptr[1], NULL, NULL, NULL);
    if (r->size[1] > 0x1000000)
        map_r(b, 0x600, ((r->size[1] + 0x5000000 - 0x6000000) >> 16) + 1,
              r->ptr[1] + 0x1000000, NULL, NULL, NULL);

    /* tilemaps, character generator */
    for (uint32_t p = 0x100; p <= 0x102; p++) {
        map_r(b, p, 1, b->tile, NULL, NULL, NULL);
        map_w(b, p, 1, NULL, w8_tile, w16_tile, w32_tile);
    }
    map_w(b, 0x104, 1, NULL, w8_nop, w16_hsync, w32_hsync);
    map_w(b, 0x106, 1, NULL, w8_nop, w16_vsync, w32_vsync);
    map_w(b, 0x114, 1, NULL, w8_nop, w16_hsync, w32_hsync);   /* mirrors */
    map_w(b, 0x116, 1, NULL, w8_nop, w16_vsync, w32_vsync);
    map_r(b, 0x108, 8, b->cg, NULL, NULL, NULL);
    map_w(b, 0x108, 8, NULL, w8_cg, w16_cg, w32_cg);
    map_r(b, 0x110, 1, b->tile, NULL, NULL, NULL);
    map_w(b, 0x110, 1, NULL, w8_tilem, w16_tilem, w32_tilem);
    map_r(b, 0x118, 8, b->cg, NULL, NULL, NULL);
    map_w(b, 0x118, 8, b->cg, NULL, NULL, NULL);

    /* palette, colour translation, network, I/O, backup RAM */
    map_r(b, 0x180, 1, b->pal, NULL, NULL, NULL);
    map_w(b, 0x180, 1, NULL, w8_pal, w16_pal, w32_pal);
    map_r(b, 0x181, 1, NULL, r8_xlat, r16_xlat, r32_xlat);
    map_w(b, 0x181, 1, NULL, w8_xlat, w16_xlat, w32_xlat);
    map_r(b, 0x1a0, 1, NULL, r8_net, r16_net, r32_net);
    map_w(b, 0x1a0, 1, NULL, w8_net, w16_net, w32_net);
    map_r(b, 0x1c0, 1, NULL, r8_io, r16_io, r32_io);
    map_w(b, 0x1c0, 1, NULL, w8_io, w16_io, w32_nop);
    map_r(b, 0x1d0, 1, b->back, NULL, NULL, NULL);
    map_w(b, 0x1d0, 1, b->back, NULL, NULL, NULL);

    /* geometry / display */
    map_w(b, 0x1000, 0x10, NULL, w8_disp, w16_disp, w32_disp);
    map_r(b, 0x1080, 1, NULL, r8_ff, r16_1080, r32_ffffffff);

    /* textures and luminance */
    for (uint32_t i = 0; i < 2; i++) {
        map_r(b, 0x1100 + 0x10 * i, 0x10, b->tex0, NULL, NULL, NULL);
        map_r(b, 0x1120 + 0x10 * i, 0x10, b->tex1, NULL, NULL, NULL);
        map_w(b, 0x1100 + 0x10 * i, 0x10, NULL, w8_tex0, w16_tex0, w32_tex0);
        map_w(b, 0x1120 + 0x10 * i, 0x10, NULL, w8_tex1, w16_tex1, w32_tex1);
        map_r(b, 0x1220 + 0x10 * i, 0x10, b->tex0, NULL, NULL, NULL);
        map_r(b, 0x1260 + 0x10 * i, 0x10, b->tex1, NULL, NULL, NULL);
    }
    map_r(b, 0x1140, 1, b->luma, NULL, NULL, NULL);
    map_w(b, 0x1140, 1, NULL, w8_luma, w16_luma, w32_luma);
    map_w(b, 0x1200, 0x40, NULL, w8_tex0h, w16_tex0h, w32_tex0h);
    map_w(b, 0x1240, 0x40, NULL, w8_tex1h, w16_tex1h, w32_tex1h);
    map_r(b, 0x1280, 2, b->luma, NULL, NULL, NULL);
    map_w(b, 0x1280, 2, NULL, w8_lumah, w16_lumah, w32_lumah);

    /* frame buffer, mirrored */
    map_w(b, 0x1160, 8, b->fb, NULL, NULL, NULL);
    map_w(b, 0x1168, 8, b->fb, NULL, NULL, NULL);
    map_r(b, 0x12c0, 8, b->fb, NULL, NULL, NULL);
    map_r(b, 0x12c8, 8, b->fb, NULL, NULL, NULL);
    map_w(b, 0x12c0, 8, b->fb, NULL, NULL, NULL);
    map_w(b, 0x12c8, 8, b->fb, NULL, NULL, NULL);

    map_w(b, 0xff00, 1, NULL, w8_nop, w16_nop, w32_iac);
}

/* ------------------------------------------------------------- lifecycle */

static void idle_hook(void *u)   /* orig 0x4c6780 */
{
    m2_board *b = B(u);
    if (++b->idle_count > 9)
        b->cpu->stop = 1;
}

m2_board *m2_create(const m2_game *game, const char *const *rom_dirs,
                    void (*log)(const char *msg))
{
    m2_board *b = calloc(1, sizeof *b);
    if (!b)
        return NULL;
    b->game = game;
    if (m2_load_roms(game, rom_dirs, &b->rom, log)) {
        m2_destroy(b);
        return NULL;
    }
    b->type = (int)b->rom.option[0];
    if (b->type > 1) {
        if (log) log("Model 2B/2C boards are not supported yet");
        m2_destroy(b);
        return NULL;
    }

    /* sizes from the original's savestate registration; BACK and ZSHARED
       get slack because handlers index past their end (QUIRKS.md) */
    b->ram = calloc(1, 0x100000);
    b->fb = calloc(1, 0x80000);
    b->ram2 = calloc(1, 0x40000);
    b->pal = calloc(1, 0x10000);
    b->cg = calloc(1, 0x100000);
    b->tile = calloc(1, 0x10000);
    b->back = calloc(1, 0x18000);
    b->xlat = calloc(1, 0x10000 + 4);   /* +4: 32-bit stores at the last masked offset */
    b->luma = calloc(1, 0x20000 + 4);
    b->bufram = calloc(1, 0x80000);
    b->tgp_prog = calloc(1, 0x40000);
    b->tgp_data = calloc(1, 0x40000);
    b->coproram = calloc(1, 0x20000);
    b->tex0 = calloc(1, 0x100000 + 4);
    b->tex1 = calloc(1, 0x100000 + 4);
    b->zshared = calloc(1, 0x18000);
    b->scsp = calloc(1, 0x10000);
    b->cpu = malloc(sizeof *b->cpu);
    if (!b->ram || !b->fb || !b->ram2 || !b->pal || !b->cg || !b->tile || !b->back ||
        !b->xlat || !b->luma || !b->bufram || !b->tgp_prog || !b->tgp_data ||
        !b->coproram || !b->tex0 || !b->tex1 || !b->zshared || !b->scsp || !b->cpu) {
        m2_destroy(b);
        return NULL;
    }
    m2_reset(b);
    return b;
}

void m2_destroy(m2_board *b)
{
    if (!b)
        return;
    if (b->cpu)
        i960_free(b->cpu);
    free(b->cpu);
    free(b->ram); free(b->fb); free(b->ram2); free(b->pal); free(b->cg); free(b->tile);
    free(b->back); free(b->xlat); free(b->luma); free(b->bufram); free(b->tgp_prog);
    free(b->tgp_data); free(b->coproram); free(b->tex0); free(b->tex1); free(b->zshared);
    free(b->scsp);
    m2_free_roms(&b->rom);
    free(b);
}

void m2_reset(m2_board *b)   /* orig 0x4cd450, after allocation */
{
    i960_init(b->cpu, b);
    setup_map(b);

    memset(b->ram, 0, 0x100000);
    memset(b->fb, 0, 0x80000);
    memset(b->ram2, 0, 0x40000);
    memset(b->coproram, 0, 0x20000);
    memset(b->back, 0, 0x10000);
    memset(b->bufram, 0, 0x80000);
    for (int n = 0; n < 4; n++)
        b->timer[n] = 0xfffff;
    b->irq_pending = b->irq_enable = 0;
    b->idle_count = 0;
    b->sound_irqs = 0;
    b->sound_count = 0;
    b->drive_last = 0xff;   /* orig 0x57285b starts as 0xff */
    b->frame_flags = 0;
    b->prev_fps = 60;
    memset(b->port, 0xff, sizeof b->port);
    ee_init(&b->eeprom);

    tgp_init(&b->tgp);
    b->tgp.prog = (uint32_t *)b->tgp_prog;
    b->tgp.prog_words = 0x10000;
    b->tgp.table_rom = b->rom.ptr[10];
    b->tgp.table_rom_size = b->rom.size[10];
    b->tgp.ext_ram = (uint32_t *)b->bufram;
    b->tgp.ext_ram_words = 0x20000;
    b->tgp.ext_rom = b->rom.size[2] ? (const uint32_t *)b->rom.ptr[2] : NULL;
    b->tgp.ext_rom_words = b->rom.size[2] / 4;
    b->tgp.data_buf = (uint32_t *)b->tgp_data;
    b->tgp.data_buf_words = 0x10000;

    /* i960: program ROM, RAM2 and RAM as code regions */
    i960_set_code_region(b->cpu, 0, 0, b->rom.size[0]);
    i960_set_code_region(b->cpu, 1, 0x200000, 0x40000);
    i960_set_code_region(b->cpu, 2, 0x500000, 0x100000);
    i960_reset(b->cpu);
    b->cpu->hook = idle_hook;

    tgp_m2_board_reset(&b->tgp);   /* copro driver entry 13 */

    if (b->game->hook == M2_HOOK_IDLE_COUNT)
        i960_hook_insn(b->cpu, b->game->hook_addr);
    else if (b->game->hook == M2_HOOK_STOP_AFTER)
        i960_stop_after_insn(b->cpu, b->game->hook_addr);
}

/* orig 0x4cc7a0 */
void m2_run_frame(m2_board *b)
{
    uint32_t fps = (uint32_t)lrintf(b->game->fps);
    int32_t timer_dec = (int32_t)(25000000u / (b->prev_fps * 210u));
    /* Model 2A uses the previous frame's rate like the timers; the original
       Model 2 runs the i960 at a 50 MHz budget with the current rate */
    int cycles = b->type == 0 ? (int)(50000000u / (fps * 210u)) : timer_dec;

    b->prev_fps = fps;
    for (int slice = 0; slice < 210; slice++) {
        i960_execute(b->cpu, cycles);
        run_timers(b, timer_dec);
        irq_dispatch(b);
        if (b->sound_irqs) {
            b->sound_irqs--;
            b->irq_pending |= 0x400;
            b->cpu->stop = 0;
            b->busy_slices++;
        } else if (!b->cpu->stop) {
            b->busy_slices++;
        }
        if (b->type == 0)
            b->net_reg2 ^= 0x80;
        if (b->hooks.slice_done)
            b->hooks.slice_done(b->hooks.user);
    }
    b->irq_pending |= 0x401;
    irq_dispatch(b);
    if (((b->display_ctrl & 4) == 0 && (b->frame & 1)) || (b->display_ctrl & 4))
        b->frame_flags ^= 4;
    b->cpu->stop = 0;
    b->idle_count = 0;
    b->frame++;
}

/* ------------------------------------------------------------ NVRAM */

int m2_nvram_load(m2_board *b, const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f)
        return -1;
    size_t n = fread(b->back, 1, 0x10000, f);
    n += fread(b->eeprom.data, 1, sizeof b->eeprom.data, f);
    fclose(f);
    return n == M2_NVRAM_SIZE ? 0 : -1;
}

int m2_nvram_save(const m2_board *b, const char *path)
{
    FILE *f = fopen(path, "wb");
    if (!f)
        return -1;
    size_t n = fwrite(b->back, 1, 0x10000, f);
    n += fwrite(b->eeprom.data, 1, sizeof b->eeprom.data, f);
    return (fclose(f) == 0 && n == M2_NVRAM_SIZE) ? 0 : -1;
}

/* CRC-16/XMODEM, as the game's own table-driven routine (daytona 0x292d8) */
static uint16_t crc16_xmodem(const uint8_t *p, size_t n)
{
    uint16_t crc = 0;
    while (n--) {
        crc ^= (uint16_t)(*p++ << 8);
        for (int i = 0; i < 8; i++)
            crc = (uint16_t)(crc & 0x8000 ? crc << 1 ^ 0x1021 : crc << 1);
    }
    return crc;
}

/* Daytona USA keeps its settings at the start of backup RAM, twice (0x00 and
   0x80): an 8-byte "SEGA@" header, a CRC-16 at +8 over +0xa..+0x80, and the
   link mode at +0xb. The factory defaults in ROM (followed by the CRC table)
   have link mode 1, so a fresh board probes the network board, shows
   "CANCELLED / NETWORK BOARD NOT PRESENT" and resets, forever, until an
   operator sets the cabinet to single in test mode. Seed both copies with the
   defaults and link mode 0 (standalone). */
int m2_nvram_defaults(m2_board *b)
{
    const uint8_t *rom = b->rom.ptr[0];
    uint32_t size = b->rom.size[0];

    if (strncmp(b->game->name, "daytona", 7) || !rom)
        return 0;
    for (uint32_t o = 0; o + 0x80 + 8 <= size; o += 0x10) {
        if (memcmp(rom + o, "SEGA@", 5) ||
            ld16(rom + o + 0x82) != 0x1021 || ld16(rom + o + 0x84) != 0x2042)
            continue;
        for (int copy = 0; copy < 2; copy++) {
            uint8_t *d = b->back + copy * 0x80;
            memcpy(d, rom + o, 0x80);
            d[0x0b] = 0;
            st16(d + 8, crc16_xmodem(d + 0x0a, 0x76));
        }
        return 1;
    }
    return 0;
}
