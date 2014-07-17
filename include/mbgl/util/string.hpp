#ifndef MBGL_UTIL_STRING
#define MBGL_UTIL_STRING

#include <string>

namespace mbgl {
namespace util {

template<size_t max, typename... Args>
inline std::string sprintf(const char *msg, Args... args) {
    char res[max];
    int len = snprintf(res, sizeof(res), msg, args...);
    return std::string(res, len);
}

template<size_t max, typename... Args>
inline std::string sprintf(const std::string &msg, Args... args) {
    return sprintf<max>(msg.c_str(), args...);
}

}
}

#endif
