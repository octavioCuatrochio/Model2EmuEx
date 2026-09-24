# Daytona USA: the i960 program recompiled to C, running on the port's
# Model 2 board (video, sound, TGP) with the SDL2 frontend.
#
#   make ROMS=~/Desktop/ROMs   generate the C from daytona.zip, build everything
#   make trace                 the coverage tracer (tools/trace.c) only
#   make apk NDK=... SDL=...   the Android app with the recompiled program (build/apk/m2emu.apk)
#   make clean
#
# The generated C is made from your ROM at build time (build/gen) and is not
# part of the repository.

CC       ?= gcc
CXX      ?= g++
CFLAGS   ?= -O2 -g
CXXFLAGS ?= -O2 -g
PYTHON   ?= python3
BUILD    := build
PORT     := ../port
ROMS     ?= $(HOME)/Desktop/ROMs

GL ?= gl
ifeq ($(GL),gles)
GL_CFLAGS := -DM2_GLES
endif

SDL_CFLAGS ?= $(shell pkg-config --cflags sdl2)
SDL_LIBS   ?= $(shell pkg-config --libs sdl2)
ZLIB_LIBS  ?= -lz

CFLAGS   += -std=c11 -D_DEFAULT_SOURCE -Wall -Wextra -MMD -MP
CXXFLAGS += -std=c++14 -Wall -MMD -MP

# the port: board, sound, video, frontend (unchanged, see ../port)
PORT_C   := i960/i960.c tgp/tgp.c m2/m2board.c m2/m2rom.c m2/m2tile.c m2/m2input.c m2/m2geo.c m2/m2dump.c m2/m2wide.c \
            m2/m2pipe.c m2/m2net.c snd/m2snd.c snd/scsp.c snd/multipcm.c
PORT_3P  := m68k/m68kcpu.c m68k/m68kops.c m68k/softfloat/softfloat.c
PORT_CXX := snd/ym3438.cpp ymfm/ymfm_opn.cpp ymfm/ymfm_adpcm.cpp ymfm/ymfm_ssg.cpp
PORT_OBJ := $(addprefix $(BUILD)/port/,$(PORT_C:.c=.o) $(PORT_3P:.c=.o) $(PORT_CXX:.cpp=.o))
BOARD_OBJ := $(filter-out $(BUILD)/port/i960/i960.o,$(PORT_OBJ))

IMGUI_CXX := imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp \
             imgui/backends/imgui_impl_sdl2.cpp
FRONT_OBJ := $(addprefix $(BUILD)/port/,sdl/m2emu.o m2/m2gl.o m2/m2glapi.o sdl/launcher.o sdl/imgui_impl_m2gl.o \
             sdl/options.o sdl/dirlist.o $(IMGUI_CXX:.cpp=.o))

# the recompiled program
GEN      := $(BUILD)/gen
GEN_SRC   = $(wildcard $(GEN)/daytona_*.c) $(GEN)/table.c $(GEN)/cycles.c
GEN_OBJ   = $(patsubst $(GEN)/%.c,$(BUILD)/genobj/%.o,$(GEN_SRC))
RT_OBJ   := $(BUILD)/rt/rt.o $(BUILD)/rt/coro.o
SRC_C    := $(wildcard src/*.c)
SRC_OBJ  := $(patsubst src/%.c,$(BUILD)/src/%.o,$(SRC_C))
TRACES   := $(wildcard traces/*.txt)

all: $(BUILD)/daytona $(BUILD)/daytona-run

$(GEN)/table.c: tools/i960rec.py tools/i960dec.py $(TRACES) $(SRC_C) $(PORT)/i960/i960_optable.inc
	rm -rf $(GEN)
	$(PYTHON) tools/i960rec.py $(ROMS) $(GEN) $(TRACES)

# the generated sources exist only after the step above: build them in a
# second make so the wildcard sees them
$(BUILD)/gen.stamp: $(GEN)/table.c
	$(MAKE) --no-print-directory gen-objs
	touch $@

gen-objs: $(GEN_OBJ)

$(BUILD)/genobj/%.o: $(GEN)/%.c rt/rt.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -fno-strict-aliasing -Wno-unused-label -Wno-unused-parameter -Irt -c -o $@ $<

$(BUILD)/daytona: $(BUILD)/gen.stamp $(RT_OBJ) $(SRC_OBJ) $(FRONT_OBJ) $(PORT_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(RT_OBJ) $(SRC_OBJ) $(wildcard $(BUILD)/genobj/*.o) $(FRONT_OBJ) $(PORT_OBJ) $(SDL_LIBS) $(ZLIB_LIBS) -lm

$(BUILD)/daytona-run: $(BUILD)/gen.stamp $(RT_OBJ) $(SRC_OBJ) $(BUILD)/port/m2/m2run.o $(BUILD)/port/m2/m2tiledump.o $(PORT_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(RT_OBJ) $(SRC_OBJ) $(wildcard $(BUILD)/genobj/*.o) $(BUILD)/port/m2/m2run.o $(BUILD)/port/m2/m2tiledump.o \
		$(PORT_OBJ) $(ZLIB_LIBS) -lm

$(BUILD)/rt/%.o: rt/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -fno-strict-aliasing -c -o $@ $<

# functions rewritten by hand
$(BUILD)/src/%.o: src/%.c $(BUILD)/gen.stamp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -fno-strict-aliasing -Wno-unused-parameter -Irt -I$(GEN) -c -o $@ $<

# the game must produce exactly what the interpreter does: a race in about
# half a minute, or FULL=1 for 12,000 frames and the two linked boards
check: $(BUILD)/daytona-run
	ROMS=$(ROMS) tools/check.sh

check-full: $(BUILD)/daytona-run
	ROMS=$(ROMS) FULL=1 tools/check.sh

# ---------------------------------------------------------------- Android
# ../port's app (../port/README.md, "Android") with the recompiled program;
# NDK: the Android NDK, SDL: SDL2's source
APK_OUT ?= $(CURDIR)/$(BUILD)/apk
apk: $(GEN)/table.c
	@test -n "$(NDK)" -a -n "$(SDL)" || { echo "set NDK=<Android NDK> SDL=<SDL2 source>"; exit 1; }
	M2_DAYTONA=$(CURDIR) M2_APK_OUT=$(APK_OUT) sh $(PORT)/android/build.sh $(NDK) $(SDL)

# ---------------------------------------------------------------- tracer
trace: $(BUILD)/trace

TRACE_OBJ := $(filter-out $(BUILD)/port/tgp/tgp.o,$(BOARD_OBJ))
$(BUILD)/trace: $(BUILD)/tools/trace.o $(BUILD)/port/i960/i960_trace.o $(BUILD)/port/tgp/tgp_trace.o $(TRACE_OBJ)
	$(CXX) -o $@ $^ $(ZLIB_LIBS) -lm

$(BUILD)/port/tgp/tgp_trace.o: $(PORT)/tgp/tgp.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DTGP_TRACE -c -o $@ $<

$(BUILD)/port/i960/i960_trace.o: $(PORT)/i960/i960.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DI960_TRACE -c -o $@ $<

# ------------------------------------------------------------ patterns
$(BUILD)/port/sdl/%.o: $(PORT)/sdl/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(GL_CFLAGS) $(SDL_CFLAGS) -c -o $@ $<

$(BUILD)/port/sdl/%.o: $(PORT)/sdl/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(PORT)/imgui $(SDL_CFLAGS) -c -o $@ $<

$(BUILD)/port/m2/m2gl.o: $(PORT)/m2/m2gl.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(GL_CFLAGS) -c -o $@ $<

$(BUILD)/port/imgui/%.o: $(PORT)/imgui/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I$(PORT)/imgui $(SDL_CFLAGS) -w -c -o $@ $<

$(BUILD)/port/m68k/%.o: $(PORT)/m68k/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -w -c -o $@ $<

$(BUILD)/port/ymfm/%.o: $(PORT)/ymfm/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -w -c -o $@ $<

$(BUILD)/port/%.o: $(PORT)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/port/%.o: $(PORT)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD)

-include $(shell find $(BUILD) -name '*.d' 2>/dev/null)

.PHONY: all check check-full clean trace gen-objs apk
