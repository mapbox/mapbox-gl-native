#pragma once

#include <stdexcept>
#include <limits>

#if __APPLE__
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
#elif __ANDROID__ || MBGL_USE_GLES2
    #define GL_GLEXT_PROTOTYPES
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#elif __QT_ && QT_VERSION >= 0x050000
    #define GL_GLEXT_PROTOTYPES
    #include <QtGui/qopengl.h>
#else
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
    #include <GL/glext.h>
#endif

namespace mbgl {
namespace gl {

struct Error : std::runtime_error {
    using std::runtime_error::runtime_error;
};

void checkError(const char *cmd, const char *file, int line);

#ifndef NDEBUG
#define MBGL_CHECK_ERROR(cmd) ([&]() { struct __MBGL_C_E { ~__MBGL_C_E() noexcept(false) { ::mbgl::gl::checkError(#cmd, __FILE__, __LINE__); } } __MBGL_C_E; return cmd; }())
#else
#define MBGL_CHECK_ERROR(cmd) (cmd)
#endif

} // namespace gl
} // namespace mbgl
