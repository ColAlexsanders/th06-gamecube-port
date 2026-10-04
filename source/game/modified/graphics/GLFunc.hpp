#pragma once

// OpenGL function pointers stubbed for GameCube
// The GfxInterface abstraction in GfxInterface.hpp provides the actual rendering API

// Stub types for GL types that may be used elsewhere
typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef int GLint;
typedef unsigned int GLbitfield;
typedef char GLchar;
typedef unsigned char GLboolean;
typedef short GLshort;
typedef unsigned short GLushort;
typedef int GLsizei;
typedef double GLdouble;
typedef unsigned int GLhandleARB;
typedef void GLvoid;

struct GLFuncTable
{
    // For GameCube, all function pointers are no-ops
    // The actual rendering is done through GfxInterface/GXBackend
    
    void ResolveFunctions(bool glesContext)
    {
        // No-op for GameCube - GX doesn't use function pointers like OpenGL
        (void)glesContext;
    }

    // Functions where arguments use doubles in OpenGL and floats in GLES and therefore need manual dispatch
    void glClearDepthf(GLclampf depth) { (void)depth; }
    void glDepthRangef(GLclampf near_val, GLclampf far_val) { (void)near_val; (void)far_val; }

    // Function pointers for functions shared between GL and GLES
    // All stubbed for GameCube
    void(*glAlphaFunc)(GLenum func, GLclampf ref);
    void(*glBindTexture)(GLenum target, GLuint texture);
    void(*glBlendFunc)(GLenum sfactor, GLenum dfactor);
    void(*glClear)(GLbitfield mask);
    void(*glClearColor)(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    void(*glColorPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
    void(*glDeleteTextures)(GLsizei n, const GLuint *textures);
    void(*glDepthFunc)(GLenum func);
    void(*glDepthMask)(GLboolean flag);
    void(*glDisableClientState)(GLenum cap);
    void(*glDrawArrays)(GLenum mode, GLint first, GLsizei count);
    void(*glEnable)(GLenum cap);
    void(*glEnableClientState)(GLenum cap);
    void(*glFogf)(GLenum pname, GLfloat param);
    void(*glFogfv)(GLenum pname, const GLfloat *params);
    void(*glGenTextures)(GLsizei n, GLuint *textures);
    GLenum(*glGetError)(void);
    void(*glGetFloatv)(GLenum pname, GLfloat *params);
    void(*glGetIntegerv)(GLenum pname, GLint *params);
    void(*glLoadIdentity)(void);
    void(*glLoadMatrixf)(const GLfloat *m);
    void(*glMatrixMode)(GLenum mode);
    void(*glMultMatrixf)(const GLfloat *m);
    void(*glPopMatrix)(void);
    void(*glPushMatrix)(void);
    void(*glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type,
                                   GLvoid *pixels);
    void(*glShadeModel)(GLenum mode);
    void(*glTexCoordPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
    void(*glTexEnvfv)(GLenum target, GLenum pname, const GLfloat *params);
    void(*glTexEnvi)(GLenum target, GLenum pname, GLint param);
    void(*glTexImage2D)(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height,
                                   GLint border, GLenum format, GLenum type, const GLvoid *pixels);
    void(*glTexParameteri)(GLenum target, GLenum pname, GLint param);
    void(*glTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
                                      GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
    void(*glVertexPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
    void(*glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);

    // GL(ES) 2.X / WebGL - stubbed for GameCube
    void(*glAttachShader)(GLenum shader, GLenum shaderType);
    void(*glBindAttribLocation)(GLenum program, GLuint index, const GLchar *name);
    void(*glCompileShader)(GLenum shader);
    GLenum(*glCreateProgram)(void);
    GLenum(*glCreateShader)(GLenum type);
    void(*glDeleteProgram)(GLuint program);
    void(*glDeleteShader)(GLuint shader);
    void(*glDisableVertexAttribArray)(GLuint index);
    void(*glEnableVertexAttribArray)(GLuint index);
    void(*glGetProgramInfoLog)(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
    void(*glGetProgramiv)(GLuint program, GLenum pname, GLint *params);
    void(*glGetShaderInfoLog)(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
    void(*glGetShaderiv)(GLuint shader, GLenum pname, GLint *params);
    GLint(*glGetUniformLocation)(GLuint program, const GLchar *name);
    void(*glLinkProgram)(GLuint program);
    void(*glShaderSource)(GLuint shader, GLsizei count, const GLchar **string, const GLint *length);
    void(*glUniform1f)(GLint location, GLfloat v0);
    void(*glUniform1i)(GLint location, GLint v0);
    void(*glUniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
    void(*glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
    void(*glUseProgram)(GLuint program);
    void(*glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);

  private:
    // GLES forms for cases where they're different
    void(*glClearDepthf_ptr)(GLclampf depth);
    void(*glDepthRangef_ptr)(GLclampf near_val, GLclampf far_val);

    // GL forms for cases where they're different
    // Stubs for double-precision GL functions (not used in GameCube)
    void(*glClearDepth)(GLdouble depth);
    void(*glDepthRange)(GLdouble near_val, GLdouble far_val);

    bool isGlesContext;
};

extern GLFuncTable g_glFuncTable;
