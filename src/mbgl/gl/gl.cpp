#include <mbgl/gl/gl.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/util.hpp>

namespace mbgl {
namespace gl {

namespace {

MBGL_CONSTEXPR const char* stringFromError(GLenum err) {
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
//    fprintf(stderr, "cmd: %s\n", cmd);
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
