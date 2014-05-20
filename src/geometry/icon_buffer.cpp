#include <llmr/geometry/icon_buffer.hpp>
#include <llmr/platform/gl.hpp>

#include <cmath>

using namespace llmr;

void IconVertexBuffer::add(vertex_type x, vertex_type y, uint16_t tx, uint16_t ty) {
    void *data = addElement();

    vertex_type *vertices = static_cast<vertex_type *>(data);
    vertices[0] = x;
    vertices[1] = y;

    uint16_t *texture = static_cast<uint16_t *>(data);
    texture[2] = tx;
    texture[3] = ty;
}
