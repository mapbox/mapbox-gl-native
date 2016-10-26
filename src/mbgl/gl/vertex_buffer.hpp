#pragma once

#include <mbgl/gl/object.hpp>

namespace mbgl {
namespace gl {

template <class Vertex>
class VertexBuffer {
public:
    static constexpr std::size_t vertexSize = sizeof(Vertex);
    std::size_t vertexCount;
    UniqueBuffer buffer;
};

} // namespace gl
} // namespace mbgl
