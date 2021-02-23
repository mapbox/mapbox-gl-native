#include <mbgl/platform/gl_functions.hpp>

#include "TargetConditionals.h"

#if TARGET_OS_IPHONE
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif TARGET_OS_SIMULATOR
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif TARGET_OS_MAC
    #include <OpenGL/OpenGL.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glext.h>
#else
    #error Unsupported Apple platform
#endif

namespace mbgl {
namespace platform {

void (* const glActiveTexture)(GLenum) = ::glActiveTexture;
void (* const glAttachShader)(GLuint, GLuint) = ::glAttachShader;
void (* const glBindAttribLocation)(GLuint, GLuint, const GLchar *) = ::glBindAttribLocation;
void (* const glBindBuffer)(GLenum, GLuint) = ::glBindBuffer;
void (* const glBindFramebuffer)(GLenum, GLuint) = ::glBindFramebuffer;
void (* const glBindRenderbuffer)(GLenum, GLuint) = ::glBindRenderbuffer;
void (* const glBindTexture)(GLenum, GLuint) = ::glBindTexture;
void (* const glBlendColor)(GLfloat, GLfloat, GLfloat, GLfloat) = ::glBlendColor;
void (* const glBlendEquation)(GLenum) = ::glBlendEquation;
void (* const glBlendEquationSeparate)(GLenum, GLenum) = ::glBlendEquationSeparate;
void (* const glBlendFunc)(GLenum, GLenum) = ::glBlendFunc;
void (* const glBlendFuncSeparate)(GLenum, GLenum, GLenum, GLenum) = ::glBlendFuncSeparate;
void (* const glBufferData)(GLenum, GLsizeiptr, const void *, GLenum) = ::glBufferData;
void (* const glBufferSubData)(GLenum, GLintptr, GLsizeiptr, const void *) = ::glBufferSubData;
GLenum (* const glCheckFramebufferStatus)(GLenum) = ::glCheckFramebufferStatus;
void (* const glClear)(GLbitfield) = ::glClear;
void (* const glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat) = ::glClearColor;
#if !TARGET_OS_IPHONE && !TARGET_OS_SIMULATOR && TARGET_OS_MAC
void (* const glClearDepthf)(GLfloat) = [](GLfloat depth) { return ::glClearDepth(depth); };
#else
void (* const glClearDepthf)(GLfloat) = ::glClearDepthf;
#endif
void (* const glClearStencil)(GLint) = ::glClearStencil;
void (* const glColorMask)(GLboolean, GLboolean, GLboolean, GLboolean) = ::glColorMask;
void (* const glCompileShader)(GLuint) = ::glCompileShader;
void (* const glCompressedTexImage2D)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void *) = ::glCompressedTexImage2D;
void (* const glCompressedTexSubImage2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const void *) = ::glCompressedTexSubImage2D;
void (* const glCopyTexImage2D)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint) = ::glCopyTexImage2D;
void (* const glCopyTexSubImage2D)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei) = ::glCopyTexSubImage2D;
GLuint (* const glCreateProgram)() = ::glCreateProgram;
GLuint (* const glCreateShader)(GLenum) = ::glCreateShader;
void (* const glCullFace)(GLenum) = ::glCullFace;
void (* const glDeleteBuffers)(GLsizei, const GLuint *) = ::glDeleteBuffers;
void (* const glDeleteFramebuffers)(GLsizei, const GLuint *) = ::glDeleteFramebuffers;
void (* const glDeleteProgram)(GLuint) = ::glDeleteProgram;
void (* const glDeleteRenderbuffers)(GLsizei, const GLuint *) = ::glDeleteRenderbuffers;
void (* const glDeleteShader)(GLuint) = ::glDeleteShader;
void (* const glDeleteTextures)(GLsizei, const GLuint *) = ::glDeleteTextures;
void (* const glDepthFunc)(GLenum) = ::glDepthFunc;
void (* const glDepthMask)(GLboolean) = ::glDepthMask;
#if !TARGET_OS_IPHONE && !TARGET_OS_SIMULATOR && TARGET_OS_MAC
void (* const glDepthRangef)(GLfloat, GLfloat) = [](GLfloat nearVal, GLfloat farVal) { return ::glDepthRange(nearVal, farVal); };
#else
void (* const glDepthRangef)(GLfloat, GLfloat) = ::glDepthRangef;
#endif
void (* const glDetachShader)(GLuint, GLuint) = ::glDetachShader;
void (* const glDisable)(GLenum) = ::glDisable;
void (* const glDisableVertexAttribArray)(GLuint) = ::glDisableVertexAttribArray;
void (* const glDrawArrays)(GLenum, GLint, GLsizei) = ::glDrawArrays;
void (* const glDrawElements)(GLenum, GLsizei, GLenum, const void *) = ::glDrawElements;
void (* const glEnable)(GLenum) = ::glEnable;
void (* const glEnableVertexAttribArray)(GLuint) = ::glEnableVertexAttribArray;
void (* const glFinish)() = ::glFinish;
void (* const glFlush)() = ::glFlush;
void (* const glFramebufferRenderbuffer)(GLenum, GLenum, GLenum, GLuint) = ::glFramebufferRenderbuffer;
void (* const glFramebufferTexture2D)(GLenum, GLenum, GLenum, GLuint, GLint) = ::glFramebufferTexture2D;
void (* const glFrontFace)(GLenum) = ::glFrontFace;
void (* const glGenBuffers)(GLsizei, GLuint *) = ::glGenBuffers;
void (* const glGenerateMipmap)(GLenum) = ::glGenerateMipmap;
void (* const glGenFramebuffers)(GLsizei, GLuint *) = ::glGenFramebuffers;
void (* const glGenRenderbuffers)(GLsizei, GLuint *) = ::glGenRenderbuffers;
void (* const glGenTextures)(GLsizei, GLuint *) = ::glGenTextures;
void (* const glGetActiveAttrib)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *) = ::glGetActiveAttrib;
void (* const glGetActiveUniform)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *) = ::glGetActiveUniform;
void (* const glGetAttachedShaders)(GLuint, GLsizei, GLsizei *, GLuint *) = ::glGetAttachedShaders;
GLint (* const glGetAttribLocation)(GLuint, const GLchar *) = ::glGetAttribLocation;
void (* const glGetBooleanv)(GLenum, GLboolean *) = ::glGetBooleanv;
void (* const glGetBufferParameteriv)(GLenum, GLenum, GLint *) = ::glGetBufferParameteriv;
GLenum (* const glGetError)() = ::glGetError;
void (* const glGetFloatv)(GLenum, GLfloat *) = ::glGetFloatv;
void (* const glGetFramebufferAttachmentParameteriv)(GLenum, GLenum, GLenum, GLint *) = ::glGetFramebufferAttachmentParameteriv;
void (* const glGetIntegerv)(GLenum, GLint *) = ::glGetIntegerv;
void (* const glGetProgramInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *) = ::glGetProgramInfoLog;
void (* const glGetProgramiv)(GLuint, GLenum, GLint *) = ::glGetProgramiv;
void (* const glGetRenderbufferParameteriv)(GLenum, GLenum, GLint *) = ::glGetRenderbufferParameteriv;
void (* const glGetShaderInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *) = ::glGetShaderInfoLog;
void (* const glGetShaderiv)(GLuint, GLenum, GLint *) = ::glGetShaderiv;
void (* const glGetShaderSource)(GLuint, GLsizei, GLsizei *, GLchar *) = ::glGetShaderSource;
const GLubyte *(*glGetString)(GLenum) = ::glGetString;
void (* const glGetTexParameterfv)(GLenum, GLenum, GLfloat *) = ::glGetTexParameterfv;
void (* const glGetTexParameteriv)(GLenum, GLenum, GLint *) = ::glGetTexParameteriv;
void (* const glGetUniformfv)(GLuint, GLint, GLfloat *) = ::glGetUniformfv;
void (* const glGetUniformiv)(GLuint, GLint, GLint *) = ::glGetUniformiv;
GLint (* const glGetUniformLocation)(GLuint, const GLchar *) = ::glGetUniformLocation;
void (* const glGetVertexAttribfv)(GLuint, GLenum, GLfloat *) = ::glGetVertexAttribfv;
void (* const glGetVertexAttribiv)(GLuint, GLenum, GLint *) = ::glGetVertexAttribiv;
void (* const glGetVertexAttribPointerv)(GLuint, GLenum, void **) = ::glGetVertexAttribPointerv;
void (* const glHint)(GLenum, GLenum) = ::glHint;
GLboolean (* const glIsBuffer)(GLuint) = ::glIsBuffer;
GLboolean (* const glIsEnabled)(GLenum) = ::glIsEnabled;
GLboolean (* const glIsFramebuffer)(GLuint) = ::glIsFramebuffer;
GLboolean (* const glIsProgram)(GLuint) = ::glIsProgram;
GLboolean (* const glIsRenderbuffer)(GLuint) = ::glIsRenderbuffer;
GLboolean (* const glIsShader)(GLuint) = ::glIsShader;
GLboolean (* const glIsTexture)(GLuint) = ::glIsTexture;
void (* const glLineWidth)(GLfloat) = ::glLineWidth;
void (* const glLinkProgram)(GLuint) = ::glLinkProgram;
void (* const glPixelStorei)(GLenum, GLint) = ::glPixelStorei;
void (* const glPolygonOffset)(GLfloat, GLfloat) = ::glPolygonOffset;
void (* const glReadPixels)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void *) = ::glReadPixels;
void (* const glRenderbufferStorage)(GLenum, GLenum, GLsizei, GLsizei) = ::glRenderbufferStorage;
void (* const glSampleCoverage)(GLfloat, GLboolean) = ::glSampleCoverage;
void (* const glScissor)(GLint, GLint, GLsizei, GLsizei) = ::glScissor;
void (* const glShaderSource)(GLuint, GLsizei, const GLchar * const*, const GLint *) = ::glShaderSource;
void (* const glStencilFunc)(GLenum, GLint, GLuint) = ::glStencilFunc;
void (* const glStencilFuncSeparate)(GLenum, GLenum, GLint, GLuint) = ::glStencilFuncSeparate;
void (* const glStencilMask)(GLuint) = ::glStencilMask;
void (* const glStencilMaskSeparate)(GLenum, GLuint) = ::glStencilMaskSeparate;
void (* const glStencilOp)(GLenum, GLenum, GLenum) = ::glStencilOp;
void (* const glStencilOpSeparate)(GLenum, GLenum, GLenum, GLenum) = ::glStencilOpSeparate;
void (* const glTexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void *) = ::glTexImage2D;
void (* const glTexParameterf)(GLenum, GLenum, GLfloat) = ::glTexParameterf;
void (* const glTexParameterfv)(GLenum, GLenum, const GLfloat *) = ::glTexParameterfv;
void (* const glTexParameteri)(GLenum, GLenum, GLint) = ::glTexParameteri;
void (* const glTexParameteriv)(GLenum, GLenum, const GLint *) = ::glTexParameteriv;
void (* const glTexSubImage2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const void *) = ::glTexSubImage2D;
void (* const glUniform1f)(GLint, GLfloat) = ::glUniform1f;
void (* const glUniform1fv)(GLint, GLsizei, const GLfloat *) = ::glUniform1fv;
void (* const glUniform1i)(GLint, GLint) = ::glUniform1i;
void (* const glUniform1iv)(GLint, GLsizei, const GLint *) = ::glUniform1iv;
void (* const glUniform2f)(GLint, GLfloat, GLfloat) = ::glUniform2f;
void (* const glUniform2fv)(GLint, GLsizei, const GLfloat *) = ::glUniform2fv;
void (* const glUniform2i)(GLint, GLint, GLint) = ::glUniform2i;
void (* const glUniform2iv)(GLint, GLsizei, const GLint *) = ::glUniform2iv;
void (* const glUniform3f)(GLint, GLfloat, GLfloat, GLfloat) = ::glUniform3f;
void (* const glUniform3fv)(GLint, GLsizei, const GLfloat *) = ::glUniform3fv;
void (* const glUniform3i)(GLint, GLint, GLint, GLint) = ::glUniform3i;
void (* const glUniform3iv)(GLint, GLsizei, const GLint *) = ::glUniform3iv;
void (* const glUniform4f)(GLint, GLfloat, GLfloat, GLfloat, GLfloat) = ::glUniform4f;
void (* const glUniform4fv)(GLint, GLsizei, const GLfloat *) = ::glUniform4fv;
void (* const glUniform4i)(GLint, GLint, GLint, GLint, GLint) = ::glUniform4i;
void (* const glUniform4iv)(GLint, GLsizei, const GLint *) = ::glUniform4iv;
void (* const glUniformMatrix2fv)(GLint, GLsizei, GLboolean, const GLfloat *) = ::glUniformMatrix2fv;
void (* const glUniformMatrix3fv)(GLint, GLsizei, GLboolean, const GLfloat *) = ::glUniformMatrix3fv;
void (* const glUniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat *) = ::glUniformMatrix4fv;
void (* const glUseProgram)(GLuint) = ::glUseProgram;
void (* const glValidateProgram)(GLuint) = ::glValidateProgram;
void (* const glVertexAttrib1f)(GLuint, GLfloat) = ::glVertexAttrib1f;
void (* const glVertexAttrib1fv)(GLuint, const GLfloat *) = ::glVertexAttrib1fv;
void (* const glVertexAttrib2f)(GLuint, GLfloat, GLfloat) = ::glVertexAttrib2f;
void (* const glVertexAttrib2fv)(GLuint, const GLfloat *) = ::glVertexAttrib2fv;
void (* const glVertexAttrib3f)(GLuint, GLfloat, GLfloat, GLfloat) = ::glVertexAttrib3f;
void (* const glVertexAttrib3fv)(GLuint, const GLfloat *) = ::glVertexAttrib3fv;
void (* const glVertexAttrib4f)(GLuint, GLfloat, GLfloat, GLfloat, GLfloat) = ::glVertexAttrib4f;
void (* const glVertexAttrib4fv)(GLuint, const GLfloat *) = ::glVertexAttrib4fv;
void (* const glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void *) = ::glVertexAttribPointer;
void (* const glViewport)(GLint, GLint, GLsizei, GLsizei) = ::glViewport;

#ifndef MBGL_USE_GLES2
void (* const glDrawPixels)(GLsizei, GLsizei, GLenum, GLenum, const GLvoid *) = ::glDrawPixels;
void (* const glGetDoublev)(GLenum, GLdouble *) = ::glGetDoublev;
void (* const glPixelTransferf)(GLenum, GLfloat) = ::glPixelTransferf;
void (* const glPixelZoom)(GLfloat, GLfloat) = ::glPixelZoom;
void (* const glPointSize)(GLfloat) = ::glPointSize;
void (* const glRasterPos4d)(GLdouble, GLdouble, GLdouble, GLdouble) = ::glRasterPos4d;
#endif

}  // namespace platform
}  // namespace mbgl
