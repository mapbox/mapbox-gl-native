#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/gl/object.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/vertex_buffer.hpp>
#include <mbgl/gl/index_buffer.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/gl/uniform.hpp>

#include <string>

namespace mbgl {
namespace gl {

template <class P, class As, class Us>
class Program {
public:
    using Primitive = P;
    using Attributes = As;
    using Uniforms = Us;

    using Vertex = typename Attributes::Vertex;
    using UniformValues = typename Uniforms::Values;

    static_assert(std::is_standard_layout<Vertex>::value, "vertex type must use standard layout");

    Program(Context& context, const std::string& vertexSource, const std::string& fragmentSource)
        : vertexShader(context.createShader(ShaderType::Vertex, vertexSource)),
          fragmentShader(context.createShader(ShaderType::Fragment, fragmentSource)),
          program(context.createProgram(vertexShader, fragmentShader)),
          attributesState(Attributes::state(program)),
          uniformsState(Uniforms::state(program)) {}

    // Indexed drawing.
    template <class DrawMode>
    void draw(Context& context,
              DrawMode drawMode,
              DepthMode depthMode,
              StencilMode stencilMode,
              ColorMode colorMode,
              UniformValues&& uniformValues,
              const VertexBuffer<Vertex>& vertexBuffer,
              const IndexBuffer<DrawMode>& indexBuffer,
              const std::vector<Segment>& segments) {
        static_assert(std::is_same<Primitive, typename DrawMode::Primitive>::value, "incompatible draw mode");
        context.draw({
            std::move(drawMode),
            std::move(depthMode),
            std::move(stencilMode),
            std::move(colorMode),
            program,
            vertexBuffer.buffer,
            indexBuffer.buffer,
            segments,
            Uniforms::binder(uniformsState, std::move(uniformValues)),
            Attributes::binder(attributesState)
        });
    }

    // Unindexed drawing.
    template <class DrawMode>
    void draw(Context& context,
              DrawMode drawMode,
              DepthMode depthMode,
              StencilMode stencilMode,
              ColorMode colorMode,
              UniformValues&& uniformValues,
              const VertexBuffer<Vertex, DrawMode>& vertexBuffer) {
        static_assert(std::is_same<Primitive, typename DrawMode::Primitive>::value, "incompatible draw mode");
        context.draw({
            std::move(drawMode),
            std::move(depthMode),
            std::move(stencilMode),
            std::move(colorMode),
            program,
            vertexBuffer.buffer,
            0,
            {{ 0, 0, vertexBuffer.vertexCount, 0 }},
            Uniforms::binder(uniformsState, std::move(uniformValues)),
            Attributes::binder(attributesState)
        });
    }

private:
    UniqueShader vertexShader;
    UniqueShader fragmentShader;
    UniqueProgram program;

    typename Attributes::State attributesState;
    typename Uniforms::State uniformsState;
};

} // namespace gl
} // namespace mbgl
