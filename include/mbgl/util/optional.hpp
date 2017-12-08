#pragma once

#include <experimental/optional>

namespace mbgl {

template <typename T>
using optional = std::experimental::optional<T>;

static const auto nullopt = std::experimental::nullopt;

} // namespace mbgl
