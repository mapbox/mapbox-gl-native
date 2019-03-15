#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/programs/textures.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/style/layers/raster_layer_properties.hpp>

namespace mbgl {

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(float, fade_t);
MBGL_DEFINE_UNIFORM_SCALAR(float, buffer_scale);
MBGL_DEFINE_UNIFORM_SCALAR(float, brightness_low);
MBGL_DEFINE_UNIFORM_SCALAR(float, brightness_high);
MBGL_DEFINE_UNIFORM_SCALAR(float, saturation_factor);
MBGL_DEFINE_UNIFORM_SCALAR(float, contrast_factor);
MBGL_DEFINE_UNIFORM_SCALAR(float, scale_parent);
MBGL_DEFINE_UNIFORM_VECTOR(float, 3, spin_weights);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, tl_parent);
} // namespace uniforms

class RasterProgram : public Program<
    RasterProgram,
    gfx::PrimitiveType::Triangle,
    TypeList<
        attributes::pos,
        attributes::texture_pos>,
    TypeList<
        uniforms::matrix,
        uniforms::opacity,
        uniforms::fade_t,
        uniforms::brightness_low,
        uniforms::brightness_high,
        uniforms::saturation_factor,
        uniforms::contrast_factor,
        uniforms::spin_weights,
        uniforms::buffer_scale,
        uniforms::scale_parent,
        uniforms::tl_parent>,
    TypeList<
        textures::image0,
        textures::image1>,
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
    RasterLayerPrograms(gfx::Context& context, const ProgramParameters& programParameters)
        : raster(context, programParameters) {}
    RasterProgram raster;
};

} // namespace mbgl
