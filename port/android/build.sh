#!/bin/sh
# m2emu.apk for Android (armeabi-v7a) without Gradle: ndk-build for the native
# code, then aapt2, javac, d8, zipalign and apksigner from the SDK.
#   android/build.sh <NDK> <SDL2 source> [<Android SDK>]      (or: make apk NDK=... SDL=...)
# Writes build-apk/m2emu.apk; signed with the debug key (~/.android/debug.keystore).
# M2_DAYTONA=<daytona_y2k dir> builds in Daytona USA's recompiled program
# (M2_APK_OUT: the output directory instead of ./build-apk).
set -e
NDK=$1
SDL=$2
SDK=${3:-${ANDROID_HOME:-$HOME/Android/Sdk}}
[ -n "$NDK" ] && [ -n "$SDL" ] || { echo "usage: $0 <NDK> <SDL2 source> [<Android SDK>]"; exit 1; }
[ -f "$SDL/Android.mk" ] || { echo "$SDL: not an SDL2 source tree"; exit 1; }

here=$(cd "$(dirname "$0")" && pwd)
out=${M2_APK_OUT:-$(pwd)/build-apk}
jar=$(ls -d "$SDK"/platforms/android-* | sort -V | tail -1)/android.jar
bt=$(ls -d "$SDK"/build-tools/* | sort -V | tail -1)
mkdir -p "$out/include"
ln -sfn "$(cd "$SDL" && pwd)/include" "$out/include/SDL2"   # for <SDL2/SDL.h>

# native: libSDL2.so, libmain.so, libc++_shared.so
"$NDK/ndk-build" -C "$here" -j"$(nproc)" NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=jni/Android.mk \
    NDK_APPLICATION_MK=jni/Application.mk NDK_OUT="$out/obj" NDK_LIBS_OUT="$out/apk/lib" SDL="$(cd "$SDL" && pwd)" M2_INC="$out/include" \
    ${M2_DAYTONA:+M2_DAYTONA="$(cd "$M2_DAYTONA" && pwd)"}

# Java: SDL's activity and ours
rm -rf "$out/classes" && mkdir -p "$out/classes"
javac -nowarn --release 11 -classpath "$jar" -d "$out/classes" \
    $(find "$SDL/android-project/app/src/main/java" "$here/src" -name '*.java')
"$bt/d8" --release --min-api 21 --lib "$jar" --output "$out/apk" $(find "$out/classes" -name '*.class')

# package, align, sign
"$bt/aapt2" link -o "$out/unsigned.apk" --manifest "$here/AndroidManifest.xml" -I "$jar" \
    --min-sdk-version 21 --target-sdk-version 29
(cd "$out/apk" && "$bt/aapt" add "$out/unsigned.apk" classes.dex lib/*/*.so >/dev/null)
"$bt/zipalign" -f -p 4 "$out/unsigned.apk" "$out/aligned.apk"
"$bt/apksigner" sign --ks "$HOME/.android/debug.keystore" --ks-pass pass:android \
    --out "$out/m2emu.apk" "$out/aligned.apk"
rm -f "$out/unsigned.apk" "$out/aligned.apk"
echo "$out/m2emu.apk"
