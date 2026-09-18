/*
 * Yamaha YMF292-F "SCSP": 32 slots of PCM/FM plus the effects DSP. C port of
 * MAME's scsp.cpp and scspdsp.cpp (BSD-3-Clause; SCSP emulation by ElSemi,
 * the Model 2 Emulator's author, with R. Belmont and kingshriek), with the
 * MAME scheduler replaced by per-sample timers and the serial MIDI port by a
 * byte FIFO. Output rate is the chip's own 44.1 kHz.
 */
#ifndef SCSP_H
#define SCSP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct scsp scsp;

/* irq(user, level 1-7, asserted): interrupt lines of the sound CPU */
typedef void (*scsp_irq_fn)(void *user, int level, int asserted);

/* `ram`: sound RAM in 68000 (big-endian) byte order, `ram_mask` + 1 bytes */
scsp    *scsp_create(uint8_t *ram, uint32_t ram_mask, scsp_irq_fn irq, void *user);
void     scsp_destroy(scsp *s);
void     scsp_reset(scsp *s);
/* registers, byte offset 0x000-0xfff */
uint16_t scsp_read16(scsp *s, uint32_t offset);
void     scsp_write16(scsp *s, uint32_t offset, uint16_t data, uint16_t mask);
void     scsp_midi_in(scsp *s, uint8_t data);
/* writes `n` stereo samples (interleaved L/R, 16-bit range) */
void     scsp_render(scsp *s, int32_t *out, int n);
int      scsp_slots_active(const scsp *s, int *mvol);   /* debugging */

#ifdef __cplusplus
}
#endif

#endif
