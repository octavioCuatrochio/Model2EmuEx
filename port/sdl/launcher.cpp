/* The launcher; see launcher.h. */
#include "launcher.h"

#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_sdl2.h"
#include "../m2/m2glapi.h"
#include "../m2/m2rom.h"
#include "dirlist.h"
#include "imgui_impl_m2gl.h"

#include <algorithm>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>

#include "font_roboto.inc"

/* ------------------------------------------------------------ state */

struct game_row {
    const m2_game *g;
    int missing, total;
};

static bool g_ready;
static std::vector<game_row> g_rows;
static std::string g_scanned;          /* folder g_rows came from */
static int g_selected = -1;
static int g_zip_count;

/* folder browser */
static bool g_browse_open;
static int g_browse_target;            /* 0: ROM folder, 1: save folder */
static char g_browse_path[1024];
static std::vector<std::string> g_browse_dirs;
static int g_browse_zips;

static const ImVec4 k_dim(0.62f, 0.66f, 0.72f, 1.0f);
static const ImVec4 k_good(0.45f, 0.85f, 0.50f, 1.0f);
static const ImVec4 k_warn(0.95f, 0.72f, 0.35f, 1.0f);
static const ImVec4 k_bad(0.95f, 0.45f, 0.42f, 1.0f);

static bool iequal(const char *a, const char *b)
{
    for (; *a && *b; a++, b++)
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return false;
    return *a == *b;
}

static bool iless(const std::string &a, const std::string &b)
{
    for (size_t i = 0; i < a.size() && i < b.size(); i++) {
        int x = tolower((unsigned char)a[i]), y = tolower((unsigned char)b[i]);
        if (x != y)
            return x < y;
    }
    return a.size() < b.size();
}

/* ImGui checkboxes take a bool; the options are ints */
static bool checkbox(const char *label, int *v)
{
    bool b = *v != 0;
    if (!ImGui::Checkbox(label, &b))
        return false;
    *v = b;
    return true;
}

static bool known_to_work(const char *name)
{
    return !strcmp(name, "daytona") || !strcmp(name, "vf2") || !strcmp(name, "srallyc");
}

static bool can_link(const char *name) { return !strncmp(name, "daytona", 7); }

/* ------------------------------------------------------------ ROM scan */

static void collect_zip(void *u, const char *name, int is_dir)
{
    size_t n = strlen(name);
    if (!is_dir && n > 4 && iequal(name + n - 4, ".zip"))
        static_cast<std::vector<std::string> *>(u)->push_back(std::string(name, n - 4));
}

static void scan(const char *dir)
{
    std::vector<std::string> zips;
    g_rows.clear();
    g_selected = -1;
    g_scanned = dir;
    if (m2dir_list(dir, collect_zip, &zips) < 0) {
        g_zip_count = -1;
        return;
    }
    g_zip_count = (int)zips.size();
    int n = 0;
    const m2_game *list = m2_game_list(&n);
    const char *dirs[] = { dir, NULL };
    for (int i = 0; i < n; i++)
        for (const std::string &z : zips)
            if (iequal(z.c_str(), list[i].name)) {
                game_row r = { &list[i], 0, 0 };
                r.missing = m2_check_roms(&list[i], dirs, &r.total);
                g_rows.push_back(r);
                break;
            }
    std::sort(g_rows.begin(), g_rows.end(), [](const game_row &a, const game_row &b) {
        if (known_to_work(a.g->name) != known_to_work(b.g->name))
            return known_to_work(a.g->name);
        return strcmp(a.g->title, b.g->title) < 0;
    });
}

/* ------------------------------------------------------------ folder browser */

struct browse_list {
    std::vector<std::string> *dirs;
    int zips;
};

static void collect_browse(void *u, const char *name, int is_dir)
{
    browse_list *b = static_cast<browse_list *>(u);
    size_t n = strlen(name);
    if (is_dir && name[0] != '.')
        b->dirs->push_back(name);
    else if (!is_dir && n > 4 && iequal(name + n - 4, ".zip"))
        b->zips++;
}

static void browse_load()
{
    g_browse_dirs.clear();
    browse_list b = { &g_browse_dirs, 0 };
    if (m2dir_list(g_browse_path, collect_browse, &b) < 0)
        g_browse_zips = -1;
    else
        g_browse_zips = b.zips;
    std::sort(g_browse_dirs.begin(), g_browse_dirs.end(), iless);
}

static void browse_start(const char *start, int target)
{
    if (start && m2dir_exists(start))
        snprintf(g_browse_path, sizeof g_browse_path, "%s", start);
    else
        m2dir_home(g_browse_path, sizeof g_browse_path);
    g_browse_target = target;
    g_browse_open = true;
    browse_load();
}

/* Returns true when a folder was chosen (copied to out). */
static bool browse_draw(char *out, size_t out_size)
{
    bool chosen = false;
    if (g_browse_open) {
        ImGui::OpenPopup("Choose a folder");
        g_browse_open = false;
    }
    ImGuiViewport *vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x * 0.8f, vp->Size.y * 0.8f));
    ImGui::SetNextWindowPos(vp->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (!ImGui::BeginPopupModal("Choose a folder", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
        return false;

    ImGui::TextUnformatted(g_browse_path);
    if (ImGui::Button("Up")) {
        if (m2dir_parent(g_browse_path))
            browse_load();
    }
    ImGui::SameLine();
    if (ImGui::Button("Home")) {
        m2dir_home(g_browse_path, sizeof g_browse_path);
        browse_load();
    }
    char drives[32];
    m2dir_drives(drives, sizeof drives);
    for (const char *d = drives; *d; d++) {
        char label[8];
        snprintf(label, sizeof label, "%c:", *d);
        ImGui::SameLine();
        if (ImGui::Button(label)) {
            snprintf(g_browse_path, sizeof g_browse_path, "%c:\\", *d);
            browse_load();
        }
    }

    const float footer = ImGui::GetFrameHeightWithSpacing() * 2.2f;
    ImGui::BeginChild("dirs", ImVec2(0, -footer), ImGuiChildFlags_Borders);
    std::string go;
    if (g_browse_zips < 0)
        ImGui::TextColored(k_bad, "This folder can't be read.");
    for (const std::string &d : g_browse_dirs)
        if (ImGui::Selectable((d + "/").c_str()))
            go = d;
    ImGui::EndChild();
    if (!go.empty()) {
        char next[1024];
        m2dir_join(next, sizeof next, g_browse_path, go.c_str());
        snprintf(g_browse_path, sizeof g_browse_path, "%s", next);
        browse_load();
    }

    if (g_browse_target == 0) {
        if (g_browse_zips > 0)
            ImGui::TextColored(k_good, "%d zip file%s in this folder", g_browse_zips, g_browse_zips == 1 ? "" : "s");
        else
            ImGui::TextColored(k_dim, "No zip files in this folder");
    } else {
        ImGui::TextColored(k_dim, "Save data will be kept in this folder");
    }
    if (ImGui::Button("Use this folder")) {
        snprintf(out, out_size, "%s", g_browse_path);
        chosen = true;
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
        ImGui::CloseCurrentPopup();
    ImGui::EndPopup();
    return chosen;
}

/* ------------------------------------------------------------ helpers */

static void explain(const char *text)
{
    ImGui::PushStyleColor(ImGuiCol_Text, k_dim);
    ImGui::Indent();
    ImGui::TextWrapped("%s", text);
    ImGui::Unindent();
    ImGui::PopStyleColor();
    ImGui::Spacing();
}

static bool combo(const char *label, int *v, const char *const *items, int n)
{
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 14);
    return ImGui::Combo(label, v, items, n);
}

/* ------------------------------------------------------------ Games tab */

static bool games_tab(m2_options *o, char *game, int game_size, bool *changed)
{
    bool play = false;
    ImGui::TextUnformatted("ROM folder");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetFontSize() * 12);
    if (ImGui::InputText("##romdir", o->romdir, sizeof o->romdir, ImGuiInputTextFlags_EnterReturnsTrue)) {
        *changed = true;
        scan(o->romdir);
    }
    ImGui::SameLine();
    if (ImGui::Button("Browse..."))
        browse_start(o->romdir, 0);
    ImGui::SameLine();
    if (ImGui::Button("Rescan"))
        scan(o->romdir);
    if (g_browse_target == 0) {
        char picked[1024];
        if (browse_draw(picked, sizeof picked)) {
            snprintf(o->romdir, sizeof o->romdir, "%s", picked);
            *changed = true;
            scan(o->romdir);
        }
    }
    if (g_scanned != o->romdir && !ImGui::IsItemActive() && !ImGui::GetIO().WantTextInput)
        scan(o->romdir);

    if (g_zip_count < 0)
        ImGui::TextColored(k_bad, "This folder can't be read. Choose the folder that holds the ROM zip files.");
    else if (g_rows.empty())
        ImGui::TextColored(k_warn, "No Model 2 ROM sets here. Choose the folder that holds daytona.zip, vf2.zip, "
                                   "srallyc.zip... (MAME-style sets; model2.zip is shared by the Model 2 games).");
    else {
        int ok = 0;
        for (const game_row &r : g_rows)
            ok += r.missing == 0;
        ImGui::TextColored(k_dim, "%d game%s found, %d with every file", (int)g_rows.size(),
                           g_rows.size() == 1 ? "" : "s", ok);
    }

    /* keep the last game selected */
    if (g_selected < 0 && game[0])
        for (size_t i = 0; i < g_rows.size(); i++)
            if (!strcmp(g_rows[i].g->name, game))
                g_selected = (int)i;

    const float footer = ImGui::GetFrameHeightWithSpacing() * 3.4f;
    if (ImGui::BeginTable("games", 5, ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersInnerH,
                          ImVec2(0, -footer))) {
        const float em = ImGui::GetFontSize();
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Game", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Set", ImGuiTableColumnFlags_WidthFixed, em * 7);
        ImGui::TableSetupColumn("Board", ImGuiTableColumnFlags_WidthFixed, em * 6);
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, em * 7);
        ImGui::TableSetupColumn("ROM files", ImGuiTableColumnFlags_WidthFixed, em * 9);
        ImGui::TableHeadersRow();
        for (size_t i = 0; i < g_rows.size(); i++) {
            const game_row &r = g_rows[i];
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::PushID((int)i);
            if (ImGui::Selectable(r.g->title, g_selected == (int)i,
                                  ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick)) {
                g_selected = (int)i;
                snprintf(game, (size_t)game_size, "%s", r.g->name);
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && r.missing == 0)
                    play = true;
            }
            ImGui::PopID();
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(r.g->name);
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(r.g->board);
            ImGui::TableNextColumn();
            if (known_to_work(r.g->name))
                ImGui::TextColored(k_good, "Works");
            else
                ImGui::TextColored(k_dim, "Not tested");
            ImGui::TableNextColumn();
            if (r.missing == 0)
                ImGui::TextColored(k_good, "Complete");
            else
                ImGui::TextColored(k_bad, "%d of %d missing", r.missing, r.total);
        }
        ImGui::EndTable();
    }

    /* the selected game */
    ImGui::Separator();
    const game_row *sel = g_selected >= 0 && g_selected < (int)g_rows.size() ? &g_rows[g_selected] : NULL;
    if (sel) {
        ImGui::Text("%s", sel->g->title);
        if (can_link(sel->g->name)) {
            ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.45f);
            if (checkbox("Two players, linked (split screen)", &o->coop))
                *changed = true;
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Two Daytona USA machines linked like two cabinets: each player drives their own\n"
                                  "car and sees the other. Player 1: keyboard and pad 1; player 2: pad 2.");
        }
        if (sel->missing)
            ImGui::TextColored(k_bad, "Can't start: %d of its %d ROM files aren't in this folder (or in model2.zip / "
                                      "the parent set).", sel->missing, sel->total);
        else if (!known_to_work(sel->g->name))
            ImGui::TextColored(k_warn, "Not tested with this emulator yet: it may not run correctly.");
        else
            ImGui::TextColored(k_dim, "In game: Esc back here, F3 reset, P pause, Tab fast forward, F11 fullscreen.");
    } else {
        ImGui::TextColored(k_dim, "Choose a game.");
        ImGui::NewLine();
    }
    ImGui::BeginDisabled(!sel || sel->missing);
    if (ImGui::Button("  Play  ", ImVec2(ImGui::GetFontSize() * 8, 0)) && sel)
        play = true;
    ImGui::EndDisabled();
    if (play && sel) {
        snprintf(game, (size_t)game_size, "%s", sel->g->name);
        if (!can_link(sel->g->name))
            o->coop = 0;
    }
    return play;
}

/* ------------------------------------------------------------ Configuration tab */

static void config_tab(m2_options *o, bool *changed)
{
    ImGui::BeginChild("config", ImVec2(0, 0));
    bool c = false;

    ImGui::SeparatorText("Display");
    c |= checkbox("Fullscreen", &o->fullscreen);
    explain("Start games fullscreen, at the display's own resolution. F11 switches while playing.");

    {
        static const char *const sizes[] = { "Automatic", "1024 x 768", "1280 x 720", "1366 x 768",
                                             "1600 x 900", "1920 x 1080", "2560 x 1440", "Custom" };
        static const int wh[][2] = { { 0, 0 }, { 1024, 768 }, { 1280, 720 }, { 1366, 768 },
                                     { 1600, 900 }, { 1920, 1080 }, { 2560, 1440 } };
        int cur = 7;
        for (int i = 0; i < 7; i++)
            if (o->win_w == wh[i][0] && o->win_h == wh[i][1])
                cur = i;
        if (combo("Window size", &cur, sizes, 8)) {
            if (cur < 7) { o->win_w = wh[cur][0]; o->win_h = wh[cur][1]; }
            else if (!o->win_w) { o->win_w = 1280; o->win_h = 960; }
            c = true;
        }
        if (cur == 7) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
            int v[2] = { o->win_w, o->win_h };
            if (ImGui::InputInt2("##custom", v)) {
                o->win_w = v[0] < 160 ? 160 : v[0];
                o->win_h = v[1] < 120 ? 120 : v[1];
                c = true;
            }
        }
    }
    explain("The window when not fullscreen. Automatic: 768 pixels high and as wide as the picture needs "
            "(twice as wide for linked play side by side).");

    {
        static const char *const scales[] = { "Auto", "1x (496 x 384)", "2x", "3x", "4x" };
        int s = o->scale >= 0 && o->scale <= 4 ? o->scale : 0;
        if (combo("Render resolution", &s, scales, 5)) { o->scale = s; c = true; }
    }
    explain("How sharp the 3D is drawn: a multiple of the original 496 x 384. Auto uses the largest that fits the "
            "window. Higher looks sharper and costs more graphics power. F10 cycles while playing.");

    {
        static const char *const tfilters[] = { "Nearest (as the hardware)", "Bilinear", "Trilinear" };
        int f = o->tex_filter >= 0 && o->tex_filter <= 2 ? o->tex_filter : 0;
        if (combo("Texture filter", &f, tfilters, 3)) { o->tex_filter = f; c = true; }
    }
    explain("How the 3D textures (track, cars, scenery) are sampled. Nearest shows their square texels, as the "
            "Model 2 hardware and MAME. Bilinear smooths them, as the original emulator did. Trilinear also "
            "averages distant and slanted surfaces over each pixel, so roads and fences in the distance don't "
            "shimmer. Bilinear and trilinear take more graphics power (fine on a PC graphics card; keep Nearest "
            "on small devices). F6 cycles while playing.");

    {
        static const char *const filters[] = { "Smooth", "Sharp" };
        int f = o->smooth ? 0 : 1;
        if (combo("Scaling filter", &f, filters, 2)) { o->smooth = f == 0; c = true; }
    }
    explain("How the finished picture is enlarged to the window: Smooth blends neighbouring pixels, Sharp keeps "
            "them square.");

    {
        static const char *const wides[] = { "Default", "Off (4:3)", "16:9", "16:10", "Fill the window" };
        int w = 0;
        if (o->wide == M2_WIDE_OFF) w = 1;
        else if (o->wide == M2_WIDE_FILL) w = 4;
        else if (o->wide == M2_WIDE_RATIO) w = o->wide_ratio < 1.7 ? 3 : 2;
        if (combo("Widescreen", &w, wides, 5)) {
            static const int modes[] = { M2_WIDE_DEFAULT, M2_WIDE_OFF, M2_WIDE_RATIO, M2_WIDE_RATIO, M2_WIDE_FILL };
            o->wide = modes[w];
            if (w == 2) o->wide_ratio = 16.0 / 9.0;
            if (w == 3) o->wide_ratio = 16.0 / 10.0;
            c = true;
        }
    }
    explain("A wider picture: in races the game draws more of the track to the sides (not stretched); menus and "
            "the HUD stay 4:3 in the middle. The rules come from the original emulator's scripts. Default is off, "
            "except that linked play one above the other fills each half. Fill the window: the window's own shape. "
            "F9 switches while playing.");

    {
        static const char *const aspects[] = { "Keep (bands)", "Stretch", "Crop" };
        int a = o->aspect >= 0 && o->aspect <= 2 ? o->aspect : 0;
        if (combo("Picture shape", &a, aspects, 3)) { o->aspect = a; c = true; }
    }
    explain("When the window, or a player's half of it, is narrower than the picture (linked play side by side): "
            "Keep shows it whole with bands above and below; Stretch squeezes it to fill (everything looks "
            "narrower); Crop fills it at the right proportions by cutting the sides of the 3D view, with the HUD "
            "squeezed in whole (menus are squeezed).");

    ImGui::SeparatorText("Colour");
    {
        static const char *const meshes[] = { "Blend (50% see-through)", "Checker (as the hardware)" };
        int m = o->mesh_blend ? 0 : 1;
        if (combo("Shadows and fences", &m, meshes, 2)) { o->mesh_blend = m == 0; c = true; }
    }
    explain("The hardware draws shadows and fences as a mesh of every other pixel. Blend shows them 50% "
            "see-through instead (smoother, like the original's MeshTransparency). F8 switches while playing.");

    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 14);
    c |= ImGui::SliderFloat("Saturation", &o->saturation, 0.5f, 2.0f, "%.2f");
    explain("Colour strength of the final picture. 1.00 is the hardware's colours; higher is more vivid. "
            "F7 cycles 1.0 / 1.2 / 1.4 while playing.");

    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 14);
    c |= ImGui::SliderFloat3("Gamma (R, G, B)", o->gamma, 0.5f, 2.0f, "%.2f");
    ImGui::SameLine();
    if (ImGui::Button("Reset##colour")) {
        o->saturation = 1.0f;
        o->gamma[0] = o->gamma[1] = o->gamma[2] = 1.0f;
        c = true;
    }
    explain("Brightness curve of each colour channel, as the original emulator's GammaR/G/B. Above 1 brightens "
            "the mid-tones, below 1 darkens them.");

    ImGui::SeparatorText("Timing");
    c |= checkbox("Frame cap", &o->frame_cap);
    explain("Run at the game's own speed, 57.5 frames per second. Off: as fast as the computer can (to measure "
            "speed; the sound keeps its normal speed).");
    c |= checkbox("VSync", &o->vsync);
    explain("Show each frame when the display refreshes: no tearing, but on a 60 Hz display a frame is shown "
            "twice now and then, as the game runs at 57.5.");
    c |= checkbox("Pipeline (emulate on a separate core)", &o->pipelined);
    explain("Emulate the next frame on another processor core while this one is drawn: much faster on multi-core "
            "processors, for one frame (about 17 ms) more delay between the controls and the screen. Off: "
            "emulate and draw in turn, for the least delay.");
    c |= checkbox("Frame skip", &o->frame_skip);
    explain("When the computer can't keep up, skip drawing a frame now and then (at most two in a row) instead "
            "of slowing the game down: the game, its sound and the controls keep their speed. Only acts when "
            "running late, so it never happens on a fast enough machine.");

    ImGui::SeparatorText("Controls");
    {
        static const char *const shifters[] = { "H-pattern (A S D F, G neutral)", "Sequential (A down, S up)" };
        int s = o->updown_gears ? 1 : 0;
        if (combo("Keyboard shifter", &s, shifters, 2)) { o->updown_gears = s; c = true; }
    }
    explain("Gears in Daytona USA and Sega Rally with the keyboard. H-pattern: A, S, D, F pick gears 1 to 4 and "
            "G neutral, like the cabinet's shifter. Sequential: A shifts down and S up. Pads always use RB (up) "
            "and LB (down).");
    c |= checkbox("Hold gears", &o->hold_gears);
    explain("H-pattern only: the gear drops to neutral when no gear key is held down, like letting go of a real "
            "shifter.");

    ImGui::SeparatorText("Linked play (Daytona USA)");
    {
        static const char *const splits[] = { "Side by side", "One above the other" };
        int s = o->split_side ? 0 : 1;
        if (combo("Layout", &s, splits, 2)) { o->split_side = s == 0; c = true; }
    }
    explain("How two linked players share the window. One above the other gives each player a wide view that "
            "fills their half; side by side uses the Picture shape setting. Turn linked play on in the Games tab.");

    ImGui::SeparatorText("Folders");
    ImGui::TextUnformatted("Save data");
    ImGui::SameLine();
    char def[1024];
    m2_options d = *o;
    d.nvdir[0] = 0;
    m2opt_nvdir(&d, def, sizeof def);
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::GetFontSize() * 6);
    c |= ImGui::InputTextWithHint("##nvdir", def, o->nvdir, sizeof o->nvdir);
    ImGui::SameLine();
    if (ImGui::Button("Browse...##nv"))
        browse_start(o->nvdir[0] ? o->nvdir : def, 1);
    if (g_browse_target == 1) {
        char picked[1024];
        if (browse_draw(picked, sizeof picked)) {
            snprintf(o->nvdir, sizeof o->nvdir, "%s", picked);
            c = true;
        }
    }
    explain("Where each game keeps its settings, high scores and bookkeeping (one file per game, the original "
            "emulator's NVDATA format). Empty: the default folder shown.");

    ImGui::Spacing();
    if (ImGui::Button("Restore all defaults")) {
        char romdir[1024];
        snprintf(romdir, sizeof romdir, "%s", o->romdir);
        m2opt_defaults(o);
        snprintf(o->romdir, sizeof o->romdir, "%s", romdir);
        c = true;
    }
    explain("Every option above back to its default (the ROM folder is kept). Settings are saved as you change "
            "them.");
    ImGui::EndChild();
    *changed |= c;
}

/* ------------------------------------------------------------ main loop */

int m2launch_init(SDL_Window *win, SDL_GLContext ctx, int es)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = NULL;   /* no imgui.ini: our settings are in m2emu.ini */
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;
    ImFontConfig cfg;
    cfg.FontDataOwnedByAtlas = true;
    io.Fonts->AddFontFromMemoryCompressedTTF(roboto_medium_compressed_data, (int)roboto_medium_compressed_size,
                                             18.0f, &cfg);
    ImGui::StyleColorsDark();
    ImGuiStyle &st = ImGui::GetStyle();
    st.WindowRounding = 0;
    st.FrameRounding = 4;
    st.GrabRounding = 4;
    st.FramePadding = ImVec2(8, 5);
    st.ItemSpacing = ImVec2(10, 7);
    if (!ImGui_ImplSDL2_InitForOpenGL(win, ctx) || !ImGui_ImplM2GL_Init(es != 0))
        return -1;
    g_ready = true;
    return 0;
}

void m2launch_shutdown(void)
{
    if (!g_ready)
        return;
    ImGui_ImplM2GL_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    g_ready = false;
}

/* debugging: M2EMU_GUI_SHOT=file.ppm:frames, M2EMU_GUI_TAB=games|config,
   M2EMU_GUI_PLAY=1 (press Play at once), M2EMU_GUI_BROWSE=1 */
static void save_shot(const char *path, int w, int h)
{
    unsigned char *px = (unsigned char *)malloc((size_t)w * h * 4);
    FILE *f = fopen(path, "wb");
    if (px && f) {
        glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, px);
        fprintf(f, "P6\n%d %d\n255\n", w, h);
        for (int y = h - 1; y >= 0; y--)
            for (int x = 0; x < w; x++)
                fwrite(px + ((size_t)y * w + x) * 4, 1, 3, f);
    }
    if (f) fclose(f);
    free(px);
}

int m2launch_run(SDL_Window *win, m2_options *o, const char *cfg_path, char *game, int game_size,
                 const char *message)
{
    char title[64];
    snprintf(title, sizeof title, "Model 2 Emulator");
    SDL_SetWindowTitle(win, title);
    SDL_GL_SetSwapInterval(1);

    char shot_path[512] = "";
    int shot_frames = 0, frame = 0;
    if (getenv("M2EMU_GUI_SHOT")) {
        snprintf(shot_path, sizeof shot_path, "%s", getenv("M2EMU_GUI_SHOT"));
        char *c = strrchr(shot_path, ':');
        if (c) { *c = 0; shot_frames = atoi(c + 1); }
    }
    const char *want_tab = getenv("M2EMU_GUI_TAB");
    static bool auto_played;   /* M2EMU_GUI_PLAY presses Play once */
    bool auto_play = getenv("M2EMU_GUI_PLAY") != NULL && !auto_played;
    auto_played = true;
    if (getenv("M2EMU_GUI_BROWSE"))
        browse_start(o->romdir, 0);

    std::string msg = message ? message : "";
    bool msg_open = !msg.empty();
    int result = -1;
    while (result < 0) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT)
                result = 0;
        }
        ImGui_ImplSDL2_NewFrame();
        int w, h;
        SDL_GL_GetDrawableSize(win, &w, &h);
        ImGui::GetStyle().FontSizeBase = h < 600 ? 16.0f : h / 36.0f;   /* readable on a TV too */
        ImGui::NewFrame();

        ImGuiViewport *vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->WorkPos);
        ImGui::SetNextWindowSize(vp->WorkSize);
        ImGui::Begin("launcher", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                           ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoSavedSettings);
        ImGui::TextUnformatted("Model 2 Emulator");
        ImGui::SameLine();
        ImGui::TextColored(k_dim, "  portable port of ElSemi's Model 2 Emulator 1.1a");
        bool changed = false, play = false;
        if (ImGui::BeginTabBar("tabs")) {
            ImGuiTabItemFlags gf = 0, cf = 0;
            if (frame == 0 && want_tab)
                (!strcmp(want_tab, "config") ? cf : gf) = ImGuiTabItemFlags_SetSelected;
            if (ImGui::BeginTabItem("Games", NULL, gf)) {
                play = games_tab(o, game, game_size, &changed);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Configuration", NULL, cf)) {
                config_tab(o, &changed);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        if (msg_open) {
            ImGui::OpenPopup("Couldn't start the game");
            msg_open = false;
        }
        if (ImGui::BeginPopupModal("Couldn't start the game", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextUnformatted(msg.c_str());
            if (ImGui::Button("OK"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
        ImGui::End();

        if (auto_play && frame == 2 && game[0]) {
            for (const game_row &r : g_rows)
                if (!strcmp(r.g->name, game) && !r.missing)
                    play = true;
        }
        if (changed || play)
            m2opt_save(o, cfg_path, game);
        if (play)
            result = 1;

        ImGui::Render();
        glViewport(0, 0, w, h);
        glClearColor(0.07f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplM2GL_RenderDrawData(ImGui::GetDrawData());
        if (++frame >= shot_frames && shot_frames && shot_path[0]) {
            save_shot(shot_path, w, h);
            result = 0;
        }
        SDL_GL_SwapWindow(win);
    }
    return result;
}
