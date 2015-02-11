#ifndef MBGL_UTIL_EXCEPTION
#define MBGL_UTIL_EXCEPTION

#include <stdexcept>

namespace mbgl {
namespace util {

struct Exception : std::runtime_error {
    inline Exception(const char *msg) : std::runtime_error(msg) {}
};

struct MisuseException : Exception {
    inline MisuseException(const char *msg) : Exception(msg) {}
};

}
}

#endif
