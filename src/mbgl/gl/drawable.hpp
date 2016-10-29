#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/gl/draw_mode.hpp>
#include <mbgl/gl/depth_mode.hpp>
#include <mbgl/gl/stencil_mode.hpp>
#include <mbgl/gl/color_mode.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/uniform.hpp>
#include <mbgl/gl/segment.hpp>

#include <cstddef>
#include <functional>
#include <limits>
#include <vector>

namespace mbgl {
namespace gl {

template <class Vertex>
class UnindexedVertices {
public:
    using VertexType = Vertex;

    UnindexedVertices(DrawMode drawMode_, const VertexBuffer<Vertex>& vertexBuffer_)
        : drawMode(std::move(drawMode_)),
          vertexBuffer(vertexBuffer_.buffer),
          segments({{ 0, 0, vertexBuffer_.vertexCount, 0 }}) {}

    DrawMode drawMode;
    gl::BufferID vertexBuffer;
    static constexpr std::size_t vertexSize = sizeof(Vertex);
    static constexpr gl::BufferID indexBuffer = 0;
    static constexpr std::size_t primitiveSize = 0;
    std::vector<Segment> segments;
};

template <class DrawMode, class Vertex, class...Args>
auto Unindexed(const VertexBuffer<Vertex>& vertexBuffer,
               Args&&... drawModeArguments) {
    return UnindexedVertices<Vertex>(
        DrawMode { std::forward<Args>(drawModeArguments)... },
        vertexBuffer);
}

template <class Vertex, class Primitive>
class SegmentedVertices {
public:
    using VertexType = Vertex;

    SegmentedVertices(DrawMode drawMode_,
                      const VertexBuffer<Vertex>& vertexBuffer_,
                      const IndexBuffer<Primitive>& indexBuffer_,
                      const std::vector<Segment>& segments_)
        : drawMode(std::move(drawMode_)),
          vertexBuffer(vertexBuffer_.buffer),
          indexBuffer(indexBuffer_.buffer),
          segments(segments_) {}

    DrawMode drawMode;
    gl::BufferID vertexBuffer;
    static constexpr std::size_t vertexSize = sizeof(Vertex);
    gl::BufferID indexBuffer;
    static constexpr std::size_t primitiveSize = sizeof(Primitive);
    const std::vector<Segment>& segments;
};

template <class DrawMode, class Vertex, class Primitive, class...Args>
auto Segmented(const VertexBuffer<Vertex>& vertexBuffer,
               const IndexBuffer<Primitive>& indexBuffer,
               const std::vector<Segment>& segments,
               Args&&... drawModeArguments) {
    static_assert(std::is_same<typename Primitive::DrawMode, DrawMode>::value, "primitive mode mismatch");
    return SegmentedVertices<Vertex, Primitive>(
        DrawMode { std::forward<Args>(drawModeArguments)... },
        vertexBuffer,
        indexBuffer,
        segments);
}

class Drawable {
public:
    template <class Program, class Subject>
    Drawable(DepthMode depthMode_,
             StencilMode stencilMode_,
             ColorMode colorMode_,
             Program& program_,
             typename Program::UniformValues&& uniformValues,
             const Subject& subject)
        : drawMode(subject.drawMode),
          depthMode(std::move(depthMode_)),
          stencilMode(std::move(stencilMode_)),
          colorMode(std::move(colorMode_)),
          program(program_.program),
          vertexBuffer(subject.vertexBuffer),
          vertexSize(subject.vertexSize),
          indexBuffer(subject.indexBuffer),
          primitiveSize(subject.primitiveSize),
          segments(subject.segments),
          bindUniforms(Program::Uniforms::binder(program_.uniformsState, std::move(uniformValues))),
          bindAttributes(Program::Attributes::binder(program_.attributesState))
    {
        static_assert(std::is_standard_layout<typename Program::Vertex>::value, "vertex type must use standard layout");
        static_assert(std::is_same<typename Program::Vertex, typename Subject::VertexType>::value, "vertex type mismatch");
    }

    DrawMode drawMode;
    DepthMode depthMode;
    StencilMode stencilMode;
    ColorMode colorMode;
    gl::ProgramID program;
    gl::BufferID vertexBuffer;
    std::size_t vertexSize;
    gl::BufferID indexBuffer;
    std::size_t primitiveSize;
    const std::vector<Segment>& segments;
    std::function<void ()> bindUniforms;
    std::function<void (std::size_t)> bindAttributes;
};

} // namespace gl
} // namespace mbgl
