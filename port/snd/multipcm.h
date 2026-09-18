/*
 * Sega 315-5560 "MultiPCM" (Yamaha YMW-258-F, GEW8): 28 voices of 8/12-bit
 * PCM. C port of MAME's gew.cpp + multipcm.cpp (BSD-3-Clause, by Miguel
 * Angel Horna (ElSemi), who wrote it for Model 2 Emulator and MAME), run
 * directly at the 44.1 kHz output rate.
 */
#ifndef MULTIPCM_H
#define MULTIPCM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* sample memory: 22-bit addresses; returns the byte at `addr` */
typedef uint8_t (*multipcm_read_fn)(void *user, uint32_t addr);

typedef struct multipcm multipcm;

multipcm *multipcm_create(float rate, multipcm_read_fn read, void *user);
void      multipcm_destroy(multipcm *m);
void      multipcm_reset(multipcm *m);
/* register interface: offset 0 = data, 1 = slot, 2 = register */
void      multipcm_write(multipcm *m, int offset, uint8_t data);
uint8_t   multipcm_read(multipcm *m);
/* adds `n` stereo samples to out (interleaved L/R, 32-bit accumulators) */
void      multipcm_render(multipcm *m, int32_t *out, int n);
int       multipcm_voices_playing(const multipcm *m);   /* debugging */

#ifdef __cplusplus
}
#endif

#endif
