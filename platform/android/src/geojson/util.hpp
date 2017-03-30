#pragma once

#include <type_traits>

namespace mbgl {
namespace android {
namespace geojson {

// Clang 3.8 fails to implicitly convert matching types, so we'll have to do it explicitly.
template <typename To, typename From>
To convertExplicit(From&& src) {
    static_assert(std::is_same<typename std::decay_t<From>::container_type,
                               typename To::container_type>::value,
                  "container types do not match");
    static_assert(std::is_rvalue_reference<From&&>::value,
                  "argument must be rvalue reference");
    return *reinterpret_cast<std::add_pointer_t<To>>(&src);
}

} // namespace geojson
} // namespace android
} // namespace mbgl
