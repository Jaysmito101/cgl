#ifndef __glad_h_
#define __glad_h_

/* Extended stub file for GLAD - actual implementation assumed to be present */

#include <KHR/khrplatform.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* OpenGL 4.3 Core Profile */
#define GL_VERSION_4_3 1

/* OpenGL Type Definitions */
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;
typedef int64_t GLint64;
typedef uint64_t GLuint64;
typedef struct __GLsync *GLsync;

/* OpenGL Constants */
#define GL_FALSE 0
#define GL_TRUE 1
#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_LINE_LOOP 0x0002
#define GL_LINE_STRIP 0x0003
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006
#define GL_QUADS 0x0007
#define GL_QUAD_STRIP 0x0008
#define GL_POLYGON 0x0009

#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406
#define GL_DOUBLE 0x140A

#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_BGR 0x80E0
#define GL_BGRA 0x80E1
#define GL_LUMINANCE 0x1909
#define GL_LUMINANCE_ALPHA 0x190A

#define GL_RGB8 0x8051
#define GL_RGBA8 0x8058
#define GL_RGB16 0x8054
#define GL_RGBA16 0x805B

#define GL_NEVER 0x0200
#define GL_LESS 0x0201
#define GL_EQUAL 0x0202
#define GL_LEQUAL 0x0203
#define GL_GREATER 0x0204
#define GL_NOTEQUAL 0x0205
#define GL_GEQUAL 0x0206
#define GL_ALWAYS 0x0207

#define GL_DEPTH_TEST 0x0B71
#define GL_DEPTH_WRITEMASK 0x0B72
#define GL_DEPTH_CLEAR_VALUE 0x0B73
#define GL_DEPTH_FUNC 0x0B74
#define GL_BLEND 0x0BE2
#define GL_CULL_FACE 0x0B44
#define GL_POLYGON_OFFSET_FILL 0x8037

#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_STENCIL_BUFFER_BIT 0x00000400

#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_GEOMETRY_SHADER 0x8DD9
#define GL_COMPUTE_SHADER 0x91B9

#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_VALIDATE_STATUS 0x8B83
#define GL_INFO_LOG_LENGTH 0x8B84

#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_UNIFORM_BUFFER 0x8A11
#define GL_SHADER_STORAGE_BUFFER 0x90D2

#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_STREAM_DRAW 0x88E0

#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE_3D 0x806F
#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_TEXTURE_2D_ARRAY 0x8C1A

#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803

#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_LINEAR_MIPMAP_LINEAR 0x2703

#define GL_CLAMP_TO_EDGE 0x812F
#define GL_REPEAT 0x2901
#define GL_MIRRORED_REPEAT 0x8370

#define GL_FRAMEBUFFER 0x8D40
#define GL_RENDERBUFFER 0x8D41
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_STENCIL_ATTACHMENT 0x8D20

#define GL_FRAMEBUFFER_COMPLETE 0x8CD5

#define GL_SHADER_STORAGE_BARRIER_BIT 0x00002000
#define GL_BUFFER_UPDATE_BARRIER_BIT 0x00000200

#define GL_PROGRAM_POINT_SIZE 0x8642

/* OpenGL Function Declarations */
extern void glClear(GLbitfield mask);
extern void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
extern void glEnable(GLenum cap);
extern void glDisable(GLenum cap);
extern void glDepthFunc(GLenum func);
extern void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
extern void glDrawArrays(GLenum mode, GLint first, GLsizei count);
extern void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices);

/* VAO Functions */
extern void glGenVertexArrays(GLsizei n, GLuint *arrays);
extern void glBindVertexArray(GLuint array);
extern void glDeleteVertexArrays(GLsizei n, const GLuint *arrays);

/* Buffer Functions */
extern void glGenBuffers(GLsizei n, GLuint *buffers);
extern void glBindBuffer(GLenum target, GLuint buffer);
extern void glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
extern void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
extern void glDeleteBuffers(GLsizei n, const GLuint *buffers);
extern void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
extern void glEnableVertexAttribArray(GLuint index);
extern void glDisableVertexAttribArray(GLuint index);

/* Shader Functions */
extern GLuint glCreateShader(GLenum shaderType);
extern void glShaderSource(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
extern void glCompileShader(GLuint shader);
extern void glGetShaderiv(GLuint shader, GLenum pname, GLint *params);
extern void glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
extern void glDeleteShader(GLuint shader);

/* Program Functions */
extern GLuint glCreateProgram(void);
extern void glAttachShader(GLuint program, GLuint shader);
extern void glLinkProgram(GLuint program);
extern void glGetProgramiv(GLuint program, GLenum pname, GLint *params);
extern void glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
extern void glUseProgram(GLuint program);
extern void glDeleteProgram(GLuint program);

/* Uniform Functions */
extern GLint glGetUniformLocation(GLuint program, const GLchar *name);
extern void glUniform1f(GLint location, GLfloat v0);
extern void glUniform2f(GLint location, GLfloat v0, GLfloat v1);
extern void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
extern void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
extern void glUniform1i(GLint location, GLint v0);
extern void glUniform2i(GLint location, GLint v0, GLint v1);
extern void glUniform3i(GLint location, GLint v0, GLint v1, GLint v2);
extern void glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
extern void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

/* Texture Functions */
extern void glGenTextures(GLsizei n, GLuint *textures);
extern void glBindTexture(GLenum target, GLuint texture);
extern void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
extern void glTexParameteri(GLenum target, GLenum pname, GLint param);
extern void glGenerateMipmap(GLenum target);
extern void glDeleteTextures(GLsizei n, const GLuint *textures);

/* Framebuffer Functions */
extern void glGenFramebuffers(GLsizei n, GLuint *framebuffers);
extern void glBindFramebuffer(GLenum target, GLuint framebuffer);
extern void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
extern GLenum glCheckFramebufferStatus(GLenum target);
extern void glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers);

/* Compute Shader Functions */
extern void glDispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
extern void glMemoryBarrier(GLbitfield barriers);

/* Buffer Binding Functions */
extern void glBindBufferBase(GLenum target, GLuint index, GLuint buffer);

/* Error Functions */
extern GLenum glGetError(void);

/* Get Functions */
extern void glGetIntegerv(GLenum pname, GLint *data);
extern void glGetFloatv(GLenum pname, GLfloat *data);
extern const GLubyte *glGetString(GLenum name);

/* GLAD specific functions */
extern int gladLoadGL(void);
extern int gladLoadGLLoader(void* loader);

#ifdef __cplusplus
}
#endif

#endif /* __glad_h_ */