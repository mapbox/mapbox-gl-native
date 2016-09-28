#include <mbgl/gl/gl.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/platform/log.hpp>

#include <cassert>
#include <iostream>
#include <unordered_map>
#include <mutex>

namespace mbgl {
namespace gl {

ExtensionFunction<void (GLuint array)>
    BindVertexArray({
        {"GL_ARB_vertex_array_object", "glBindVertexArray"},
        {"GL_OES_vertex_array_object", "glBindVertexArrayOES"},
        {"GL_APPLE_vertex_array_object", "glBindVertexArrayAPPLE"}
    });

ExtensionFunction<void (GLsizei n, const GLuint* arrays)>
    DeleteVertexArrays({
        {"GL_ARB_vertex_array_object", "glDeleteVertexArrays"},
        {"GL_OES_vertex_array_object", "glDeleteVertexArraysOES"},
        {"GL_APPLE_vertex_array_object", "glDeleteVertexArraysAPPLE"}
    });

ExtensionFunction<void (GLsizei n, GLuint* arrays)>
    GenVertexArrays({
        {"GL_ARB_vertex_array_object", "glGenVertexArrays"},
        {"GL_OES_vertex_array_object", "glGenVertexArraysOES"},
        {"GL_APPLE_vertex_array_object", "glGenVertexArraysAPPLE"}
    });

std::vector<ExtensionFunctionBase*>& ExtensionFunctionBase::functions() {
    static std::vector<ExtensionFunctionBase*> functions;
    return functions;
}

static std::once_flag initializeExtensionsOnce;

void InitializeExtensions(glProc (*getProcAddress)(const char *)) {
    std::call_once(initializeExtensionsOnce, [getProcAddress] {
        const char * extensionsPtr = reinterpret_cast<const char *>(
            MBGL_CHECK_ERROR(glGetString(GL_EXTENSIONS)));

        if (!extensionsPtr)
            return;

        const std::string extensions = extensionsPtr;
        for (auto fn : ExtensionFunctionBase::functions()) {
            for (auto probe : fn->probes) {
                if (extensions.find(probe.first) != std::string::npos) {
                    fn->ptr = getProcAddress(probe.second);
                    break;
                }
            }
        }
    });
}

namespace {

constexpr const char* stringFromError(GLenum err) {
    switch (err) {
    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";

    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";

    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";

    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";

    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";

#ifdef GL_TABLE_TOO_LARGE
    case GL_TABLE_TOO_LARGE:
        return "GL_TABLE_TOO_LARGE";
#endif

#ifdef GL_STACK_OVERFLOW
    case GL_STACK_OVERFLOW:
        return "GL_STACK_OVERFLOW";
#endif

#ifdef GL_STACK_UNDERFLOW
    case GL_STACK_UNDERFLOW:
        return "GL_STACK_UNDERFLOW";
#endif

#ifdef GL_CONTEXT_LOST
    case GL_CONTEXT_LOST:
        return "GL_CONTEXT_LOST";
#endif

    default:
        return "GL_UNKNOWN";
    }
}

} // namespace

void checkError(const char* cmd, const char* file, int line) {
    GLenum err = GL_NO_ERROR;
    if ((err = glGetError()) != GL_NO_ERROR) {
        std::string message = std::string(cmd) + ": Error " + stringFromError(err);

        // Check for further errors
        while ((err = glGetError()) != GL_NO_ERROR) {
            message += ", ";
            message += stringFromError(err);
        }

        throw Error(message + " at " + file + ":" + util::toString(line));
    }
}

} // namespace gl
} // namespace mbgl
