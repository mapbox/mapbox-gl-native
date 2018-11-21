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
MBGL_DEFINE_UNIFORM_SCALAR(float, u_collision_y_stretch);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_pitch);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_bearing);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_radius);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_stroke_width);
MBGL_DEFINE_UNIFORM_SCALAR(Color, u_stroke_color);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_stroke_opacity);
MBGL_DEFINE_UNIFORM_SCALAR(Color, u_fill_color);
MBGL_DEFINE_UNIFORM_SCALAR(Color, u_halo_color);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_halo_width);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_halo_blur);
MBGL_DEFINE_UNIFORM_SCALAR(Color, u_outline_color);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_height);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_base);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_width);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_floorwidth);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_gapwidth);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_offset);
MBGL_DEFINE_UNIFORM_SCALAR(Size, u_world);
MBGL_DEFINE_UNIFORM_SCALAR(Size, u_texsize);
MBGL_DEFINE_UNIFORM_SCALAR(bool, u_pitch_with_map);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_camera_to_center_distance);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_fade);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_fade_change);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_weight);

MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_extrude_scale);

namespace heatmap {
MBGL_DEFINE_UNIFORM_SCALAR(float, u_extrude_scale);
} // namespace heatmap

MBGL_DEFINE_UNIFORM_VECTOR(uint16_t, 4, u_pattern_from);
MBGL_DEFINE_UNIFORM_VECTOR(uint16_t, 4, u_pattern_to);
MBGL_DEFINE_UNIFORM_VECTOR(float, 4, u_scale);
MBGL_DEFINE_UNIFORM_VECTOR(uint16_t, 2, u_pattern_tl_a);
MBGL_DEFINE_UNIFORM_VECTOR(uint16_t, 2, u_pattern_br_a);
MBGL_DEFINE_UNIFORM_VECTOR(uint16_t, 2, u_pattern_tl_b);
MBGL_DEFINE_UNIFORM_VECTOR(uint16_t, 2, u_pattern_br_b);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_pattern_size_a);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_pattern_size_b);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_pixel_coord_upper);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_pixel_coord_lower);

MBGL_DEFINE_UNIFORM_SCALAR(float, u_mix);
MBGL_DEFINE_UNIFORM_SCALAR(gl::TextureUnit, u_image);
MBGL_DEFINE_UNIFORM_SCALAR(gl::TextureUnit, u_fadetexture);
MBGL_DEFINE_UNIFORM_SCALAR(float,    u_scale_a);
MBGL_DEFINE_UNIFORM_SCALAR(float,    u_scale_b);
MBGL_DEFINE_UNIFORM_SCALAR(float,    u_tile_units_to_pixels);
MBGL_DEFINE_UNIFORM_SCALAR(float,    u_overscale_factor);

} // namespace uniforms
} // namespace mbgl
