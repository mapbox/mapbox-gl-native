#pragma once

#include <mbgl/gl/uniform.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {
namespace uniforms {

// Uniforms common to several shaders.

MBGL_DEFINE_UNIFORM_MATRIX(double, 4, u_matrix);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_opacity);
MBGL_DEFINE_UNIFORM_SCALAR(Color, u_color);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_blur);

MBGL_DEFINE_UNIFORM_SCALAR(float, u_zoom);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_pitch);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_bearing);

MBGL_DEFINE_UNIFORM_SCALAR(Size, u_world);

MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_extrude_scale);

MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_pattern_tl_a);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_pattern_br_a);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_pattern_tl_b);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_pattern_br_b);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_pattern_size_a);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_pattern_size_b);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_pixel_coord_upper);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_pixel_coord_lower);

MBGL_DEFINE_UNIFORM_SCALAR(float, u_mix);
MBGL_DEFINE_UNIFORM_SCALAR(gl::TextureUnit, u_image);
MBGL_DEFINE_UNIFORM_SCALAR(float,    u_scale_a);
MBGL_DEFINE_UNIFORM_SCALAR(float,    u_scale_b);
MBGL_DEFINE_UNIFORM_SCALAR(float,    u_tile_units_to_pixels);

} // namespace uniforms
} // namespace mbgl
