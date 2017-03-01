#pragma once

#include <mbgl/gl/program.hpp>
#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/style/paint_property.hpp>

#include <sstream>
#include <cassert>

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
        : program(context, vertexSource(programParameters), fragmentSource(programParameters))
        {}

    static std::string pixelRatioDefine(const ProgramParameters& parameters) {
        std::ostringstream pixelRatioSS;
        pixelRatioSS.imbue(std::locale("C"));
        pixelRatioSS.setf(std::ios_base::showpoint);
        pixelRatioSS << parameters.pixelRatio;
        return std::string("#define DEVICE_PIXEL_RATIO ") + pixelRatioSS.str() + "\n";
    }

    static std::string fragmentSource(const ProgramParameters& parameters) {
        std::string source = pixelRatioDefine(parameters) + Shaders::fragmentSource;
        if (parameters.overdraw) {
            assert(source.find("#ifdef OVERDRAW_INSPECTOR") != std::string::npos);
            source.replace(source.find_first_of('\n'), 1, "\n#define OVERDRAW_INSPECTOR\n");
        }
        return source;
    }

    static std::string vertexSource(const ProgramParameters& parameters) {
        return pixelRatioDefine(parameters) + Shaders::vertexSource;
    }

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
