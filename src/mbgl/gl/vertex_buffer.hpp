#pragma once

#include <mbgl/gl/object.hpp>

#include <vector>

namespace mbgl {
namespace gl {

template <class V>
class VertexBuffer {
public:
    using Vertex = V;
    static constexpr std::size_t vertexSize = sizeof(Vertex);

    std::size_t vertexCount;
    UniqueBuffer buffer;
};

} // namespace gl
} // namespace mbgl
