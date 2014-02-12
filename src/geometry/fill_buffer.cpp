#include <llmr/geometry/fill_buffer.hpp>
#include <llmr/platform/gl.hpp>

#include <climits>

using namespace llmr;

void FillVertexBuffer::addDegenerate() {
    vertex_type *vertices = static_cast<vertex_type *>(addElement());
    vertices[0] = 32767;
    vertices[1] = 0;
}

void FillVertexBuffer::add(vertex_type x, vertex_type y) {
    vertex_type *vertices = static_cast<vertex_type *>(addElement());
    vertices[0] = x;
    vertices[1] = y;
}
