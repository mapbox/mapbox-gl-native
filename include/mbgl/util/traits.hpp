#ifndef MBGL_UTIL_TRAITS
#define MBGL_UTIL_TRAITS

#include <type_traits>

namespace mbgl {

template<typename T>
constexpr auto underlying_type(T t) -> typename std::underlying_type<T>::type {
    return static_cast<typename std::underlying_type<T>::type>(t);
}

} // namespace mbgl

#endif // MBGL_UTIL_TRAITS
