#pragma once

#include <mbgl/geometry/buffer.hpp>
#include <mbgl/geometry/vao.hpp>

#include <mbgl/util/noncopyable.hpp>

#include <array>

namespace mbgl {

template <uint8_t count>
struct ElementGroup : public util::noncopyable {
    std::array<VertexArrayObject, count> array;
    uint32_t vertex_length;
    uint32_t elements_length;

    ElementGroup(uint32_t vertex_length_ = 0, uint32_t elements_length_ = 0)
        : vertex_length(vertex_length_)
        , elements_length(elements_length_)
    {
    }
};

class TriangleElementsBuffer : public Buffer<
    6, // bytes per triangle (3 * unsigned short == 6 bytes)
    gl::BufferType::Element
> {
public:
    typedef uint16_t element_type;

    void add(element_type a, element_type b, element_type c);
};


class LineElementsBuffer : public Buffer<
    4, // bytes per triangle (2 * unsigned short == 6 bytes)
    gl::BufferType::Element
> {
public:
    typedef uint16_t element_type;

    void add(element_type a, element_type b);
};

} // namespace mbgl
