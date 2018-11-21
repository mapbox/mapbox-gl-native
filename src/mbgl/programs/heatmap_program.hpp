#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shaders/heatmap.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/style/layers/heatmap_layer_properties.hpp>

namespace mbgl {

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(float, u_intensity);
} // namespace uniforms

class HeatmapProgram : public Program<
    shaders::heatmap,
    gl::Triangle,
    gl::Attributes<
        attributes::a_pos>,
    gl::Uniforms<
        uniforms::u_intensity,
        uniforms::u_matrix,
        uniforms::heatmap::u_extrude_scale>,
    style::HeatmapPaintProperties>
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

using HeatmapLayoutVertex = HeatmapProgram::LayoutVertex;
using HeatmapAttributes = HeatmapProgram::Attributes;

} // namespace mbgl
