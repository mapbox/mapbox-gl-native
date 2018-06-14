#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shaders/fill_extrusion.hpp>
#include <mbgl/shaders/fill_extrusion_pattern.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_properties.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/renderer/render_light.hpp>

#include <string>

namespace mbgl {

class ImagePosition;
class UnwrappedTileID;
class TransformState;
template <class> class Faded;

namespace uniforms {
MBGL_DEFINE_UNIFORM_VECTOR(float, 3, u_lightpos);
MBGL_DEFINE_UNIFORM_VECTOR(float, 3, u_lightcolor);
MBGL_DEFINE_UNIFORM_SCALAR(float,    u_lightintensity);
MBGL_DEFINE_UNIFORM_SCALAR(float,    u_height_factor);
} // namespace uniforms

struct FillExtrusionLayoutAttributes : gl::Attributes<
    attributes::a_pos,
    attributes::a_normal_ed>
{};

struct FillExtrusionUniforms : gl::Uniforms<
    uniforms::u_matrix,
    uniforms::u_lightcolor,
    uniforms::u_lightpos,
    uniforms::u_lightintensity>
{
    static Values values(mat4,
                         const TransformState&,
                         const EvaluatedLight&);
};

struct FillExtrusionPatternUniforms : gl::Uniforms<
    uniforms::u_matrix,
    uniforms::u_scale,
    uniforms::u_texsize,
    uniforms::u_fade,
    uniforms::u_image,
    uniforms::u_pixel_coord_upper,
    uniforms::u_pixel_coord_lower,
    uniforms::u_height_factor,
    uniforms::u_lightcolor,
    uniforms::u_lightpos,
    uniforms::u_lightintensity>
{
    static Values values(mat4,
                         Size atlasSize,
                         const CrossfadeParameters&,
                         const UnwrappedTileID&,
                         const TransformState&,
                         const float heightFactor,
                         const float pixelRatio,
                         const EvaluatedLight&);
};

class FillExtrusionProgram : public Program<
    shaders::fill_extrusion,
    gl::Triangle,
    FillExtrusionLayoutAttributes,
    FillExtrusionUniforms,
    style::FillExtrusionPaintProperties>
{
public:
    using Program::Program;

    static LayoutVertex layoutVertex(Point<int16_t> p, double nx, double ny, double nz, unsigned short t, uint16_t e) {
        const auto factor = pow(2, 13);

        return LayoutVertex {
            {{
                p.x,
                p.y
            }},
            {{
                // Multiply normal vector components by 2^14 to pack them into integers
                // We pack a bool (`t`) into the x component indicating whether it is an upper or lower vertex
                static_cast<int16_t>(floor(nx * factor) * 2 + t),
                static_cast<int16_t>(ny * factor * 2),
                static_cast<int16_t>(nz * factor * 2),
                // The edgedistance attribute is used for wrapping fill_extrusion patterns
                static_cast<int16_t>(e)
            }}
        };
    }
};

class FillExtrusionPatternProgram : public Program<
    shaders::fill_extrusion_pattern,
    gl::Triangle,
    FillExtrusionLayoutAttributes,
    FillExtrusionPatternUniforms,
    style::FillExtrusionPaintProperties>
{
public:
    using Program::Program;
};

using FillExtrusionLayoutVertex = FillExtrusionProgram::LayoutVertex;
using FillExtrusionAttributes = FillExtrusionProgram::Attributes;

} // namespace mbgl
