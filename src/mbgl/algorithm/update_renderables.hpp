#pragma once

#include <cstdint>

namespace mbgl {
namespace algorithm {

template <typename Layer>
void updateRenderables(Layer& layer, const uint8_t z);

} // namespace algorithm
} // namespace mbgl
