#pragma once

#include <mbgl/gfx/uniform.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {
namespace uniforms {

// Uniforms common to several shaders.

MBGL_DEFINE_UNIFORM_MATRIX(double, 4, matrix);
MBGL_DEFINE_UNIFORM_SCALAR(float, opacity);
MBGL_DEFINE_UNIFORM_SCALAR(Color, color);
MBGL_DEFINE_UNIFORM_SCALAR(float, blur);

MBGL_DEFINE_UNIFORM_SCALAR(float, zoom);
MBGL_DEFINE_UNIFORM_SCALAR(float, collision_y_stretch);
MBGL_DEFINE_UNIFORM_SCALAR(float, pitch);
MBGL_DEFINE_UNIFORM_SCALAR(float, bearing);
MBGL_DEFINE_UNIFORM_SCALAR(float, radius);
MBGL_DEFINE_UNIFORM_SCALAR(float, stroke_width);
MBGL_DEFINE_UNIFORM_SCALAR(Color, stroke_color);
MBGL_DEFINE_UNIFORM_SCALAR(float, stroke_opacity);
MBGL_DEFINE_UNIFORM_SCALAR(Color, fill_color);
MBGL_DEFINE_UNIFORM_SCALAR(Color, halo_color);
MBGL_DEFINE_UNIFORM_SCALAR(float, halo_width);
MBGL_DEFINE_UNIFORM_SCALAR(float, halo_blur);
MBGL_DEFINE_UNIFORM_SCALAR(Color, outline_color);
MBGL_DEFINE_UNIFORM_SCALAR(float, height);
MBGL_DEFINE_UNIFORM_SCALAR(float, base);
MBGL_DEFINE_UNIFORM_SCALAR(float, width);
MBGL_DEFINE_UNIFORM_SCALAR(float, floorwidth);
MBGL_DEFINE_UNIFORM_SCALAR(float, gapwidth);
MBGL_DEFINE_UNIFORM_SCALAR(float, offset);
MBGL_DEFINE_UNIFORM_SCALAR(Size, world);
MBGL_DEFINE_UNIFORM_SCALAR(Size, texsize);
MBGL_DEFINE_UNIFORM_SCALAR(Size, texsize_icon);
MBGL_DEFINE_UNIFORM_SCALAR(bool, pitch_with_map);
MBGL_DEFINE_UNIFORM_SCALAR(float, camera_to_center_distance);
MBGL_DEFINE_UNIFORM_SCALAR(float, device_pixel_ratio);
MBGL_DEFINE_UNIFORM_SCALAR(float, fade);
MBGL_DEFINE_UNIFORM_SCALAR(float, fade_change);
MBGL_DEFINE_UNIFORM_SCALAR(float, weight);

MBGL_DEFINE_UNIFORM_VECTOR(float, 2, extrude_scale);

namespace heatmap {
MBGL_DEFINE_UNIFORM_SCALAR(float, extrude_scale);
} // namespace heatmap

MBGL_DEFINE_UNIFORM_VECTOR(uint16_t, 4, pattern_from);
MBGL_DEFINE_UNIFORM_VECTOR(uint16_t, 4, pattern_to);
MBGL_DEFINE_UNIFORM_VECTOR(float, 4, scale);
MBGL_DEFINE_UNIFORM_VECTOR(uint16_t, 2, pattern_tl_a);
MBGL_DEFINE_UNIFORM_VECTOR(uint16_t, 2, pattern_br_a);
MBGL_DEFINE_UNIFORM_VECTOR(uint16_t, 2, pattern_tl_b);
MBGL_DEFINE_UNIFORM_VECTOR(uint16_t, 2, pattern_br_b);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, pattern_size_a);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, pattern_size_b);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, pixel_coord_upper);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, pixel_coord_lower);

MBGL_DEFINE_UNIFORM_SCALAR(float, mix);
MBGL_DEFINE_UNIFORM_SCALAR(float, scale_a);
MBGL_DEFINE_UNIFORM_SCALAR(float, scale_b);
MBGL_DEFINE_UNIFORM_SCALAR(float, tile_units_to_pixels);
MBGL_DEFINE_UNIFORM_SCALAR(float, overscale_factor);

} // namespace uniforms
} // namespace mbgl
