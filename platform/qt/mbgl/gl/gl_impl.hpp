#pragma once

#include <QtGlobal>

// Qt4
#if QT_VERSION < 0x050000
    #if __APPLE__
        #include "TargetConditionals.h"
        #include <OpenGL/OpenGL.h>
        #include <OpenGL/gl.h>
        #include <OpenGL/glext.h>
    #elif MBGL_USE_GLES2
        #define GL_GLEXT_PROTOTYPES
        #include <GLES2/gl2.h>
        #include <GLES2/gl2ext.h>
    #else
        #define GL_GLEXT_PROTOTYPES
        #include <GL/gl.h>
        #include <GL/glext.h>
    #endif

// Qt5
#else
    #include <QOpenGLContext>
    #include <QOpenGLFunctions>

    #ifndef GL_RGBA8_OES
    #define GL_RGBA8_OES GL_RGBA8
    #endif

    #ifndef GL_DEPTH24_STENCIL8_OES
    #define GL_DEPTH24_STENCIL8_OES GL_DEPTH24_STENCIL8
    #endif

    #define glActiveTexture(...) QOpenGLContext::currentContext()->functions()->glActiveTexture(__VA_ARGS__)
    #define glAttachShader(...) QOpenGLContext::currentContext()->functions()->glAttachShader(__VA_ARGS__)
    #define glBindAttribLocation(...) QOpenGLContext::currentContext()->functions()->glBindAttribLocation(__VA_ARGS__)
    #define glBindBuffer(...) QOpenGLContext::currentContext()->functions()->glBindBuffer(__VA_ARGS__)
    #define glBindFramebuffer(...) QOpenGLContext::currentContext()->functions()->glBindFramebuffer(__VA_ARGS__)
    #define glBindRenderbuffer(...) QOpenGLContext::currentContext()->functions()->glBindRenderbuffer(__VA_ARGS__)
    #define glBindTexture(...) QOpenGLContext::currentContext()->functions()->glBindTexture(__VA_ARGS__)
    #define glBlendColor(...) QOpenGLContext::currentContext()->functions()->glBlendColor(__VA_ARGS__)
    #define glBlendEquation(...) QOpenGLContext::currentContext()->functions()->glBlendEquation(__VA_ARGS__)
    #define glBlendEquationSeparate(...) QOpenGLContext::currentContext()->functions()->glBlendEquationSeparate(__VA_ARGS__)
    #define glBlendFunc(...) QOpenGLContext::currentContext()->functions()->glBlendFunc(__VA_ARGS__)
    #define glBlendFuncSeparate(...) QOpenGLContext::currentContext()->functions()->glBlendFuncSeparate(__VA_ARGS__)
    #define glBufferData(...) QOpenGLContext::currentContext()->functions()->glBufferData(__VA_ARGS__)
    #define glBufferSubData(...) QOpenGLContext::currentContext()->functions()->glBufferSubData(__VA_ARGS__)
    #define glCheckFramebufferStatus(...) QOpenGLContext::currentContext()->functions()->glCheckFramebufferStatus(__VA_ARGS__)
    #define glClear(...) QOpenGLContext::currentContext()->functions()->glClear(__VA_ARGS__)
    #define glClearColor(...) QOpenGLContext::currentContext()->functions()->glClearColor(__VA_ARGS__)
    #define glClearDepthf(...) QOpenGLContext::currentContext()->functions()->glClearDepthf(__VA_ARGS__)
    #define glClearStencil(...) QOpenGLContext::currentContext()->functions()->glClearStencil(__VA_ARGS__)
    #define glColorMask(...) QOpenGLContext::currentContext()->functions()->glColorMask(__VA_ARGS__)
    #define glCompileShader(...) QOpenGLContext::currentContext()->functions()->glCompileShader(__VA_ARGS__)
    #define glCompressedTexImage2D(...) QOpenGLContext::currentContext()->functions()->glCompressedTexImage2D(__VA_ARGS__)
    #define glCompressedTexSubImage2D(...) QOpenGLContext::currentContext()->functions()->glCompressedTexSubImage2D(__VA_ARGS__)
    #define glCopyTexImage2D(...) QOpenGLContext::currentContext()->functions()->glCopyTexImage2D(__VA_ARGS__)
    #define glCopyTexSubImage2D(...) QOpenGLContext::currentContext()->functions()->glCopyTexSubImage2D(__VA_ARGS__)
    #define glCreateProgram(...) QOpenGLContext::currentContext()->functions()->glCreateProgram(__VA_ARGS__)
    #define glCreateShader(...) QOpenGLContext::currentContext()->functions()->glCreateShader(__VA_ARGS__)
    #define glCullFace(...) QOpenGLContext::currentContext()->functions()->glCullFace(__VA_ARGS__)
    #define glDeleteBuffers(...) QOpenGLContext::currentContext()->functions()->glDeleteBuffers(__VA_ARGS__)
    #define glDeleteFramebuffers(...) QOpenGLContext::currentContext()->functions()->glDeleteFramebuffers(__VA_ARGS__)
    #define glDeleteProgram(...) QOpenGLContext::currentContext()->functions()->glDeleteProgram(__VA_ARGS__)
    #define glDeleteRenderbuffers(...) QOpenGLContext::currentContext()->functions()->glDeleteRenderbuffers(__VA_ARGS__)
    #define glDeleteShader(...) QOpenGLContext::currentContext()->functions()->glDeleteShader(__VA_ARGS__)
    #define glDeleteTextures(...) QOpenGLContext::currentContext()->functions()->glDeleteTextures(__VA_ARGS__)
    #define glDepthFunc(...) QOpenGLContext::currentContext()->functions()->glDepthFunc(__VA_ARGS__)
    #define glDepthMask(...) QOpenGLContext::currentContext()->functions()->glDepthMask(__VA_ARGS__)
    #define glDepthRangef(...) QOpenGLContext::currentContext()->functions()->glDepthRangef(__VA_ARGS__)
    #define glDetachShader(...) QOpenGLContext::currentContext()->functions()->glDetachShader(__VA_ARGS__)
    #define glDisable(...) QOpenGLContext::currentContext()->functions()->glDisable(__VA_ARGS__)
    #define glDisableVertexAttribArray(...) QOpenGLContext::currentContext()->functions()->glDisableVertexAttribArray(__VA_ARGS__)
    #define glDrawArrays(...) QOpenGLContext::currentContext()->functions()->glDrawArrays(__VA_ARGS__)
    #define glDrawElements(...) QOpenGLContext::currentContext()->functions()->glDrawElements(__VA_ARGS__)
    #define glEnable(...) QOpenGLContext::currentContext()->functions()->glEnable(__VA_ARGS__)
    #define glEnableVertexAttribArray(...) QOpenGLContext::currentContext()->functions()->glEnableVertexAttribArray(__VA_ARGS__)
    #define glFinish(...) QOpenGLContext::currentContext()->functions()->glFinish(__VA_ARGS__)
    #define glFlush(...) QOpenGLContext::currentContext()->functions()->glFlush(__VA_ARGS__)
    #define glFramebufferRenderbuffer(...) QOpenGLContext::currentContext()->functions()->glFramebufferRenderbuffer(__VA_ARGS__)
    #define glFramebufferTexture2D(...) QOpenGLContext::currentContext()->functions()->glFramebufferTexture2D(__VA_ARGS__)
    #define glFrontFace(...) QOpenGLContext::currentContext()->functions()->glFrontFace(__VA_ARGS__)
    #define glGenBuffers(...) QOpenGLContext::currentContext()->functions()->glGenBuffers(__VA_ARGS__)
    #define glGenerateMipmap(...) QOpenGLContext::currentContext()->functions()->glGenerateMipmap(__VA_ARGS__)
    #define glGenFramebuffers(...) QOpenGLContext::currentContext()->functions()->glGenFramebuffers(__VA_ARGS__)
    #define glGenRenderbuffers(...) QOpenGLContext::currentContext()->functions()->glGenRenderbuffers(__VA_ARGS__)
    #define glGenTextures(...) QOpenGLContext::currentContext()->functions()->glGenTextures(__VA_ARGS__)
    #define glGetActiveAttrib(...) QOpenGLContext::currentContext()->functions()->glGetActiveAttrib(__VA_ARGS__)
    #define glGetActiveUniform(...) QOpenGLContext::currentContext()->functions()->glGetActiveUniform(__VA_ARGS__)
    #define glGetAttachedShaders(...) QOpenGLContext::currentContext()->functions()->glGetAttachedShaders(__VA_ARGS__)
    #define glGetAttribLocation(...) QOpenGLContext::currentContext()->functions()->glGetAttribLocation(__VA_ARGS__)
    #define glGetBooleanv(...) QOpenGLContext::currentContext()->functions()->glGetBooleanv(__VA_ARGS__)
    #define glGetBufferParameteriv(...) QOpenGLContext::currentContext()->functions()->glGetBufferParameteriv(__VA_ARGS__)
    #define glGetError(...) QOpenGLContext::currentContext()->functions()->glGetError(__VA_ARGS__)
    #define glGetFloatv(...) QOpenGLContext::currentContext()->functions()->glGetFloatv(__VA_ARGS__)
    #define glGetFramebufferAttachmentParameteriv(...) QOpenGLContext::currentContext()->functions()->glGetFramebufferAttachmentParameteriv(__VA_ARGS__)
    #define glGetIntegerv(...) QOpenGLContext::currentContext()->functions()->glGetIntegerv(__VA_ARGS__)
    #define glGetProgramInfoLog(...) QOpenGLContext::currentContext()->functions()->glGetProgramInfoLog(__VA_ARGS__)
    #define glGetProgramiv(...) QOpenGLContext::currentContext()->functions()->glGetProgramiv(__VA_ARGS__)
    #define glGetRenderbufferParameteriv(...) QOpenGLContext::currentContext()->functions()->glGetRenderbufferParameteriv(__VA_ARGS__)
    #define glGetShaderInfoLog(...) QOpenGLContext::currentContext()->functions()->glGetShaderInfoLog(__VA_ARGS__)
    #define glGetShaderiv(...) QOpenGLContext::currentContext()->functions()->glGetShaderiv(__VA_ARGS__)
    #define glGetShaderPrecisionFormat(...) QOpenGLContext::currentContext()->functions()->glGetShaderPrecisionFormat(__VA_ARGS__)
    #define glGetShaderSource(...) QOpenGLContext::currentContext()->functions()->glGetShaderSource(__VA_ARGS__)
    #define glGetString(...) QOpenGLContext::currentContext()->functions()->glGetString(__VA_ARGS__)
    #define glGetTexParameterfv(...) QOpenGLContext::currentContext()->functions()->glGetTexParameterfv(__VA_ARGS__)
    #define glGetTexParameteriv(...) QOpenGLContext::currentContext()->functions()->glGetTexParameteriv(__VA_ARGS__)
    #define glGetUniformfv(...) QOpenGLContext::currentContext()->functions()->glGetUniformfv(__VA_ARGS__)
    #define glGetUniformiv(...) QOpenGLContext::currentContext()->functions()->glGetUniformiv(__VA_ARGS__)
    #define glGetUniformLocation(...) QOpenGLContext::currentContext()->functions()->glGetUniformLocation(__VA_ARGS__)
    #define glGetVertexAttribfv(...) QOpenGLContext::currentContext()->functions()->glGetVertexAttribfv(__VA_ARGS__)
    #define glGetVertexAttribiv(...) QOpenGLContext::currentContext()->functions()->glGetVertexAttribiv(__VA_ARGS__)
    #define glGetVertexAttribPointerv(...) QOpenGLContext::currentContext()->functions()->glGetVertexAttribPointerv(__VA_ARGS__)
    #define glHint(...) QOpenGLContext::currentContext()->functions()->glHint(__VA_ARGS__)
    #define glIsBuffer(...) QOpenGLContext::currentContext()->functions()->glIsBuffer(__VA_ARGS__)
    #define glIsEnabled(...) QOpenGLContext::currentContext()->functions()->glIsEnabled(__VA_ARGS__)
    #define glIsFramebuffer(...) QOpenGLContext::currentContext()->functions()->glIsFramebuffer(__VA_ARGS__)
    #define glIsProgram(...) QOpenGLContext::currentContext()->functions()->glIsProgram(__VA_ARGS__)
    #define glIsRenderbuffer(...) QOpenGLContext::currentContext()->functions()->glIsRenderbuffer(__VA_ARGS__)
    #define glIsShader(...) QOpenGLContext::currentContext()->functions()->glIsShader(__VA_ARGS__)
    #define glIsTexture(...) QOpenGLContext::currentContext()->functions()->glIsTexture(__VA_ARGS__)
    #define glLineWidth(...) QOpenGLContext::currentContext()->functions()->glLineWidth(__VA_ARGS__)
    #define glLinkProgram(...) QOpenGLContext::currentContext()->functions()->glLinkProgram(__VA_ARGS__)
    #define glPixelStorei(...) QOpenGLContext::currentContext()->functions()->glPixelStorei(__VA_ARGS__)
    #define glPolygonOffset(...) QOpenGLContext::currentContext()->functions()->glPolygonOffset(__VA_ARGS__)
    #define glReadPixels(...) QOpenGLContext::currentContext()->functions()->glReadPixels(__VA_ARGS__)
    #define glReleaseShaderCompiler(...) QOpenGLContext::currentContext()->functions()->glReleaseShaderCompiler(__VA_ARGS__)
    #define glRenderbufferStorage(...) QOpenGLContext::currentContext()->functions()->glRenderbufferStorage(__VA_ARGS__)
    #define glSampleCoverage(...) QOpenGLContext::currentContext()->functions()->glSampleCoverage(__VA_ARGS__)
    #define glScissor(...) QOpenGLContext::currentContext()->functions()->glScissor(__VA_ARGS__)
    #define glShaderBinary(...) QOpenGLContext::currentContext()->functions()->glShaderBinary(__VA_ARGS__)
    #define glShaderSource(...) QOpenGLContext::currentContext()->functions()->glShaderSource(__VA_ARGS__)
    #define glStencilFunc(...) QOpenGLContext::currentContext()->functions()->glStencilFunc(__VA_ARGS__)
    #define glStencilFuncSeparate(...) QOpenGLContext::currentContext()->functions()->glStencilFuncSeparate(__VA_ARGS__)
    #define glStencilMask(...) QOpenGLContext::currentContext()->functions()->glStencilMask(__VA_ARGS__)
    #define glStencilMaskSeparate(...) QOpenGLContext::currentContext()->functions()->glStencilMaskSeparate(__VA_ARGS__)
    #define glStencilOp(...) QOpenGLContext::currentContext()->functions()->glStencilOp(__VA_ARGS__)
    #define glStencilOpSeparate(...) QOpenGLContext::currentContext()->functions()->glStencilOpSeparate(__VA_ARGS__)
    #define glTexImage2D(...) QOpenGLContext::currentContext()->functions()->glTexImage2D(__VA_ARGS__)
    #define glTexLevelParameteriv(...) QOpenGLContext::currentContext()->functions()->glTexLevelParameteriv(__VA_ARGS__)
    #define glTexParameterf(...) QOpenGLContext::currentContext()->functions()->glTexParameterf(__VA_ARGS__)
    #define glTexParameterfv(...) QOpenGLContext::currentContext()->functions()->glTexParameterfv(__VA_ARGS__)
    #define glTexParameteri(...) QOpenGLContext::currentContext()->functions()->glTexParameteri(__VA_ARGS__)
    #define glTexParameteriv(...) QOpenGLContext::currentContext()->functions()->glTexParameteriv(__VA_ARGS__)
    #define glTexSubImage2D(...) QOpenGLContext::currentContext()->functions()->glTexSubImage2D(__VA_ARGS__)
    #define glUniform1f(...) QOpenGLContext::currentContext()->functions()->glUniform1f(__VA_ARGS__)
    #define glUniform1fv(...) QOpenGLContext::currentContext()->functions()->glUniform1fv(__VA_ARGS__)
    #define glUniform1i(...) QOpenGLContext::currentContext()->functions()->glUniform1i(__VA_ARGS__)
    #define glUniform1iv(...) QOpenGLContext::currentContext()->functions()->glUniform1iv(__VA_ARGS__)
    #define glUniform2f(...) QOpenGLContext::currentContext()->functions()->glUniform2f(__VA_ARGS__)
    #define glUniform2fv(...) QOpenGLContext::currentContext()->functions()->glUniform2fv(__VA_ARGS__)
    #define glUniform2i(...) QOpenGLContext::currentContext()->functions()->glUniform2i(__VA_ARGS__)
    #define glUniform2iv(...) QOpenGLContext::currentContext()->functions()->glUniform2iv(__VA_ARGS__)
    #define glUniform3f(...) QOpenGLContext::currentContext()->functions()->glUniform3f(__VA_ARGS__)
    #define glUniform3fv(...) QOpenGLContext::currentContext()->functions()->glUniform3fv(__VA_ARGS__)
    #define glUniform3i(...) QOpenGLContext::currentContext()->functions()->glUniform3i(__VA_ARGS__)
    #define glUniform3iv(...) QOpenGLContext::currentContext()->functions()->glUniform3iv(__VA_ARGS__)
    #define glUniform4f(...) QOpenGLContext::currentContext()->functions()->glUniform4f(__VA_ARGS__)
    #define glUniform4fv(...) QOpenGLContext::currentContext()->functions()->glUniform4fv(__VA_ARGS__)
    #define glUniform4i(...) QOpenGLContext::currentContext()->functions()->glUniform4i(__VA_ARGS__)
    #define glUniform4iv(...) QOpenGLContext::currentContext()->functions()->glUniform4iv(__VA_ARGS__)
    #define glUniformMatrix2fv(...) QOpenGLContext::currentContext()->functions()->glUniformMatrix2fv(__VA_ARGS__)
    #define glUniformMatrix3fv(...) QOpenGLContext::currentContext()->functions()->glUniformMatrix3fv(__VA_ARGS__)
    #define glUniformMatrix4fv(...) QOpenGLContext::currentContext()->functions()->glUniformMatrix4fv(__VA_ARGS__)
    #define glUseProgram(...) QOpenGLContext::currentContext()->functions()->glUseProgram(__VA_ARGS__)
    #define glValidateProgram(...) QOpenGLContext::currentContext()->functions()->glValidateProgram(__VA_ARGS__)
    #define glVertexAttrib1f(...) QOpenGLContext::currentContext()->functions()->glVertexAttrib1f(__VA_ARGS__)
    #define glVertexAttrib1fv(...) QOpenGLContext::currentContext()->functions()->glVertexAttrib1fv(__VA_ARGS__)
    #define glVertexAttrib2f(...) QOpenGLContext::currentContext()->functions()->glVertexAttrib2f(__VA_ARGS__)
    #define glVertexAttrib2fv(...) QOpenGLContext::currentContext()->functions()->glVertexAttrib2fv(__VA_ARGS__)
    #define glVertexAttrib3f(...) QOpenGLContext::currentContext()->functions()->glVertexAttrib3f(__VA_ARGS__)
    #define glVertexAttrib3fv(...) QOpenGLContext::currentContext()->functions()->glVertexAttrib3fv(__VA_ARGS__)
    #define glVertexAttrib4f(...) QOpenGLContext::currentContext()->functions()->glVertexAttrib4f(__VA_ARGS__)
    #define glVertexAttrib4fv(...) QOpenGLContext::currentContext()->functions()->glVertexAttrib4fv(__VA_ARGS__)
    #define glVertexAttribPointer(...) QOpenGLContext::currentContext()->functions()->glVertexAttribPointer(__VA_ARGS__)
    #define glViewport(...) QOpenGLContext::currentContext()->functions()->glViewport(__VA_ARGS__)
#endif
