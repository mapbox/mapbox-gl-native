#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/programs/textures.hpp>
#include <mbgl/shaders/raster.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/style/layers/raster_layer_properties.hpp>

namespace mbgl {

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(float, u_fade_t);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_buffer_scale);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_brightness_low);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_brightness_high);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_saturation_factor);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_contrast_factor);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_scale_parent);
MBGL_DEFINE_UNIFORM_VECTOR(float, 3, u_spin_weights);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_tl_parent);
} // namespace uniforms

class RasterProgram : public Program<
    shaders::raster,
    gfx::Triangle,
    TypeList<
        attributes::a_pos,
        attributes::a_texture_pos>,
    TypeList<
        uniforms::u_matrix,
        uniforms::u_opacity,
        uniforms::u_fade_t,
        uniforms::u_brightness_low,
        uniforms::u_brightness_high,
        uniforms::u_saturation_factor,
        uniforms::u_contrast_factor,
        uniforms::u_spin_weights,
        uniforms::u_buffer_scale,
        uniforms::u_scale_parent,
        uniforms::u_tl_parent>,
    TypeList<
        textures::u_image0,
        textures::u_image1>,
    style::RasterPaintProperties>
{
public:
    using Program::Program;

    static LayoutVertex layoutVertex(Point<int16_t> p, Point<uint16_t> t) {
        return LayoutVertex {
            {{
                p.x,
                p.y
            }},
            {{
                t.x,
                t.y
            }}
        };
    }
};

using RasterLayoutVertex = RasterProgram::LayoutVertex;
using RasterAttributes = RasterProgram::AttributeList;

class RasterLayerPrograms final : public LayerTypePrograms {
public:
    RasterLayerPrograms(gl::Context& context, const ProgramParameters& programParameters)
        : raster(context, programParameters) {}
    RasterProgram raster;
};

} // namespace mbgl
