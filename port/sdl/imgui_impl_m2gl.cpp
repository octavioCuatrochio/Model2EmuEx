/* Dear ImGui renderer on the port's GL layer; see imgui_impl_m2gl.h.
   Follows imgui_impl_opengl3.cpp (MIT, Omar Cornut), cut down to the GLES
   2.0 subset: no vertex array objects, no base vertex (so 16-bit index
   ranges, which ImGui splits for us), row-by-row texture updates. */
#include "imgui_impl_m2gl.h"

#include "../imgui/imgui.h"
#include "../m2/m2glapi.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static GLuint g_prog, g_vbo, g_ibo;
static GLint  g_u_tex, g_u_scale, g_u_off;
static ImVector<unsigned char> g_rows;   /* texture update staging */

static const char *vs_src =
    "attribute vec2 pos;\n"
    "attribute vec2 uv;\n"
    "attribute vec4 col;\n"
    "uniform vec2 scale;\n"
    "uniform vec2 off;\n"
    "varying vec2 v_uv;\n"
    "varying vec4 v_col;\n"
    "void main() { v_uv = uv; v_col = col; gl_Position = vec4(pos * scale + off, 0.0, 1.0); }\n";

static const char *fs_src =
    "uniform sampler2D tex;\n"
    "varying vec2 v_uv;\n"
    "varying vec4 v_col;\n"
    "void main() { gl_FragColor = v_col * texture2D(tex, v_uv); }\n";

static GLuint compile(bool es, GLenum type, const char *src)
{
    const char *head = !es ? "#version 120\n"
                     : type == GL_FRAGMENT_SHADER ? "#version 100\nprecision mediump float;\n"
                     : "#version 100\n";
    const char *parts[2] = { head, src };
    GLuint s = glCreateShader(type);
    GLint ok = 0;
    glShaderSource(s, 2, parts, NULL);
    glCompileShader(s);
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(s, sizeof log, NULL, log);
        fprintf(stderr, "launcher: shader: %s\n", log);
    }
    return s;
}

bool ImGui_ImplM2GL_Init(bool es)
{
    ImGuiIO &io = ImGui::GetIO();
    io.BackendRendererName = "imgui_impl_m2gl";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;

    g_prog = glCreateProgram();
    glAttachShader(g_prog, compile(es, GL_VERTEX_SHADER, vs_src));
    glAttachShader(g_prog, compile(es, GL_FRAGMENT_SHADER, fs_src));
    glBindAttribLocation(g_prog, 0, "pos");
    glBindAttribLocation(g_prog, 1, "uv");
    glBindAttribLocation(g_prog, 2, "col");
    glLinkProgram(g_prog);
    GLint ok = 0;
    glGetProgramiv(g_prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(g_prog, sizeof log, NULL, log);
        fprintf(stderr, "launcher: link: %s\n", log);
        return false;
    }
    g_u_tex = glGetUniformLocation(g_prog, "tex");
    g_u_scale = glGetUniformLocation(g_prog, "scale");
    g_u_off = glGetUniformLocation(g_prog, "off");
    glGenBuffers(1, &g_vbo);
    glGenBuffers(1, &g_ibo);
    return true;
}

static void destroy_texture(ImTextureData *tex)
{
    GLuint id = (GLuint)(intptr_t)tex->TexID;
    glDeleteTextures(1, &id);
    tex->SetTexID(ImTextureID_Invalid);
    tex->SetStatus(ImTextureStatus_Destroyed);
}

static void update_texture(ImTextureData *tex)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    if (tex->Status == ImTextureStatus_WantCreate) {
        GLuint id = 0;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->Width, tex->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     tex->GetPixels());
        tex->SetTexID((ImTextureID)(intptr_t)id);
        tex->SetStatus(ImTextureStatus_OK);
    } else if (tex->Status == ImTextureStatus_WantUpdates) {
        glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)tex->TexID);
        for (const ImTextureRect &r : tex->Updates) {   /* GLES 2 has no UNPACK_ROW_LENGTH */
            const int pitch = r.w * tex->BytesPerPixel;
            g_rows.resize(r.h * pitch);
            for (int y = 0; y < r.h; y++)
                memcpy(g_rows.Data + y * pitch, tex->GetPixelsAt(r.x, r.y + y), (size_t)pitch);
            glTexSubImage2D(GL_TEXTURE_2D, 0, r.x, r.y, r.w, r.h, GL_RGBA, GL_UNSIGNED_BYTE, g_rows.Data);
        }
        tex->SetStatus(ImTextureStatus_OK);
    } else if (tex->Status == ImTextureStatus_WantDestroy && tex->UnusedFrames > 0) {
        destroy_texture(tex);
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void ImGui_ImplM2GL_RenderDrawData(ImDrawData *dd)
{
    const int fb_w = (int)(dd->DisplaySize.x * dd->FramebufferScale.x);
    const int fb_h = (int)(dd->DisplaySize.y * dd->FramebufferScale.y);
    if (fb_w <= 0 || fb_h <= 0)
        return;
    if (dd->Textures)
        for (ImTextureData *tex : *dd->Textures)
            if (tex->Status != ImTextureStatus_OK)
                update_texture(tex);

    glViewport(0, 0, fb_w, fb_h);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glUseProgram(g_prog);
    glUniform1i(g_u_tex, 0);
    /* display rectangle -> clip space, y down */
    const float sx = 2.0f / dd->DisplaySize.x, sy = -2.0f / dd->DisplaySize.y;
    glUniform2f(g_u_scale, sx, sy);
    glUniform2f(g_u_off, -1.0f - dd->DisplayPos.x * sx, 1.0f - dd->DisplayPos.y * sy);
    glActiveTexture(GL_TEXTURE0);
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ibo);
    for (int a = 0; a < 3; a++)
        glEnableVertexAttribArray((GLuint)a);
    glDisableVertexAttribArray(3);
    const GLsizei st = sizeof(ImDrawVert);
    const GLenum idx_type = sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : 0x1405 /* GL_UNSIGNED_INT */;

    const ImVec2 clip_off = dd->DisplayPos, clip_scale = dd->FramebufferScale;
    for (const ImDrawList *list : dd->CmdLists) {
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)list->VtxBuffer.Size * (GLsizeiptr)sizeof(ImDrawVert),
                     list->VtxBuffer.Data, GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)list->IdxBuffer.Size * (GLsizeiptr)sizeof(ImDrawIdx),
                     list->IdxBuffer.Data, GL_STREAM_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, st, (const void *)offsetof(ImDrawVert, pos));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, st, (const void *)offsetof(ImDrawVert, uv));
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, st, (const void *)offsetof(ImDrawVert, col));
        for (const ImDrawCmd &cmd : list->CmdBuffer) {
            if (cmd.UserCallback) {
                if (cmd.UserCallback != ImDrawCallback_ResetRenderState)
                    cmd.UserCallback(list, &cmd);
                continue;
            }
            ImVec2 mn((cmd.ClipRect.x - clip_off.x) * clip_scale.x, (cmd.ClipRect.y - clip_off.y) * clip_scale.y);
            ImVec2 mx((cmd.ClipRect.z - clip_off.x) * clip_scale.x, (cmd.ClipRect.w - clip_off.y) * clip_scale.y);
            if (mx.x <= mn.x || mx.y <= mn.y)
                continue;
            glScissor((int)mn.x, (int)((float)fb_h - mx.y), (int)(mx.x - mn.x), (int)(mx.y - mn.y));
            glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)cmd.GetTexID());
            glDrawElements(GL_TRIANGLES, (GLsizei)cmd.ElemCount, idx_type,
                           (const void *)(intptr_t)(cmd.IdxOffset * sizeof(ImDrawIdx)));
        }
    }
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_BLEND);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ImGui_ImplM2GL_Shutdown()
{
    for (ImTextureData *tex : ImGui::GetPlatformIO().Textures)
        if (tex->RefCount == 1 && tex->Status != ImTextureStatus_Destroyed)
            destroy_texture(tex);
    glDeleteBuffers(1, &g_vbo);
    glDeleteBuffers(1, &g_ibo);
    glDeleteProgram(g_prog);
    ImGuiIO &io = ImGui::GetIO();
    io.BackendRendererName = NULL;
    io.BackendFlags &= ~ImGuiBackendFlags_RendererHasTextures;
}
