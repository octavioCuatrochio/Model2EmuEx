/* 68000 disassembler for sound-program dumps (Musashi's m68kdasm).
   usage: m68kdis file base start count [swap]   (swap: 16-bit words little-endian) */
#include <stdio.h>
#include <stdlib.h>
#include "../port/m68k/m68k.h"

static unsigned char *mem;
static long size, base;
static int swap;

static unsigned rd8(unsigned a)
{
    long o = (long)a - base;
    if (swap) o ^= 1;
    return o >= 0 && o < size ? mem[o] : 0;
}
unsigned int m68k_read_disassembler_16(unsigned int a) { return rd8(a) << 8 | rd8(a + 1); }
unsigned int m68k_read_disassembler_32(unsigned int a) { return m68k_read_disassembler_16(a) << 16 | m68k_read_disassembler_16(a + 2); }
unsigned int m68k_read_memory_8(unsigned int a) { return rd8(a); }
unsigned int m68k_read_memory_16(unsigned int a) { return m68k_read_disassembler_16(a); }
unsigned int m68k_read_memory_32(unsigned int a) { return m68k_read_disassembler_32(a); }
void m68k_write_memory_8(unsigned int a, unsigned int v) { (void)a; (void)v; }
void m68k_write_memory_16(unsigned int a, unsigned int v) { (void)a; (void)v; }
void m68k_write_memory_32(unsigned int a, unsigned int v) { (void)a; (void)v; }

int main(int argc, char **argv)
{
    FILE *f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END); size = ftell(f); fseek(f, 0, SEEK_SET);
    mem = malloc(size); fread(mem, 1, size, f); fclose(f);
    base = strtol(argv[2], 0, 0);
    unsigned pc = strtoul(argv[3], 0, 0);
    int n = atoi(argv[4]);
    swap = argc > 5;
    char buf[128];
    for (int i = 0; i < n; i++) {
        unsigned len = m68k_disassemble(buf, pc, M68K_CPU_TYPE_68000);
        printf("%06x: %s\n", pc, buf);
        pc += len;
    }
    return 0;
}
