#include <llmr/geometry/line_buffer.hpp>
#include <llmr/platform/gl.hpp>

using namespace llmr;

LineBuffer::~LineBuffer() {
    if (buffer != 0) {
        glDeleteBuffers(1, &buffer);
        buffer = 0;
    }
}

uint32_t LineBuffer::length() const {
    // We store 2 coordinates per vertex + 1 linesofar + 1 extrude coord pair == 4 (== 8 bytes)
    return array.size() / 4;
}

void LineBuffer::add(vertex_type x, vertex_type y, float ex, float ey, int8_t tx, int8_t ty, int32_t linesofar) {
    array.push_back((x * 2) | tx);
    array.push_back((y * 2) | ty);
    array.push_back(linesofar);

    int16_t extrude = 0;
    *(((int8_t *)&extrude) + 0) = round(extrudeScale * ex);
    *(((int8_t *)&extrude) + 1) = round(extrudeScale * ey);

    array.push_back(extrude);
    if (!dirty) dirty = true;
};

void LineBuffer::addDegenerate() {
    add(16383, 16383, 0, 0, 1, 1, 0);
}

void LineBuffer::bind() {
    if (buffer == 0) {
        glGenBuffers(1, &buffer);
    }

    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    if (dirty) {
        glBufferData(GL_ARRAY_BUFFER, array.size() * sizeof(vertex_type), array.data(), GL_STATIC_DRAW);
        dirty = false;
    }
}
