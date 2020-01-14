#pragma once

#if __has_include(<optional>) && __cplusplus >= 201703L
#include <optional>

namespace mbgl {

template <typename T>
using optional = std::optional<T>;

using nullopt_t = std::nullopt_t;
constexpr nullopt_t nullopt = std::nullopt;

} // namespace mbgl
#else
#include <optional.hpp>

namespace mbgl {

template <typename T>
using optional = std::experimental::optional<T>;

using nullopt_t = std::experimental::nullopt_t;
constexpr nullopt_t nullopt = std::experimental::nullopt;

} // namespace mbgl
#endif
