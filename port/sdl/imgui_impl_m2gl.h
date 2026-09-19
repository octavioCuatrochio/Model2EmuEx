/* Dear ImGui renderer on the port's GL layer (m2/m2glapi.h): the OpenGL
   ES 2.0 subset, so it runs on desktop OpenGL 2.1+ and OpenGL ES 2 alike.
   Load the GL functions (m2glapi_load) before Init. */
#ifndef IMGUI_IMPL_M2GL_H
#define IMGUI_IMPL_M2GL_H

struct ImDrawData;

bool ImGui_ImplM2GL_Init(bool es);
void ImGui_ImplM2GL_Shutdown();
void ImGui_ImplM2GL_RenderDrawData(ImDrawData *draw_data);

#endif
