#pragma once

#include <type_traits>

namespace mbgl {

template<typename T>
constexpr auto underlying_type(T t) -> typename std::underlying_type_t<T> {
    return typename std::underlying_type_t<T>(t);
}

} // namespace mbgl
