/*
 * Model 2 sound boards, driven by the i960's sound port:
 *  - Model 2 (board type 0): the Model 1 sound board, a 68000 at 10 MHz with
 *    two MultiPCMs and a YM3438; commands arrive through a UART (IRQ 2).
 *  - Model 2A (type 1): a 68000 at 11.29 MHz with an SCSP; commands arrive
 *    through the SCSP's MIDI input.
 * Memory maps as the original (0x4ccbe0), I/O details as MAME's
 * segam1audio.cpp and model2.cpp. The 68000 is Musashi (the original's
 * A68K is x86 assembly). Output: 44.1 kHz stereo.
 */
#ifndef M2SND_H
#define M2SND_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct m2_board;
typedef struct m2_snd m2_snd;

#define M2SND_RATE 44100

/* only one instance at a time (Musashi is a single global CPU) */
m2_snd *m2snd_create(const struct m2_board *b);
void    m2snd_destroy(m2_snd *s);
void    m2snd_reset(m2_snd *s);
/* a byte from the i960 (board hook sound_command) */
void    m2snd_command(m2_snd *s, uint8_t cmd);
/* runs the sound board for `n` samples, writing interleaved stereo */
void    m2snd_render(m2_snd *s, int16_t *out, int n);
/* debugging: one-line state summary */
void    m2snd_debug(m2_snd *s, char *buf, int size);

#ifdef __cplusplus
}
#endif

#endif
