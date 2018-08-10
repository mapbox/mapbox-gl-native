#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include <expected.hpp>
#pragma GCC diagnostic pop

namespace mbgl {

template <class T, class E>
using expected = nonstd::expected<T, E>;

template <class E>
using unexpected = nonstd::unexpected_type<E>;

} // namespace mbgl
