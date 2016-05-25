#pragma once

#include <mbgl/geometry/buffer.hpp>

namespace mbgl {

class CircleVertexBuffer : public Buffer<
    4 // 2 bytes per short * 4 of them.
> {
public:
    typedef int16_t vertex_type;

    /*
     * Add a vertex to this buffer
     *
     * @param {number} x vertex position
     * @param {number} y vertex position
     * @param {number} ex extrude normal
     * @param {number} ey extrude normal
     */
    void add(vertex_type x, vertex_type y, float ex, float ey);
};

} // namespace mbgl
