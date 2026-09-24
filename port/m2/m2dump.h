/*
 * A frame's 3D scene as a model: every polygon the display list draws, all
 * objects where the game put them (camera space), written as a Wavefront
 * OBJ with its MTL and one PNG texture atlas, for Blender and the like.
 * No OpenGL: it reads the board's texture, luma and colour memories.
 */
#ifndef M2DUMP_H
#define M2DUMP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct m2_board;
struct m2_geo;

/* Runs the list in `b` again from `live`'s geometrizer state (not touched)
   and writes <dir>/scene.obj, scene.mtl and scene.png, making dir (and its
   parent) if needed. remap and gamma: the tile generator's colour tables
   (m2_tilegen). Returns 0, with a one-line summary in msg, or -1 with the
   reason there. */
int m2dump_scene(const struct m2_geo *live, const struct m2_board *b, const uint8_t remap[256],
                 const uint8_t gamma[3][256], const char *dir, char *msg, int msg_size);

#ifdef __cplusplus
}
#endif

#endif
