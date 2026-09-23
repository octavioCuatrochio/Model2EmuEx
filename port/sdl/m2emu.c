/*
 * SDL2 + OpenGL frontend for the Model 2 port.
 *
 *   m2emu                                 the launcher (launcher.cpp): games and settings
 *   m2emu --no-gui [options] <game>       a game straight from the command line
 *
 * options: [-r romdir] [-n nvdir] [--vsync] [--no-frame-cap] [--fullscreen]
 *          [--size WxH] [--widescreen 16:9|16:10|fill|off] [--scale N|auto]
 *          [--sharp] [--texture-filter nearest|bilinear|trilinear]
 *          [--mesh blend|checker] [--saturation S]
 *          [--gamma G | --gamma R,G,B] [--shifter sequential|hpattern]
 *          [--hold-gears] [--pipeline on|off] [--frame-skip auto|off]
 *          [--aspect keep|stretch|crop]
 *          [--coop [--split side|stack]]
 *
 * --coop (Daytona USA): two boards linked through their network boards, as
 * two cabinets, in one window side by side (--aspect stretch or crop fill
 * each half); --split stack puts them one above the other with wide views
 * that fill each half. Keyboard and pad 1 drive player 1 (left/top), pad 2
 * player 2. Only player 1's machine is heard.
 *
 * Keys (from the original's input definitions): 5/6 coin, 1/2 start,
 * F1 service, F2 test, arrows, Z X C V / A S D F G game buttons.
 * Frontend: Esc back to the launcher (quit with --no-gui), F3 reset,
 * P pause, Tab (hold) fast forward, F6 texture filter (nearest, bilinear,
 * trilinear), F7 saturation (1.0, 1.2, 1.4), F8 mesh blend/checker, F9
 * widescreen on/off, F10 render scale (auto, 1-4), F11 fullscreen.
 * Pads (up to 2, hot-plug): see m2/m2input.c for the layout.
 */
#include "../m2/m2board.h"
#include "../m2/m2geo.h"
#include "../m2/m2gl.h"
#include "../m2/m2input.h"
#include "../m2/m2net.h"
#include "../m2/m2pipe.h"
#include "../m2/m2tile.h"
#include "../m2/m2wide.h"
#include "../snd/m2snd.h"

#include "../m2/m2glapi.h"
#include "launcher.h"
#include "options.h"

#include <SDL2/SDL.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef __ANDROID__
#include <unistd.h>
#endif

/* The GL flavour this build asks for: desktop OpenGL 2.1 or later
   (default), or OpenGL ES 2 with `make GL=gles` (M2_GLES). */
#ifdef M2_GLES
#define USE_GLES 1
#else
#define USE_GLES 0
#endif

/* CPU time of the calling thread, seconds */
static double thread_cpu_s(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

/* One emulated machine and everything that draws and plays it. Normally
   one; with --coop two linked boards (split screen). */
typedef struct {
    m2_board *b;
    m2_snd   *snd;
    m2_pipe  *pipe;
    m2_hooks  replay;              /* video hooks, called on the main thread */
    m2_tilegen *tg;
    m2_geo   *geo;
    m2_gl    *gl;
    m2_input_state in;
    const m2_board *vb;            /* the newest frame's video memories */
    m2_pipe_frame pf;              /* the newest frame */
    m2_view   view;                /* how it will be drawn (set when it arrives) */
    int       tiles_due;           /* its tile layers and uploads are still to do */
    int       geo_busy;            /* the geometrizer thread is on it */
    /* the frame on screen: drawn from these while the next one is made */
    m2_pipe_frame pf_cur;
    m2_view   view_cur;
    const m2_mesh *drawn;
    m2_mesh  *mesh[2];             /* the geometrizer thread builds one while the other is drawn */
    int       mesh_next;
    int       has_cur;
    int       rx, ry, rw, rh;      /* its window rectangle (GL: origin bottom left) */
    char      nvpath[2048];
    /* board thread: `go` starts a frame, `done` says it is captured;
       between the two the main thread leaves the board alone */
    SDL_Thread *thread;
    SDL_sem  *go, *done;
    int       quit;
    Uint64    ticks;               /* time spent in the last frame */
    /* geometrizer thread: the 3D of the newest frame, while the main thread
       draws the one before and then makes the new one's tile layers and
       uploads (all only read `vb`) */
    SDL_Thread *tthread;
    SDL_sem  *tgo, *tdone;
} player;

#define MAX_PLAYERS 2
static player pl[MAX_PLAYERS];
static int nplayers = 1;

static void on_tile(void *u, uint32_t o) { m2tile_tile_written(((player *)u)->tg, o); }
static void on_cg(void *u) { m2tile_cg_written(((player *)u)->tg); }
static void on_xlat(void *u)
{
    player *p = u;
    m2tile_xlat_written(p->tg);
    if (p->gl) m2gl_xlat_written(p->gl);
}
static void on_tex(void *u, int bank, uint32_t o) { player *p = u; if (p->gl) m2gl_texture_written(p->gl, bank, o); }
static void on_luma(void *u) { player *p = u; if (p->gl) m2gl_luma_written(p->gl); }
static void on_pal(void *u, uint32_t o)
{
    player *p = u;
    m2tile_palette_written(p->tg, o);
    if (p->gl) m2gl_palette_written(p->gl, o);
}
static void on_sound(void *u, uint8_t c) { player *p = u; if (p->snd) m2snd_command(p->snd, c); }

/* The sound board runs here, on SDL's audio thread, at the device's pace:
   its tempo stays right even when the emulation can't keep full speed.
   With --coop only player 1's machine has one. */
static void audio_callback(void *u, Uint8 *stream, int len)
{
    m2snd_render(u, (int16_t *)stream, len / 4);
}

/* The board runs on its own thread, one frame ahead of the renderer: while
   the main thread draws frame N (tiles, geometrizer, GL), this runs frame
   N + 1 and hands it over through m2pipe. */
static int emu_thread(void *arg)
{
    player *p = arg;
    for (;;) {
        SDL_SemWait(p->go);
        if (p->quit)
            break;
        Uint64 t0 = SDL_GetPerformanceCounter();
        m2_run_frame(p->b);
        m2pipe_capture(p->pipe, p->b);
        p->ticks = SDL_GetPerformanceCounter() - t0;
        SDL_SemPost(p->done);
    }
    return 0;
}

static int geo_thread(void *arg)
{
    player *p = arg;
    for (;;) {
        SDL_SemWait(p->tgo);
        if (p->quit)
            break;
        m2geo_run(p->geo, p->vb);
        m2_geo_frame f = m2geo_frame(p->geo);   /* and its vertices, off the main thread */
        m2gl_mesh_build(p->mesh[p->mesh_next], &f, p->view.frame_w);
        SDL_SemPost(p->tdone);
    }
    return 0;
}

/* debugging: M2EMU_KEYS=frame:dik:frames,... (dik in hex) */
static void script_keys(const char *script, uint32_t frame, uint8_t *key)
{
    for (const char *k = script; *k;) {
        int at = 0, len = 0;
        unsigned dik = 0;
        if (sscanf(k, "%d:%x:%d", &at, &dik, &len) == 3 && dik < 256) {
            if ((int)frame == at) key[dik] = 1;
            if ((int)frame == at + len) key[dik] = 0;
        }
        k = strchr(k, ',');
        if (!k) break;
        k++;
    }
}

/* ROM loader messages; the first is kept for the launcher to show */
static char first_log[256];
static void logmsg(const char *m)
{
    fprintf(stderr, "%s\n", m);
    if (!first_log[0])
        snprintf(first_log, sizeof first_log, "%s", m);
}

/* SDL scancode -> DirectInput scan code (PC set 1), for the keys games use */
static uint8_t dik_from_sdl(SDL_Scancode sc)
{
    static const char row2[] = "qwertyuiop";   /* DIK 0x10.. */
    static const char row3[] = "asdfghjkl";    /* 0x1e.. */
    static const char row4[] = "zxcvbnm";      /* 0x2c.. */
    if (sc >= SDL_SCANCODE_1 && sc <= SDL_SCANCODE_0)
        return (uint8_t)(0x02 + (sc - SDL_SCANCODE_1));
    if (sc >= SDL_SCANCODE_F1 && sc <= SDL_SCANCODE_F10)
        return (uint8_t)(0x3b + (sc - SDL_SCANCODE_F1));
    switch (sc) {
    case SDL_SCANCODE_UP: return 0xc8;
    case SDL_SCANCODE_DOWN: return 0xd0;
    case SDL_SCANCODE_LEFT: return 0xcb;
    case SDL_SCANCODE_RIGHT: return 0xcd;
    case SDL_SCANCODE_SPACE: return 0x39;
    case SDL_SCANCODE_RETURN: return 0x1c;
    case SDL_SCANCODE_LSHIFT: return 0x2a;
    case SDL_SCANCODE_RSHIFT: return 0x36;
    case SDL_SCANCODE_LCTRL: return 0x1d;
    case SDL_SCANCODE_LALT: return 0x38;
    case SDL_SCANCODE_F11: return 0x57;
    case SDL_SCANCODE_F12: return 0x58;
    default: break;
    }
    /* letters by position (scancode), so AZERTY etc. keep the layout */
    if (sc >= SDL_SCANCODE_A && sc <= SDL_SCANCODE_Z) {
        char c = (char)('a' + (sc - SDL_SCANCODE_A));
        const char *p;
        if ((p = strchr(row2, c))) return (uint8_t)(0x10 + (p - row2));
        if ((p = strchr(row3, c))) return (uint8_t)(0x1e + (p - row3));
        if ((p = strchr(row4, c))) return (uint8_t)(0x2c + (p - row4));
    }
    return 0;
}

/* a pad with at least one stick; TV remotes and media keys only have buttons */
static int pad_has_sticks(int device)
{
    SDL_Joystick *j = SDL_JoystickOpen(device);
    int axes = j ? SDL_JoystickNumAxes(j) : 0;
    if (j) SDL_JoystickClose(j);
    return axes >= 2;
}

static int pad_button_from_sdl(int b)
{
    return b >= 0 && b < M2PAD_BUTTONS ? b : -1;   /* same order as SDL_GameControllerButton */
}


/* The window a game asks for: the size set, or 768 high and as wide as
   the picture needs (twice as wide for two players side by side). */
static void game_window_size(const m2_options *o, int *w, int *h)
{
    if (o->win_w > 0 && o->win_h > 0) {
        *w = o->win_w;
        *h = o->win_h;
        return;
    }
    *h = 768;
    if (o->coop && !o->split_side)
        *w = 1365;   /* 16:9, each player 32:9 */
    else
        *w = (int)(768 * (o->wide == M2_WIDE_RATIO ? o->wide_ratio : 4.0 / 3.0) + 0.5) * (o->coop ? 2 : 1);
}

/* One game, in `win` (GL context current, functions loaded). Returns 0
   when the player leaves with Esc (from the launcher), 1 when the window
   is closed (or Esc without the launcher), -1 if it can't start (why, in
   err). */
static int run_game(SDL_Window *win, const m2_options *o, const char *game_name, int from_gui,
                    char *err, int err_size)
{
#define FAIL(...) do { snprintf(err, (size_t)err_size, __VA_ARGS__); goto fail; } while (0)
    /* the settings, under the names the code below has always used */
    const char *romdir = o->romdir;
    char nvdir[1024];
    m2opt_nvdir(o, nvdir, sizeof nvdir);
    int vsync = o->vsync, fullscreen = o->fullscreen, smooth = o->smooth, frame_cap = o->frame_cap;
    int tex_filter = o->tex_filter;
    int wide_on = o->wide == M2_WIDE_RATIO || o->wide == M2_WIDE_FILL;
    int wide_fill = o->wide == M2_WIDE_FILL, wide_set = o->wide != M2_WIDE_DEFAULT;
    double wide_ratio = o->wide == M2_WIDE_RATIO ? o->wide_ratio : 16.0 / 9.0;
    int scale_opt = o->scale, split_side = o->split_side, aspect_mode = o->aspect;
    int mesh_blend = o->mesh_blend, updown_gears = o->updown_gears, hold_gears = o->hold_gears;
    int pipelined = o->pipelined, coop = o->coop;
    float saturation = o->saturation, gamma[3] = { o->gamma[0], o->gamma[1], o->gamma[2] };
    m2_net *net = NULL;
    int quit_app = 0;

    memset(pl, 0, sizeof pl);
    first_log[0] = 0;
    const m2_game *game = m2_find_game(game_name);
    if (!game) FAIL("Unknown game %s", game_name);
    const char *dirs[] = { romdir, NULL };
    if (coop && strncmp(game->name, "daytona", 7))
        FAIL("Linked play (--coop): only Daytona USA for now");
    nplayers = coop ? 2 : 1;
    if (coop && !split_side && !wide_set)   /* one above the other: wide views fill each half */
        wide_on = wide_fill = 1;
    m2opt_mkdirs(nvdir);
    for (int c = 0; c < 3; c++)
        if (gamma[c] <= 0) gamma[c] = 1.0f;

    for (int i = 0; i < nplayers; i++) {
        player *p = &pl[i];
        p->b = m2_create(game, dirs, logmsg);
        p->tg = malloc(sizeof *p->tg);
        p->pipe = m2pipe_create();
        p->geo = m2geo_create();
        p->mesh[0] = m2gl_mesh_create();
        p->mesh[1] = m2gl_mesh_create();
        if (!p->b)
            FAIL("%s: the ROM files can't be loaded from %.400s%s%s", game->title, romdir,
                 first_log[0] ? "\n" : "", first_log);
        if (!p->tg || !p->pipe || !p->geo || !p->mesh[0] || !p->mesh[1]) FAIL("Out of memory");
        /* linked boards keep their own save data (settings differ) */
        if (coop) snprintf(p->nvpath, sizeof p->nvpath, "%s/%s-coop%d.DAT", nvdir, game->name, i + 1);
        else snprintf(p->nvpath, sizeof p->nvpath, "%s/%s.DAT", nvdir, game->name);
        if (!m2_nvram_load(p->b, p->nvpath))
            printf("NVRAM loaded from %s\n", p->nvpath);
        else if (m2_nvram_defaults(p->b))
            printf("NVRAM: first-boot defaults\n");
        if (coop)   /* player 1's board is the master, player 2's the slave; cars 1 and 2 */
            m2_nvram_set_link(p->b, i == 0 ? 1 : 2, i + 1);

        m2tile_init(p->tg, gamma[0], gamma[1], gamma[2]);
        p->b->hooks.user = p;
        p->b->hooks.tilemap_written = on_tile;
        p->b->hooks.cg_written = on_cg;
        p->b->hooks.xlat_written = on_xlat;
        p->b->hooks.texture_written = on_tex;
        p->b->hooks.luma_written = on_luma;
        p->b->hooks.palette_written = on_pal;
        p->b->hooks.sound_command = on_sound;
        if (i == 0) {   /* --coop: player 2's machine plays silently */
            p->snd = m2snd_create(p->b);
            if (!p->snd)
                fprintf(stderr, "no sound board (missing sound ROMs?)\n");
        }
        p->replay = p->b->hooks;   /* the video hooks now run on the main thread */
        m2pipe_attach(p->pipe, p->b);
        p->in.updown_gears = updown_gears;   /* the original's UpDownGears / HoldGears */
        p->in.hold_gears = hold_gears;
        m2input_init(&p->in);
    }
    if (coop) {   /* the two boards' network boards, linked in a ring */
        m2_board *ring[MAX_PLAYERS] = { pl[0].b, pl[1].b };
        net = m2net_create(ring, nplayers);
        if (!net) FAIL("Out of memory");
    }

    char title[256];
    snprintf(title, sizeof title, "%s - Model 2", game->title);
    SDL_SetWindowTitle(win, title);
    if (from_gui) {   /* the launcher's window becomes the game's */
        int ww, wh;
        game_window_size(o, &ww, &wh);
        SDL_SetWindowFullscreen(win, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
        if (!fullscreen) {
            SDL_SetWindowSize(win, ww, wh);
            SDL_SetWindowPosition(win, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        }
    }
    SDL_GL_SetSwapInterval(vsync);
    for (int i = 0; i < nplayers; i++)
        if (!(pl[i].gl = m2gl_create(USE_GLES))) FAIL("OpenGL: the renderer can't start");

    SDL_AudioDeviceID audio = 0;
    if (pl[0].snd) {
        SDL_AudioSpec want, have;
        SDL_zero(want);
        want.freq = M2SND_RATE;
        want.format = AUDIO_S16SYS;
        want.channels = 2;
        want.samples = 1024;
        want.callback = audio_callback;
        want.userdata = pl[0].snd;
        audio = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
        if (!audio)
            fprintf(stderr, "audio: %s\n", SDL_GetError());
        else
            SDL_PauseAudioDevice(audio, 0);
    }

    /* the pads already connected (their "added" events may have gone to the
       launcher): real pads first, then devices without sticks (a TV box's IR
       remote shows up as a pad with only a d-pad) */
    SDL_GameController *pads[2] = { NULL, NULL };
    for (int pass = 0, p = 0; pass < 2; pass++)
        for (int j = 0; j < SDL_NumJoysticks() && p < 2; j++)
            if (SDL_IsGameController(j) && pad_has_sticks(j) == !pass && (pads[p] = SDL_GameControllerOpen(j))) {
                printf("pad %d: %s\n", p + 1, SDL_GameControllerName(pads[p]));
                p++;
            }
    /* host controls as they arrive; handed to the players before each frame
       (--coop: keyboard and pad 1 drive player 1, pad 2 player 2) */
    m2_input_state raw;
    memset(&raw, 0, sizeof raw);

    char shot_path[512] = "";
    int shot_frame = 0;
    if (getenv("M2EMU_SHOT")) {
        snprintf(shot_path, sizeof shot_path, "%s", getenv("M2EMU_SHOT"));
        char *c = strrchr(shot_path, ':');
        if (c) { *c = 0; shot_frame = atoi(c + 1); }
    }

    const char *script = getenv("M2EMU_KEYS");
    const char *script2 = getenv("M2EMU_KEYS2");   /* --coop: player 2's keys */
    uint8_t key2[256];
    memset(key2, 0, sizeof key2);
    const char *actions = getenv("M2EMU_ACTIONS");
    long iter = 0;
    int bench = getenv("M2EMU_BENCH") ? atoi(getenv("M2EMU_BENCH")) : 0;
    double bt[2] = { 0, 0 };
    int bench_from = getenv("M2EMU_BENCH_FROM") ? atoi(getenv("M2EMU_BENCH_FROM")) : 1;
    double bench_cpu0 = 0;

    for (int i = 0; i < nplayers; i++) {
        player *p = &pl[i];
        p->go = SDL_CreateSemaphore(0);
        p->done = SDL_CreateSemaphore(0);
        p->tgo = SDL_CreateSemaphore(0);
        p->tdone = SDL_CreateSemaphore(0);
        int sems = p->go && p->done && p->tgo && p->tdone;
        p->thread = sems ? SDL_CreateThread(emu_thread, "m2 board", p) : NULL;
        p->tthread = p->thread ? SDL_CreateThread(geo_thread, "m2 geometrizer", p) : NULL;
        if (!p->tthread) {   /* stop the ones already running */
            for (int k = 0; k <= i; k++) {
                pl[k].quit = 1;
                if (pl[k].thread) {
                    SDL_SemPost(pl[k].go);
                    SDL_WaitThread(pl[k].thread, NULL);
                    pl[k].thread = NULL;
                }
                if (pl[k].tthread) {
                    SDL_SemPost(pl[k].tgo);
                    SDL_WaitThread(pl[k].tthread, NULL);
                    pl[k].tthread = NULL;
                }
            }
            FAIL("Can't start the emulation threads: %s", SDL_GetError());
        }
    }
    int in_flight = 0, want_reset = 0;

    const double frame_s = 1.0 / game->fps;
    const double freq = (double)SDL_GetPerformanceFrequency();
    double next = SDL_GetPerformanceCounter() / freq;
    int running = 1, paused = 0, fast = 0, frames = 0, drawn = 0;
    /* frame skip: when running late, the frame just collected isn't drawn
       (nor its tile layers made and uploaded: what changed carries over to
       the next one); the board and the geometrizer still run every frame */
    int frame_skip = o->frame_skip && !bench && !shot_frame;
    int skip_draw = 0, skip_next = 0, skips_in_row = 0;
    double board_avg = 0;   /* seconds per board frame, running average */
    double fps_t = next, bench_t0 = next;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                running = 0;
                quit_app = 1;
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                int down = e.type == SDL_KEYDOWN;
                SDL_Scancode sc = e.key.keysym.scancode;
                if (down && !e.key.repeat) {
                    if (sc == SDL_SCANCODE_ESCAPE) {   /* back to the launcher, or quit */
                        running = 0;
                        quit_app = !from_gui;
                    }
                    else if (sc == SDL_SCANCODE_P) {
                        paused = !paused;
                        if (audio) SDL_PauseAudioDevice(audio, paused);
                    }
                    else if (sc == SDL_SCANCODE_F3) want_reset = 1;
                    else if (sc == SDL_SCANCODE_F6) tex_filter = (tex_filter + 1) % 3;
                    else if (sc == SDL_SCANCODE_F7) saturation = saturation < 1.1f ? 1.2f : saturation < 1.3f ? 1.4f : 1.0f;
                    else if (sc == SDL_SCANCODE_F8) mesh_blend = !mesh_blend;
                    else if (sc == SDL_SCANCODE_F9) wide_on = !wide_on;
                    else if (sc == SDL_SCANCODE_F10) scale_opt = (scale_opt + 1) % 5;
                    else if (sc == SDL_SCANCODE_F11) {
                        fullscreen = !fullscreen;
                        SDL_SetWindowFullscreen(win, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
                    }
                }
                if (sc == SDL_SCANCODE_TAB) fast = down;
                uint8_t dik = dik_from_sdl(sc);
                if (dik) raw.key[dik] = (uint8_t)down;
                break;
            }
            case SDL_CONTROLLERDEVICEADDED: {
                int open = 0;   /* already opened above */
                for (int p = 0; p < 2; p++)
                    if (pads[p] && SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pads[p])) ==
                                   SDL_JoystickGetDeviceInstanceID(e.cdevice.which))
                        open = 1;
                if (open) break;
                /* a real pad takes the place of a stickless one (an IR remote) */
                int real = pad_has_sticks(e.cdevice.which);
                for (int p = 0; p < 2 && real; p++)
                    if (pads[p] && SDL_JoystickNumAxes(SDL_GameControllerGetJoystick(pads[p])) < 2) {
                        printf("pad %d: %s set aside\n", p + 1, SDL_GameControllerName(pads[p]));
                        SDL_GameControllerClose(pads[p]);
                        pads[p] = NULL;
                        memset(raw.pad[p], 0, sizeof raw.pad[p]);
                        memset(raw.axis[p], 0, sizeof raw.axis[p]);
                        break;
                    }
                for (int p = 0; p < 2; p++)
                    if (!pads[p]) {
                        pads[p] = SDL_GameControllerOpen(e.cdevice.which);
                        if (pads[p])
                            printf("pad %d: %s\n", p + 1, SDL_GameControllerName(pads[p]));
                        break;
                    }
                break;
            }
            case SDL_CONTROLLERDEVICEREMOVED:
                for (int p = 0; p < 2; p++)
                    if (pads[p] && SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pads[p])) == e.cdevice.which) {
                        printf("pad %d disconnected\n", p + 1);
                        SDL_GameControllerClose(pads[p]);
                        pads[p] = NULL;
                        memset(raw.pad[p], 0, sizeof raw.pad[p]);
                        memset(raw.axis[p], 0, sizeof raw.axis[p]);
                    }
                break;
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                for (int p = 0; p < 2; p++)
                    if (pads[p] && SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pads[p])) == e.cbutton.which) {
                        int k = pad_button_from_sdl(e.cbutton.button);
                        if (k >= 0) raw.pad[p][k] = e.type == SDL_CONTROLLERBUTTONDOWN;
                    }
                break;
            case SDL_CONTROLLERAXISMOTION:
                for (int p = 0; p < 2; p++)
                    if (pads[p] && SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pads[p])) == e.caxis.which &&
                        e.caxis.axis < M2PAD_AXES)
                        raw.axis[p][e.caxis.axis] = e.caxis.value;
                break;
            }
        }

        if (actions) {   /* debugging: M2EMU_ACTIONS=iteration:p|r|q,... (pause, F3, quit) */
            for (const char *k = actions; *k;) {
                long at = 0;
                char act = 0;
                if (sscanf(k, "%ld:%c", &at, &act) == 2 && at == iter) {
                    if (act == 'p') paused = !paused;
                    if (act == 'r') want_reset = 1;
                    if (act == 'q') running = 0;
                    if (act == 'p' && audio) SDL_PauseAudioDevice(audio, paused);
                }
                k = strchr(k, ',');
                if (!k) break;
                k++;
            }
        }
        iter++;

        /* the frames whose 3D the geometrizer threads were making: they go
           on screen in this iteration; the threads are then free for the
           frames collected next */
        int drew = 0;
        for (int i = 0; i < nplayers; i++) {
            player *p = &pl[i];
            if (!p->geo_busy)
                continue;
            SDL_SemWait(p->tdone);
            p->geo_busy = 0;
            p->drawn = p->mesh[p->mesh_next];
            p->mesh_next ^= 1;
            p->view_cur = p->view;
            p->pf_cur = p->pf;
            p->has_cur = 1;
            drew = 1;
        }

        /* collect the frame the board threads were running; from here until
           the next go, the boards are ours */
        Uint64 t0 = SDL_GetPerformanceCounter();
        int got = in_flight;
        if (in_flight) {
            for (int i = 0; i < nplayers; i++)
                SDL_SemWait(pl[i].done);
            in_flight = 0;
            if (net)   /* the ring, between the boards' frames */
                m2net_frame(net);
        }
        Uint64 t1 = SDL_GetPerformanceCounter();

        if (want_reset) {   /* F3: reset, keeping backup RAM and EEPROM */
            if (audio) SDL_LockAudioDevice(audio);
            for (int i = 0; i < nplayers; i++) {
                player *p = &pl[i];
                m2_nvram_save(p->b, p->nvpath);
                m2_reset(p->b);
                m2_nvram_load(p->b, p->nvpath);
                m2pipe_invalidate(p->pipe);
                if (net) m2net_reset(net, i);
                if (p->snd) m2snd_reset(p->snd);
            }
            if (audio) SDL_UnlockAudioDevice(audio);
            want_reset = 0;
        }
        if (script)
            script_keys(script, (uint32_t)pl[0].b->frame, raw.key);
        if (script2)
            script_keys(script2, (uint32_t)pl[0].b->frame, key2);
        if (!paused) {   /* start the next frame */
            if (!coop) {
                memcpy(pl[0].in.key, raw.key, sizeof raw.key);
                memcpy(pl[0].in.pad, raw.pad, sizeof raw.pad);
                memcpy(pl[0].in.axis, raw.axis, sizeof raw.axis);
            } else {
                for (int i = 0; i < 2; i++) {
                    m2_input_state *in = &pl[i].in;
                    memcpy(in->key, i ? key2 : raw.key, sizeof in->key);
                    memset(in->pad, 0, sizeof in->pad);
                    memset(in->axis, 0, sizeof in->axis);
                    memcpy(in->pad[0], raw.pad[i], sizeof in->pad[0]);
                    memcpy(in->axis[0], raw.axis[i], sizeof in->axis[0]);
                }
            }
            for (int i = 0; i < nplayers; i++)
                m2input_apply(&pl[i].in, pl[i].b);
            frames++;
            if (pipelined) {
                for (int i = 0; i < nplayers; i++)
                    SDL_SemPost(pl[i].go);
                in_flight = 1;
            } else {   /* --pipeline off: run them here and draw them now */
                for (int i = 0; i < nplayers; i++) {
                    m2_run_frame(pl[i].b);
                    m2pipe_capture(pl[i].pipe, pl[i].b);
                }
                if (net)
                    m2net_frame(net);
                t1 = SDL_GetPerformanceCounter();
                pl[0].ticks = t1 - t0;
                got = 1;
            }
        }

        /* the collected frames: their views, and their 3D onto the
           geometrizer threads, which make it while the frames before are
           drawn below; --coop: player 1 above (or left), player 2 below (or
           right) */
        int w, h;
        SDL_GL_GetDrawableSize(win, &w, &h);
        for (int i = 0; i < nplayers; i++) {
            player *p = &pl[i];
            int rx = 0, ry = 0, rw = w, rh = h;   /* GL window coordinates, origin bottom left */
            if (nplayers > 1 && split_side) {   /* player 1 left */
                rx = i * (w / nplayers);
                rw = i == nplayers - 1 ? w - rx : w / nplayers;
            } else if (nplayers > 1) {
                rh = h / nplayers;
                ry = (nplayers - 1 - i) * rh;
                if (i == 0) rh = h - ry;
            }
            p->rx = rx; p->ry = ry; p->rw = rw; p->rh = rh;
            if (!got)
                continue;
            /* after start and reset the pipe reports everything as written */
            p->vb = m2pipe_apply(p->pipe, &p->replay, &p->pf);
            /* the frame's aspect: 4:3, a fixed wide ratio, or that of its
               rectangle (fill; never narrower than 4:3, at most 4:1) */
            double rect = (double)rw / (rh > 0 ? rh : 1);
            double ratio = !wide_on ? 4.0 / 3.0 : wide_fill ? rect : wide_ratio;
            if (ratio < 4.0 / 3.0) ratio = 4.0 / 3.0;
            if (ratio > 4.0) ratio = 4.0;
            /* a rectangle narrower than the frame (side by side on a wide
               screen): keep = bands above and below; stretch = squeezed;
               crop = while the game shows 3D, the frame's centre at the
               right proportions, with the HUD (every layer but the low-
               priority background, i.e. the sky) squeezed in whole; other
               screens are squeezed */
            int crop = aspect_mode == 2 && rect < ratio && p->pf.wide.widescreen;
            int squeeze = aspect_mode == 1 || (aspect_mode == 2 && rect < ratio && !crop);
            if (crop)
                ratio = rect < 0.5 ? 0.5 : rect;
            /* widescreen: a wider frame; the game's rule decides each frame
               whether the 3D sees the extra width and which layers stretch */
            m2_view *view = &p->view;
            view->frame_w = ((int)(M2_SCREEN_W * ratio * 0.75 + 0.5) + 1) & ~1;
            view->stretch = crop ? M2_STRETCH_A_LOW | M2_STRETCH_A_HIGH | M2_STRETCH_B_HIGH
                                 : wide_on ? p->pf.wide.stretch : 0;
            view->smooth = smooth;
            view->tex_filter = tex_filter;
            view->fill = squeeze;
            view->mesh_blend = mesh_blend;
            view->saturation = saturation;
            view->scale = scale_opt ? scale_opt : rh / M2_SCREEN_H;
            if (view->scale < 1) view->scale = 1;
            p->geo->wide_extra = (view->frame_w - M2_SCREEN_W) * 0.5f;
            p->geo->wide_fov = wide_on && p->pf.wide.widescreen && !crop;
            SDL_SemPost(p->tgo);
            p->geo_busy = 1;
            p->tiles_due = 1;
        }

        /* draw the frames made (or the last ones again) */
        skip_draw = skip_next && drew;
        skip_next = 0;
        if (!pl[0].has_cur && !skip_draw) {   /* none yet */
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);
        }
        for (int i = 0; i < nplayers; i++) {
            player *p = &pl[i];
            if (p->has_cur && !skip_draw) {
                m2_view v = p->view_cur;   /* the settings keys act at once */
                v.smooth = smooth;
                v.tex_filter = tex_filter;
                v.mesh_blend = mesh_blend;
                v.saturation = saturation;
                m2gl_draw_rect(p->gl, p->drawn, &v, p->rx, p->ry, p->rw, p->rh);
            }
        }
        const m2_pipe_frame *pf = &pl[0].pf_cur;
        if (bench && drew) {   /* M2EMU_BENCH=frames: unthrottled, timing split */
            Uint64 board = 0;
            for (int i = 0; i < nplayers; i++)
                if (pl[i].ticks > board) board = pl[i].ticks;
            bt[0] += (double)board / freq;
            bt[1] += (double)(t1 - t0) / freq;
            if ((int)pf->frame == bench_from) {   /* M2EMU_BENCH_FROM: time from this frame on */
                memset(bt, 0, sizeof bt);
                bench_t0 = SDL_GetPerformanceCounter() / freq;
                bench_cpu0 = thread_cpu_s();
            }
            if ((int)pf->frame >= bench) {
                double wall = SDL_GetPerformanceCounter() / freq - bench_t0;
                int n = bench - bench_from;
                printf("frames %d-%d: %.1f fps | per frame: board %.2f ms (own thread%s), "
                       "main thread CPU %.2f ms, main thread waiting for the board %.2f ms\n",
                       bench_from, bench, n / wall, bt[0] * 1000 / n, nplayers > 1 ? "s, slowest" : "",
                       (thread_cpu_s() - bench_cpu0) * 1000 / n, bt[1] * 1000 / n);
                running = 0;
            }
        }
        if (shot_frame && drew && (int)pf->frame == shot_frame) {   /* debugging: M2EMU_SHOT=file.ppm:frame */
            unsigned char *px = malloc((size_t)w * h * 4);
            FILE *f = fopen(shot_path, "wb");
            if (px && f) {
                glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, px);
                fprintf(f, "P6\n%d %d\n255\n", w, h);
                for (int y = h - 1; y >= 0; y--)
                    for (int x = 0; x < w; x++)
                        fwrite(px + ((size_t)y * w + x) * 4, 1, 3, f);
            }
            if (f) fclose(f);
            free(px);
            running = 0;
        }
        if (!skip_draw) {
            SDL_GL_SwapWindow(win);
            drawn += drew;
        }

        /* the collected frames' tile layers and uploads, while the GPU draws
           and the geometrizer threads run (the uploads are for the next draw);
           none if that draw will be skipped: already later than its start,
           at most two frames in a row, and only while the boards keep up
           (when they are what's slow, skipping the drawing gains nothing) */
        int due = 0;
        Uint64 board_ticks = 0;
        for (int i = 0; i < nplayers; i++) {
            due |= pl[i].tiles_due;
            if (pl[i].ticks > board_ticks) board_ticks = pl[i].ticks;
        }
        if (due)
            board_avg = board_avg * 0.8 + (double)board_ticks / freq * 0.2;
        if (due && frame_skip && frame_cap && !fast && skips_in_row < 2 && board_avg < frame_s * 0.9 &&
            SDL_GetPerformanceCounter() / freq > next + frame_s) {
            skip_next = 1;
            skips_in_row++;
            for (int i = 0; i < nplayers; i++)
                pl[i].tiles_due = 0;
        } else if (due) {
            skips_in_row = 0;
        }
        for (int i = 0; i < nplayers; i++) {
            player *p = &pl[i];
            if (!p->tiles_due)
                continue;
            m2tile_render(p->tg, p->vb);
            m2gl_upload(p->gl, p->vb, p->tg, 1);
            p->tiles_due = 0;
        }

        /* pace to the game's refresh rate */
        double now = SDL_GetPerformanceCounter() / freq;
        next += frame_s;
        if (fast || bench || !frame_cap || next < now - 0.1)
            next = now;
        else if (next > now)
            SDL_Delay((Uint32)((next - now) * 1000.0));
        if (now - fps_t >= 1.0) {
            char gear[32] = "";
            if (game->input_fn == 0x4c9080 || game->input_fn == 0x4cb870) {
                if (coop)
                    snprintf(gear, sizeof gear, " - gears %c / %c",
                             pl[0].in.gear[0] ? '0' + pl[0].in.gear[0] : 'N',
                             pl[1].in.gear[0] ? '0' + pl[1].in.gear[0] : 'N');
                else
                    snprintf(gear, sizeof gear, " - gear %c", pl[0].in.gear[0] ? '0' + pl[0].in.gear[0] : 'N');
            }
            char shown[32] = "";   /* frames drawn, when frame skip left some out */
            if (drawn < frames)
                snprintf(shown, sizeof shown, " (%.1f drawn)", drawn / (now - fps_t));
            snprintf(title, sizeof title, "%s - Model 2%s - %.1f fps%s%s%s%s", game->title, coop ? " - linked" : "",
                     frames / (now - fps_t), shown, gear, wide_on ? " - wide" : "", paused ? " (paused)" : "");
            SDL_SetWindowTitle(win, title);
#ifdef __ANDROID__   /* no title bar to show it in: to the log */
            printf("%s\n", title);
#endif
            frames = drawn = 0;
            fps_t = now;
        }
    }

    for (int i = 0; i < nplayers; i++) {
        player *p = &pl[i];
        if (in_flight)
            SDL_SemWait(p->done);
        p->quit = 1;
        SDL_SemPost(p->go);
        SDL_WaitThread(p->thread, NULL);
        SDL_SemPost(p->tgo);
        SDL_WaitThread(p->tthread, NULL);
        SDL_DestroySemaphore(p->go);
        SDL_DestroySemaphore(p->done);
        SDL_DestroySemaphore(p->tgo);
        SDL_DestroySemaphore(p->tdone);
    }
    if (audio) SDL_CloseAudioDevice(audio);
    for (int i = 0; i < nplayers; i++) {
        player *p = &pl[i];
        if (m2_nvram_save(p->b, p->nvpath))
            fprintf(stderr, "cannot save %s: %s\n", p->nvpath, strerror(errno));
        m2pipe_destroy(p->pipe);
        m2snd_destroy(p->snd);
        m2gl_destroy(p->gl);
        m2geo_destroy(p->geo);
        m2gl_mesh_destroy(p->mesh[0]);
        m2gl_mesh_destroy(p->mesh[1]);
        free(p->tg);
    }
    m2net_destroy(net);
    for (int i = 0; i < nplayers; i++)
        m2_destroy(pl[i].b);
    for (int p = 0; p < 2; p++)
        if (pads[p])
            SDL_GameControllerClose(pads[p]);
    memset(pl, 0, sizeof pl);
    return quit_app;

fail:
    for (int i = 0; i < MAX_PLAYERS; i++) {
        player *p = &pl[i];
        if (p->go) SDL_DestroySemaphore(p->go);
        if (p->done) SDL_DestroySemaphore(p->done);
        if (p->tgo) SDL_DestroySemaphore(p->tgo);
        if (p->tdone) SDL_DestroySemaphore(p->tdone);
        m2pipe_destroy(p->pipe);
        m2snd_destroy(p->snd);
        m2gl_destroy(p->gl);
        m2geo_destroy(p->geo);
        m2gl_mesh_destroy(p->mesh[0]);
        m2gl_mesh_destroy(p->mesh[1]);
        free(p->tg);
        m2_destroy(p->b);
    }
    m2net_destroy(net);
    memset(pl, 0, sizeof pl);
    return -1;
#undef FAIL
}

static void usage(void)
{
    int n;
    const m2_game *list = m2_game_list(&n);
    fprintf(stderr, "usage: m2emu                      the launcher (games and settings)\n"
                    "       m2emu --no-gui [options] <game>\n"
                    "options: [-r romdir] [-n nvdir] [--vsync] [--no-frame-cap] [--fullscreen] [--size WxH]\n"
                    "         [--widescreen 16:9|16:10|fill|off] [--scale N|auto] [--sharp]\n"
                    "         [--texture-filter nearest|bilinear|trilinear]\n"
                    "         [--mesh blend|checker] [--saturation S] [--gamma G | --gamma R,G,B]\n"
                    "         [--shifter sequential|hpattern] [--hold-gears] [--pipeline on|off] [--frame-skip auto|off]\n"
                    "         [--aspect keep|stretch|crop] [--coop [--split side|stack]]\n"
                    "keys: Esc quit (with the launcher: back to it), F3 reset, P pause, Tab fast forward,\n"
                    "      F6 texture filter, F7 saturation, F8 mesh, F9 widescreen, F10 scale,\n"
                    "      F11 fullscreen.\n"
                    "Full guide: port/README.md\n"
                    "games:");
    for (int i = 0; i < n; i++)
        fprintf(stderr, " %s", list[i].name);
    fprintf(stderr, "\n");
}

int main(int argc, char **argv)
{
    m2_options o;
    const char *game_name = NULL;
    int no_gui = 0, help = 0;
    char cfg[1024], last[64] = "";

#ifdef __ANDROID__
    /* Android: work in the app's folder on the shared storage
       (/sdcard/Android/data/org.m2emu/files: roms/, NVDATA/, m2emu.ini), and
       send stdout and stderr, which go nowhere there, to m2emu.log in it */
    const char *ext = SDL_AndroidGetExternalStoragePath();
    if (ext && chdir(ext) == 0 && freopen("m2emu.log", "w", stdout)) {
        setvbuf(stdout, NULL, _IOLBF, 0);
        dup2(fileno(stdout), fileno(stderr));
        setvbuf(stderr, NULL, _IOLBF, 0);
    }
#endif

    /* --no-gui: the command line alone. Otherwise the launcher, with the
       settings it keeps in m2emu.ini (a game named on the command line is
       selected in it). */
    m2opt_defaults(&o);
    if (m2opt_parse(&o, argc, argv, &game_name, &no_gui, &help))
        return 1;
    if (help || (no_gui && !game_name)) {
        usage();
        return help ? 0 : 1;
    }
    m2opt_config_path(cfg, sizeof cfg);
    if (!no_gui) {
        m2opt_defaults(&o);
        m2opt_load(&o, cfg, last, sizeof last);
    }
    if (game_name && !m2_find_game(game_name)) {
        fprintf(stderr, "unknown game %s (m2emu --help lists them)\n", game_name);
        return 1;
    }

    SDL_SetHint(SDL_HINT_ACCELEROMETER_AS_JOYSTICK, "0");   /* Android: not a pad */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    if (USE_GLES) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    } else {   /* 2.1, compatibility profile: luminance textures, GLSL 1.20 */
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    }
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    const int launcher_w = 1280, launcher_h = 800;
    int win_w = launcher_w, win_h = launcher_h;
    if (no_gui)
        game_window_size(&o, &win_w, &win_h);
    SDL_Window *win = SDL_CreateWindow("Model 2 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       win_w, win_h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                                       (no_gui && o.fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
    if (!win) { fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError()); return 1; }
    SDL_GLContext ctx = SDL_GL_CreateContext(win);
    if (!ctx) { fprintf(stderr, "%s context: %s\n", USE_GLES ? "OpenGL ES 2" : "OpenGL 2.1", SDL_GetError()); return 1; }
    const char *missing = NULL;
    if (!m2glapi_load(SDL_GL_GetProcAddress, &missing)) {
        fprintf(stderr, "OpenGL: %s not available\n", missing);
        return 1;
    }

    /* extra pad mappings (SDL_GameControllerDB format), if present */
    {
        char dbpath[1200];
        char *base = SDL_GetBasePath();
        snprintf(dbpath, sizeof dbpath, "%sgamecontrollerdb.txt", base ? base : "");
        SDL_free(base);
        if (SDL_GameControllerAddMappingsFromFile(dbpath) > 0)
            printf("pad mappings loaded from %s\n", dbpath);
        for (int j = 0; j < SDL_NumJoysticks(); j++)
            if (!SDL_IsGameController(j))
                printf("joystick %d (%s) has no pad mapping; add it to gamecontrollerdb.txt\n",
                       j, SDL_JoystickNameForIndex(j));
    }

    char err[1024] = "";
    int status = 0;
    if (no_gui) {
        if (run_game(win, &o, game_name, 0, err, sizeof err) < 0) {
            fprintf(stderr, "%s\n", err);
            status = 1;
        }
    } else if (m2launch_init(win, ctx, USE_GLES)) {
        fprintf(stderr, "the launcher can't start (use --no-gui)\n");
        status = 1;
    } else {
        char game[64];
        snprintf(game, sizeof game, "%s", game_name ? game_name : last);
        const char *msg = NULL;
        while (m2launch_run(win, &o, cfg, game, sizeof game, msg)) {
            int r = run_game(win, &o, game, 1, err, sizeof err);
            SDL_SetWindowFullscreen(win, 0);   /* back to the launcher's window */
            SDL_SetWindowSize(win, launcher_w, launcher_h);
            SDL_SetWindowPosition(win, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
            if (r > 0)
                break;
            msg = r < 0 ? err : NULL;
        }
        m2launch_shutdown();
    }
    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return status;
}
