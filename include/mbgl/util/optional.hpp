#pragma once

#include <experimental/optional>

namespace mbgl {

template <typename T>
using optional = std::experimental::optional<T>;

using nullopt_t = std::experimental::nullopt_t;
constexpr auto nullopt = std::experimental::nullopt;

} // namespace mbgl
