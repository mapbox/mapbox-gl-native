#pragma once

#include <optional.hpp>

namespace mbgl {

template <typename T>
using optional = std::experimental::optional<T>;

using nullopt_t = std::experimental::nullopt_t;
constexpr nullopt_t nullopt = std::experimental::nullopt;

} // namespace mbgl
