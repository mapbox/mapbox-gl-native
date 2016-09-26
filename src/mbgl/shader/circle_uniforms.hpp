#pragma once

#include <mbgl/shader/uniforms.hpp>

namespace mbgl {

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(float, u_radius);
MBGL_DEFINE_UNIFORM_SCALAR(bool, u_scale_with_map);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_devicepixelratio);
} // namespace uniforms

struct CircleUniforms : gl::Uniforms<
    uniforms::u_matrix,
    uniforms::u_opacity,
    uniforms::u_color,
    uniforms::u_radius,
    uniforms::u_blur,
    uniforms::u_scale_with_map,
    uniforms::u_extrude_scale,
    uniforms::u_devicepixelratio> {};

} // namespace mbgl
