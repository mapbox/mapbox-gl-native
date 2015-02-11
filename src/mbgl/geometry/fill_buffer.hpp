#ifndef MBGL_GEOMETRY_FILL_BUFFER
#define MBGL_GEOMETRY_FILL_BUFFER

#include <mbgl/geometry/buffer.hpp>
#include <vector>
#include <cstdint>

namespace mbgl {

class FillVertexBuffer : public Buffer<
    4 // bytes per coordinates (2 * unsigned short == 4 bytes)
> {
public:
    typedef int16_t vertex_type;

    void add(vertex_type x, vertex_type y);
};

}

#endif
