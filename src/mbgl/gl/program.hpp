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

    using UniformValues = typename Uniforms::Values;
    using AttributeBindings = typename Attributes::Bindings;

    Program(Context& context, const std::string& vertexSource, const std::string& fragmentSource)
        : program(
              context.createProgram(context.createShader(ShaderType::Vertex, vertexSource),
                                    context.createShader(ShaderType::Fragment, fragmentSource))),
          attributeLocations(Attributes::bindLocations(program)),
          uniformsState((context.linkProgram(program), Uniforms::bindLocations(program))) {
    }

    template <class BinaryProgram>
    Program(Context& context, const BinaryProgram& binaryProgram)
        : program(context.createProgram(binaryProgram.format(), binaryProgram.code())),
          attributeLocations(Attributes::loadNamedLocations(binaryProgram)),
          uniformsState(Uniforms::loadNamedLocations(binaryProgram)) {
    }

    template <class BinaryProgram>
    optional<BinaryProgram> get(Context& context, const std::string& identifier) const {
        if (auto binaryProgram = context.getBinaryProgram(program)) {
            return BinaryProgram{ binaryProgram->first, std::move(binaryProgram->second),
                                  identifier, Attributes::getNamedLocations(attributeLocations),
                                  Uniforms::getNamedLocations(uniformsState) };
        }
        return {};
    }

    template <class DrawMode>
    void draw(Context& context,
              DrawMode drawMode,
              DepthMode depthMode,
              StencilMode stencilMode,
              ColorMode colorMode,
              UniformValues&& uniformValues,
              AttributeBindings&& attributeBindings,
              const IndexBuffer<DrawMode>& indexBuffer,
              const SegmentVector<Attributes>& segments) {
        static_assert(std::is_same<Primitive, typename DrawMode::Primitive>::value, "incompatible draw mode");

        context.setDrawMode(drawMode);
        context.setDepthMode(depthMode);
        context.setStencilMode(stencilMode);
        context.setColorMode(colorMode);

        context.program = program;

        Uniforms::bind(uniformsState, std::move(uniformValues));

        for (const auto& segment : segments) {
            segment.bind(context,
                         indexBuffer.buffer,
                         attributeLocations,
                         attributeBindings);

            context.draw(drawMode.primitiveType,
                         segment.indexOffset,
                         segment.indexLength);
        }
    }

private:
    UniqueProgram program;

    typename Attributes::Locations attributeLocations;
    typename Uniforms::State uniformsState;
};

} // namespace gl
} // namespace mbgl
