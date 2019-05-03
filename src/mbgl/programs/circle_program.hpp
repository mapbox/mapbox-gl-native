#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/style/layers/circle_layer_properties.hpp>

namespace mbgl {

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(bool, scale_with_map);
} // namespace uniforms

class CircleProgram : public Program<
    CircleProgram,
    gfx::PrimitiveType::Triangle,
    TypeList<
        attributes::pos>,
    TypeList<
        uniforms::matrix,
        uniforms::scale_with_map,
        uniforms::extrude_scale,
        uniforms::device_pixel_ratio,
        uniforms::camera_to_center_distance,
        uniforms::pitch_with_map>,
    TypeList<>,
    style::CirclePaintProperties>
{
public:
    using Program::Program;

    /*
     * @param {number} x vertex position
     * @param {number} y vertex position
     * @param {number} ex extrude normal
     * @param {number} ey extrude normal
     */
    static LayoutVertex vertex(Point<int16_t> p, float ex, float ey) {
        return LayoutVertex {
            {{
                static_cast<int16_t>((p.x * 2) + ((ex + 1) / 2)),
                static_cast<int16_t>((p.y * 2) + ((ey + 1) / 2))
            }}
        };
    }
};

using CircleLayoutVertex = CircleProgram::LayoutVertex;
using CircleAttributes = CircleProgram::AttributeList;

class CircleLayerPrograms final : public LayerTypePrograms  {
public:
    CircleLayerPrograms(gfx::Context& context, const ProgramParameters& programParameters)
        : circle(context, programParameters) {}
    CircleProgram circle;
};

} // namespace mbgl
