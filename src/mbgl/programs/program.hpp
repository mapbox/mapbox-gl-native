#pragma once

#include <mbgl/gl/program.hpp>
#include <mbgl/gl/features.hpp>
#include <mbgl/programs/segment.hpp>
#include <mbgl/programs/binary_program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/renderer/paint_property_binder.hpp>
#include <mbgl/shaders/shaders.hpp>
#include <mbgl/util/io.hpp>

#include <unordered_map>

namespace mbgl {

template <class Shaders,
          class Primitive,
          class LayoutAttributeList,
          class UniformList,
          class TextureList,
          class PaintProps>
class Program {
public:
    using LayoutAttributes = gl::Attributes<LayoutAttributeList>;
    using LayoutVertex = gfx::Vertex<LayoutAttributeList>;

    using PaintProperties = PaintProps;
    using Binders = PaintPropertyBinders<typename PaintProperties::DataDrivenProperties>;
    using PaintAttributeList = typename Binders::AttributeList;
    using Attributes = gl::Attributes<TypeListConcat<LayoutAttributeList, PaintAttributeList>>;

    using UniformValues = gfx::UniformValues<UniformList>;
    using PaintUniformList = typename Binders::UniformList;
    using AllUniforms = gl::Uniforms<TypeListConcat<UniformList, PaintUniformList>>;

    using TextureBindings = gfx::TextureBindings<TextureList>;

    using ProgramType = gl::Program<Primitive, Attributes, AllUniforms, TextureList>;

    ProgramType program;

    Program(gl::Context& context, const ProgramParameters& programParameters)
        : program(ProgramType::createProgram(
            context,
            programParameters,
            Shaders::name,
            Shaders::vertexSource,
            Shaders::fragmentSource)) {
    }

    static typename AllUniforms::Values computeAllUniformValues(
        const UniformValues& uniformValues,
        const Binders& paintPropertyBinders,
        const typename PaintProperties::PossiblyEvaluated& currentProperties,
        float currentZoom) {
        return uniformValues
            .concat(paintPropertyBinders.uniformValues(currentZoom, currentProperties));
    }

    static typename Attributes::Bindings computeAllAttributeBindings(
        const gfx::VertexBuffer<LayoutVertex>& layoutVertexBuffer,
        const Binders& paintPropertyBinders,
        const typename PaintProperties::PossiblyEvaluated& currentProperties) {
        return LayoutAttributes::bindings(layoutVertexBuffer)
            .concat(paintPropertyBinders.attributeBindings(currentProperties));
    }

    static uint32_t activeBindingCount(const typename Attributes::Bindings& allAttributeBindings) {
        return Attributes::activeBindingCount(allAttributeBindings);
    }

    template <class DrawMode>
    void draw(gl::Context& context,
              DrawMode drawMode,
              gfx::DepthMode depthMode,
              gfx::StencilMode stencilMode,
              gfx::ColorMode colorMode,
              gfx::CullFaceMode cullFaceMode,
              const gfx::IndexBuffer& indexBuffer,
              const SegmentVector<Attributes>& segments,
              const typename AllUniforms::Values& allUniformValues,
              const typename Attributes::Bindings& allAttributeBindings,
              const TextureBindings& textureBindings,
              const std::string& layerID) {
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
                std::move(cullFaceMode),
                allUniformValues,
                vertexArrayIt->second,
                Attributes::offsetBindings(allAttributeBindings, segment.vertexOffset),
                textureBindings,
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
    using Binders = typename Program::Binders;
    using Bitset = typename Binders::Bitset;

    ProgramMap(gl::Context& context_, ProgramParameters parameters_)
        : context(context_),
          parameters(std::move(parameters_)) {
    }

    Program& get(const typename PaintProperties::PossiblyEvaluated& currentProperties) {
        Bitset bits = Binders::constants(currentProperties);
        auto it = programs.find(bits);
        if (it != programs.end()) {
            return it->second;
        }
        return programs.emplace(std::piecewise_construct,
                                std::forward_as_tuple(bits),
                                std::forward_as_tuple(context,
                                    parameters.withAdditionalDefines(Binders::defines(currentProperties)))).first->second;
    }

private:
    gl::Context& context;
    ProgramParameters parameters;
    std::unordered_map<Bitset, Program> programs;
};

class LayerTypePrograms {
public:
    virtual ~LayerTypePrograms() = default;
};

} // namespace mbgl
