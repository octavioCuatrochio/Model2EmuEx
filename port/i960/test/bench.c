/* Throughput check: runs a two-instruction loop from RAM and reports
   instructions per second. Build natively or for the target device. */
#include "../i960.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static double now(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

static void put32(uint8_t *p, uint32_t v) { memcpy(p, &v, 4); }

int main(int argc, char **argv)
{
    long iters = argc > 1 ? atol(argv[1]) : 50000000;
    static uint8_t ram[0x10000];
    i960_state *s = malloc(sizeof *s);

    /* boot record: SAT, PRCB, -, first IP; PRCB+0x18 = interrupt stack */
    put32(ram + 0x00, 0);
    put32(ram + 0x04, 0x800);
    put32(ram + 0x0c, 0x100);
    put32(ram + 0x818, 0x4000);
    /* 0x100: addo 1, r4, r4
       0x104: cmpobl r4, r5, 0x100
       0x108: b 0x108 */
    put32(ram + 0x100, 0x59210801);
    put32(ram + 0x104, 0x34215ffc);
    put32(ram + 0x108, 0x08000000);

    i960_init(s, NULL);
    i960_map_read(s, 0, sizeof ram - 1, ram, NULL, NULL, NULL);
    i960_map_write(s, 0, sizeof ram - 1, ram, NULL, NULL, NULL);
    i960_set_code_region(s, 0, 0, sizeof ram);
    i960_set_code_region(s, 1, 0xfff00000, 0);
    i960_set_code_region(s, 2, 0xfff00000, 0);
    i960_reset(s);
    s->r[4] = 0;
    s->r[5] = (uint32_t)iters;

    double t0 = now();
    long cycles = 0;
    while (s->r[4] < s->r[5])
        cycles += i960_execute(s, 1000000);
    double dt = now() - t0;

    printf("%ld loop iterations (%ld instructions, %ld i960 cycles) in %.3f s\n",
           iters, iters * 2, cycles, dt);
    printf("%.1f M instructions/s, %.1f M i960 cycles/s (Model 2 i960 runs at 25 MHz)\n",
           iters * 2 / dt / 1e6, cycles / dt / 1e6);
    i960_free(s);
    free(s);
    return 0;
}
