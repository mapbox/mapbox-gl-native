#include <mbgl/shader/raster_vertex.hpp>
#include <mbgl/gl/shader.hpp>
#include <mbgl/gl/gl.hpp>

namespace mbgl {

void RasterVertex::bind(const int8_t* offset) {
    static_assert(sizeof(RasterVertex) == 8, "expected RasterVertex size");

    MBGL_BIND_VERTEX_ATTRIBUTE(RasterVertex, a_pos, offset);
    MBGL_BIND_VERTEX_ATTRIBUTE(RasterVertex, a_texture_pos, offset);
}

} // namespace mbgl
