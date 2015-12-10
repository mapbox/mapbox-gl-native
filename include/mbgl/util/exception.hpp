#ifndef MBGL_UTIL_EXCEPTION
#define MBGL_UTIL_EXCEPTION

#include <stdexcept>

namespace mbgl {
namespace util {

struct Exception : std::runtime_error {
    inline Exception(const char *msg) : std::runtime_error(msg) {}
    inline Exception(const std::string &msg) : std::runtime_error(msg) {}
};

struct SpriteImageException final : Exception {
    inline SpriteImageException(const char *msg) : Exception(msg) {}
    inline SpriteImageException(const std::string &msg) : Exception(msg) {}
};

struct GlyphRangeLoadingException final : Exception {
    inline GlyphRangeLoadingException(const char *msg) : Exception(msg) {}
    inline GlyphRangeLoadingException(const std::string &msg) : Exception(msg) {}
};

struct MisuseException final : Exception {
    inline MisuseException(const char *msg) : Exception(msg) {}
    inline MisuseException(const std::string &msg) : Exception(msg) {}
};

struct ShaderException final : Exception {
    inline ShaderException(const char *msg) : Exception(msg) {}
    inline ShaderException(const std::string &msg) : Exception(msg) {}
};

struct SourceLoadingException final : Exception {
    inline SourceLoadingException(const char *msg) : Exception(msg) {}
    inline SourceLoadingException(const std::string &msg) : Exception(msg) {}
};

struct SpriteLoadingException final : Exception {
    inline SpriteLoadingException(const char *msg) : Exception(msg) {}
    inline SpriteLoadingException(const std::string &msg) : Exception(msg) {}
};

struct TileLoadingException final : Exception {
    inline TileLoadingException(const char *msg) : Exception(msg) {}
    inline TileLoadingException(const std::string &msg) : Exception(msg) {}
};

} // namespace util
} // namespace mbgl

#endif
