#ifndef MBGL_RENDERER_GL
#define MBGL_RENDERER_GL

#include <string>
#include <stdexcept>

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
    #else
        #error Unsupported Apple platform
    #endif
#elif __ANDROID__
    #define GL_GLEXT_PROTOTYPES
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#else
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
    #include <GL/glext.h>
#endif

namespace mbgl {
namespace gl {

struct Error : ::std::runtime_error {
    inline Error(GLenum err, const std::string &msg) : ::std::runtime_error(msg), code(err) {};
    const GLenum code;
};

void checkError(const char *cmd, const char *file, int line);

#if defined(DEBUG)
#define MBGL_CHECK_ERROR(cmd) ([&]() { struct _ { inline ~_() { ::mbgl::gl::checkError(#cmd, __FILE__, __LINE__); } } _; return cmd; }())
#else
#define MBGL_CHECK_ERROR(cmd) (cmd)
#endif

// GL_KHR_debug / GL_ARB_debug_output
#define GL_DEBUG_OUTPUT_SYNCHRONOUS       0x8242
#define GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH 0x8243
#define GL_DEBUG_CALLBACK_FUNCTION        0x8244
#define GL_DEBUG_CALLBACK_USER_PARAM      0x8245
#define GL_DEBUG_SOURCE_API               0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM     0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER   0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY       0x8249
#define GL_DEBUG_SOURCE_APPLICATION       0x824A
#define GL_DEBUG_SOURCE_OTHER             0x824B
#define GL_DEBUG_TYPE_ERROR               0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR  0x824E
#define GL_DEBUG_TYPE_PORTABILITY         0x824F
#define GL_DEBUG_TYPE_PERFORMANCE         0x8250
#define GL_DEBUG_TYPE_OTHER               0x8251
#define GL_MAX_DEBUG_MESSAGE_LENGTH       0x9143
#define GL_MAX_DEBUG_LOGGED_MESSAGES      0x9144
#define GL_DEBUG_LOGGED_MESSAGES          0x9145
#define GL_DEBUG_SEVERITY_HIGH            0x9146
#define GL_DEBUG_SEVERITY_MEDIUM          0x9147
#define GL_DEBUG_SEVERITY_LOW             0x9148
#define GL_DEBUG_TYPE_MARKER              0x8268
#define GL_DEBUG_TYPE_PUSH_GROUP          0x8269
#define GL_DEBUG_TYPE_POP_GROUP           0x826A
#define GL_DEBUG_SEVERITY_NOTIFICATION    0x826B
#define GL_MAX_DEBUG_GROUP_STACK_DEPTH    0x826C
#define GL_DEBUG_GROUP_STACK_DEPTH        0x826D
#define GL_BUFFER                         0x82E0
#define GL_SHADER                         0x82E1
#define GL_PROGRAM                        0x82E2
#define GL_QUERY                          0x82E3
#define GL_PROGRAM_PIPELINE               0x82E4
#define GL_SAMPLER                        0x82E6
#define GL_MAX_LABEL_LENGTH               0x82E8
#define GL_DEBUG_OUTPUT                   0x92E0
#define GL_CONTEXT_FLAG_DEBUG_BIT         0x00000002
#define GL_DISPLAY_LIST                   0x82E7
#define GL_VERTEX_ARRAY                   0x8074
#define GL_TRANSFORM_FEEDBACK             0x8E22
#define GL_TEXTURE                        0x1702
#define GL_RENDERBUFFER                   0x8D41
#define GL_FRAMEBUFFER                    0x8D40
typedef void (* GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
typedef void (* PFNGLDEBUGMESSAGECONTROLPROC) (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
typedef void (* PFNGLDEBUGMESSAGEINSERTPROC) (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf);
typedef void (* PFNGLDEBUGMESSAGECALLBACKPROC) (GLDEBUGPROC callback, const void *userParam);
typedef GLuint (* PFNGLGETDEBUGMESSAGELOGPROC) (GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);
typedef void (* PFNGLGETPOINTERVPROC) (GLenum pname, void **params);
typedef void (* PFNGLPUSHDEBUGGROUPPROC) (GLenum source, GLuint id, GLsizei length, const GLchar *message);
typedef void (* PFNGLPOPDEBUGGROUPPROC) (void);
typedef void (* PFNGLOBJECTLABELPROC) (GLenum identifier, GLuint name, GLsizei length, const GLchar *label);
typedef void (* PFNGLGETOBJECTLABELPROC) (GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label);
typedef void (* PFNGLOBJECTPTRLABELPROC) (const void *ptr, GLsizei length, const GLchar *label);
typedef void (* PFNGLGETOBJECTPTRLABELPROC) (const void *ptr, GLsizei bufSize, GLsizei *length, GLchar *label);
extern PFNGLDEBUGMESSAGECONTROLPROC DebugMessageControl;
extern PFNGLDEBUGMESSAGEINSERTPROC DebugMessageInsert;
extern PFNGLDEBUGMESSAGECALLBACKPROC DebugMessageCallback;
extern PFNGLGETDEBUGMESSAGELOGPROC GetDebugMessageLog;
extern PFNGLGETPOINTERVPROC GetPointerv;
extern PFNGLPUSHDEBUGGROUPPROC PushDebugGroup;
extern PFNGLPOPDEBUGGROUPPROC PopDebugGroup;
extern PFNGLOBJECTLABELPROC ObjectLabel;
extern PFNGLGETOBJECTLABELPROC GetObjectLabel;
extern PFNGLOBJECTPTRLABELPROC ObjectPtrLabel;
extern PFNGLGETOBJECTPTRLABELPROC GetObjectPtrLabel;
void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

// GL_EXT_debug_marker
typedef void (* PFNGLINSERTEVENTMARKEREXTPROC) (GLsizei length, const GLchar *marker);
typedef void (* PFNGLPUSHGROUPMARKEREXTPROC) (GLsizei length, const GLchar *marker);
typedef void (* PFNGLPOPGROUPMARKEREXTPROC) (void);
extern PFNGLINSERTEVENTMARKEREXTPROC InsertEventMarkerEXT;
extern PFNGLPUSHGROUPMARKEREXTPROC PushGroupMarkerEXT;
extern PFNGLPOPGROUPMARKEREXTPROC PopGroupMarkerEXT;

// GL_EXT_debug_label
#define GL_PROGRAM_PIPELINE_OBJECT_EXT    0x8A4F
#define GL_PROGRAM_OBJECT_EXT             0x8B40
#define GL_SHADER_OBJECT_EXT              0x8B48
#define GL_BUFFER_OBJECT_EXT              0x9151
#define GL_QUERY_OBJECT_EXT               0x9153
#define GL_VERTEX_ARRAY_OBJECT_EXT        0x9154
typedef void (* PFNGLLABELOBJECTEXTPROC) (GLenum type, GLuint object, GLsizei length, const GLchar *label);
typedef void (* PFNGLGETOBJECTLABELEXTPROC) (GLenum type, GLuint object, GLsizei bufSize, GLsizei *length, GLchar *label);
extern PFNGLLABELOBJECTEXTPROC LabelObjectEXT;
extern PFNGLGETOBJECTLABELEXTPROC GetObjectLabelEXT;

// GL_ARB_vertex_array_object / GL_OES_vertex_array_object
#define GL_VERTEX_ARRAY_BINDING 0x85B5
typedef void (* PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void (* PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint* arrays);
typedef void (* PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint* arrays);
typedef GLboolean (* PFNGLISVERTEXARRAYPROC) (GLuint array);
extern PFNGLBINDVERTEXARRAYPROC BindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC DeleteVertexArrays;
extern PFNGLGENVERTEXARRAYSPROC GenVertexArrays;
extern PFNGLISVERTEXARRAYPROC IsVertexArray;

// GL_EXT_packed_depth_stencil / GL_OES_packed_depth_stencil
extern bool isPackedDepthStencilSupported;
#define GL_DEPTH24_STENCIL8 0x88F0

// GL_OES_depth24
extern bool isDepth24Supported;
#define GL_DEPTH_COMPONENT24 0x81A6

// Debug group markers, useful for debugging on iOS
#if defined(DEBUG)
// static int indent = 0;
inline void start_group(const std::string &str) {
    if (gl::PushDebugGroup != nullptr) {
        MBGL_CHECK_ERROR(gl::PushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, GLsizei(str.size()), str.c_str()));
    } else if (gl::PushGroupMarkerEXT != nullptr) {
        MBGL_CHECK_ERROR(gl::PushGroupMarkerEXT(GLsizei(str.size() + 1), str.c_str()));
    }
    // fprintf(stderr, "%s%s\n", std::string(indent * 4, ' ').c_str(), str.c_str());
    // indent++;
}

inline void end_group() {
    if (gl::PopDebugGroup != nullptr) {
        MBGL_CHECK_ERROR(gl::PopDebugGroup());
    } else if (gl::PopGroupMarkerEXT != nullptr) {
        MBGL_CHECK_ERROR(gl::PopGroupMarkerEXT());
    }
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

#endif
