#pragma once

#include <mbgl/geometry/buffer.hpp>
#include <array>

namespace mbgl {

class TextVertexBuffer : public Buffer <
    16,
    gl::BufferType::Vertex,
    32768
> {
public:
    typedef int16_t vertex_type;

    size_t add(int16_t x, int16_t y, float ox, float oy, uint16_t tx, uint16_t ty, float minzoom, float maxzoom, float labelminzoom, uint8_t labelangle);
};


} // namespace mbgl
