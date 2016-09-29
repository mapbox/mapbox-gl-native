#pragma once

#include <mbgl/geometry/buffer.hpp>
#include <array>

namespace mbgl {

class CollisionBoxVertexBuffer : public Buffer <
    12,
    gl::BufferType::Vertex,
    32768
> {
public:
    typedef int16_t vertex_type;

    size_t add(int16_t x, int16_t y, float ex, float ey, float maxzoom, float placementZoom);
};


} // namespace mbgl
