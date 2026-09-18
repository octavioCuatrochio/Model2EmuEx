/* ctypes access for difftest.py */
#include "../tgp.h"

#include <stdlib.h>

tgp *t_new(void) { tgp *t = malloc(sizeof *t); tgp_init(t); return t; }

void t_setup(tgp *t, uint32_t *prog, uint32_t prog_words, const uint8_t *table, uint32_t table_size,
             uint32_t *ext_ram, uint32_t ext_words, const uint32_t *ext_rom, uint32_t rom_words)
{
    t->prog = prog; t->prog_words = prog_words;
    t->table_rom = table; t->table_rom_size = table_size;
    t->ext_ram = ext_ram; t->ext_ram_words = ext_words;
    t->ext_rom = ext_rom; t->ext_rom_words = rom_words;
}

uint8_t  *t_blk(tgp *t) { return t->blk; }
tgp_fifo *t_in(tgp *t) { return &t->in; }
tgp_fifo *t_out(tgp *t) { return &t->out; }
void t_run(tgp *t, int n) { tgp_run_count(t, n); }
void t_reset(tgp *t, uint32_t bytes) { tgp_reset(t, bytes); }

void *t_board(tgp *t) { return &t->prog_upload_ctrl; }
void t_set_data(tgp *t, uint32_t *buf, uint32_t words) { t->data_buf = buf; t->data_buf_words = words; }
void t_set_ptr_values(tgp *t, uint32_t table, uint32_t prog) { t->table_ptr_value = table; t->prog_ptr_value = prog; }

uint32_t t_call(tgp *t, int op, uint32_t addr, uint32_t v)
{
    switch (op) {
    case 0: tgp_m2_port_write32(t, addr, v); return 0;
    case 1: tgp_m2_port_write16(t, addr, (uint16_t)v); return 0;
    case 2: return tgp_m2_port_read32(t, addr);
    case 3: return tgp_m2_port_read16(t, addr);
    case 4: return tgp_m2_port_read8(t, addr);
    case 5: tgp_m2_ctrl_write32(t, addr, v); return 0;
    case 6: return tgp_m2_ctrl_read32(t, addr);
    case 7: tgp_m2_upload_write(t, addr, v); return 0;
    case 8: tgp_m2_ctrl_write16(t, addr, (uint16_t)v); return 0;
    }
    return 0;
}
