#ifndef MBGL_UTIL_VARIANT
#define MBGL_UTIL_VARIANT

#include <mapbox/variant.hpp>

namespace mbgl {

template <typename... T>
using variant = mapbox::util::variant<T...>;

} // namespace mbgl

#endif
