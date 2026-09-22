#!/bin/sh
# The recompiled program must behave exactly like the interpreter: same 3D
# and tile output, same sound, same RAM.
#
#   tools/check.sh            a race in about 30 seconds (make check)
#   FULL=1 tools/check.sh     12,000 frames, and the two linked boards
#   FRAMES=n MODES="..."      whatever you like
#
# The interpreter's side only changes when the emulator or the ROM does, so
# it is kept in build/ref and reused; that halves every run after the first.
set -e
cd "$(dirname "$0")/.."
ROMS=${ROMS:-$HOME/Desktop/ROMs}
RUN=build/daytona-run
REF=build/ref

# coin, the two menus, then hold the accelerator; a second coin later on
KEYS=200:06:5,300:02:5,500:02:5,700:02:5,900:02:5,1000:c8:11000,1400:cb:60,1800:cd:60,2500:cb:90,3000:cd:90,4000:cb:120,6000:06:5,6100:02:5,6300:02:5,6500:02:5,6700:02:5

if [ -n "$FULL" ]; then
    FRAMES=${FRAMES:-12000}
    MODES=${MODES:-"plain M2_RESET_AT=4000 M2_COOP=1 M2_PIPE=1"}
else
    FRAMES=${FRAMES:-3000}
    MODES=${MODES:-"plain M2_RESET_AT=1500 M2_PIPE=1"}
fi

OUT=$(mktemp -d)
trap 'rm -rf "$OUT"' EXIT
mkdir -p "$REF"

run() {   # run <prefix> <env>
    env $2 M2_GEOHASH=1 M2_KEYS=$KEYS M2_WAV="$1.wav" M2_RAM="$1.ram" \
        $RUN daytona "$FRAMES" "$ROMS" 2>/dev/null | grep geohash > "$1.txt"
}

# The interpreter's answer for this mode and length, from build/ref unless
# the emulator, the recompiler or the ROM has changed since.
reference() {   # reference <name> <mode>
    ref="$REF/$1-$FRAMES"
    if [ -f "$ref.ram" ] && [ -z "$(find ../port rt tools "$ROMS/daytona.zip" -newer "$ref.ram" 2>/dev/null | head -1)" ]; then
        return
    fi
    run "$ref" "DAYTONA_INTERP=1 $2"
}

# All of main RAM must match, the register frames on the stack included:
# the recompiled code and the functions rewritten by hand pay their cycles
# instruction by instruction, so even the interrupts land where they did.
# When only the frames differ (0x5ff500 up), the likely cause is a
# hand-written function whose registers at some instruction don't match.
RAM_BELOW_STACK=1045760

fail=0
for mode in $MODES; do
    [ "$mode" = plain ] && mode=""
    name=$(echo "${mode:-plain}" | tr -d '= ')
    reference "$name" "$mode"
    run "$OUT/r-$name" "$mode"
    for part in txt wav; do
        if cmp -s "$REF/$name-$FRAMES.$part" "$OUT/r-$name.$part"; then
            echo "ok    ${mode:-plain} $part"
        else
            echo "DIFFER ${mode:-plain} $part"
            fail=1
        fi
    done
    if cmp -s "$REF/$name-$FRAMES.ram" "$OUT/r-$name.ram"; then
        echo "ok    ${mode:-plain} ram"
    elif cmp -s -n $RAM_BELOW_STACK "$REF/$name-$FRAMES.ram" "$OUT/r-$name.ram"; then
        echo "DIFFER ${mode:-plain} ram (only the register frames on the stack)"
        fail=1
    else
        echo "DIFFER ${mode:-plain} ram"
        fail=1
    fi
done
[ $fail = 0 ] && echo "identical to the interpreter over $FRAMES frames"
exit $fail
