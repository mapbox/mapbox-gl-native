#pragma once

#include <mapbox/variant.hpp>

namespace mbgl {

template <typename... T>
using variant = mapbox::util::variant<T...>;

} // namespace mbgl
