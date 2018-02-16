#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shaders/collision_box.hpp>
#include <mbgl/shaders/collision_circle.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/util/geometry.hpp>

#include <cmath>

namespace mbgl {

using CollisionBoxLayoutAttributes = gl::Attributes<
    attributes::a_pos,
    attributes::a_anchor_pos,
    attributes::a_extrude>;

struct CollisionBoxDynamicAttributes : gl::Attributes<attributes::a_placed> {
    static Vertex vertex(bool placed, bool notUsed) {
        return Vertex {
            {{ static_cast<uint8_t>(placed), static_cast<uint8_t>(notUsed)  }}
        };
    }
};

class CollisionBoxProgram : public Program<
    shaders::collision_box,
    gl::Line,
    gl::ConcatenateAttributes<CollisionBoxLayoutAttributes, CollisionBoxDynamicAttributes>,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_extrude_scale,
        uniforms::u_camera_to_center_distance>,
    style::Properties<>>
{
public:
    using Program::Program;

    static CollisionBoxLayoutAttributes::Vertex vertex(Point<float> a, Point<float> anchor, Point<float> o) {
        return CollisionBoxLayoutAttributes::Vertex {
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
            }}
        };
    }

	template <class DrawMode>
    void draw(gl::Context& context,
              DrawMode drawMode,
              gl::DepthMode depthMode,
              gl::StencilMode stencilMode,
              gl::ColorMode colorMode,
              const UniformValues& uniformValues,
              const gl::VertexBuffer<CollisionBoxLayoutAttributes::Vertex>& layoutVertexBuffer,
              const gl::VertexBuffer<CollisionBoxDynamicAttributes::Vertex>& dynamicVertexBuffer,
              const gl::IndexBuffer<DrawMode>& indexBuffer,
              const SegmentVector<Attributes>& segments,
              const PaintPropertyBinders& paintPropertyBinders,
              const typename PaintProperties::PossiblyEvaluated& currentProperties,
              float currentZoom,
              const std::string& layerID) {
        typename AllUniforms::Values allUniformValues = uniformValues
            .concat(paintPropertyBinders.uniformValues(currentZoom, currentProperties));

        typename Attributes::Bindings allAttributeBindings = CollisionBoxLayoutAttributes::bindings(layoutVertexBuffer)
            .concat(CollisionBoxDynamicAttributes::bindings(dynamicVertexBuffer))
            .concat(paintPropertyBinders.attributeBindings(currentProperties));

        assert(layoutVertexBuffer.vertexCount == dynamicVertexBuffer.vertexCount);

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


class CollisionCircleProgram : public Program<
    shaders::collision_circle,
    gl::Triangle,
    gl::ConcatenateAttributes<CollisionBoxLayoutAttributes, CollisionBoxDynamicAttributes>,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_extrude_scale,
        uniforms::u_overscale_factor,
        uniforms::u_camera_to_center_distance>,
    style::Properties<>>
{
public:
    using Program::Program;

    static CollisionBoxLayoutAttributes::Vertex vertex(Point<float> a, Point<float> anchor, Point<float> o) {
        return CollisionBoxLayoutAttributes::Vertex {
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
            }}
        };
    }

    template <class DrawMode>
    void draw(gl::Context& context,
              DrawMode drawMode,
              gl::DepthMode depthMode,
              gl::StencilMode stencilMode,
              gl::ColorMode colorMode,
              const UniformValues& uniformValues,
              const gl::VertexBuffer<CollisionBoxLayoutAttributes::Vertex>& layoutVertexBuffer,
              const gl::VertexBuffer<CollisionBoxDynamicAttributes::Vertex>& dynamicVertexBuffer,
              const gl::IndexBuffer<DrawMode>& indexBuffer,
              const SegmentVector<Attributes>& segments,
              const PaintPropertyBinders& paintPropertyBinders,
              const typename PaintProperties::PossiblyEvaluated& currentProperties,
              float currentZoom,
              const std::string& layerID) {
        typename AllUniforms::Values allUniformValues = uniformValues
            .concat(paintPropertyBinders.uniformValues(currentZoom, currentProperties));

        typename Attributes::Bindings allAttributeBindings = CollisionBoxLayoutAttributes::bindings(layoutVertexBuffer)
            .concat(CollisionBoxDynamicAttributes::bindings(dynamicVertexBuffer))
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

using CollisionBoxVertex = CollisionBoxProgram::LayoutVertex;

} // namespace mbgl
