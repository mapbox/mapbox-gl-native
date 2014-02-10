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

void FillElementsBuffer::add(element_type a, element_type b, element_type c) {
    element_type *elements = static_cast<element_type *>(addElement());
    elements[0] = a;
    elements[1] = b;
    elements[2] = c;
}
