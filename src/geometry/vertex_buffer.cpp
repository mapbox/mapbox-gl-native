#include <mbgl/geometry/vertex_buffer.hpp>
#include <mbgl/platform/gl.hpp>

using namespace mbgl;


VertexBuffer::VertexBuffer(std::initializer_list<int16_t> init) : array(init) {}

VertexBuffer::~VertexBuffer() {
    if (buffer != 0) {
        glDeleteBuffers(1, &buffer);
    }
}

size_t VertexBuffer::index() const {
    // We store 2 coordinates per vertex + 1 linesofar + 1 extrude coord pair == 4 (== 8 bytes)
    return array.size() / 2;
}

void VertexBuffer::bind() {
    if (buffer == 0) {
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, array.size() * sizeof(vertex_type), array.data(), GL_STATIC_DRAW);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
    }
}
