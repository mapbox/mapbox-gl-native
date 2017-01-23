#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shader/circle.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(float, u_radius);
MBGL_DEFINE_UNIFORM_SCALAR(Color, u_stroke_color);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_stroke_width);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_stroke_opacity);
MBGL_DEFINE_UNIFORM_SCALAR(bool, u_scale_with_map);
} // namespace uniforms

using CircleAttributes = gl::Attributes<
    attributes::a_pos>;

class CircleProgram : public Program<
    shaders::circle,
    gl::Triangle,
    CircleAttributes,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_opacity,
        uniforms::u_color,
        uniforms::u_radius,
        uniforms::u_blur,
        uniforms::u_stroke_color,
        uniforms::u_stroke_width,
        uniforms::u_stroke_opacity,
        uniforms::u_scale_with_map,
        uniforms::u_extrude_scale>>
{
public:
    using Program::Program;

    /*
     * @param {number} x vertex position
     * @param {number} y vertex position
     * @param {number} ex extrude normal
     * @param {number} ey extrude normal
     */
    static Vertex vertex(Point<int16_t> p, float ex, float ey) {
        return Vertex {
            {
                static_cast<int16_t>((p.x * 2) + ((ex + 1) / 2)),
                static_cast<int16_t>((p.y * 2) + ((ey + 1) / 2))
            }
        };
    }
};

using CircleVertex = CircleProgram::Vertex;

} // namespace mbgl
