#include <mbgl/platform/gl.hpp>
#include <mbgl/platform/log.hpp>

#include <iostream>


namespace mbgl {
namespace gl {

PFNGLDEBUGMESSAGECONTROLPROC DebugMessageControl;
PFNGLDEBUGMESSAGEINSERTPROC DebugMessageInsert;
PFNGLDEBUGMESSAGECALLBACKPROC DebugMessageCallback;
PFNGLGETDEBUGMESSAGELOGPROC GetDebugMessageLog;
PFNGLGETPOINTERVPROC GetPointerv;
PFNGLPUSHDEBUGGROUPPROC PushDebugGroup;
PFNGLPOPDEBUGGROUPPROC PopDebugGroup;
PFNGLOBJECTLABELPROC ObjectLabel;
PFNGLGETOBJECTLABELPROC GetObjectLabel;
PFNGLOBJECTPTRLABELPROC ObjectPtrLabel;
PFNGLGETOBJECTPTRLABELPROC GetObjectPtrLabel;

void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei, const GLchar *message, const void *) {
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

    mbgl::Log::Record(evtSeverity, mbgl::Event::OpenGL, "GL Debug Message: %s %s %u %s - %s", strSource.c_str(), strType.c_str(), id, strSeverity.c_str(), message);
}

PFNGLBINDVERTEXARRAYPROC BindVertexArray = nullptr;
PFNGLDELETEVERTEXARRAYSPROC DeleteVertexArrays = nullptr;
PFNGLGENVERTEXARRAYSPROC GenVertexArrays = nullptr;
PFNGLISVERTEXARRAYPROC IsVertexArray = nullptr;

}
}

void _CHECK_GL_ERROR(const char *cmd, const char *file, int line) {
    std::cout <<  cmd << ";" << std::endl;

    GLenum err;

    while ((err = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (err) {
            case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
            case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE: error = "INVALID_VALUE"; break;
            case GL_OUT_OF_MEMORY: error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
            default: error = "(unknown)"; break;
        }

        mbgl::Log::Error(mbgl::Event::OpenGL, "GL Error: GL_%s(0x%04X) - %s:%i", error.c_str(), file, line, err);
        exit(1);
    }
}
