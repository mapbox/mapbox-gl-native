#pragma once

#include <cmath>

namespace mbgl {
namespace util {

// Constrains n to the given range (including min, excluding max) via modular
// arithmetic.
template <typename T>
T wrap(T value, T min, T max) {
    T d = max - min;
    return std::fmod((std::fmod((value - min), d) + d), d) + min;
}

} // namespace util
} // namespace mbgl
