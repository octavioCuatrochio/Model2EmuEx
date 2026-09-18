# Model 2 port: desktop build (Linux, SDL2 + OpenGL ES 2 through Mesa/EGL)
#   make            -> build/m2emu (windowed emulator), build/m2run (headless)

CC       ?= gcc
CXX      ?= g++
CFLAGS   ?= -O2 -g
CXXFLAGS ?= -O2 -g
CFLAGS   += -std=c11 -D_DEFAULT_SOURCE -Wall -Wextra -MMD -MP
CXXFLAGS += -std=c++14 -Wall -MMD -MP
BUILD    := build

CORE_C := i960/i960.c tgp/tgp.c m2/m2board.c m2/m2rom.c m2/m2tile.c m2/m2input.c m2/m2geo.c m2/m2wide.c m2/m2pipe.c \
          snd/m2snd.c snd/scsp.c snd/multipcm.c
M68K_C := m68k/m68kcpu.c m68k/m68kops.c m68k/softfloat/softfloat.c
CORE_CXX := snd/ym3438.cpp ymfm/ymfm_opn.cpp ymfm/ymfm_adpcm.cpp ymfm/ymfm_ssg.cpp

OBJ = $(addprefix $(BUILD)/,$(CORE_C:.c=.o) $(M68K_C:.c=.o) $(CORE_CXX:.cpp=.o))

all: $(BUILD)/m2emu $(BUILD)/m2run

$(BUILD)/m2emu: $(BUILD)/sdl/m2emu.o $(BUILD)/m2/m2gl.o $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ $(shell pkg-config --libs sdl2 glesv2) -lz -lm

$(BUILD)/m2run: $(BUILD)/m2/m2run.o $(BUILD)/m2/m2tiledump.o $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^ -lz -lm

$(BUILD)/sdl/m2emu.o: sdl/m2emu.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(shell pkg-config --cflags sdl2 glesv2) -c -o $@ $<

# Musashi (third party): its own warnings are not ours
$(BUILD)/m68k/%.o: m68k/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -w -c -o $@ $<

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD)

-include $(shell find $(BUILD) -name '*.d' 2>/dev/null)

.PHONY: all clean
