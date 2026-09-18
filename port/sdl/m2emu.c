/*
 * SDL2 + OpenGL ES 2 frontend for the Model 2 port.
 *
 *   m2emu [-r romdir] [-n nvdir] [--vsync] [--fullscreen] [--size WxH]
 *         [--widescreen 16:9|16:10|off] [--scale N|auto] [--sharp]
 *         [--mesh blend|checker] [--saturation S] [--gamma G | --gamma R,G,B]
 *         [--shifter sequential|hpattern] [--hold-gears] [--pipeline on|off]
 *         [--coop] <game>
 *
 * --coop (Daytona USA): two boards linked through their network boards, as
 * two cabinets, in one window side by side; keyboard and pad 1 drive the
 * left one, pad 2 the right one.
 *
 * Keys (from the original's input definitions): 5/6 coin, 1/2 start,
 * F1 service, F2 test, arrows, Z X C V / A S D F G game buttons.
 * Frontend: Esc quit, F3 reset, P pause, Tab (hold) fast forward,
 * F7 saturation (1.0, 1.2, 1.4), F8 mesh blend/checker, F9 widescreen on/off,
 * F10 render scale (auto, 1-4), F11 fullscreen.
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

#include <SDL2/SDL.h>
#include <GLES2/gl2.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

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
    const m2_board *vb;            /* the drawn frame's video memories */
    m2_pipe_frame pf;
    char      nvpath[1200];
    /* board thread: `go` starts a frame, `done` says it is captured;
       between the two the main thread leaves the board alone */
    SDL_Thread *thread;
    SDL_sem  *go, *done;
    int       quit;
    Uint64    ticks;               /* time spent in the last frame */
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

/* The sound boards run here, on SDL's audio thread, at the device's pace:
   their tempo stays right even when the emulation can't keep full speed.
   Two boards (--coop) are mixed. */
static void audio_callback(void *u, Uint8 *stream, int len)
{
    (void)u;
    int16_t *out = (int16_t *)stream;
    int n = len / 4;
    if (nplayers == 1) {
        m2snd_render(pl[0].snd, out, n);
        return;
    }
    static int16_t a[1024 * 2], b[1024 * 2];
    while (n > 0) {
        int k = n < 1024 ? n : 1024;
        memset(a, 0, sizeof(int16_t) * (size_t)k * 2);
        memset(b, 0, sizeof(int16_t) * (size_t)k * 2);
        if (pl[0].snd) m2snd_render(pl[0].snd, a, k);
        if (pl[1].snd) m2snd_render(pl[1].snd, b, k);
        for (int i = 0; i < k * 2; i++) {
            int v = (a[i] + b[i]) * 3 / 4;   /* two cabinets' worth of sound */
            out[i] = (int16_t)(v > 32767 ? 32767 : v < -32768 ? -32768 : v);
        }
        out += k * 2;
        n -= k;
    }
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

static void logmsg(const char *m) { fprintf(stderr, "%s\n", m); }

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

static int pad_button_from_sdl(int b)
{
    return b >= 0 && b < M2PAD_BUTTONS ? b : -1;   /* same order as SDL_GameControllerButton */
}

static void mkdirs(const char *path)
{
    char tmp[1024];
    snprintf(tmp, sizeof tmp, "%s", path);
    for (char *p = tmp + 1; *p; p++)
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, 0755);
            *p = '/';
        }
    mkdir(tmp, 0755);
}

int main(int argc, char **argv)
{
    const char *romdir = getenv("M2_ROMS") ? getenv("M2_ROMS") : "roms";
    const char *game_name = NULL;
    char nvdir[1024] = "";
    int vsync = 0, fullscreen = 0, win_w = 0, win_h = 0, smooth = 1;
    int wide_on = 0, scale_opt = 0;   /* scale 0 = auto */
    double wide_ratio = 16.0 / 9.0;
    int mesh_blend = 1, updown_gears = 0, hold_gears = 0, pipelined = 1, coop = 0;
    float saturation = 1.0f, gamma[3] = { 1.0f, 1.0f, 1.0f };

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-r") && i + 1 < argc) romdir = argv[++i];
        else if (!strcmp(argv[i], "-n") && i + 1 < argc) snprintf(nvdir, sizeof nvdir, "%s", argv[++i]);
        else if (!strcmp(argv[i], "--vsync")) vsync = 1;
        else if (!strcmp(argv[i], "--fullscreen")) fullscreen = 1;
        else if (!strcmp(argv[i], "--sharp")) smooth = 0;
        else if (!strcmp(argv[i], "--shifter") && i + 1 < argc) updown_gears = !strcmp(argv[++i], "sequential");
        else if (!strcmp(argv[i], "--hold-gears")) hold_gears = 1;
        else if (!strcmp(argv[i], "--pipeline") && i + 1 < argc) pipelined = strcmp(argv[++i], "off") != 0;
        else if (!strcmp(argv[i], "--coop")) coop = 1;
        else if (!strcmp(argv[i], "--mesh") && i + 1 < argc) mesh_blend = strcmp(argv[++i], "checker") != 0;
        else if (!strcmp(argv[i], "--saturation") && i + 1 < argc) saturation = (float)atof(argv[++i]);
        else if (!strcmp(argv[i], "--gamma") && i + 1 < argc) {
            int n = sscanf(argv[++i], "%f,%f,%f", &gamma[0], &gamma[1], &gamma[2]);
            if (n == 1) gamma[1] = gamma[2] = gamma[0];
        }
        else if (!strcmp(argv[i], "--size") && i + 1 < argc) sscanf(argv[++i], "%dx%d", &win_w, &win_h);
        else if (!strcmp(argv[i], "--scale") && i + 1 < argc) { i++; scale_opt = !strcmp(argv[i], "auto") ? 0 : atoi(argv[i]); }
        else if (!strcmp(argv[i], "--widescreen") && i + 1 < argc) {
            int a = 0, b = 0;
            i++;
            if (sscanf(argv[i], "%d:%d", &a, &b) == 2 && a > 0 && b > 0) { wide_ratio = (double)a / b; wide_on = 1; }
            else wide_on = 0;
        }
        else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) { game_name = NULL; break; }
        else if (argv[i][0] != '-') game_name = argv[i];
        else { fprintf(stderr, "unknown option %s\n", argv[i]); return 1; }
    }
    if (!game_name) {
        int n;
        const m2_game *list = m2_game_list(&n);
        fprintf(stderr, "usage: m2emu [-r romdir] [-n nvdir] [--vsync] [--fullscreen] [--size WxH]\n"
                        "             [--widescreen 16:9|16:10|off] [--scale N|auto] [--sharp]\n"
                        "             [--mesh blend|checker] [--saturation S] [--gamma G | --gamma R,G,B]\n"
                        "             [--shifter sequential|hpattern] [--hold-gears] [--pipeline on|off]\n"
                        "             [--coop] <game>\n"
                        "keys: Esc quit, F3 reset, P pause, Tab fast forward, F7 saturation, F8 mesh,\n"
                        "      F9 widescreen, F10 scale, F11 fullscreen. Full guide: port/README.md\n"
                        "games:");
        for (int i = 0; i < n; i++)
            fprintf(stderr, " %s", list[i].name);
        fprintf(stderr, "\n");
        return 1;
    }
    if (!nvdir[0]) {
        const char *xdg = getenv("XDG_DATA_HOME"), *home = getenv("HOME");
        if (xdg) snprintf(nvdir, sizeof nvdir, "%s/m2emu/NVDATA", xdg);
        else snprintf(nvdir, sizeof nvdir, "%s/.local/share/m2emu/NVDATA", home ? home : ".");
    }

    const m2_game *game = m2_find_game(game_name);
    if (!game) { fprintf(stderr, "unknown game %s\n", game_name); return 1; }
    const char *dirs[] = { romdir, NULL };
    if (coop && strncmp(game->name, "daytona", 7)) {
        fprintf(stderr, "--coop: only Daytona USA for now\n");
        return 1;
    }
    nplayers = coop ? 2 : 1;
    mkdirs(nvdir);
    for (int c = 0; c < 3; c++)
        if (gamma[c] <= 0) gamma[c] = 1.0f;

    for (int i = 0; i < nplayers; i++) {
        player *p = &pl[i];
        p->b = m2_create(game, dirs, logmsg);
        p->tg = malloc(sizeof *p->tg);
        p->pipe = m2pipe_create();
        p->geo = m2geo_create();
        if (!p->b || !p->tg || !p->pipe || !p->geo) { fprintf(stderr, "out of memory\n"); return 1; }
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
        p->snd = m2snd_create(p->b);
        if (!p->snd)
            fprintf(stderr, "no sound board (missing sound ROMs?)\n");
        p->replay = p->b->hooks;   /* the video hooks now run on the main thread */
        m2pipe_attach(p->pipe, p->b);
        p->in.updown_gears = updown_gears;   /* the original's UpDownGears / HoldGears */
        p->in.hold_gears = hold_gears;
        m2input_init(&p->in);
    }
    m2_net *net = NULL;
    if (coop) {   /* the two boards' network boards, linked in a ring */
        m2_board *ring[MAX_PLAYERS] = { pl[0].b, pl[1].b };
        net = m2net_create(ring, nplayers);
        if (!net) { fprintf(stderr, "out of memory\n"); return 1; }
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    char title[256];
    snprintf(title, sizeof title, "%s - Model 2", game->title);
    if (win_w <= 0 || win_h <= 0) {
        win_h = 768;
        win_w = (int)(win_h * (wide_on ? wide_ratio : 4.0 / 3.0) + 0.5) * nplayers;
    }
    SDL_Window *win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_w, win_h,
                                       SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                                       (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
    if (!win) { fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError()); return 1; }
    SDL_GLContext ctx = SDL_GL_CreateContext(win);
    if (!ctx) { fprintf(stderr, "GLES 2 context: %s\n", SDL_GetError()); return 1; }
    SDL_GL_SetSwapInterval(vsync);
    for (int i = 0; i < nplayers; i++)
        if (!(pl[i].gl = m2gl_create())) return 1;

    SDL_AudioDeviceID audio = 0;
    if (pl[0].snd || (coop && pl[1].snd)) {
        SDL_AudioSpec want, have;
        SDL_zero(want);
        want.freq = M2SND_RATE;
        want.format = AUDIO_S16SYS;
        want.channels = 2;
        want.samples = 1024;
        want.callback = audio_callback;
        audio = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
        if (!audio)
            fprintf(stderr, "audio: %s\n", SDL_GetError());
        else
            SDL_PauseAudioDevice(audio, 0);
    }

    /* extra pad mappings (SDL_GameControllerDB format), if present */
    {
        char dbpath[1200];
        const char *base = SDL_GetBasePath();
        snprintf(dbpath, sizeof dbpath, "%sgamecontrollerdb.txt", base ? base : "");
        if (SDL_GameControllerAddMappingsFromFile(dbpath) > 0)
            printf("pad mappings loaded from %s\n", dbpath);
        for (int j = 0; j < SDL_NumJoysticks(); j++)
            if (!SDL_IsGameController(j))
                printf("joystick %d (%s) has no pad mapping; add it to gamecontrollerdb.txt\n",
                       j, SDL_JoystickNameForIndex(j));
    }
    SDL_GameController *pads[2] = { NULL, NULL };
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
    double bt[4] = { 0, 0, 0, 0 };

    for (int i = 0; i < nplayers; i++) {
        player *p = &pl[i];
        p->go = SDL_CreateSemaphore(0);
        p->done = SDL_CreateSemaphore(0);
        p->thread = p->go && p->done ? SDL_CreateThread(emu_thread, "m2 board", p) : NULL;
        if (!p->thread) { fprintf(stderr, "emulation thread: %s\n", SDL_GetError()); return 1; }
    }
    int in_flight = 0, want_reset = 0;

    const double frame_s = 1.0 / game->fps;
    const double freq = (double)SDL_GetPerformanceFrequency();
    double next = SDL_GetPerformanceCounter() / freq;
    int running = 1, paused = 0, fast = 0, frames = 0;
    double fps_t = next, bench_t0 = next;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_QUIT:
                running = 0;
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                int down = e.type == SDL_KEYDOWN;
                SDL_Scancode sc = e.key.keysym.scancode;
                if (down && !e.key.repeat) {
                    if (sc == SDL_SCANCODE_ESCAPE) running = 0;
                    else if (sc == SDL_SCANCODE_P) {
                        paused = !paused;
                        if (audio) SDL_PauseAudioDevice(audio, paused);
                    }
                    else if (sc == SDL_SCANCODE_F3) want_reset = 1;
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
            case SDL_CONTROLLERDEVICEADDED:
                for (int p = 0; p < 2; p++)
                    if (!pads[p]) {
                        pads[p] = SDL_GameControllerOpen(e.cdevice.which);
                        if (pads[p])
                            printf("pad %d: %s\n", p + 1, SDL_GameControllerName(pads[p]));
                        break;
                    }
                break;
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

        /* draw the collected frames (or the last ones again), side by side */
        int w, h;
        SDL_GL_GetDrawableSize(win, &w, &h);
        int vw = w / nplayers;
        for (int i = 0; i < nplayers; i++) {
            player *p = &pl[i];
            if (got)   /* after start and reset the pipe reports everything as written */
                p->vb = m2pipe_apply(p->pipe, &p->replay, &p->pf);
            /* widescreen: a wider frame; the game's rule decides each frame
               whether the 3D sees the extra width and which layers stretch */
            m2_view view;
            view.frame_w = wide_on ? ((int)(M2_SCREEN_W * wide_ratio * 0.75 + 0.5) + 1) & ~1 : M2_SCREEN_W;
            view.stretch = wide_on ? p->pf.wide.stretch : 0;
            view.smooth = smooth;
            view.mesh_blend = mesh_blend;
            view.saturation = saturation;
            view.scale = scale_opt ? scale_opt : h / M2_SCREEN_H;
            if (view.scale < 1) view.scale = 1;
            p->geo->wide_extra = (view.frame_w - M2_SCREEN_W) * 0.5f;
            p->geo->wide_fov = wide_on && p->pf.wide.widescreen;
            if (got) {
                m2tile_render(p->tg, p->vb);
                m2geo_run(p->geo, p->vb);
            }
            if (p->vb)
                m2gl_draw_rect(p->gl, p->vb, p->tg, p->geo, &view, i * vw, 0, i == nplayers - 1 ? w - i * vw : vw, h);
        }
        const m2_pipe_frame *pf = &pl[0].pf;
        if (bench && got) {   /* M2EMU_BENCH=frames: unthrottled, timing split */
            glFinish();
            Uint64 t3 = SDL_GetPerformanceCounter();
            Uint64 board = 0;
            for (int i = 0; i < nplayers; i++)
                if (pl[i].ticks > board) board = pl[i].ticks;
            bt[0] += (double)board / freq;
            bt[1] += (double)(t1 - t0) / freq;
            bt[3] += (double)(t3 - t1) / freq;
            if (pf->frame == 1)
                bench_t0 = SDL_GetPerformanceCounter() / freq;
            if ((int)pf->frame >= bench) {
                double wall = SDL_GetPerformanceCounter() / freq - bench_t0;
                printf("%d frames: %.1f fps | per frame: board %.2f ms (own thread%s), "
                       "tiles+geo+gl %.2f ms, waiting for the board %.2f ms\n",
                       bench, (bench - 1) / wall, bt[0] * 1000 / bench, nplayers > 1 ? "s, slowest" : "",
                       bt[3] * 1000 / bench, bt[1] * 1000 / bench);
                running = 0;
            }
        }
        if (shot_frame && got && (int)pf->frame == shot_frame) {   /* debugging: M2EMU_SHOT=file.ppm:frame */
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
        SDL_GL_SwapWindow(win);

        /* pace to the game's refresh rate */
        double now = SDL_GetPerformanceCounter() / freq;
        next += frame_s;
        if (fast || bench || next < now - 0.1)
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
            snprintf(title, sizeof title, "%s - Model 2%s - %.1f fps%s%s%s", game->title, coop ? " - linked" : "",
                     frames / (now - fps_t), gear, wide_on ? " - wide" : "", paused ? " (paused)" : "");
            SDL_SetWindowTitle(win, title);
            frames = 0;
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
        SDL_DestroySemaphore(p->go);
        SDL_DestroySemaphore(p->done);
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
        free(p->tg);
    }
    m2net_destroy(net);
    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(win);
    SDL_Quit();
    for (int i = 0; i < nplayers; i++)
        m2_destroy(pl[i].b);
    return 0;
}
