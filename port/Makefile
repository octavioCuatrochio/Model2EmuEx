# Model 2 port: m2emu (SDL2 + OpenGL frontend) and m2run (headless tool).
#
#   make                 Linux, or Windows under MSYS2: desktop OpenGL (default)
#   make GL=gles         the same with OpenGL ES 2 (Android, Mali, SBCs)
#   make windows ...     Windows cross-compile with MinGW-w64 (see below)
#   make android NDK=... headless m2run for Android armeabi-v7a (see below)
#
# OpenGL is loaded at run time (m2/m2glapi.c): nothing links against a GL
# library, GL= only picks the context m2emu asks for and the shader dialect.

CC       ?= gcc
CXX      ?= g++
CFLAGS   ?= -O2 -g
CXXFLAGS ?= -O2 -g
BUILD    := build

# desktop OpenGL 2.1+ (gl) or OpenGL ES 2 (gles)
GL ?= gl
ifeq ($(GL),gles)
GL_CFLAGS := -DM2_GLES
else ifneq ($(GL),gl)
$(error GL must be gl or gles)
endif

# libraries: pkg-config by default, override for other setups
SDL_CFLAGS  ?= $(shell pkg-config --cflags sdl2)
SDL_LIBS    ?= $(shell pkg-config --libs sdl2)
ZLIB_CFLAGS ?=
ZLIB_LIBS   ?= -lz

# Windows (MSYS2 or cross): .exe, and the GCC/C++ runtimes linked in
ifeq ($(OS),Windows_NT)
EXE     ?= .exe
LDFLAGS ?= -static-libgcc -static-libstdc++
endif
EXE ?=

CFLAGS   += -std=c11 -D_DEFAULT_SOURCE -Wall -Wextra -MMD -MP $(ZLIB_CFLAGS)
CXXFLAGS += -std=c++14 -Wall -MMD -MP

CORE_C := i960/i960.c tgp/tgp.c m2/m2board.c m2/m2rom.c m2/m2tile.c m2/m2input.c m2/m2geo.c m2/m2wide.c m2/m2pipe.c m2/m2net.c \
          snd/m2snd.c snd/scsp.c snd/multipcm.c
M68K_C := m68k/m68kcpu.c m68k/m68kops.c m68k/softfloat/softfloat.c
CORE_CXX := snd/ym3438.cpp ymfm/ymfm_opn.cpp ymfm/ymfm_adpcm.cpp ymfm/ymfm_ssg.cpp

OBJ = $(addprefix $(BUILD)/,$(CORE_C:.c=.o) $(M68K_C:.c=.o) $(CORE_CXX:.cpp=.o))

all: $(BUILD)/m2emu$(EXE) $(BUILD)/m2run$(EXE)

# the launcher: Dear ImGui (port/imgui, MIT) on SDL2 and our GL layer
IMGUI_CXX := imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp \
             imgui/backends/imgui_impl_sdl2.cpp
GUI_OBJ = $(addprefix $(BUILD)/,sdl/launcher.o sdl/imgui_impl_m2gl.o $(IMGUI_CXX:.cpp=.o) \
            sdl/options.o sdl/dirlist.o)

$(BUILD)/m2emu$(EXE): $(BUILD)/sdl/m2emu.o $(BUILD)/m2/m2gl.o $(BUILD)/m2/m2glapi.o $(GUI_OBJ) $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(SDL_LIBS) $(ZLIB_LIBS) -lm

$(BUILD)/m2run$(EXE): $(BUILD)/m2/m2run.o $(BUILD)/m2/m2tiledump.o $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(ZLIB_LIBS) -lm

$(BUILD)/sdl/%.o: sdl/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(GL_CFLAGS) $(SDL_CFLAGS) -c -o $@ $<

$(BUILD)/sdl/%.o: sdl/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Iimgui $(SDL_CFLAGS) -c -o $@ $<

# Dear ImGui (third party): its own warnings are not ours
$(BUILD)/imgui/%.o: imgui/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Iimgui $(SDL_CFLAGS) -w -c -o $@ $<

# Musashi and ymfm (third party): their own warnings are not ours
$(BUILD)/m68k/%.o: m68k/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -w -c -o $@ $<

$(BUILD)/ymfm/%.o: ymfm/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -w -c -o $@ $<

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Windows, cross-compiled with MinGW-w64 (GCC, or llvm-mingw which has the
# same x86_64-w64-mingw32-gcc names): build-windows/m2emu.exe and m2run.exe.
#   make windows SDL2=<SDL2-devel-2.x-mingw>/x86_64-w64-mingw32 ZLIB=<zlib for MinGW> [MINGW=<prefix>]
# SDL2: the "SDL2-devel-...-mingw" package from libsdl.org; ZLIB: a directory
# with include/zlib.h and lib/libz.a (see README.md). m2emu.exe needs SDL2.dll
# next to it.
MINGW ?= x86_64-w64-mingw32-
windows:
	@test -n "$(SDL2)" || { echo "set SDL2=<SDL2 MinGW development package>/x86_64-w64-mingw32"; exit 1; }
	@test -n "$(ZLIB)" || { echo "set ZLIB=<directory with include/zlib.h and lib/libz.a>"; exit 1; }
	$(MAKE) BUILD=build-windows EXE=.exe CC=$(MINGW)gcc CXX=$(MINGW)g++ \
		SDL_CFLAGS="-I$(SDL2)/include -I$(SDL2)/include/SDL2" SDL_LIBS="-L$(SDL2)/lib -lmingw32 -lSDL2main -lSDL2 -mwindows" \
		ZLIB_CFLAGS="-I$(ZLIB)/include" ZLIB_LIBS="$(ZLIB)/lib/libz.a" \
		LDFLAGS="-static-libgcc -static-libstdc++" \
		build-windows/m2emu.exe build-windows/m2run.exe
	cp $(SDL2)/bin/SDL2.dll build-windows/

# Android, armeabi-v7a (the Allwinner H3's Cortex-A7): the headless m2run,
# for timing on the device (adb push; M2_BENCH=1 build-android/m2run ...).
#   make android NDK=~/Android/Sdk/ndk/<version>
ANDROID_API ?= 21
ARM_FLAGS   := -O2 -g -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mthumb
NDK_BIN      = $(NDK)/toolchains/llvm/prebuilt/linux-x86_64/bin
android:
	@test -n "$(NDK)" || { echo "set NDK=<path to the Android NDK>"; exit 1; }
	CFLAGS="$(ARM_FLAGS)" CXXFLAGS="$(ARM_FLAGS)" LDFLAGS="-static-libstdc++" \
	$(MAKE) BUILD=build-android CC=$(NDK_BIN)/armv7a-linux-androideabi$(ANDROID_API)-clang \
		CXX=$(NDK_BIN)/armv7a-linux-androideabi$(ANDROID_API)-clang++ GL=gles build-android/m2run

clean:
	rm -rf $(BUILD)

-include $(shell find $(BUILD) -name '*.d' 2>/dev/null)

.PHONY: all clean android windows
