#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/programs/textures.hpp>
#include <mbgl/shaders/heatmap_texture.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

class HeatmapTextureProgram : public Program<
    shaders::heatmap_texture,
    gfx::Triangle,
    TypeList<attributes::a_pos>,
    TypeList<
        uniforms::u_matrix,
        uniforms::u_world,
        uniforms::u_opacity>,
    TypeList<
        textures::u_image,
        textures::u_color_ramp>,
    style::Properties<>> {
public:
    using Program::Program;

    static LayoutVertex layoutVertex(Point<int16_t> p) {
        return LayoutVertex{
            {{
                p.x,
                p.y
            }}
        };
    }
};

using HeatmapTextureLayoutVertex = HeatmapTextureProgram::LayoutVertex;
using HeatmapTextureAttributes = HeatmapTextureProgram::Attributes;

} // namespace mbgl
