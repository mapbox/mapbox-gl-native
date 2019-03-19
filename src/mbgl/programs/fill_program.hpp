#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/programs/textures.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/style/layers/fill_layer_properties.hpp>

#include <string>

namespace mbgl {

class ImagePosition;
class UnwrappedTileID;
class TransformState;
template <class> class Faded;

using FillLayoutAttributes = PositionOnlyLayoutAttributes;

using FillUniforms = TypeList<
    uniforms::matrix,
    uniforms::world>;

using FillPatternUniforms = TypeList<
    uniforms::matrix,
    uniforms::world,
    uniforms::texsize,
    uniforms::scale,
    uniforms::fade,
    uniforms::pixel_coord_upper,
    uniforms::pixel_coord_lower>;

class FillProgram : public Program<
    FillProgram,
    gfx::PrimitiveType::Triangle,
    FillLayoutAttributes,
    FillUniforms,
    TypeList<>,
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
    FillPatternProgram,
    gfx::PrimitiveType::Triangle,
    FillLayoutAttributes,
    FillPatternUniforms,
    TypeList<
        textures::image>,
    style::FillPaintProperties>
{
public:
    using Program::Program;

    static LayoutUniformValues layoutUniformValues(mat4 matrix,
                                                   Size framebufferSize,
                                                   Size atlasSize,
                                                   const CrossfadeParameters& crossfade,
                                                   const UnwrappedTileID&,
                                                   const TransformState&,
                                                   const float pixelRatio);
};

class FillOutlineProgram : public Program<
    FillOutlineProgram,
    gfx::PrimitiveType::Line,
    FillLayoutAttributes,
    FillUniforms,
    TypeList<>,
    style::FillPaintProperties>
{
public:
    using Program::Program;
};

class FillOutlinePatternProgram : public Program<
    FillOutlinePatternProgram,
    gfx::PrimitiveType::Line,
    FillLayoutAttributes,
    FillPatternUniforms,
    TypeList<
        textures::image>,
    style::FillPaintProperties>
{
public:
    using Program::Program;
};

using FillLayoutVertex = FillProgram::LayoutVertex;
using FillAttributes = FillProgram::AttributeList;

class FillLayerPrograms final : public LayerTypePrograms {
public:
    FillLayerPrograms(gfx::Context& context, const ProgramParameters& programParameters)
        : fill(context, programParameters),
          fillPattern(context, programParameters),
          fillOutline(context, programParameters),
          fillOutlinePattern(context, programParameters) {}
    FillProgram fill;
    FillPatternProgram fillPattern;
    FillOutlineProgram fillOutline;
    FillOutlinePatternProgram fillOutlinePattern;
};

} // namespace mbgl
