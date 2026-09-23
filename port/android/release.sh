#!/bin/sh
# The final m2emu.apk: a clean build, libraries stripped, with Daytona USA's
# program recompiled to C when daytona.zip is in the ROM folder (else the
# interpreter runs it, as every other game).
#
#   port/android/release.sh [ROM folder]      (default ~/Desktop/ROMs)
#
# NDK and SDL (SDL2's source) default to the newest found under
# ~/Android/Sdk/ndk and ~/Android/SDL2-*; set them to use others.
# Writes port/build-release/m2emu.apk.
set -e
root=$(cd "$(dirname "$0")/../.." && pwd)
roms=${1:-$HOME/Desktop/ROMs}
NDK=${NDK:-$(ls -d "$HOME"/Android/Sdk/ndk/* 2>/dev/null | sort -V | tail -1)}
SDL=${SDL:-$(ls -d "$HOME"/Android/SDL2-* 2>/dev/null | grep -v '\.tar' | sort -V | tail -1)}
[ -d "$NDK" ] || { echo "no Android NDK: set NDK=<path>"; exit 1; }
[ -f "$SDL/Android.mk" ] || { echo "no SDL2 source: set SDL=<path>"; exit 1; }
out=$root/port/build-release
rm -rf "$out"   # ndk-build doesn't rebuild when only the flags changed
unset M2_DEBUG

if [ -f "$roms/daytona.zip" ]; then
    make -C "$root/daytona_y2k" apk ROMS="$roms" NDK="$NDK" SDL="$SDL" APK_OUT="$out"
else
    echo "no $roms/daytona.zip: Daytona USA runs on the interpreter"
    (cd "$root/port" && M2_APK_OUT="$out" sh android/build.sh "$NDK" "$SDL")
fi
ls -l "$out/m2emu.apk"
