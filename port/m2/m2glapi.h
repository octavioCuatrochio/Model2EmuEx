/*
 * The part of OpenGL the renderer uses: the OpenGL ES 2.0 subset that
 * desktop OpenGL 2.1 also has, with the same values. Every function is
 * loaded at run time (m2glapi_load), so nothing links against a GL
 * library and the same code runs on desktop OpenGL (Windows, Linux) and
 * OpenGL ES 2 (Android, Mali). Include this instead of the system's GL
 * headers.
 */
#ifndef M2GLAPI_H
#define M2GLAPI_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) && !defined(_WIN64)
#define M2GL_APIENTRY __stdcall
#else
#define M2GL_APIENTRY
#endif

typedef unsigned int  GLenum;
typedef unsigned int  GLuint;
typedef unsigned int  GLbitfield;
typedef int           GLint;
typedef int           GLsizei;
typedef unsigned char GLboolean;
typedef unsigned char GLubyte;
typedef float         GLfloat;
typedef char          GLchar;
typedef ptrdiff_t     GLsizeiptr;
typedef void          GLvoid;

#define GL_FALSE                 0
#define GL_TRUE                  1
#define GL_TRIANGLES             0x0004
#define GL_TRIANGLE_STRIP        0x0005
#define GL_SRC_ALPHA             0x0302
#define GL_ONE_MINUS_SRC_ALPHA   0x0303
#define GL_CULL_FACE             0x0B44
#define GL_DEPTH_TEST            0x0B71
#define GL_BLEND                 0x0BE2
#define GL_SCISSOR_TEST          0x0C11
#define GL_UNPACK_ALIGNMENT      0x0CF5
#define GL_TEXTURE_2D            0x0DE1
#define GL_UNSIGNED_BYTE         0x1401
#define GL_UNSIGNED_SHORT        0x1403
#define GL_FLOAT                 0x1406
#define GL_RGBA                  0x1908
#define GL_LUMINANCE             0x1909
#define GL_LUMINANCE_ALPHA       0x190A
#define GL_NEAREST               0x2600
#define GL_LINEAR                0x2601
#define GL_TEXTURE_MAG_FILTER    0x2800
#define GL_TEXTURE_MIN_FILTER    0x2801
#define GL_TEXTURE_WRAP_S        0x2802
#define GL_TEXTURE_WRAP_T        0x2803
#define GL_CLAMP_TO_EDGE         0x812F
#define GL_TEXTURE0              0x84C0
#define GL_TEXTURE1              0x84C1
#define GL_TEXTURE2              0x84C2
#define GL_ARRAY_BUFFER          0x8892
#define GL_ELEMENT_ARRAY_BUFFER  0x8893
#define GL_STREAM_DRAW           0x88E0
#define GL_STATIC_DRAW           0x88E4
#define GL_FRAGMENT_SHADER       0x8B30
#define GL_VERTEX_SHADER         0x8B31
#define GL_COMPILE_STATUS        0x8B81
#define GL_LINK_STATUS           0x8B82
#define GL_FRAMEBUFFER_COMPLETE  0x8CD5
#define GL_COLOR_ATTACHMENT0     0x8CE0
#define GL_FRAMEBUFFER           0x8D40
#define GL_COLOR_BUFFER_BIT      0x00004000

/* return type, name, parameters */
#define M2GL_FUNCTIONS(X) \
    X(void,   glActiveTexture, (GLenum texture)) \
    X(void,   glAttachShader, (GLuint program, GLuint shader)) \
    X(void,   glBindAttribLocation, (GLuint program, GLuint index, const GLchar *name)) \
    X(void,   glBindBuffer, (GLenum target, GLuint buffer)) \
    X(void,   glBindFramebuffer, (GLenum target, GLuint framebuffer)) \
    X(void,   glBindTexture, (GLenum target, GLuint texture)) \
    X(void,   glBlendFunc, (GLenum sfactor, GLenum dfactor)) \
    X(void,   glBufferData, (GLenum target, GLsizeiptr size, const void *data, GLenum usage)) \
    X(GLenum, glCheckFramebufferStatus, (GLenum target)) \
    X(void,   glClear, (GLbitfield mask)) \
    X(void,   glClearColor, (GLfloat r, GLfloat g, GLfloat b, GLfloat a)) \
    X(void,   glCompileShader, (GLuint shader)) \
    X(GLuint, glCreateProgram, (void)) \
    X(GLuint, glCreateShader, (GLenum type)) \
    X(void,   glDeleteBuffers, (GLsizei n, const GLuint *buffers)) \
    X(void,   glDeleteFramebuffers, (GLsizei n, const GLuint *framebuffers)) \
    X(void,   glDeleteProgram, (GLuint program)) \
    X(void,   glDeleteTextures, (GLsizei n, const GLuint *textures)) \
    X(void,   glDisable, (GLenum cap)) \
    X(void,   glDisableVertexAttribArray, (GLuint index)) \
    X(void,   glDrawArrays, (GLenum mode, GLint first, GLsizei count)) \
    X(void,   glDrawElements, (GLenum mode, GLsizei count, GLenum type, const void *indices)) \
    X(void,   glEnable, (GLenum cap)) \
    X(void,   glEnableVertexAttribArray, (GLuint index)) \
    X(void,   glFinish, (void)) \
    X(void,   glFramebufferTexture2D, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)) \
    X(void,   glGenBuffers, (GLsizei n, GLuint *buffers)) \
    X(void,   glGenFramebuffers, (GLsizei n, GLuint *framebuffers)) \
    X(void,   glGenTextures, (GLsizei n, GLuint *textures)) \
    X(void,   glGetProgramInfoLog, (GLuint program, GLsizei size, GLsizei *length, GLchar *log)) \
    X(void,   glGetProgramiv, (GLuint program, GLenum pname, GLint *params)) \
    X(void,   glGetShaderInfoLog, (GLuint shader, GLsizei size, GLsizei *length, GLchar *log)) \
    X(void,   glGetShaderiv, (GLuint shader, GLenum pname, GLint *params)) \
    X(GLint,  glGetUniformLocation, (GLuint program, const GLchar *name)) \
    X(void,   glLinkProgram, (GLuint program)) \
    X(void,   glPixelStorei, (GLenum pname, GLint param)) \
    X(void,   glReadPixels, (GLint x, GLint y, GLsizei w, GLsizei h, GLenum format, GLenum type, void *pixels)) \
    X(void,   glScissor, (GLint x, GLint y, GLsizei w, GLsizei h)) \
    X(void,   glShaderSource, (GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length)) \
    X(void,   glTexImage2D, (GLenum target, GLint level, GLint internalformat, GLsizei w, GLsizei h, GLint border, GLenum format, GLenum type, const void *pixels)) \
    X(void,   glTexParameteri, (GLenum target, GLenum pname, GLint param)) \
    X(void,   glTexSubImage2D, (GLenum target, GLint level, GLint x, GLint y, GLsizei w, GLsizei h, GLenum format, GLenum type, const void *pixels)) \
    X(void,   glUniform1f, (GLint location, GLfloat v0)) \
    X(void,   glUniform1i, (GLint location, GLint v0)) \
    X(void,   glUniform2f, (GLint location, GLfloat v0, GLfloat v1)) \
    X(void,   glUseProgram, (GLuint program)) \
    X(void,   glVertexAttribPointer, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)) \
    X(void,   glViewport, (GLint x, GLint y, GLsizei w, GLsizei h))

#define M2GL_DECLARE(ret, name, args) extern ret (M2GL_APIENTRY *m2_##name) args;
M2GL_FUNCTIONS(M2GL_DECLARE)
#undef M2GL_DECLARE

/* call through the loaded pointers */
#define glActiveTexture m2_glActiveTexture
#define glAttachShader m2_glAttachShader
#define glBindAttribLocation m2_glBindAttribLocation
#define glBindBuffer m2_glBindBuffer
#define glBindFramebuffer m2_glBindFramebuffer
#define glBindTexture m2_glBindTexture
#define glBlendFunc m2_glBlendFunc
#define glBufferData m2_glBufferData
#define glCheckFramebufferStatus m2_glCheckFramebufferStatus
#define glClear m2_glClear
#define glClearColor m2_glClearColor
#define glCompileShader m2_glCompileShader
#define glCreateProgram m2_glCreateProgram
#define glCreateShader m2_glCreateShader
#define glDeleteBuffers m2_glDeleteBuffers
#define glDeleteFramebuffers m2_glDeleteFramebuffers
#define glDeleteProgram m2_glDeleteProgram
#define glDeleteTextures m2_glDeleteTextures
#define glDisable m2_glDisable
#define glDisableVertexAttribArray m2_glDisableVertexAttribArray
#define glDrawArrays m2_glDrawArrays
#define glDrawElements m2_glDrawElements
#define glEnable m2_glEnable
#define glEnableVertexAttribArray m2_glEnableVertexAttribArray
#define glFinish m2_glFinish
#define glFramebufferTexture2D m2_glFramebufferTexture2D
#define glGenBuffers m2_glGenBuffers
#define glGenFramebuffers m2_glGenFramebuffers
#define glGenTextures m2_glGenTextures
#define glGetProgramInfoLog m2_glGetProgramInfoLog
#define glGetProgramiv m2_glGetProgramiv
#define glGetShaderInfoLog m2_glGetShaderInfoLog
#define glGetShaderiv m2_glGetShaderiv
#define glGetUniformLocation m2_glGetUniformLocation
#define glLinkProgram m2_glLinkProgram
#define glPixelStorei m2_glPixelStorei
#define glReadPixels m2_glReadPixels
#define glScissor m2_glScissor
#define glShaderSource m2_glShaderSource
#define glTexImage2D m2_glTexImage2D
#define glTexParameteri m2_glTexParameteri
#define glTexSubImage2D m2_glTexSubImage2D
#define glUniform1f m2_glUniform1f
#define glUniform1i m2_glUniform1i
#define glUniform2f m2_glUniform2f
#define glUseProgram m2_glUseProgram
#define glVertexAttribPointer m2_glVertexAttribPointer
#define glViewport m2_glViewport

/* Loads every function through `getproc` (SDL_GL_GetProcAddress, or
   eglGetProcAddress / wglGetProcAddress + GetProcAddress) with a context
   current. Returns 0 and names the first missing function in *missing. */
int m2glapi_load(void *(*getproc)(const char *name), const char **missing);

#ifdef __cplusplus
}
#endif

#endif
