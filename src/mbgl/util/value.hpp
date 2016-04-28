#pragma once

#include <mbgl/util/variant.hpp>

namespace mbgl {

typedef variant<bool, int64_t, uint64_t, double, std::string> Value;

} // namespace mbgl
