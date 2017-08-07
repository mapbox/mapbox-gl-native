#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shaders/circle.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/style/layers/circle_layer_properties.hpp>

namespace mbgl {

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(bool, u_scale_with_map);
} // namespace uniforms

class CircleProgram : public Program<
    shaders::circle,
    gl::Triangle,
    gl::Attributes<
        attributes::a_pos>,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_scale_with_map,
        uniforms::u_extrude_scale,
        uniforms::u_camera_to_center_distance,
        uniforms::u_pitch_with_map>,
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
using CircleAttributes = CircleProgram::Attributes;

} // namespace mbgl
