#pragma once

#include <mbgl/gl/object.hpp>
#include <mbgl/gl/primitives.hpp>
#include <mbgl/gl/draw_mode.hpp>
#include <mbgl/util/ignore.hpp>

#include <vector>

namespace mbgl {
namespace gl {

template <class V, class DrawMode = Indexed>
class VertexVector {
public:
    using Vertex = V;
    static constexpr std::size_t groupSize = DrawMode::bufferGroupSize;

    template <class... Args>
    void emplace_back(Args&&... args) {
        static_assert(sizeof...(args) == groupSize, "wrong buffer element count");
        util::ignore({(v.emplace_back(std::forward<Args>(args)), 0)...});
    }

    std::size_t vertexSize() const { return v.size(); }
    std::size_t byteSize() const { return v.size() * sizeof(Vertex); }

    bool empty() const { return v.empty(); }
    void clear() { v.clear(); }
    const Vertex* data() const { return v.data(); }
    const std::vector<Vertex>& vector() const { return v; }

private:
    std::vector<Vertex> v;
};

template <class V, class DrawMode = Indexed>
class VertexBuffer {
public:
    using Vertex = V;
    static constexpr std::size_t vertexSize = sizeof(Vertex);

    std::size_t vertexCount;
    UniqueBuffer buffer;
};

} // namespace gl
} // namespace mbgl
