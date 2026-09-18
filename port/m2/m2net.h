/*
 * Model 2 network board link ("ring"), for several boards in one process
 * (split-screen linked play). Ported from Model 2 Emulator 1.1a's network
 * code (thread 0x4cf740, receive 0x4cf3d0, frame end of 0x4cc7a0), which
 * passes the same packets between emulator instances over UDP.
 *
 * The boards run in lockstep: after every board has finished frame N and
 * before any starts frame N + 1, m2net_frame does what the original's
 * network thread did in the meantime (search, link-up, and the exchange of
 * each board's 0xe00-byte block with the next one in the ring).
 */
#ifndef M2NET_H
#define M2NET_H

#include "m2board.h"

#ifdef __cplusplus
extern "C" {
#endif

#define M2NET_MAX 8

typedef struct m2_net m2_net;

/* The boards in ring order; turns their network boards on. */
m2_net *m2net_create(m2_board *const *boards, int n);
void    m2net_destroy(m2_net *r);
/* Between frames, with every board stopped. */
void    m2net_frame(m2_net *r);
/* After m2_reset of board i (orig 0x4cad60): its link starts over. */
void    m2net_reset(m2_net *r, int i);
/* debugging: 0 off or searching, 1 waiting for the link, 3 linked */
int     m2net_state(const m2_net *r, int i);

#ifdef __cplusplus
}
#endif

#endif
