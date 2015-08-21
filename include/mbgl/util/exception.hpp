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

struct GlyphRangeLoadingException : Exception {
    inline GlyphRangeLoadingException(const char *msg) : Exception(msg) {}
    inline GlyphRangeLoadingException(const std::string &msg) : Exception(msg) {}
};

struct MisuseException : Exception {
    inline MisuseException(const char *msg) : Exception(msg) {}
    inline MisuseException(const std::string &msg) : Exception(msg) {}
};

struct ShaderException : Exception {
    inline ShaderException(const char *msg) : Exception(msg) {}
    inline ShaderException(const std::string &msg) : Exception(msg) {}
};

struct SourceLoadingException : Exception {
    inline SourceLoadingException(const char *msg) : Exception(msg) {}
    inline SourceLoadingException(const std::string &msg) : Exception(msg) {}
};

struct SpriteLoadingException : Exception {
    inline SpriteLoadingException(const char *msg) : Exception(msg) {}
    inline SpriteLoadingException(const std::string &msg) : Exception(msg) {}
};

struct TileLoadingException : Exception {
    inline TileLoadingException(const char *msg) : Exception(msg) {}
    inline TileLoadingException(const std::string &msg) : Exception(msg) {}
};

}
}

#endif
