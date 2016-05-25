#pragma once

#include <experimental/optional>

namespace mbgl {

template <typename T>
using optional = std::experimental::optional<T>;

} // namespace mbgl
