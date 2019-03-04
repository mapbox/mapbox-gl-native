#pragma once

#include <cmath>
#include <limits>
#include <type_traits>

namespace mbgl {
namespace util {

// https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
template <typename T, typename = std::enable_if<std::is_floating_point<T>::value>>
T floor(T a) {
    return ::fabs(a) <= std::numeric_limits<T>::epsilon() ? 0 : ::floor(a);
}

} // namespace util
} // namespace mbgl
