#pragma once

#include <sstream>
#include <string>
#include <cassert>
#include <cstdlib>
#include <exception>

// Polyfill needed by Qt when building for Android with GCC
#if defined(__ANDROID__) && defined(__GLIBCXX__)

namespace std {

template <typename T>
std::string to_string(T value)
{
    std::ostringstream oss;
    oss << value;

    return oss.str();
}

inline int stoi(const std::string &str)
{
    return atoi(str.c_str());
}

inline float stof(const std::string &str) {
    return static_cast<float>(atof(str.c_str()));
}

} // namespace std

#endif

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

std::string toString(float);
std::string toString(double);
std::string toString(long double);

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

inline float stof(const std::string& str) {
    return std::stof(str);
}

} // namespace util
} // namespace mbgl
