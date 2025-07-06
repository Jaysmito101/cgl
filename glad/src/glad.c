/* Stub file for GLAD - actual implementation assumed to be present */

#include <glad/glad.h>

/* Stub implementations for GLAD functions */
int gladLoadGL(void) {
    return 1;
}

int gladLoadGLLoader(void* loader) {
    (void)loader;
    return 1;
}

/* Stub implementations for OpenGL functions */
void glClear(GLbitfield mask) { (void)mask; }
void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) { (void)red; (void)green; (void)blue; (void)alpha; }
void glEnable(GLenum cap) { (void)cap; }
void glDisable(GLenum cap) { (void)cap; }
void glDepthFunc(GLenum func) { (void)func; }
void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) { (void)x; (void)y; (void)width; (void)height; }
void glDrawArrays(GLenum mode, GLint first, GLsizei count) { (void)mode; (void)first; (void)count; }
void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices) { (void)mode; (void)count; (void)type; (void)indices; }

void glGenVertexArrays(GLsizei n, GLuint *arrays) { (void)n; (void)arrays; }
void glBindVertexArray(GLuint array) { (void)array; }
void glDeleteVertexArrays(GLsizei n, const GLuint *arrays) { (void)n; (void)arrays; }

void glGenBuffers(GLsizei n, GLuint *buffers) { (void)n; (void)buffers; }
void glBindBuffer(GLenum target, GLuint buffer) { (void)target; (void)buffer; }
void glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage) { (void)target; (void)size; (void)data; (void)usage; }
void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void *data) { (void)target; (void)offset; (void)size; (void)data; }
void glDeleteBuffers(GLsizei n, const GLuint *buffers) { (void)n; (void)buffers; }
void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) { (void)index; (void)size; (void)type; (void)normalized; (void)stride; (void)pointer; }
void glEnableVertexAttribArray(GLuint index) { (void)index; }
void glDisableVertexAttribArray(GLuint index) { (void)index; }

GLuint glCreateShader(GLenum shaderType) { (void)shaderType; return 1; }
void glShaderSource(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length) { (void)shader; (void)count; (void)string; (void)length; }
void glCompileShader(GLuint shader) { (void)shader; }
void glGetShaderiv(GLuint shader, GLenum pname, GLint *params) { (void)shader; (void)pname; if (params) *params = GL_TRUE; }
void glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog) { (void)shader; (void)maxLength; if (length) *length = 0; if (infoLog) infoLog[0] = '\0'; }
void glDeleteShader(GLuint shader) { (void)shader; }

GLuint glCreateProgram(void) { return 1; }
void glAttachShader(GLuint program, GLuint shader) { (void)program; (void)shader; }
void glLinkProgram(GLuint program) { (void)program; }
void glGetProgramiv(GLuint program, GLenum pname, GLint *params) { (void)program; (void)pname; if (params) *params = GL_TRUE; }
void glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog) { (void)program; (void)maxLength; if (length) *length = 0; if (infoLog) infoLog[0] = '\0'; }
void glUseProgram(GLuint program) { (void)program; }
void glDeleteProgram(GLuint program) { (void)program; }

GLint glGetUniformLocation(GLuint program, const GLchar *name) { (void)program; (void)name; return 0; }
void glUniform1f(GLint location, GLfloat v0) { (void)location; (void)v0; }
void glUniform2f(GLint location, GLfloat v0, GLfloat v1) { (void)location; (void)v0; (void)v1; }
void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) { (void)location; (void)v0; (void)v1; (void)v2; }
void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) { (void)location; (void)v0; (void)v1; (void)v2; (void)v3; }
void glUniform1i(GLint location, GLint v0) { (void)location; (void)v0; }
void glUniform2i(GLint location, GLint v0, GLint v1) { (void)location; (void)v0; (void)v1; }
void glUniform3i(GLint location, GLint v0, GLint v1, GLint v2) { (void)location; (void)v0; (void)v1; (void)v2; }
void glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3) { (void)location; (void)v0; (void)v1; (void)v2; (void)v3; }
void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { (void)location; (void)count; (void)transpose; (void)value; }

void glGenTextures(GLsizei n, GLuint *textures) { (void)n; (void)textures; }
void glBindTexture(GLenum target, GLuint texture) { (void)target; (void)texture; }
void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels) { (void)target; (void)level; (void)internalformat; (void)width; (void)height; (void)border; (void)format; (void)type; (void)pixels; }
void glTexParameteri(GLenum target, GLenum pname, GLint param) { (void)target; (void)pname; (void)param; }
void glGenerateMipmap(GLenum target) { (void)target; }
void glDeleteTextures(GLsizei n, const GLuint *textures) { (void)n; (void)textures; }

void glGenFramebuffers(GLsizei n, GLuint *framebuffers) { (void)n; (void)framebuffers; }
void glBindFramebuffer(GLenum target, GLuint framebuffer) { (void)target; (void)framebuffer; }
void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) { (void)target; (void)attachment; (void)textarget; (void)texture; (void)level; }
GLenum glCheckFramebufferStatus(GLenum target) { (void)target; return GL_FRAMEBUFFER_COMPLETE; }
void glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers) { (void)n; (void)framebuffers; }

void glDispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z) { (void)num_groups_x; (void)num_groups_y; (void)num_groups_z; }
void glMemoryBarrier(GLbitfield barriers) { (void)barriers; }

void glBindBufferBase(GLenum target, GLuint index, GLuint buffer) { (void)target; (void)index; (void)buffer; }

GLenum glGetError(void) { return 0; }
void glGetIntegerv(GLenum pname, GLint *data) { (void)pname; (void)data; }
void glGetFloatv(GLenum pname, GLfloat *data) { (void)pname; (void)data; }
const GLubyte *glGetString(GLenum name) { (void)name; return (const GLubyte *)"OpenGL ES Stub"; }