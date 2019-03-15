#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/util/geometry.hpp>

#include <cmath>

namespace mbgl {

using CollisionBoxLayoutAttributes = TypeList<
    attributes::a_pos,
    attributes::a_anchor_pos,
    attributes::a_extrude,
    attributes::a_shift>;

using CollisionBoxDynamicAttributes = TypeList<attributes::a_placed>;

class CollisionBoxProgram : public Program<
    CollisionBoxProgram,
    gfx::PrimitiveType::Line,
    TypeListConcat<CollisionBoxLayoutAttributes, CollisionBoxDynamicAttributes>,
    TypeList<
        uniforms::u_matrix,
        uniforms::u_extrude_scale,
        uniforms::u_camera_to_center_distance>,
    TypeList<>,
    style::Properties<>>
{
public:
    using Program::Program;

    static gfx::Vertex<CollisionBoxLayoutAttributes> layoutVertex(Point<float> a, Point<float> anchor, Point<float> o) {
        return {
            {{
                static_cast<int16_t>(a.x),
                static_cast<int16_t>(a.y)
            }},
            {{
                static_cast<int16_t>(anchor.x),
                static_cast<int16_t>(anchor.y)
            }},
            {{
                static_cast<int16_t>(::round(o.x)),
                static_cast<int16_t>(::round(o.y))
            }},
            {{
                0.0f,
                0.0f
            }}
        };
    }

    static gfx::Vertex<CollisionBoxDynamicAttributes> dynamicVertex(bool placed, bool notUsed) {
        return {
            {{ static_cast<uint8_t>(placed), static_cast<uint8_t>(notUsed)  }}
        };
    }

	template <class DrawMode>
    void draw(gfx::Context& context,
              const DrawMode& drawMode,
              const gfx::DepthMode& depthMode,
              const gfx::StencilMode& stencilMode,
              const gfx::ColorMode& colorMode,
              const gfx::CullFaceMode& cullFaceMode,
              const LayoutUniformValues& layoutUniformValues,
              const gfx::VertexBuffer<gfx::Vertex<CollisionBoxLayoutAttributes>>& layoutVertexBuffer,
              const gfx::VertexBuffer<gfx::Vertex<CollisionBoxDynamicAttributes>>& dynamicVertexBuffer,
              const gfx::IndexBuffer& indexBuffer,
              const SegmentVector<AttributeList>& segments,
              const Binders& paintPropertyBinders,
              const typename PaintProperties::PossiblyEvaluated& currentProperties,
              const TextureBindings& textureBindings,
              float currentZoom,
              const std::string& layerID) {
        UniformValues uniformValues = layoutUniformValues
            .concat(paintPropertyBinders.uniformValues(currentZoom, currentProperties));

        AttributeBindings allAttributeBindings = gfx::AttributeBindings<CollisionBoxLayoutAttributes>(layoutVertexBuffer)
            .concat(gfx::AttributeBindings<CollisionBoxDynamicAttributes>(dynamicVertexBuffer))
            .concat(paintPropertyBinders.attributeBindings(currentProperties));

        assert(layoutVertexBuffer.elements == dynamicVertexBuffer.elements);

        for (auto& segment : segments) {
            auto drawScopeIt = segment.drawScopes.find(layerID);

            if (drawScopeIt == segment.drawScopes.end()) {
                drawScopeIt = segment.drawScopes.emplace(layerID, context.createDrawScope()).first;
            }

            program->draw(
                    context,
                    std::move(drawMode),
                    std::move(depthMode),
                    std::move(stencilMode),
                    std::move(colorMode),
                    std::move(cullFaceMode),
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


class CollisionCircleProgram : public Program<
    CollisionCircleProgram,
    gfx::PrimitiveType::Triangle,
    TypeListConcat<CollisionBoxLayoutAttributes, CollisionBoxDynamicAttributes>,
    TypeList<
        uniforms::u_matrix,
        uniforms::u_extrude_scale,
        uniforms::u_overscale_factor,
        uniforms::u_camera_to_center_distance>,
    TypeList<>,
    style::Properties<>>
{
public:
    using Program::Program;

    static gfx::Vertex<CollisionBoxLayoutAttributes> vertex(Point<float> a, Point<float> anchor, Point<float> o) {
        return {
            {{
                static_cast<int16_t>(a.x),
                static_cast<int16_t>(a.y)
            }},
            {{
                static_cast<int16_t>(anchor.x),
                static_cast<int16_t>(anchor.y)
            }},
            {{
                static_cast<int16_t>(::round(o.x)),
                static_cast<int16_t>(::round(o.y))
            }},
            {{
                0.0f,
                0.0f
            }}
        };
    }

    template <class DrawMode>
    void draw(gfx::Context& context,
              const DrawMode& drawMode,
              const gfx::DepthMode& depthMode,
              const gfx::StencilMode& stencilMode,
              const gfx::ColorMode& colorMode,
              const gfx::CullFaceMode& cullFaceMode,
              const LayoutUniformValues& layoutUniformValues,
              const gfx::VertexBuffer<gfx::Vertex<CollisionBoxLayoutAttributes>>& layoutVertexBuffer,
              const gfx::VertexBuffer<gfx::Vertex<CollisionBoxDynamicAttributes>>& dynamicVertexBuffer,
              const gfx::IndexBuffer& indexBuffer,
              const SegmentVector<AttributeList>& segments,
              const Binders& paintPropertyBinders,
              const typename PaintProperties::PossiblyEvaluated& currentProperties,
              const TextureBindings& textureBindings,
              float currentZoom,
              const std::string& layerID) {
        UniformValues uniformValues = layoutUniformValues
            .concat(paintPropertyBinders.uniformValues(currentZoom, currentProperties));

        AttributeBindings allAttributeBindings = gfx::AttributeBindings<CollisionBoxLayoutAttributes>(layoutVertexBuffer)
            .concat(gfx::AttributeBindings<CollisionBoxDynamicAttributes>(dynamicVertexBuffer))
            .concat(paintPropertyBinders.attributeBindings(currentProperties));

        for (auto& segment : segments) {
            auto drawScopeIt = segment.drawScopes.find(layerID);

            if (drawScopeIt == segment.drawScopes.end()) {
                drawScopeIt = segment.drawScopes.emplace(layerID, context.createDrawScope()).first;
            }

            program->draw(
                    context,
                    std::move(drawMode),
                    std::move(depthMode),
                    std::move(stencilMode),
                    std::move(colorMode),
                    std::move(cullFaceMode),
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

using CollisionBoxVertex = CollisionBoxProgram::LayoutVertex;

} // namespace mbgl
