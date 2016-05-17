#ifndef MBGL_UTIL_DTOA
#define MBGL_UTIL_DTOA

#include <string>

namespace mbgl {
namespace util {

char* dtoa(double value, char* buffer);

inline std::string dtoa(double value) {
    std::string data;
    data.resize(25);
    auto end = dtoa(value, const_cast<char*>(data.data()));
    data.resize(end - data.data());
    return data;
}

} // end namespace util
} // end namespace mbgl

#endif
