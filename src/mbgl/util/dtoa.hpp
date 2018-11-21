#pragma once

#include <string>
#include <cstdlib>

namespace mbgl {
namespace util {

std::string dtoa(double value, bool decimal = false);

inline double stod(const std::string& str) {
    return ::strtod(str.c_str(), nullptr);
}

} // end namespace util
} // end namespace mbgl
