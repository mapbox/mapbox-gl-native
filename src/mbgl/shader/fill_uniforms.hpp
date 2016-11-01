#pragma once

#include <mbgl/shader/uniforms.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/size.hpp>

#include <string>

namespace mbgl {

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(Size,     u_world);
MBGL_DEFINE_UNIFORM_SCALAR(Color,    u_outline_color);
MBGL_DEFINE_UNIFORM_SCALAR(float,    u_scale_a);
MBGL_DEFINE_UNIFORM_SCALAR(float,    u_scale_b);
MBGL_DEFINE_UNIFORM_SCALAR(float,    u_tile_units_to_pixels);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_pixel_coord_upper);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_pixel_coord_lower);
} // namespace uniforms

struct FillColorUniforms : gl::Uniforms<
    uniforms::u_matrix,
    uniforms::u_opacity,
    uniforms::u_color,
    uniforms::u_outline_color,
    uniforms::u_world> {};

class SpriteAtlasPosition;
class UnwrappedTileID;
class TransformState;

namespace style {
template <class> class Faded;
} // namespace style

struct FillPatternUniforms : gl::Uniforms<
    uniforms::u_matrix,
    uniforms::u_opacity,
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
    uniforms::u_tile_units_to_pixels> {
    static Values values(mat4 matrix,
                         float opacity,
                         Size framebufferSize,
                         const SpriteAtlasPosition&,
                         const SpriteAtlasPosition&,
                         const style::Faded<std::string>&,
                         const UnwrappedTileID&,
                         const TransformState&);
};

} // namespace mbgl
