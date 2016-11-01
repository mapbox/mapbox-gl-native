#pragma once

#include <mbgl/shader/uniforms.hpp>

namespace mbgl {

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(gl::TextureUnit, u_image0);
MBGL_DEFINE_UNIFORM_SCALAR(gl::TextureUnit, u_image1);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_opacity0);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_opacity1);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_buffer_scale);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_brightness_low);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_brightness_high);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_saturation_factor);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_contrast_factor);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_scale_parent);
MBGL_DEFINE_UNIFORM_VECTOR(float, 3, u_spin_weights);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_tl_parent);
} // namespace uniforms

struct RasterUniforms : gl::Uniforms<
    uniforms::u_matrix,
    uniforms::u_image0,
    uniforms::u_image1,
    uniforms::u_opacity0,
    uniforms::u_opacity1,
    uniforms::u_brightness_low,
    uniforms::u_brightness_high,
    uniforms::u_saturation_factor,
    uniforms::u_contrast_factor,
    uniforms::u_spin_weights,
    uniforms::u_buffer_scale,
    uniforms::u_scale_parent,
    uniforms::u_tl_parent> {};

} // namespace mbgl
