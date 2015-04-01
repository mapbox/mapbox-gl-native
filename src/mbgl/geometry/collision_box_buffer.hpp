#ifndef MBGL_GEOMETRY_COLLISIONBOX_BUFFER
#define MBGL_GEOMETRY_COLLISIONBOX_BUFFER

#include <mbgl/geometry/buffer.hpp>
#include <array>

namespace mbgl {

class CollisionBoxVertexBuffer : public Buffer <
    12,
    GL_ARRAY_BUFFER,
    32768
> {
public:
    typedef int16_t vertex_type;

    size_t add(int16_t x, int16_t y, float ex, float ey, float maxzoom, float placementZoom);
};


}

#endif
