#pragma once

#include <mbgl/math/minmax.hpp>

namespace mbgl {
namespace util {

template <typename T>
T clamp(T value, T min_, T max_) {
    return max(min_, min(max_, value));
}

} // namespace util
} // namespace mbgl
