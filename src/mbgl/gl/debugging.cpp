#include <mbgl/gl/debugging.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/gl/extension.hpp>
#include <mbgl/util/event.hpp>
#include <mbgl/util/logging.hpp>

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

namespace mbgl {
namespace gl {
namespace debugging {

typedef void (*GLDEBUGPROC)(GLenum source,
                            GLenum type,
                            GLuint id,
                            GLenum severity,
                            GLsizei length,
                            const GLchar *message,
                            const void *userParam);

static ExtensionFunction<
    void (GLenum source,
          GLenum type,
          GLenum severity,
          GLsizei count,
          const GLuint *ids,
          GLboolean enabled)>
    DebugMessageControl({
        {"GL_KHR_debug",        "glDebugMessageControl"},
        {"GL_ARB_debug_output", "glDebugMessageControlARB"}
    });

static ExtensionFunction<
    void (GLDEBUGPROC callback,
          const void *userParam)>
    DebugMessageCallback({
        {"GL_KHR_debug",        "glDebugMessageCallback"},
        {"GL_ARB_debug_output", "glDebugMessageCallbackARB"}
    });

static ExtensionFunction<
    void (GLenum source,
          GLuint id,
          GLsizei length,
          const GLchar *message)>
    PushDebugGroup({
        {"GL_KHR_debug", "glPushDebugGroup"}
    });

static ExtensionFunction<
    void ()>
    PopDebugGroup({
        {"GL_KHR_debug", "glPopDebugGroup"}
    });

static ExtensionFunction<
    void (GLsizei length,
          const GLchar *marker)>
    PushGroupMarkerEXT({
      {"GL_EXT_debug_marker", "glPushGroupMarkerEXT"}
    });

static ExtensionFunction<
    void ()>
    PopGroupMarkerEXT({
      {"GL_EXT_debug_marker", "glPopGroupMarkerEXT"}
    });

void debugCallback(GLenum source,
                   GLenum type,
                   GLuint id,
                   GLenum severity,
                   GLsizei /*length*/,
                   const GLchar* message,
                   const void* /*userParam*/)
{
    std::string strSource;
    switch (source) {
        case GL_DEBUG_SOURCE_API: strSource = "DEBUG_SOURCE_API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: strSource = "DEBUG_SOURCE_WINDOW_SYSTEM"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: strSource = "DEBUG_SOURCE_SHADER_COMPILER"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: strSource = "DEBUG_SOURCE_THIRD_PARTY"; break;
        case GL_DEBUG_SOURCE_APPLICATION: strSource = "DEBUG_SOURCE_APPLICATION"; break;
        case GL_DEBUG_SOURCE_OTHER: strSource = "DEBUG_SOURCE_OTHER"; break;
        default: strSource = "(unknown)"; break;
    }

    std::string strType;
    switch (type) {
        case GL_DEBUG_TYPE_ERROR: strType = "DEBUG_TYPE_ERROR"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: strType = "DEBUG_TYPE_DEPRECATED_BEHAVIOR"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: strType = "DEBUG_TYPE_UNDEFINED_BEHAVIOR"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: strType = "DEBUG_TYPE_PERFORMANCE"; break;
        case GL_DEBUG_TYPE_PORTABILITY: strType = "DEBUG_TYPE_PORTABILITY"; break;
        case GL_DEBUG_TYPE_OTHER: strType = "DEBUG_TYPE_OTHER"; break;
        case GL_DEBUG_TYPE_MARKER: strType = "DEBUG_TYPE_MARKER"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP: strType = "DEBUG_TYPE_OTHER"; break;
        case GL_DEBUG_TYPE_POP_GROUP: strType = "DEBUG_TYPE_POP_GROUP"; break;
        default: strSource = "(unknown)"; break;
    }

    std::string strSeverity;
    mbgl::EventSeverity evtSeverity;
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: strSeverity = "DEBUG_SEVERITY_HIGH"; evtSeverity = mbgl::EventSeverity::Error; break;
        case GL_DEBUG_SEVERITY_MEDIUM: strSeverity = "DEBUG_SEVERITY_MEDIUM"; evtSeverity = mbgl::EventSeverity::Warning; break;
        case GL_DEBUG_SEVERITY_LOW: strSeverity = "DEBUG_SEVERITY_LOW"; evtSeverity = mbgl::EventSeverity::Info; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: strSeverity = "DEBUG_SEVERITY_NOTIFICATION"; evtSeverity = mbgl::EventSeverity::Debug; break;
        default: strSource = "(unknown)"; evtSeverity = mbgl::EventSeverity::Debug; break;
    }

    mbgl::Log::Record(evtSeverity, mbgl::Event::OpenGL, "GL_%s GL_%s %u GL_%s - %s", strSource.c_str(), strType.c_str(), id, strSeverity.c_str(), message);
}

void enable() {
    if (!DebugMessageControl || !DebugMessageCallback) {
        return;
    }

    // This will enable all messages including performance hints
    //MBGL_CHECK_ERROR(DebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE));

    // This will only enable high and medium severity messages
    MBGL_CHECK_ERROR(DebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE));
    MBGL_CHECK_ERROR(DebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, GL_TRUE));
    MBGL_CHECK_ERROR(DebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE));

    MBGL_CHECK_ERROR(DebugMessageCallback(debugCallback, nullptr));
}

group::group(const std::string& str) {
    if (PushDebugGroup) {
        PushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, GLsizei(str.size()), str.c_str());
    } else if (PushGroupMarkerEXT) {
        PushGroupMarkerEXT(GLsizei(str.size() + 1), str.c_str());
    }
}

group::~group() {
    if (PopDebugGroup) {
        PopDebugGroup();
    } else if (PopGroupMarkerEXT) {
        PopGroupMarkerEXT();
    }
}

} // namespace debugging
} // namespace gl
} // namespace mbgl
