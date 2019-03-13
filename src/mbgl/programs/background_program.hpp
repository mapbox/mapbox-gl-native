#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/programs/textures.hpp>
#include <mbgl/shaders/background.hpp>
#include <mbgl/shaders/background_pattern.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/style/layers/background_layer_properties.hpp>

#include <string>

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
    shaders::background,
    gfx::Triangle,
    BackgroundLayoutAttributes,
    BackgroundUniforms,
    TypeList<>,
    style::Properties<>>
{
public:
    using Program::Program;
};

class BackgroundPatternProgram : public Program<
    shaders::background_pattern,
    gfx::Triangle,
    BackgroundLayoutAttributes,
    BackgroundPatternUniforms,
    TypeList<
        textures::u_image>,
    style::Properties<>>
{
public:
    using Program::Program;

    static UniformValues uniformValues(mat4 matrix,
                                       float opacity,
                                       Size atlasSize,
                                       const ImagePosition&,
                                       const ImagePosition&,
                                       const CrossfadeParameters&,
                                       const UnwrappedTileID&,
                                       const TransformState&);
};

using BackgroundLayoutVertex = BackgroundProgram::LayoutVertex;
using BackgroundAttributes = BackgroundProgram::Attributes;

class BackgroundLayerPrograms final : public LayerTypePrograms  {
public:
    BackgroundLayerPrograms(gl::Context& context, const ProgramParameters& programParameters)
        : background(context, programParameters),
          backgroundPattern(context, programParameters) {}
    BackgroundProgram background;
    BackgroundPatternProgram backgroundPattern;
};

} // namespace mbgl
