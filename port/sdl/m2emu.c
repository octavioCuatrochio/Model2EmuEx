/*
 * SDL2 + OpenGL ES 2 frontend for the Model 2 port.
 *
 *   m2emu [-r romdir] [-n nvdir] [--vsync] [--fullscreen] [--size WxH]
 *         [--widescreen 16:9|16:10|off] [--scale N|auto] [--sharp]
 *         [--mesh blend|checker] [--saturation S] [--gamma G | --gamma R,G,B]
 *         [--shifter sequential|hpattern] [--hold-gears] <game>
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

static m2_tilegen tilegen;
static m2_gl *gl;
static m2_snd *snd;

static void on_tile(void *u, uint32_t o) { (void)u; m2tile_tile_written(&tilegen, o); }
static void on_cg(void *u) { (void)u; m2tile_cg_written(&tilegen); }
static void on_xlat(void *u) { (void)u; m2tile_xlat_written(&tilegen); if (gl) m2gl_xlat_written(gl); }
static void on_tex(void *u, int bank, uint32_t o) { (void)u; if (gl) m2gl_texture_written(gl, bank, o); }
static void on_luma(void *u) { (void)u; if (gl) m2gl_luma_written(gl); }
static void on_pal(void *u, uint32_t o) { (void)u; m2tile_palette_written(&tilegen, o); if (gl) m2gl_palette_written(gl, o); }
static void on_sound(void *u, uint8_t c) { (void)u; if (snd) m2snd_command(snd, c); }

/* The sound board runs here, on SDL's audio thread, at the device's pace:
   its tempo stays right even when the emulation can't keep full speed. */
static void audio_callback(void *u, Uint8 *stream, int len)
{
    m2snd_render(u, (int16_t *)stream, len / 4);
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
    int mesh_blend = 1, updown_gears = 0, hold_gears = 0;
    float saturation = 1.0f, gamma[3] = { 1.0f, 1.0f, 1.0f };

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-r") && i + 1 < argc) romdir = argv[++i];
        else if (!strcmp(argv[i], "-n") && i + 1 < argc) snprintf(nvdir, sizeof nvdir, "%s", argv[++i]);
        else if (!strcmp(argv[i], "--vsync")) vsync = 1;
        else if (!strcmp(argv[i], "--fullscreen")) fullscreen = 1;
        else if (!strcmp(argv[i], "--sharp")) smooth = 0;
        else if (!strcmp(argv[i], "--shifter") && i + 1 < argc) updown_gears = !strcmp(argv[++i], "sequential");
        else if (!strcmp(argv[i], "--hold-gears")) hold_gears = 1;
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
                        "             [--shifter sequential|hpattern] [--hold-gears] <game>\n"
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
    m2_board *b = m2_create(game, dirs, logmsg);
    if (!b) return 1;

    char nvpath[1200];
    mkdirs(nvdir);
    snprintf(nvpath, sizeof nvpath, "%s/%s.DAT", nvdir, game->name);
    if (!m2_nvram_load(b, nvpath))
        printf("NVRAM loaded from %s\n", nvpath);
    else if (m2_nvram_defaults(b))
        printf("NVRAM: first-boot defaults\n");

    for (int c = 0; c < 3; c++)
        if (gamma[c] <= 0) gamma[c] = 1.0f;
    m2tile_init(&tilegen, gamma[0], gamma[1], gamma[2]);
    b->hooks.tilemap_written = on_tile;
    b->hooks.cg_written = on_cg;
    b->hooks.xlat_written = on_xlat;
    b->hooks.texture_written = on_tex;
    b->hooks.luma_written = on_luma;
    b->hooks.palette_written = on_pal;
    b->hooks.sound_command = on_sound;
    snd = m2snd_create(b);
    if (!snd)
        fprintf(stderr, "no sound board (missing sound ROMs?)\n");
    m2_geo *geo = m2geo_create();
    if (!geo) return 1;

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
        win_w = (int)(win_h * (wide_on ? wide_ratio : 4.0 / 3.0) + 0.5);
    }
    SDL_Window *win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_w, win_h,
                                       SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                                       (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
    if (!win) { fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError()); return 1; }
    SDL_GLContext ctx = SDL_GL_CreateContext(win);
    if (!ctx) { fprintf(stderr, "GLES 2 context: %s\n", SDL_GetError()); return 1; }
    SDL_GL_SetSwapInterval(vsync);
    gl = m2gl_create();
    if (!gl) return 1;

    SDL_AudioDeviceID audio = 0;
    if (snd) {
        SDL_AudioSpec want, have;
        SDL_zero(want);
        want.freq = M2SND_RATE;
        want.format = AUDIO_S16SYS;
        want.channels = 2;
        want.samples = 1024;
        want.callback = audio_callback;
        want.userdata = snd;
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
    m2_input_state in;
    in.updown_gears = updown_gears;   /* the original's UpDownGears / HoldGears */
    in.hold_gears = hold_gears;
    m2input_init(&in);

    char shot_path[512] = "";
    int shot_frame = 0;
    if (getenv("M2EMU_SHOT")) {
        snprintf(shot_path, sizeof shot_path, "%s", getenv("M2EMU_SHOT"));
        char *c = strrchr(shot_path, ':');
        if (c) { *c = 0; shot_frame = atoi(c + 1); }
    }

    const char *script = getenv("M2EMU_KEYS");
    int bench = getenv("M2EMU_BENCH") ? atoi(getenv("M2EMU_BENCH")) : 0;
    double bt[3] = { 0, 0, 0 };

    const double frame_s = 1.0 / game->fps;
    const double freq = (double)SDL_GetPerformanceFrequency();
    double next = SDL_GetPerformanceCounter() / freq;
    int running = 1, paused = 0, fast = 0, frames = 0;
    double fps_t = next;

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
                    else if (sc == SDL_SCANCODE_F3) {   /* reset, keeping backup RAM and EEPROM */
                        m2_nvram_save(b, nvpath);
                        m2_reset(b);
                        m2_nvram_load(b, nvpath);
                        m2tile_init(&tilegen, gamma[0], gamma[1], gamma[2]);
                        if (audio) SDL_LockAudioDevice(audio);
                        if (snd) m2snd_reset(snd);
                        if (audio) SDL_UnlockAudioDevice(audio);
                    }
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
                if (dik) in.key[dik] = (uint8_t)down;
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
                        memset(in.pad[p], 0, sizeof in.pad[p]);
                        memset(in.axis[p], 0, sizeof in.axis[p]);
                    }
                break;
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                for (int p = 0; p < 2; p++)
                    if (pads[p] && SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pads[p])) == e.cbutton.which) {
                        int k = pad_button_from_sdl(e.cbutton.button);
                        if (k >= 0) in.pad[p][k] = e.type == SDL_CONTROLLERBUTTONDOWN;
                    }
                break;
            case SDL_CONTROLLERAXISMOTION:
                for (int p = 0; p < 2; p++)
                    if (pads[p] && SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(pads[p])) == e.caxis.which &&
                        e.caxis.axis < M2PAD_AXES)
                        in.axis[p][e.caxis.axis] = e.caxis.value;
                break;
            }
        }

        if (script) {   /* debugging: M2EMU_KEYS=frame:dik:frames,... (dik in hex) */
            for (const char *k = script; *k;) {
                int at = 0, len = 0;
                unsigned dik = 0;
                if (sscanf(k, "%d:%x:%d", &at, &dik, &len) == 3 && dik < 256) {
                    if ((int)b->frame == at) in.key[dik] = 1;
                    if ((int)b->frame == at + len) in.key[dik] = 0;
                }
                k = strchr(k, ',');
                if (!k) break;
                k++;
            }
        }
        Uint64 t0 = SDL_GetPerformanceCounter();
        if (!paused) {
            m2input_apply(&in, b);
            m2_run_frame(b);
            frames++;
        }
        Uint64 t1 = SDL_GetPerformanceCounter();
        int w, h;
        SDL_GL_GetDrawableSize(win, &w, &h);
        /* widescreen: a wider frame; the game's rule decides each frame
           whether the 3D sees the extra width and which layers stretch */
        m2_view view;
        m2_wide_state ws;
        m2wide_update(b, &ws);
        view.frame_w = wide_on ? ((int)(M2_SCREEN_W * wide_ratio * 0.75 + 0.5) + 1) & ~1 : M2_SCREEN_W;
        view.stretch = wide_on ? ws.stretch : 0;
        view.smooth = smooth;
        view.mesh_blend = mesh_blend;
        view.saturation = saturation;
        view.scale = scale_opt ? scale_opt : h / M2_SCREEN_H;
        if (view.scale < 1) view.scale = 1;
        geo->wide_extra = (view.frame_w - M2_SCREEN_W) * 0.5f;
        geo->wide_fov = wide_on && ws.widescreen;

        m2tile_render(&tilegen, b);
        m2geo_run(geo, b);
        Uint64 t2 = SDL_GetPerformanceCounter();
        m2gl_draw(gl, b, &tilegen, geo, &view, w, h);
        if (bench) {   /* M2EMU_BENCH=frames: unthrottled, timing split */
            glFinish();
            Uint64 t3 = SDL_GetPerformanceCounter();
            bt[0] += (double)(t1 - t0) / freq;
            bt[1] += (double)(t2 - t1) / freq;
            bt[2] += (double)(t3 - t2) / freq;
            if ((int)b->frame >= bench) {
                double tot = bt[0] + bt[1] + bt[2];
                printf("%d frames: %.1f fps | per frame: emu %.2f ms, tiles+geo %.2f ms, gl %.2f ms\n",
                       bench, bench / tot, bt[0] * 1000 / bench, bt[1] * 1000 / bench, bt[2] * 1000 / bench);
                running = 0;
            }
        }
        if (shot_frame && (int)b->frame == shot_frame) {   /* debugging: M2EMU_SHOT=file.ppm:frame */
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
            if (game->input_fn == 0x4c9080 || game->input_fn == 0x4cb870)
                snprintf(gear, sizeof gear, " - gear %c", in.gear[0] ? '0' + in.gear[0] : 'N');
            snprintf(title, sizeof title, "%s - Model 2 - %.1f fps%s%s%s", game->title,
                     frames / (now - fps_t), gear, wide_on ? " - wide" : "", paused ? " (paused)" : "");
            SDL_SetWindowTitle(win, title);
            frames = 0;
            fps_t = now;
        }
    }

    if (m2_nvram_save(b, nvpath))
        fprintf(stderr, "cannot save %s: %s\n", nvpath, strerror(errno));
    if (audio) SDL_CloseAudioDevice(audio);
    m2snd_destroy(snd);
    m2gl_destroy(gl);
    m2geo_destroy(geo);
    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(win);
    SDL_Quit();
    m2_destroy(b);
    return 0;
}
