/*
 * Yamaha YM3438 (OPN2C) through ymfm (BSD-3-Clause, Aaron Giles), with a C
 * interface. The Model 1 sound board's driver takes its tempo from the
 * chip's timer flags, so the timers matter as much as the FM output.
 */
#ifndef YM3438_H
#define YM3438_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ym3438 ym3438;

ym3438 *ym3438_create(uint32_t clock, uint32_t out_rate);
void    ym3438_destroy(ym3438 *y);
void    ym3438_reset(ym3438 *y);
/* offset 0-3: address/data port 0, address/data port 1 */
void    ym3438_write(ym3438 *y, int offset, uint8_t data);
uint8_t ym3438_read(ym3438 *y, int offset);
/* adds `n` stereo samples at out_rate, scaled by gain (MAME: 0.30) */
void    ym3438_render(ym3438 *y, int32_t *out, int n, float gain);

#ifdef __cplusplus
}
#endif

#endif
