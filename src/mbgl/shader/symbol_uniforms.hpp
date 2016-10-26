#pragma once

#include <mbgl/shader/uniforms.hpp>
#include <mbgl/util/size.hpp>

#include <array>

namespace mbgl {

namespace style {
class SymbolPropertyValues;
} // namespace style

class RenderTile;
class TransformState;

namespace uniforms {
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_texsize);
MBGL_DEFINE_UNIFORM_SCALAR(bool, u_rotate_with_map);
MBGL_DEFINE_UNIFORM_SCALAR(bool, u_pitch_with_map);
MBGL_DEFINE_UNIFORM_SCALAR(gl::TextureUnit, u_texture);
MBGL_DEFINE_UNIFORM_SCALAR(gl::TextureUnit, u_fadetexture);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_buffer);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_gamma);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_aspect_ratio);
} // namespace uniforms

struct SymbolIconUniforms : gl::Uniforms<
    uniforms::u_matrix,
    uniforms::u_opacity,
    uniforms::u_extrude_scale,
    uniforms::u_texsize,
    uniforms::u_zoom,
    uniforms::u_rotate_with_map,
    uniforms::u_texture,
    uniforms::u_fadetexture>
{
    static Values values(const style::SymbolPropertyValues&,
                         const Size& texsize,
                         const std::array<float, 2>& pixelsToGLUnits,
                         const RenderTile&,
                         const TransformState&);
};

struct SymbolSDFUniforms : gl::Uniforms<
    uniforms::u_matrix,
    uniforms::u_opacity,
    uniforms::u_extrude_scale,
    uniforms::u_texsize,
    uniforms::u_zoom,
    uniforms::u_rotate_with_map,
    uniforms::u_texture,
    uniforms::u_fadetexture,
    uniforms::u_color,
    uniforms::u_buffer,
    uniforms::u_gamma,
    uniforms::u_pitch,
    uniforms::u_bearing,
    uniforms::u_aspect_ratio,
    uniforms::u_pitch_with_map>
{
    static Values haloValues(const style::SymbolPropertyValues&,
                              const Size& texsize,
                              const std::array<float, 2>& pixelsToGLUnits,
                              const RenderTile&,
                              const TransformState&,
                              float pixelRatio);

    static Values foregroundValues(const style::SymbolPropertyValues&,
                              const Size& texsize,
                              const std::array<float, 2>& pixelsToGLUnits,
                              const RenderTile&,
                              const TransformState&,
                              float pixelRatio);
};

} // namespace mbgl
