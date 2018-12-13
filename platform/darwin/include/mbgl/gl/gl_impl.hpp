#pragma once

#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif TARGET_IPHONE_SIMULATOR
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif TARGET_OS_MAC
    #if MBGL_USE_GLES2
        #define GL_GLEXT_PROTOTYPES
        #include <GLES2/gl2.h>
        #include <GLES2/gl2ext.h>
    #else
        #include <OpenGL/OpenGL.h>
        #include <OpenGL/gl.h>
        #include <OpenGL/glext.h>
    #endif
#else
    #error Unsupported Apple platform
#endif
