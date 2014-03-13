#ifndef LLMR_RENDERER_GL
#define LLMR_RENDERER_GL

#ifdef NVIDIA
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
    // #include <EGL/egl.h>
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>
        #define glGenVertexArrays glGenVertexArraysOES
        #define glBindVertexArray glBindVertexArrayOES
        #define glDeleteVertexArrays glDeleteVertexArraysOES
        #define GL_ARB_vertex_array_object 1
    #elif TARGET_IPHONE_SIMULATOR
        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>
    #elif TARGET_OS_MAC
        #include <OpenGL/OpenGL.h>
        #include <OpenGL/gl.h>
        #define glGenVertexArrays glGenVertexArraysAPPLE
        #define glBindVertexArray glBindVertexArrayAPPLE
        #define glDeleteVertexArrays glDeleteVertexArraysAPPLE
    #else
        #error Unsupported Apple platform
    #endif
#else
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <GL/glext.h>
#endif

#ifdef GL_ES_VERSION_2_0
    #define glClearDepth glClearDepthf
    #define glDepthRange glDepthRangef
#endif

void _CHECK_GL_ERROR(const char *cmd, const char *file, int line);

#define _CHECK_ERROR(cmd, file, line) \
    cmd; \
    do { _CHECK_GL_ERROR(#cmd, file, line); } while (false);

#define CHECK_ERROR(cmd) _CHECK_ERROR(cmd, __FILE__, __LINE__)

#endif
