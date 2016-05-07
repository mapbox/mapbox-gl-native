#ifndef MBGL_UTIL_STRING
#define MBGL_UTIL_STRING

#include <string>
#include <cassert>
#include <exception>
#include <cmath>

namespace mbgl {
namespace util {

template <class T>
inline std::string toString(T t) {
    return std::to_string(t);
}

inline std::string toString(double num) {
    if (std::fmod(num, 1)) {
        std::string str = std::to_string(num);
        str.erase(str.find_last_not_of('0') + 1, std::string::npos);
        if (str.back() == '.') {
            str.pop_back();
        }
        return str;
    } else {
        return std::to_string(int(num));
    }
}

inline std::string toString(int8_t num) {
    return std::to_string(int(num));
}

inline std::string toString(uint8_t num) {
    return std::to_string(unsigned(num));
}

inline std::string toString(std::exception_ptr error) {
    assert(error);

    if (!error) {
        return "(null)";
    }

    try {
        std::rethrow_exception(error);
    } catch (const std::exception& ex) {
        return ex.what();
    } catch (...) {
        return "Unknown exception type";
    }
}

} // namespace util
} // namespace mbgl

#endif
