#include <mbgl/geometry/fill_buffer.hpp>

#include <mbgl/platform/gl.hpp>

#include <climits>

using namespace mbgl;

void FillVertexBuffer::add(vertex_type x, vertex_type y) {
    vertex_type *vertices = static_cast<vertex_type *>(addElement());
    vertices[0] = x;
    vertices[1] = y;
}
