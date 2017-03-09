#pragma once

#include <mbgl/gl/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/shaders/shaders.hpp>

namespace mbgl {

template <class Shaders,
          class Primitive,
          class LayoutAttrs,
          class Uniforms,
          class PaintProperties>
class Program {
public:
    using LayoutAttributes = LayoutAttrs;
    using LayoutVertex = typename LayoutAttributes::Vertex;

    using PaintPropertyBinders = typename PaintProperties::Binders;
    using PaintAttributes = typename PaintPropertyBinders::Attributes;
    using Attributes = gl::ConcatenateAttributes<LayoutAttributes, PaintAttributes>;

    using UniformValues = typename Uniforms::Values;
    using PaintUniforms = typename PaintPropertyBinders::Uniforms;
    using AllUniforms = gl::ConcatenateUniforms<Uniforms, PaintUniforms>;

    using ProgramType = gl::Program<Primitive, Attributes, AllUniforms>;

    ProgramType program;

    Program(gl::Context& context, const ProgramParameters& programParameters)
        : program(context,
                  shaders::vertexSource(programParameters, Shaders::vertexSource),
                  shaders::fragmentSource(programParameters, Shaders::fragmentSource))
        {}

    template <class DrawMode>
    void draw(gl::Context& context,
              DrawMode drawMode,
              gl::DepthMode depthMode,
              gl::StencilMode stencilMode,
              gl::ColorMode colorMode,
              UniformValues&& uniformValues,
              const gl::VertexBuffer<LayoutVertex>& layoutVertexBuffer,
              const gl::IndexBuffer<DrawMode>& indexBuffer,
              const gl::SegmentVector<Attributes>& segments,
              const PaintPropertyBinders& paintPropertyBinders,
              const typename PaintProperties::Evaluated& currentProperties,
              float currentZoom) {
        program.draw(
            context,
            std::move(drawMode),
            std::move(depthMode),
            std::move(stencilMode),
            std::move(colorMode),
            uniformValues
                .concat(paintPropertyBinders.uniformValues(currentZoom)),
            LayoutAttributes::allVariableBindings(layoutVertexBuffer)
                .concat(paintPropertyBinders.attributeBindings(currentProperties)),
            indexBuffer,
            segments
        );
    }
};

} // namespace mbgl
