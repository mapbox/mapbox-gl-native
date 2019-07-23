#include <mbgl/gl/debugging_extension.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace gl {
namespace extension {

void Debugging::DebugCallback(GLenum source,
                              GLenum type,
                              GLuint id,
                              GLenum severity,
                              GLsizei /* length */,
                              const GLchar* message,
                              const void* /* userParam */) {
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

} // namespace extension
} // namespace gl
} // namespace mbgl
