/*
 * Hands finished frames from the emulation to the renderer, so the two can
 * run on different threads: while the renderer draws frame N, the board
 * already runs frame N + 1.
 *
 * On the emulation thread, m2pipe_attach puts recording hooks on the board
 * (the video notifications; the others are forwarded untouched) and
 * m2pipe_capture copies what the renderer reads at the end of each frame:
 * tile RAM and the display list every frame, the other video memories only
 * when written. On the render thread, m2pipe_apply brings its own copies up
 * to date, replays the frame's notifications into the renderer's hooks, and
 * returns a board whose video memories are those copies.
 *
 * Two slots: capture fills one while apply reads the other. The caller does
 * the synchronisation: capture of frame N + 1 may run during apply and
 * rendering of frame N, but capture of frame N + 2 must wait until
 * m2pipe_apply of frame N has returned. No threads or locks in here.
 */
#ifndef M2PIPE_H
#define M2PIPE_H

#include "m2board.h"
#include "m2wide.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct m2_pipe m2_pipe;

typedef struct {
    uint32_t      frame;   /* b->frame after the captured frame */
    m2_wide_state wide;    /* widescreen rule state of that frame */
} m2_pipe_frame;

m2_pipe *m2pipe_create(void);
void     m2pipe_destroy(m2_pipe *p);
/* Emulation side. attach: the board's current hooks become the next in line
   (call again after changing b->hooks). capture: after m2_run_frame. */
void     m2pipe_attach(m2_pipe *p, m2_board *b);
void     m2pipe_capture(m2_pipe *p, const m2_board *b);
/* The next capture copies everything and reports everything written (after
   a reset or anything else that changes memories without notifications). */
void     m2pipe_invalidate(m2_pipe *p);
/* Render side: applies the oldest captured frame and replays its
   notifications into `replay` (NULL entries skipped). The returned board is
   valid for m2tile_render, m2geo_run and m2gl_draw until the next apply. */
const m2_board *m2pipe_apply(m2_pipe *p, const m2_hooks *replay, m2_pipe_frame *info);

#ifdef __cplusplus
}
#endif

#endif
