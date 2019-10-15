#include <mbgl/gfx/rendering_stats.hpp>

namespace mbgl {
namespace gfx {

bool RenderingStats::isZero() const {
    return numActiveTextures == 0 && numCreatedTextures == 0 && numBuffers == 0 && numFrameBuffers == 0 &&
           memTextures == 0 && memIndexBuffers == 0 && memVertexBuffers == 0;
}

} // namespace gfx
} // namespace mbgl