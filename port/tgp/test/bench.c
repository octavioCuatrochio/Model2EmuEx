/* Throughput check: a multiply-accumulate loop (ALU op 9 with two RAM
   operands and post-increment addressing) closed by a loop-counter branch. */
#include "../tgp.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double now(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(int argc, char **argv)
{
    long insns = argc > 1 ? atol(argv[1]) : 100000000;
    static uint32_t prog[0x100];
    static uint8_t table[0x40000];
    tgp *t = malloc(sizeof *t);

    prog[0] = (0x13u << 26) | 0xffffff;                              /* r12 = 16M */
    prog[1] = (9u << 21) | (0xdu << 16) | (0x01u << 9) | 0x1c1;       /* MAC, A=RAM[X++], B=RAM B[Y] */
    prog[2] = (0x2fu << 26) | (0x10u << 20) | 1;                     /* if --r12: goto 1 */
    prog[3] = (0x2fu << 26) | (0x16u << 20) | 0;                     /* goto 0 */

    tgp_init(t);
    t->prog = prog;
    t->prog_words = 0x100;
    t->table_rom = table;
    t->table_rom_size = sizeof table;
    tgp_reset(t, sizeof prog);

    double t0 = now();
    long done = 0;
    while (done < insns) {
        tgp_run(t);
        done += 1000;
    }
    double dt = now() - t0;
    printf("%ld TGP instructions in %.3f s: %.1f M instructions/s\n", done, dt, done / dt / 1e6);
    free(t);
    return 0;
}
