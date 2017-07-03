#pragma once

#include <mbgl/gl/context.hpp>
#include <mbgl/gl/vertex_array.hpp>
#include <mbgl/util/optional.hpp>

#include <cstddef>
#include <vector>
#include <map>

namespace mbgl {

template <class Attributes>
class Segment {
public:
    Segment(std::size_t vertexOffset_,
            std::size_t indexOffset_,
            std::size_t vertexLength_ = 0,
            std::size_t indexLength_ = 0)
        : vertexOffset(vertexOffset_),
          indexOffset(indexOffset_),
          vertexLength(vertexLength_),
          indexLength(indexLength_) {}

    const std::size_t vertexOffset;
    const std::size_t indexOffset;

    std::size_t vertexLength;
    std::size_t indexLength;

    mutable optional<gl::VertexArray> vertexArray;
};

template <class Attributes>
class SegmentVector : public std::vector<Segment<Attributes>> {
public:
    SegmentVector() = default;
};

} // namespace mbgl
