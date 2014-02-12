#ifndef LLMR_GEOMETRY_TRIANGLE_ELEMENTS_BUFFER
#define LLMR_GEOMETRY_TRIANGLE_ELEMENTS_BUFFER

#include <llmr/geometry/buffer.hpp>

namespace llmr {

template <typename Shader>
class VertexArrayObject;

template <typename Shader>
struct ElementGroup {
    VertexArrayObject<Shader> array;
    uint32_t vertex_length;
    uint32_t elements_length;

    ElementGroup() : vertex_length(0), elements_length(0) {}
    ElementGroup(uint32_t vertex_length, uint32_t elements_length)
        : vertex_length(vertex_length),
          elements_length(elements_length) {
    }
};

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
