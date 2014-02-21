#include <llmr/geometry/point_buffer.hpp>
#include <llmr/platform/gl.hpp>

#include <cmath>

using namespace llmr;

void PointVertexBuffer::add(vertex_type x, vertex_type y) {
    vertex_type *vertices = static_cast<vertex_type *>(addElement());
    vertices[0] = x;
    vertices[1] = y;
}
