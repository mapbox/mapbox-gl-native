#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/programs/textures.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/size.hpp>

namespace mbgl {

class ImagePosition;
class UnwrappedTileID;
class TransformState;
template <class> class Faded;

using BackgroundLayoutAttributes = PositionOnlyLayoutAttributes;

using BackgroundUniforms = TypeList<
    uniforms::matrix,
    uniforms::color,
    uniforms::opacity>;

using BackgroundPatternUniforms = TypeList<
    uniforms::matrix,
    uniforms::opacity,
    uniforms::texsize,
    uniforms::pattern_tl_a,
    uniforms::pattern_br_a,
    uniforms::pattern_tl_b,
    uniforms::pattern_br_b,
    uniforms::pattern_size_a,
    uniforms::pattern_size_b,
    uniforms::scale_a,
    uniforms::scale_b,
    uniforms::mix,
    uniforms::pixel_coord_upper,
    uniforms::pixel_coord_lower,
    uniforms::tile_units_to_pixels>;

class BackgroundProgram : public Program<
    BackgroundProgram,
    gfx::PrimitiveType::Triangle,
    BackgroundLayoutAttributes,
    BackgroundUniforms,
    TypeList<>,
    style::Properties<>>
{
public:
    using Program::Program;
};

class BackgroundPatternProgram : public Program<
    BackgroundPatternProgram,
    gfx::PrimitiveType::Triangle,
    BackgroundLayoutAttributes,
    BackgroundPatternUniforms,
    TypeList<
        textures::image>,
    style::Properties<>>
{
public:
    using Program::Program;

    static LayoutUniformValues layoutUniformValues(mat4 matrix,
                                                   float opacity,
                                                   Size atlasSize,
                                                   const ImagePosition&,
                                                   const ImagePosition&,
                                                   const CrossfadeParameters&,
                                                   const UnwrappedTileID&,
                                                   const TransformState&);
};

using BackgroundLayoutVertex = BackgroundProgram::LayoutVertex;
using BackgroundAttributes = BackgroundProgram::AttributeList;

class BackgroundLayerPrograms final : public LayerTypePrograms  {
public:
    BackgroundLayerPrograms(gfx::Context& context, const ProgramParameters& programParameters)
        : background(context, programParameters),
          backgroundPattern(context, programParameters) {}
    BackgroundProgram background;
    BackgroundPatternProgram backgroundPattern;
};

} // namespace mbgl
