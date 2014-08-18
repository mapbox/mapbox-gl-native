#ifndef MBGL_GEOMETRY_STATIC_VERTEX_BUFFER
#define MBGL_GEOMETRY_STATIC_VERTEX_BUFFER

#include <mbgl/geometry/buffer.hpp>

#include <vector>
#include <cstddef>
#include <cstdint>
#include <cmath>

namespace mbgl {

class StaticVertexBuffer : public Buffer<
    4, // bytes per vertex (2 * signed short == 4 bytes)
    GL_ARRAY_BUFFER,
    32 // default length
> {
public:
    typedef int16_t vertex_type;

    StaticVertexBuffer(std::initializer_list<std::pair<int16_t, int16_t>> init);
};

}

#endif
