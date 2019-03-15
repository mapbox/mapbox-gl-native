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
    uniforms::u_matrix,
    uniforms::u_color,
    uniforms::u_opacity>;

using BackgroundPatternUniforms = TypeList<
    uniforms::u_matrix,
    uniforms::u_opacity,
    uniforms::u_texsize,
    uniforms::u_pattern_tl_a,
    uniforms::u_pattern_br_a,
    uniforms::u_pattern_tl_b,
    uniforms::u_pattern_br_b,
    uniforms::u_pattern_size_a,
    uniforms::u_pattern_size_b,
    uniforms::u_scale_a,
    uniforms::u_scale_b,
    uniforms::u_mix,
    uniforms::u_pixel_coord_upper,
    uniforms::u_pixel_coord_lower,
    uniforms::u_tile_units_to_pixels>;

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
        textures::u_image>,
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
