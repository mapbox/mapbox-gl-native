#ifndef LLMR_RENDERER_GL
#define LLMR_RENDERER_GL

#ifdef EMSCRIPTEN
    #include <GLES2/gl2.h>
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>
        #define glGenVertexArrays glGenVertexArraysOES
        #define glBindVertexArray glBindVertexArrayOES
        #define glDeleteVertexArrays glDeleteVertexArraysOES
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

#endif
