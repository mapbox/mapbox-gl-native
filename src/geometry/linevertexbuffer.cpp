#include <llmr/geometry/linevertexbuffer.hpp>
#include <llmr/platform/gl.hpp>

#include <cstdio>

using namespace llmr;

linevertexbuffer::linevertexbuffer()
    : buffer(0) {
}

void linevertexbuffer::addDegenerate() {
    array.push_back(32767);
    array.push_back(0);
}

void linevertexbuffer::addCoordinate(int16_t x, int16_t y) {
    array.push_back(x);
    array.push_back(y);
}

uint32_t linevertexbuffer::length() {
    // We store 2 coordinates per vertex
    return array.size() / 2;
}

void linevertexbuffer::bind() {
    if (buffer == 0) {
        glGenBuffers(1, &buffer);
        fprintf(stderr, "new buffer id: %d\n", buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, array.size() * sizeof(uint16_t), array.data(), GL_STATIC_DRAW);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
    }
}
