#pragma once

#include <mbgl/gl/vao.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <array>

namespace mbgl {
namespace gl {

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

} // namespace gl
} // namespace mbgl
