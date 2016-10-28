#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shader/fill.hpp>
#include <mbgl/shader/fill_pattern.hpp>
#include <mbgl/shader/fill_outline.hpp>
#include <mbgl/shader/fill_outline_pattern.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/style/layers/fill_layer_properties.hpp>

#include <string>

namespace mbgl {

class SpriteAtlasPosition;
class UnwrappedTileID;
class TransformState;

namespace style {
template <class> class Faded;
} // namespace style

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(Size,     u_world);
MBGL_DEFINE_UNIFORM_SCALAR(float,    u_scale_a);
MBGL_DEFINE_UNIFORM_SCALAR(float,    u_scale_b);
MBGL_DEFINE_UNIFORM_SCALAR(float,    u_tile_units_to_pixels);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_pixel_coord_upper);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_pixel_coord_lower);
} // namespace uniforms

struct FillLayoutAttributes : gl::Attributes<
    attributes::a_pos>
{};

struct FillUniforms : gl::Uniforms<
    uniforms::u_matrix,
    uniforms::u_world>
{};

struct FillPatternUniforms : gl::Uniforms<
    uniforms::u_matrix,
    uniforms::u_world,
    uniforms::u_pattern_tl_a,
    uniforms::u_pattern_br_a,
    uniforms::u_pattern_tl_b,
    uniforms::u_pattern_br_b,
    uniforms::u_pattern_size_a,
    uniforms::u_pattern_size_b,
    uniforms::u_scale_a,
    uniforms::u_scale_b,
    uniforms::u_mix,
    uniforms::u_image,
    uniforms::u_pixel_coord_upper,
    uniforms::u_pixel_coord_lower,
    uniforms::u_tile_units_to_pixels>
{
    static Values values(mat4 matrix,
                         Size framebufferSize,
                         const SpriteAtlasPosition&,
                         const SpriteAtlasPosition&,
                         const style::Faded<std::string>&,
                         const UnwrappedTileID&,
                         const TransformState&);
};

class FillProgram : public Program<
    shaders::fill,
    gl::Triangle,
    FillLayoutAttributes,
    FillUniforms,
    style::FillPaintProperties>
{
public:
    using Program::Program;

    static LayoutVertex layoutVertex(Point<int16_t> p) {
        return LayoutVertex {
            {{
                p.x,
                p.y
            }}
        };
    }
};

class FillPatternProgram : public Program<
    shaders::fill_pattern,
    gl::Triangle,
    FillLayoutAttributes,
    FillPatternUniforms,
    style::FillPaintProperties>
{
public:
    using Program::Program;
};

class FillOutlineProgram : public Program<
    shaders::fill_outline,
    gl::Line,
    FillLayoutAttributes,
    FillUniforms,
    style::FillPaintProperties>
{
public:
    using Program::Program;
};

class FillOutlinePatternProgram : public Program<
    shaders::fill_outline_pattern,
    gl::Line,
    FillLayoutAttributes,
    FillPatternUniforms,
    style::FillPaintProperties>
{
public:
    using Program::Program;
};

using FillLayoutVertex = FillProgram::LayoutVertex;
using FillAttributes = FillProgram::Attributes;

} // namespace mbgl
