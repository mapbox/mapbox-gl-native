#ifndef MBGL_GEOMETRY_TEXT_BUFFER
#define MBGL_GEOMETRY_TEXT_BUFFER

#include <mbgl/geometry/buffer.hpp>
#include <array>

namespace mbgl {

class TextVertexBuffer : public Buffer <
    16,
    GL_ARRAY_BUFFER,
    32768
> {
public:
    typedef int16_t vertex_type;

    static const double angleFactor;

    size_t add(int16_t x, int16_t y, float ox, float oy, uint16_t tx, uint16_t ty, float angle, float minzoom, std::array<float, 2> range, float maxzoom, float labelminzoom);
};


}

#endif
