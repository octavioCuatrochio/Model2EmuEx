/*
 * The launcher: a window with two tabs, Games (the ROM sets in a folder,
 * to start one) and Configuration (every option, explained, remembered in
 * m2emu.ini). Dear ImGui on SDL2 and the port's GL layer, so it runs
 * wherever the emulator does, with mouse, keyboard or a pad.
 */
#ifndef M2_LAUNCHER_H
#define M2_LAUNCHER_H

#include <SDL2/SDL.h>
#include "options.h"

#ifdef __cplusplus
extern "C" {
#endif

/* With the GL functions loaded and the context current. */
int  m2launch_init(SDL_Window *win, SDL_GLContext ctx, int es);
void m2launch_shutdown(void);
/* Shows the launcher until a game is chosen (returns 1: `game` holds its
   name, *o the settings to play with, all saved to cfg_path) or the window
   is closed (returns 0). `message`, if not NULL, is shown first (why the
   last game couldn't start). `game` may hold a game to select. */
int  m2launch_run(SDL_Window *win, m2_options *o, const char *cfg_path,
                  char *game, int game_size, const char *message);

/* The pause popup, over the game's last picture (the launcher's GL and
   ImGui setup, so after m2launch_init). While it's open: every event to
   m2pause_event, and each frame, after the game is drawn and before the
   swap, m2pause_frame, which returns 0 while it stays open, 1 for Continue,
   2 for Exit. m2pause_open before its first frame. */
int  m2pause_available(void);
void m2pause_open(void);
void m2pause_event(const SDL_Event *e);
int  m2pause_frame(SDL_Window *win);

#ifdef __cplusplus
}
#endif

#endif
