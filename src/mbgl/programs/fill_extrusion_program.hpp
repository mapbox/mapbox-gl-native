#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/programs/textures.hpp>
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
MBGL_DEFINE_UNIFORM_VECTOR(float, 3, lightpos);
MBGL_DEFINE_UNIFORM_VECTOR(float, 3, lightcolor);
MBGL_DEFINE_UNIFORM_SCALAR(float,    lightintensity);
MBGL_DEFINE_UNIFORM_SCALAR(float,    vertical_gradient);
MBGL_DEFINE_UNIFORM_SCALAR(float,    height_factor);
} // namespace uniforms

using FillExtrusionLayoutAttributes = TypeList<
    attributes::pos,
    attributes::normal_ed>;

using FillExtrusionUniforms = TypeList<
    uniforms::matrix,
    uniforms::opacity,
    uniforms::lightcolor,
    uniforms::lightpos,
    uniforms::lightintensity,
    uniforms::vertical_gradient>;

using FillExtrusionPatternUniforms = TypeList<
    uniforms::matrix,
    uniforms::opacity,
    uniforms::scale,
    uniforms::texsize,
    uniforms::fade,
    uniforms::pixel_coord_upper,
    uniforms::pixel_coord_lower,
    uniforms::height_factor,
    uniforms::lightcolor,
    uniforms::lightpos,
    uniforms::lightintensity,
    uniforms::vertical_gradient>;

class FillExtrusionProgram : public Program<
    FillExtrusionProgram,
    gfx::PrimitiveType::Triangle,
    FillExtrusionLayoutAttributes,
    FillExtrusionUniforms,
    TypeList<>,
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

    static LayoutUniformValues
    layoutUniformValues(mat4, const TransformState&, const float opacity, const EvaluatedLight&, const float verticalGradient);
};

class FillExtrusionPatternProgram : public Program<
    FillExtrusionPatternProgram,
    gfx::PrimitiveType::Triangle,
    FillExtrusionLayoutAttributes,
    FillExtrusionPatternUniforms,
    TypeList<
        textures::image>,
    style::FillExtrusionPaintProperties>
{
public:
    using Program::Program;

    static LayoutUniformValues layoutUniformValues(mat4,
                                                   Size atlasSize,
                                                   const CrossfadeParameters&,
                                                   const UnwrappedTileID&,
                                                   const TransformState&,
                                                   const float opacity,
                                                   const float heightFactor,
                                                   const float pixelRatio,
                                                   const EvaluatedLight&,
                                                   const float verticalGradient);
};

using FillExtrusionLayoutVertex = FillExtrusionProgram::LayoutVertex;
using FillExtrusionAttributes = FillExtrusionProgram::AttributeList;


class FillExtrusionLayerPrograms final : public LayerTypePrograms {
public:
    FillExtrusionLayerPrograms(gfx::Context& context, const ProgramParameters& programParameters)
        : fillExtrusion(context, programParameters),
          fillExtrusionPattern(context, programParameters) {
    }
    FillExtrusionProgram fillExtrusion;
    FillExtrusionPatternProgram fillExtrusionPattern;
};

} // namespace mbgl
