#ifndef MBGL_ALGORITHM_UPDATE_RENDERABLES
#define MBGL_ALGORITHM_UPDATE_RENDERABLES

#include <cstdint>

namespace mbgl {
namespace algorithm {

template <typename Layer>
void updateRenderables(Layer& layer, const uint8_t z);

} // namespace algorithm
} // namespace mbgl

#endif
