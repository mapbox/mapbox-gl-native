#pragma once

#include <mbgl/gl/program.hpp>
#include <mbgl/gl/features.hpp>
#include <mbgl/programs/segment.hpp>
#include <mbgl/programs/binary_program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/shaders/shaders.hpp>
#include <mbgl/util/io.hpp>

#include <unordered_map>

namespace mbgl {

template <class Shaders,
          class Primitive,
          class LayoutAttrs,
          class Uniforms,
          class PaintProps>
class Program {
public:
    using LayoutAttributes = LayoutAttrs;
    using LayoutVertex = typename LayoutAttributes::Vertex;

    using PaintProperties = PaintProps;
    using PaintPropertyBinders = typename PaintProperties::Binders;
    using PaintAttributes = typename PaintPropertyBinders::Attributes;
    using Attributes = gl::ConcatenateAttributes<LayoutAttributes, PaintAttributes>;

    using UniformValues = typename Uniforms::Values;
    using PaintUniforms = typename PaintPropertyBinders::Uniforms;
    using AllUniforms = gl::ConcatenateUniforms<Uniforms, PaintUniforms>;

    using ProgramType = gl::Program<Primitive, Attributes, AllUniforms>;

    ProgramType program;

    Program(gl::Context& context, const ProgramParameters& programParameters)
        : program(ProgramType::createProgram(
            context,
            programParameters,
            Shaders::name,
            Shaders::vertexSource,
            Shaders::fragmentSource)) {
    }

    template <class DrawMode>
    void draw(gl::Context& context,
              DrawMode drawMode,
              gl::DepthMode depthMode,
              gl::StencilMode stencilMode,
              gl::ColorMode colorMode,
              const UniformValues& uniformValues,
              const gl::VertexBuffer<LayoutVertex>& layoutVertexBuffer,
              const gl::IndexBuffer<DrawMode>& indexBuffer,
              const SegmentVector<Attributes>& segments,
              const PaintPropertyBinders& paintPropertyBinders,
              const typename PaintProperties::PossiblyEvaluated& currentProperties,
              float currentZoom,
              const std::string& layerID) {
        typename AllUniforms::Values allUniformValues = uniformValues
            .concat(paintPropertyBinders.uniformValues(currentZoom, currentProperties));

        typename Attributes::Bindings allAttributeBindings = LayoutAttributes::bindings(layoutVertexBuffer)
            .concat(paintPropertyBinders.attributeBindings(currentProperties));

        for (auto& segment : segments) {
            auto vertexArrayIt = segment.vertexArrays.find(layerID);

            if (vertexArrayIt == segment.vertexArrays.end()) {
                vertexArrayIt = segment.vertexArrays.emplace(layerID, context.createVertexArray()).first;
            }

            program.draw(
                context,
                std::move(drawMode),
                std::move(depthMode),
                std::move(stencilMode),
                std::move(colorMode),
                allUniformValues,
                vertexArrayIt->second,
                Attributes::offsetBindings(allAttributeBindings, segment.vertexOffset),
                indexBuffer,
                segment.indexOffset,
                segment.indexLength);
        }
    }
};

template <class Program>
class ProgramMap {
public:
    using PaintProperties = typename Program::PaintProperties;
    using PaintPropertyBinders = typename Program::PaintPropertyBinders;
    using Bitset = typename PaintPropertyBinders::Bitset;

    ProgramMap(gl::Context& context_, ProgramParameters parameters_)
        : context(context_),
          parameters(std::move(parameters_)) {
    }

    Program& get(const typename PaintProperties::PossiblyEvaluated& currentProperties) {
        Bitset bits = PaintPropertyBinders::constants(currentProperties);
        auto it = programs.find(bits);
        if (it != programs.end()) {
            return it->second;
        }
        return programs.emplace(std::piecewise_construct,
                                std::forward_as_tuple(bits),
                                std::forward_as_tuple(context,
                                    parameters.withAdditionalDefines(PaintPropertyBinders::defines(currentProperties)))).first->second;
    }

private:
    gl::Context& context;
    ProgramParameters parameters;
    std::unordered_map<Bitset, Program> programs;
};

} // namespace mbgl
