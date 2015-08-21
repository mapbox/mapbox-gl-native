#ifndef MBGL_GEOMETRY_LINE_BUFFER
#define MBGL_GEOMETRY_LINE_BUFFER

#include <mbgl/geometry/buffer.hpp>

namespace mbgl {

class LineVertexBuffer : public Buffer<
    8 // 2 coordinates per vertex + 1 linesofar + 1 extrude coord pair == 4 (== 8 bytes)
> {
public:
    typedef int16_t vertex_type;

    /*
     * Scale the extrusion vector so that the normal length is this value.
     * Contains the "texture" normals (-1..1). This is distinct from the extrude
     * normals for line joins, because the x-value remains 0 for the texture
     * normal array, while the extrude normal actually moves the vertex to create
     * the acute/bevelled line join.
     */
    static const int8_t extrudeScale = 63;

    /*
     * Add a vertex to this buffer
     *
     * @param {number} x vertex position
     * @param {number} y vertex position
     * @param {number} ex extrude normal
     * @param {number} ey extrude normal
     * @param {number} tx texture normal
     * @param {number} ty texture normal
     */
    size_t add(vertex_type x, vertex_type y, float ex, float ey, int8_t tx, int8_t ty, int32_t linesofar = 0);
};


}

#endif
