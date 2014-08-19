#ifndef MBGL_RENDERER_GL
#define MBGL_RENDERER_GL

#include <string>

#ifdef NVIDIA
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>

    extern PFNGLDISCARDFRAMEBUFFEREXTPROC glDiscardFramebufferEXT;

    #define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>
        #define glGenVertexArrays glGenVertexArraysOES
        #define glBindVertexArray glBindVertexArrayOES
        #define glDeleteVertexArrays glDeleteVertexArraysOES
        #define GL_ARB_vertex_array_object 1
        #define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
    #elif TARGET_IPHONE_SIMULATOR
        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>
    #elif TARGET_OS_MAC
        #include <OpenGL/OpenGL.h>
        #include <OpenGL/gl.h>
        #if GL_APPLE_vertex_array_object
            #define GL_ARB_vertex_array_object 1
            #define glGenVertexArrays glGenVertexArraysAPPLE
            #define glBindVertexArray glBindVertexArrayAPPLE
            #define glDeleteVertexArrays glDeleteVertexArraysAPPLE
        #endif
    #else
        #error Unsupported Apple platform
    #endif
#else
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
    #include <GL/glu.h>
    #include <GL/glext.h>
#endif

namespace mbgl {
namespace gl {
// Debug group markers, useful for debuggin on iOS
#if defined(__APPLE__) && defined(DEBUG) && defined(GL_EXT_debug_marker)
// static int indent = 0;
inline void start_group(const std::string &str) {
    glPushGroupMarkerEXT(0, str.c_str());
    // fprintf(stderr, "%s%s\n", std::string(indent * 4, ' ').c_str(), str.c_str());
    // indent++;
}

inline void end_group() {
    glPopGroupMarkerEXT();
    // indent--;
}
#else
inline void start_group(const std::string &) {}
inline void end_group() {}
#endif


struct group {
    inline group(const std::string &str) { start_group(str); }
    ~group() { end_group(); };
};
}
}

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
