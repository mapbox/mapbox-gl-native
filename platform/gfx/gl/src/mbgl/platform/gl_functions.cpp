#include <mbgl/platform/gl_functions.hpp>

#include <mbgl/util/logging.hpp>

namespace mbgl {
namespace platform {

#ifndef NDEBUG
void glCheckError(const char* cmd, const char* file, int line) {
    if (GLenum err = glGetError()) {
        Log::Warning(Event::OpenGL, "Error %d: %s - %s:%d", err, cmd, file, line);
    }
}
#endif

}  // namespace platform
}  // namespace mbgl
