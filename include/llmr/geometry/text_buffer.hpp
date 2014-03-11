#ifndef LLMR_GEOMETRY_TEXT_BUFFER
#define LLMR_GEOMETRY_TEXT_BUFFER

#include "buffer.hpp"

namespace llmr {

class GlyphPosition {
public:
    int16_t x = 0, y = 0;
    float ox = 0.0f, oy = 0.0f;
    uint16_t tx = 0, ty = 0;
    float angle = 0.0f;
    float minzoom = 0.0f;
    float rangefrom = 0.0f, rangeto = 0.0f;
    float maxzoom = 25.0f;
    float labelminzoom = 0.0f;
};

class TextVertexBuffer : public Buffer <
    16,
    GL_ARRAY_BUFFER,
    32768
> {
public:
    typedef int16_t vertex_type;

    static const double angleFactor;

    size_t add(const GlyphPosition& pos);
};


}

#endif
