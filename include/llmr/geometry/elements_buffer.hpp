#ifndef LLMR_GEOMETRY_TRIANGLE_ELEMENTS_BUFFER
#define LLMR_GEOMETRY_TRIANGLE_ELEMENTS_BUFFER

#include "buffer.hpp"

namespace llmr {

class TriangleElementsBuffer : public Buffer<
    6, // bytes per triangle (3 * unsigned short == 6 bytes)
    GL_ELEMENT_ARRAY_BUFFER
> {
public:
    typedef uint16_t element_type;

    void add(element_type a, element_type b, element_type c);
};

class PointElementsBuffer : public Buffer<
    2, // bytes per point (1 unsigned short)
    GL_ELEMENT_ARRAY_BUFFER
> {
public:
    typedef uint16_t element_type;

    void add(element_type a);
};

}

#endif
