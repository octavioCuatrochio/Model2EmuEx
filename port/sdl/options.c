/* Frontend settings: command line and m2emu.ini; see options.h. */
#include "options.h"

#include <SDL2/SDL.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#define make_dir(p) _mkdir(p)
#else
#define make_dir(p) mkdir((p), 0755)
#endif

void m2opt_defaults(m2_options *o)
{
    memset(o, 0, sizeof *o);
    snprintf(o->romdir, sizeof o->romdir, "%s", getenv("M2_ROMS") ? getenv("M2_ROMS") : "roms");
    o->frame_cap = 1;
    o->pipelined = 1;
    o->smooth = 1;
    o->wide = M2_WIDE_DEFAULT;
    o->wide_ratio = 16.0 / 9.0;
    o->mesh_blend = 1;
    o->saturation = 1.0f;
    o->gamma[0] = o->gamma[1] = o->gamma[2] = 1.0f;
    o->split_side = 1;
}

static int set_wide(m2_options *o, const char *v)
{
    int a = 0, b = 0;
    if (!strcmp(v, "fill")) o->wide = M2_WIDE_FILL;
    else if (!strcmp(v, "off")) o->wide = M2_WIDE_OFF;
    else if (!strcmp(v, "default")) o->wide = M2_WIDE_DEFAULT;
    else if (sscanf(v, "%d:%d", &a, &b) == 2 && a > 0 && b > 0) {
        o->wide = M2_WIDE_RATIO;
        o->wide_ratio = (double)a / b;
    } else
        return -1;
    return 0;
}

static int set_aspect(m2_options *o, const char *v)
{
    if (!strcmp(v, "keep")) o->aspect = M2_ASPECT_KEEP;
    else if (!strcmp(v, "stretch")) o->aspect = M2_ASPECT_STRETCH;
    else if (!strcmp(v, "crop")) o->aspect = M2_ASPECT_CROP;
    else return -1;
    return 0;
}

static void set_gamma(m2_options *o, const char *v)
{
    int n = sscanf(v, "%f,%f,%f", &o->gamma[0], &o->gamma[1], &o->gamma[2]);
    if (n == 1) o->gamma[1] = o->gamma[2] = o->gamma[0];
    for (int c = 0; c < 3; c++)
        if (o->gamma[c] <= 0) o->gamma[c] = 1.0f;
}

int m2opt_parse(m2_options *o, int argc, char **argv, const char **game, int *no_gui, int *help)
{
    *game = NULL;
    *no_gui = *help = 0;
    for (int i = 1; i < argc; i++) {
        const char *a = argv[i], *v = i + 1 < argc ? argv[i + 1] : NULL;
        if (!strcmp(a, "-r") && v) { snprintf(o->romdir, sizeof o->romdir, "%s", v); i++; }
        else if (!strcmp(a, "-n") && v) { snprintf(o->nvdir, sizeof o->nvdir, "%s", v); i++; }
        else if (!strcmp(a, "--no-gui")) *no_gui = 1;
        else if (!strcmp(a, "--vsync")) o->vsync = 1;
        else if (!strcmp(a, "--no-frame-cap")) o->frame_cap = 0;
        else if (!strcmp(a, "--fullscreen")) o->fullscreen = 1;
        else if (!strcmp(a, "--sharp")) o->smooth = 0;
        else if (!strcmp(a, "--shifter") && v) { o->updown_gears = !strcmp(v, "sequential"); i++; }
        else if (!strcmp(a, "--hold-gears")) o->hold_gears = 1;
        else if (!strcmp(a, "--pipeline") && v) { o->pipelined = strcmp(v, "off") != 0; i++; }
        else if (!strcmp(a, "--coop")) o->coop = 1;
        else if (!strcmp(a, "--mesh") && v) { o->mesh_blend = strcmp(v, "checker") != 0; i++; }
        else if (!strcmp(a, "--saturation") && v) { o->saturation = (float)atof(v); i++; }
        else if (!strcmp(a, "--gamma") && v) { set_gamma(o, v); i++; }
        else if (!strcmp(a, "--size") && v) { sscanf(v, "%dx%d", &o->win_w, &o->win_h); i++; }
        else if (!strcmp(a, "--scale") && v) { o->scale = !strcmp(v, "auto") ? 0 : atoi(v); i++; }
        else if (!strcmp(a, "--widescreen") && v) {
            if (set_wide(o, v)) o->wide = M2_WIDE_OFF;   /* anything else: off, as before */
            i++;
        }
        else if (!strcmp(a, "--split") && v) {
            if (!strcmp(v, "side")) o->split_side = 1;
            else if (!strcmp(v, "stack")) o->split_side = 0;
            else { fprintf(stderr, "--split: side or stack\n"); return 1; }
            i++;
        }
        else if (!strcmp(a, "--aspect") && v) {
            if (set_aspect(o, v)) { fprintf(stderr, "--aspect: keep, stretch or crop\n"); return 1; }
            i++;
        }
        else if (!strcmp(a, "-h") || !strcmp(a, "--help")) { *help = 1; return 0; }
        else if (a[0] != '-') *game = a;
        else { fprintf(stderr, "unknown option %s\n", a); return 1; }
    }
    return 0;
}

void m2opt_mkdirs(const char *path)
{
    char tmp[1024];
    snprintf(tmp, sizeof tmp, "%s", path);
    for (char *p = tmp + 1; *p; p++)
        if (*p == '/' || *p == '\\') {
            char c = *p;
            *p = 0;
            make_dir(tmp);
            *p = c;
        }
    make_dir(tmp);
}

void m2opt_nvdir(const m2_options *o, char *out, int size)
{
    if (o->nvdir[0]) {
        snprintf(out, (size_t)size, "%s", o->nvdir);
        return;
    }
#ifdef _WIN32   /* next to the program, as the original's NVDATA folder */
    char *base = SDL_GetBasePath();
    snprintf(out, (size_t)size, "%sNVDATA", base ? base : "");
    SDL_free(base);
#else
    const char *xdg = getenv("XDG_DATA_HOME"), *home = getenv("HOME");
    if (xdg && xdg[0]) snprintf(out, (size_t)size, "%s/m2emu/NVDATA", xdg);
    else snprintf(out, (size_t)size, "%s/.local/share/m2emu/NVDATA", home ? home : ".");
#endif
}

void m2opt_config_path(char *out, int size)
{
#ifdef _WIN32   /* next to the program, as the original's EMULATOR.INI */
    char *base = SDL_GetBasePath();
    snprintf(out, (size_t)size, "%sm2emu.ini", base ? base : "");
    SDL_free(base);
#else
    const char *xdg = getenv("XDG_CONFIG_HOME"), *home = getenv("HOME");
    if (xdg && xdg[0]) snprintf(out, (size_t)size, "%s/m2emu/m2emu.ini", xdg);
    else snprintf(out, (size_t)size, "%s/.config/m2emu/m2emu.ini", home ? home : ".");
#endif
}

static char *trim(char *s)
{
    while (isspace((unsigned char)*s)) s++;
    size_t n = strlen(s);
    while (n && isspace((unsigned char)s[n - 1])) s[--n] = 0;
    return s;
}

int m2opt_load(m2_options *o, const char *path, char *last_game, int last_game_size)
{
    FILE *f = fopen(path, "r");
    if (!f)
        return -1;
    char line[1200];
    while (fgets(line, sizeof line, f)) {
        char *eq = strchr(line, '=');
        if (line[0] == '#' || line[0] == ';' || line[0] == '[' || !eq)
            continue;
        *eq = 0;
        char *k = trim(line), *v = trim(eq + 1);
        if (!strcmp(k, "rom_folder")) snprintf(o->romdir, sizeof o->romdir, "%s", v);
        else if (!strcmp(k, "save_folder")) snprintf(o->nvdir, sizeof o->nvdir, "%s", v);
        else if (!strcmp(k, "fullscreen")) o->fullscreen = atoi(v) != 0;
        else if (!strcmp(k, "window_size")) { o->win_w = o->win_h = 0; sscanf(v, "%dx%d", &o->win_w, &o->win_h); }
        else if (!strcmp(k, "vsync")) o->vsync = atoi(v) != 0;
        else if (!strcmp(k, "frame_cap")) o->frame_cap = atoi(v) != 0;
        else if (!strcmp(k, "pipeline")) o->pipelined = atoi(v) != 0;
        else if (!strcmp(k, "render_scale")) o->scale = atoi(v);
        else if (!strcmp(k, "smooth")) o->smooth = atoi(v) != 0;
        else if (!strcmp(k, "widescreen")) set_wide(o, v);
        else if (!strcmp(k, "aspect")) set_aspect(o, v);
        else if (!strcmp(k, "mesh")) o->mesh_blend = strcmp(v, "checker") != 0;
        else if (!strcmp(k, "saturation")) o->saturation = (float)atof(v);
        else if (!strcmp(k, "gamma")) set_gamma(o, v);
        else if (!strcmp(k, "shifter")) o->updown_gears = !strcmp(v, "sequential");
        else if (!strcmp(k, "hold_gears")) o->hold_gears = atoi(v) != 0;
        else if (!strcmp(k, "linked_play")) o->coop = atoi(v) != 0;
        else if (!strcmp(k, "split")) o->split_side = strcmp(v, "stack") != 0;
        else if (!strcmp(k, "last_game") && last_game) snprintf(last_game, (size_t)last_game_size, "%s", v);
    }
    fclose(f);
    if (o->scale < 0 || o->scale > 8) o->scale = 0;
    if (o->saturation <= 0) o->saturation = 1.0f;
    return 0;
}

int m2opt_save(const m2_options *o, const char *path, const char *last_game)
{
    char dir[1024];
    snprintf(dir, sizeof dir, "%s", path);
    char *slash = strrchr(dir, '/'), *bslash = strrchr(dir, '\\');
    if (bslash > slash) slash = bslash;
    if (slash) {
        *slash = 0;
        m2opt_mkdirs(dir);
    }
    FILE *f = fopen(path, "w");
    if (!f)
        return -1;
    char wide[32];
    switch (o->wide) {
    case M2_WIDE_OFF: snprintf(wide, sizeof wide, "off"); break;
    case M2_WIDE_FILL: snprintf(wide, sizeof wide, "fill"); break;
    case M2_WIDE_RATIO:
        if (o->wide_ratio > 1.77 && o->wide_ratio < 1.78) snprintf(wide, sizeof wide, "16:9");
        else if (o->wide_ratio > 1.59 && o->wide_ratio < 1.61) snprintf(wide, sizeof wide, "16:10");
        else snprintf(wide, sizeof wide, "%d:1000", (int)(o->wide_ratio * 1000 + 0.5));
        break;
    default: snprintf(wide, sizeof wide, "default"); break;
    }
    static const char *const aspects[] = { "keep", "stretch", "crop" };
    fprintf(f, "# Model 2 emulator settings, written by the launcher (m2emu --no-gui uses\n"
               "# command-line options instead). Each option is explained in the launcher's\n"
               "# Configuration tab and in port/README.md.\n[m2emu]\n");
    fprintf(f, "rom_folder = %s\n", o->romdir);
    fprintf(f, "save_folder = %s\n", o->nvdir);
    fprintf(f, "fullscreen = %d\n", o->fullscreen);
    fprintf(f, "window_size = %dx%d\n", o->win_w, o->win_h);
    fprintf(f, "vsync = %d\n", o->vsync);
    fprintf(f, "frame_cap = %d\n", o->frame_cap);
    fprintf(f, "pipeline = %d\n", o->pipelined);
    fprintf(f, "render_scale = %d\n", o->scale);
    fprintf(f, "smooth = %d\n", o->smooth);
    fprintf(f, "widescreen = %s\n", wide);
    fprintf(f, "aspect = %s\n", aspects[o->aspect >= 0 && o->aspect <= 2 ? o->aspect : 0]);
    fprintf(f, "mesh = %s\n", o->mesh_blend ? "blend" : "checker");
    fprintf(f, "saturation = %.2f\n", o->saturation);
    fprintf(f, "gamma = %.2f,%.2f,%.2f\n", o->gamma[0], o->gamma[1], o->gamma[2]);
    fprintf(f, "shifter = %s\n", o->updown_gears ? "sequential" : "hpattern");
    fprintf(f, "hold_gears = %d\n", o->hold_gears);
    fprintf(f, "linked_play = %d\n", o->coop);
    fprintf(f, "split = %s\n", o->split_side ? "side" : "stack");
    fprintf(f, "last_game = %s\n", last_game ? last_game : "");
    return fclose(f) ? -1 : 0;
}
