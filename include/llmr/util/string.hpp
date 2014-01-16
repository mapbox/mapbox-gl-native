#ifndef LLMR_UTIL_STRING
#define LLMR_UTIL_STRING

#include <string>

namespace llmr {
namespace util {

template<typename... Args>
inline std::string sprintf(const char *msg, Args... args) {
    char res[1024];
    int len = snprintf(res, sizeof(res), msg, args...);
    return std::string(res, len);
}

}
}

#endif
