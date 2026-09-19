/*
 * The frontend's settings: the command-line options, the launcher's
 * Configuration tab and the settings file (m2emu.ini) all fill this.
 */
#ifndef M2_OPTIONS_H
#define M2_OPTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

enum { M2_WIDE_DEFAULT, M2_WIDE_OFF, M2_WIDE_RATIO, M2_WIDE_FILL };
enum { M2_ASPECT_KEEP, M2_ASPECT_STRETCH, M2_ASPECT_CROP };

typedef struct {
    char   romdir[1024];
    char   nvdir[1024];      /* "" = the default save folder */
    int    fullscreen;
    int    win_w, win_h;     /* 0 = automatic */
    int    vsync;
    int    frame_cap;        /* 1: run at the game's rate; 0: as fast as possible */
    int    pipelined;
    int    scale;            /* render scale, 0 = auto */
    int    smooth;           /* linear filtering when scaling */
    int    wide;             /* M2_WIDE_*; DEFAULT = off, but fill with --coop --split stack */
    double wide_ratio;       /* with M2_WIDE_RATIO */
    int    aspect;           /* M2_ASPECT_* */
    int    mesh_blend;
    float  saturation;
    float  gamma[3];
    int    updown_gears;     /* sequential shifter */
    int    hold_gears;
    int    coop;
    int    split_side;       /* --coop: 1 side by side, 0 one above the other */
} m2_options;

void m2opt_defaults(m2_options *o);
/* Command line. Returns 0, or 1 with a message on stderr for a bad option.
   *game: the game named (or NULL); *no_gui: --no-gui given; *help: -h. */
int  m2opt_parse(m2_options *o, int argc, char **argv, const char **game, int *no_gui, int *help);
/* Settings file: m2emu.ini next to the program on Windows, else
   $XDG_CONFIG_HOME/m2emu/m2emu.ini or ~/.config/m2emu/m2emu.ini. */
void m2opt_config_path(char *out, int size);
/* Unknown keys are ignored; last_game gets the last game played (may be NULL). */
int  m2opt_load(m2_options *o, const char *path, char *last_game, int last_game_size);
int  m2opt_save(const m2_options *o, const char *path, const char *last_game);
/* The save folder to use: o->nvdir, or the default one. */
void m2opt_nvdir(const m2_options *o, char *out, int size);
/* Creates a folder and its parents. */
void m2opt_mkdirs(const char *path);

#ifdef __cplusplus
}
#endif

#endif
