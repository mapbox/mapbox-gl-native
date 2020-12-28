#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/heatmap_texture_program.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/style/layers/heatmap_layer_properties.hpp>

namespace mbgl {

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(float, intensity);
} // namespace uniforms

class HeatmapProgram : public Program<
    HeatmapProgram,
    gfx::PrimitiveType::Triangle,
    TypeList<
        attributes::pos>,
    TypeList<
        uniforms::intensity,
        uniforms::matrix,
        uniforms::heatmap::extrude_scale>,
    TypeList<>,
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
using HeatmapAttributes = HeatmapProgram::AttributeList;

class HeatmapLayerPrograms final : public LayerTypePrograms  {
public:
    HeatmapLayerPrograms(gfx::Context& context, const ProgramParameters& programParameters) 
        : heatmap(context, programParameters),
          heatmapTexture(context, programParameters) {}
    HeatmapProgram heatmap;
    HeatmapTextureProgram heatmapTexture;
};

} // namespace mbgl
