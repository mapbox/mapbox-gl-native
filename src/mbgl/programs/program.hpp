#pragma once

#include <mbgl/gfx/attribute.hpp>
#include <mbgl/gfx/uniform.hpp>
#include <mbgl/programs/segment.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/program_parameters.hpp>
#include <mbgl/style/paint_property.hpp>
#include <mbgl/renderer/paint_property_binder.hpp>
#include <mbgl/util/io.hpp>

#include <unordered_map>

namespace mbgl {

template <class Name,
          gfx::PrimitiveType Primitive,
          class LayoutAttributeList,
          class LayoutUniformList,
          class Textures,
          class PaintProps>
class Program {
public:
    using LayoutVertex = gfx::Vertex<LayoutAttributeList>;

    using PaintProperties = PaintProps;
    using Binders = PaintPropertyBinders<typename PaintProperties::DataDrivenProperties>;

    using PaintAttributeList = typename Binders::AttributeList;
    using AttributeList = TypeListConcat<LayoutAttributeList, PaintAttributeList>;
    using AttributeBindings = gfx::AttributeBindings<AttributeList>;

    using PaintUniformList = typename Binders::UniformList;
    using UniformList = TypeListConcat<LayoutUniformList, PaintUniformList>;
    using LayoutUniformValues = gfx::UniformValues<LayoutUniformList>;
    using UniformValues = gfx::UniformValues<UniformList>;

    using TextureList = Textures;
    using TextureBindings = gfx::TextureBindings<TextureList>;

    std::unique_ptr<gfx::Program<Name>> program;

    Program(gfx::Context& context, const ProgramParameters& programParameters)
        : program(context.createProgram<Name>(programParameters)) {
    }

    static UniformValues computeAllUniformValues(
        const LayoutUniformValues& layoutUniformValues,
        const Binders& paintPropertyBinders,
        const typename PaintProperties::PossiblyEvaluated& currentProperties,
        float currentZoom) {
        return layoutUniformValues
            .concat(paintPropertyBinders.uniformValues(currentZoom, currentProperties));
    }

    static AttributeBindings computeAllAttributeBindings(
        const gfx::VertexBuffer<LayoutVertex>& layoutVertexBuffer,
        const Binders& paintPropertyBinders,
        const typename PaintProperties::PossiblyEvaluated& currentProperties) {
        return gfx::AttributeBindings<LayoutAttributeList>(layoutVertexBuffer)
            .concat(paintPropertyBinders.attributeBindings(currentProperties));
    }

    static uint32_t activeBindingCount(const AttributeBindings& allAttributeBindings) {
        return allAttributeBindings.activeCount();
    }

    template <class DrawMode>
    void draw(gfx::Context& context,
              const DrawMode& drawMode,
              const gfx::DepthMode& depthMode,
              const gfx::StencilMode& stencilMode,
              const gfx::ColorMode& colorMode,
              const gfx::CullFaceMode& cullFaceMode,
              const gfx::IndexBuffer& indexBuffer,
              const SegmentVector<AttributeList>& segments,
              const UniformValues& uniformValues,
              const AttributeBindings& allAttributeBindings,
              const TextureBindings& textureBindings,
              const std::string& layerID) {
        static_assert(Primitive == gfx::PrimitiveTypeOf<DrawMode>::value, "incompatible draw mode");

        if (!program) {
            return;
        }

        for (auto& segment : segments) {
            auto drawScopeIt = segment.drawScopes.find(layerID);

            if (drawScopeIt == segment.drawScopes.end()) {
                drawScopeIt = segment.drawScopes.emplace(layerID, context.createDrawScope()).first;
            }

            program->draw(
                context,
                drawMode,
                depthMode,
                stencilMode,
                colorMode,
                cullFaceMode,
                uniformValues,
                drawScopeIt->second,
                allAttributeBindings.offset(segment.vertexOffset),
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

    ProgramMap(gfx::Context& context_, ProgramParameters parameters_)
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
    gfx::Context& context;
    ProgramParameters parameters;
    std::unordered_map<Bitset, Program> programs;
};

class LayerTypePrograms {
public:
    virtual ~LayerTypePrograms() = default;
};

} // namespace mbgl
