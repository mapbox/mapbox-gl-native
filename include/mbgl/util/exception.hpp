#ifndef MBGL_UTIL_EXCEPTION
#define MBGL_UTIL_EXCEPTION

#include <stdexcept>

namespace mbgl {
namespace util {

struct Exception : std::runtime_error {
    inline Exception(const char *msg) : std::runtime_error(msg) {}
    inline Exception(const std::string &msg) : std::runtime_error(msg) {}
};

struct SpriteImageException : Exception {
    inline SpriteImageException(const char *msg) : Exception(msg) {}
    inline SpriteImageException(const std::string &msg) : Exception(msg) {}
};

struct MisuseException : Exception {
    inline MisuseException(const char *msg) : Exception(msg) {}
    inline MisuseException(const std::string &msg) : Exception(msg) {}
};

struct ShaderException : Exception {
    inline ShaderException(const char *msg) : Exception(msg) {}
    inline ShaderException(const std::string &msg) : Exception(msg) {}
};

} // namespace util
} // namespace mbgl

#endif
