#include "dtoa.hpp"

// Clang/C2 on Windows 64-bits can't parse rapidjson's dtoa
// and it was causing the compiler to crash.
#if !defined(_WINDOWS)
#include <rapidjson/internal/dtoa.h>
#endif

#include <mbgl/util/string.hpp>

namespace mbgl {
namespace util {

#if !defined(_WINDOWS)

std::string dtoa(double value, bool decimal) {
    std::string data;
    data.resize(25);
    auto end = rapidjson::internal::dtoa(value, const_cast<char*>(data.data()));
    auto length = end - data.data();
    if (!decimal && length >= 3 && end[-1] == '0' && end[-2] == '.') {
        // Remove trailing ".0" for integers
        length -= 2;
    }
    data.resize(length);
    return data;
}

#else

std::string dtoa(double value) {
    return std::to_string(value);
}

#endif

} // namespace util
} // namespace mbgl
