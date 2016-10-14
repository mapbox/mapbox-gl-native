#pragma once

#include <mbgl/shader/uniforms.hpp>

namespace mbgl {

namespace uniforms {
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_dimension);
MBGL_DEFINE_UNIFORM_SCALAR(int32_t, u_mode);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_azimuth);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_zenith);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_mipmap);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_exaggeration);
MBGL_DEFINE_UNIFORM_SCALAR(Color, u_shadow);
MBGL_DEFINE_UNIFORM_SCALAR(Color, u_highlight);
MBGL_DEFINE_UNIFORM_SCALAR(Color, u_accent);
} // namespace uniforms

struct TerrainPrepareUniforms : gl::Uniforms<
    uniforms::u_matrix,
    uniforms::u_image,
    uniforms::u_dimension,
    uniforms::u_zoom> {};

struct TerrainUniforms : gl::Uniforms<
    uniforms::u_matrix,
    uniforms::u_image,
    uniforms::u_mode,
    uniforms::u_dimension,
    uniforms::u_zoom,
    uniforms::u_azimuth,
    uniforms::u_zenith,
    uniforms::u_mipmap,
    uniforms::u_exaggeration,
    uniforms::u_shadow,
    uniforms::u_highlight,
    uniforms::u_accent> {};

} // namespace mbgl
