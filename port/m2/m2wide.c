/* Widescreen rules; see m2wide.h. */
#include "m2wide.h"
#include "m2board.h"

#include <string.h>

/* I960_ReadByte for the two RAMs the scripts read */
static unsigned rd_ram(const m2_board *b, unsigned addr)   /* 0x500000 */
{
    return b->ram[(addr - 0x500000) & 0xfffff];
}
static unsigned rd_ram2(const m2_board *b, unsigned addr)  /* 0x200000 */
{
    return b->ram2[(addr - 0x200000) & 0x3ffff];
}

void m2wide_update(const m2_board *b, m2_wide_state *st)
{
    const char *name = b->game->name;
    st->widescreen = 1;
    st->stretch = 0;

    if (!strcmp(name, "daytona")) {   /* SCRIPTS\daytona.lua */
        unsigned state = rd_ram(b, 0x5010a4);
        /* in game, attract, attract high scores, attract VR */
        int wide = state == 0x16 || (state >= 0x03 && state <= 0x07);
        st->widescreen = wide;
        st->stretch = wide ? M2_STRETCH_B_LOW : 0;   /* sky and clouds */
    } else if (!strncmp(name, "vf2", 3)) {
        /* no script in the original (no widescreen there); the stage sky is
           tilemap B below the 3D, so stretch it like Daytona's */
        st->stretch = M2_STRETCH_B_LOW;
    } else if (!strcmp(name, "srallyc") || !strcmp(name, "srallycb")) {   /* srallyc.lua, srallycb.lua */
        unsigned s1 = rd_ram2(b, 0x202098), s2 = rd_ram2(b, 0x20209c), s3 = rd_ram2(b, 0x2020ac);
        st->widescreen = !(s1 == 3 && s2 == 5 && s3 == 1);
    }
}
