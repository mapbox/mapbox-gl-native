#pragma once

#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif TARGET_IPHONE_SIMULATOR
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif TARGET_OS_MAC
    #include <OpenGL/OpenGL.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glext.h>
#else
    #error Unsupported Apple platform
#endif
