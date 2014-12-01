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

void debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
    //mbgl::Error(mbgl::Event::OpenGL)
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

        mbgl::Log::Error(mbgl::Event::OpenGL, "GL_%s(0x%04X) - %s:%i", error.c_str(), file, line, err);
        exit(1);
    }
}
