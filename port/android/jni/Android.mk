# m2emu for Android: SDL2 (built from its source, SDL=<path>) and libmain.so,
# the desktop frontend (sdl/m2emu.c, OpenGL ES 2) with the whole machine.
# Run through port/android/build.sh (make apk).

M2_ROOT := $(call my-dir)/../..

include $(SDL)/Android.mk

include $(CLEAR_VARS)
LOCAL_PATH   := $(M2_ROOT)
LOCAL_MODULE := main

LOCAL_SRC_FILES := \
    sdl/m2emu.c sdl/options.c sdl/dirlist.c sdl/launcher.cpp sdl/imgui_impl_m2gl.cpp \
    m2/m2gl.c m2/m2glapi.c \
    imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp \
    imgui/backends/imgui_impl_sdl2.cpp \
    i960/i960.c tgp/tgp.c m2/m2board.c m2/m2rom.c m2/m2tile.c m2/m2input.c m2/m2geo.c \
    m2/m2wide.c m2/m2pipe.c m2/m2net.c \
    snd/m2snd.c snd/scsp.c snd/multipcm.c \
    m68k/m68kcpu.c m68k/m68kops.c m68k/softfloat/softfloat.c \
    snd/ym3438.cpp ymfm/ymfm_opn.cpp ymfm/ymfm_adpcm.cpp ymfm/ymfm_ssg.cpp

LOCAL_C_INCLUDES   := $(M2_ROOT)/imgui $(SDL)/include $(M2_INC)   # M2_INC: SDL2/ -> SDL/include
# as the Makefile's android target; -w: the Makefile's warnings are for the desktop builds
LOCAL_CFLAGS       := -O2 -mcpu=cortex-a7 -mfpu=neon-vfpv4 -DM2_GLES -D_DEFAULT_SOURCE -w
LOCAL_CONLYFLAGS   := -std=c11
LOCAL_CPPFLAGS     := -std=c++14
LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_LDLIBS       := -lz -llog -lm
LOCAL_STRIP_MODE   := none   # symbols for simpleperf and crash reports
include $(BUILD_SHARED_LIBRARY)
