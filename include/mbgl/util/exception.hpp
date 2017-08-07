#pragma once

#include <stdexcept>

namespace mbgl {
namespace util {

struct Exception : std::runtime_error {
    Exception(const char *msg) : std::runtime_error(msg) {}
    Exception(const std::string &msg) : std::runtime_error(msg) {}
};

struct SpriteImageException : Exception {
    SpriteImageException(const char *msg) : Exception(msg) {}
    SpriteImageException(const std::string &msg) : Exception(msg) {}
};

struct MisuseException : Exception {
    MisuseException(const char *msg) : Exception(msg) {}
    MisuseException(const std::string &msg) : Exception(msg) {}
};

struct StyleParseException : Exception {
    StyleParseException(const char *msg) : Exception(msg) {}
    StyleParseException(const std::string &msg) : Exception(msg) {}
};

struct StyleLoadException : Exception {
    StyleLoadException(const char *msg) : Exception(msg) {}
    StyleLoadException(const std::string &msg) : Exception(msg) {}
};

struct NotFoundException : Exception {
    NotFoundException(const char *msg) : Exception(msg) {}
    NotFoundException(const std::string &msg) : Exception(msg) {}
};

} // namespace util
} // namespace mbgl
