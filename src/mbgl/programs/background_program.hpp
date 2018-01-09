#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
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

struct BackgroundUniforms : gl::Uniforms<
    uniforms::u_matrix,
    uniforms::u_color,
    uniforms::u_opacity>
{};

struct BackgroundPatternUniforms : gl::Uniforms<
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
    uniforms::u_image,
    uniforms::u_pixel_coord_upper,
    uniforms::u_pixel_coord_lower,
    uniforms::u_tile_units_to_pixels>
{
    static Values values(mat4 matrix,
                         float opacity,
                         Size atlasSize,
                         const ImagePosition&,
                         const ImagePosition&,
                         const Faded<std::string>&,
                         const UnwrappedTileID&,
                         const TransformState&);
};

class BackgroundProgram : public Program<
    shaders::background,
    gl::Triangle,
    BackgroundLayoutAttributes,
    BackgroundUniforms,
    style::Properties<>>
{
public:
    using Program::Program;
};

class BackgroundPatternProgram : public Program<
    shaders::background_pattern,
    gl::Triangle,
    BackgroundLayoutAttributes,
    BackgroundPatternUniforms,
    style::Properties<>>
{
public:
    using Program::Program;
};

using BackgroundLayoutVertex = BackgroundProgram::LayoutVertex;
using BackgroundAttributes = BackgroundProgram::Attributes;

} // namespace mbgl
