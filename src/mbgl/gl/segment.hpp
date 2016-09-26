#pragma once

#include <cstddef>

namespace mbgl {
namespace gl {

class Segment {
public:
    Segment(std::size_t vertexOffset_,
            std::size_t primitiveOffset_,
            std::size_t vertexLength_ = 0,
            std::size_t primitiveLength_ = 0)
        : vertexOffset(vertexOffset_),
          primitiveOffset(primitiveOffset_),
          vertexLength(vertexLength_),
          primitiveLength(primitiveLength_) {}

    const std::size_t vertexOffset;
    const std::size_t primitiveOffset;

    std::size_t vertexLength;
    std::size_t primitiveLength;
};

} // namespace gl
} // namespace mbgl
