#pragma once

#include <cmath>

namespace mbgl {
namespace util {

// Constrains n to the given range (including min, excluding max) via modular
// arithmetic.
template <typename T>
T wrap(T value, T min, T max) {
    if (value >= min && value < max) {
        return value;
    } else if (value == max) {
        return min;
    }

    const T delta = max - min;
    const T wrapped = min + std::fmod(value - min, delta);
    return value < min ? wrapped + delta : wrapped;
}

} // namespace util
} // namespace mbgl
