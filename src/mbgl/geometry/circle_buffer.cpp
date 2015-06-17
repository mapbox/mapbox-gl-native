#include <mbgl/geometry/circle_buffer.hpp>

#include <mbgl/platform/gl.hpp>

#include <climits>

using namespace mbgl;

void CircleVertexBuffer::add(vertex_type x, vertex_type y, float ex, float ey) {
    vertex_type *vertices = static_cast<vertex_type *>(addElement());
    vertices[0] = (x * 2) + ((ex + 1) / 2);
    vertices[1] = (y * 2) + ((ey + 1) / 2);
}
