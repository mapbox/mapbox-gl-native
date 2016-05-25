#pragma once

#include <string>
#include <cassert>
#include <exception>

#include <mbgl/util/dtoa.hpp>

namespace mbgl {
namespace util {

template <class T>
inline std::string toString(T t) {
    return std::to_string(t);
}

inline std::string toString(int8_t num) {
    return std::to_string(int(num));
}

inline std::string toString(uint8_t num) {
    return std::to_string(unsigned(num));
}

inline std::string toString(float num) {
    return dtoa(num);
}

inline std::string toString(double num) {
    return dtoa(num);
}

inline std::string toString(long double num) {
    return dtoa(num);
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
