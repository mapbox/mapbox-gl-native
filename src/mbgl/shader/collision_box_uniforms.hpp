#pragma once

#include <mbgl/shader/uniforms.hpp>

namespace mbgl {

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(float, u_scale);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_maxzoom);
} // namespace uniforms

struct CollisionBoxUniforms : gl::Uniforms<
    uniforms::u_matrix,
    uniforms::u_scale,
    uniforms::u_zoom,
    uniforms::u_maxzoom> {};

} // namespace mbgl
