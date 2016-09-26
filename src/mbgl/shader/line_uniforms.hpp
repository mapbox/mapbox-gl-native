#pragma once

#include <mbgl/shader/uniforms.hpp>

namespace mbgl {

namespace style {
class LinePaintProperties;
} // namespace style

class RenderTile;
class TransformState;
class LinePatternPos;
class SpriteAtlasPosition;

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(float, u_ratio);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_linewidth);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_gapwidth);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_antialiasing);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_extra);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_offset);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_tex_y_a);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_tex_y_b);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_sdfgamma);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_fade);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_patternscale_a);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_patternscale_b);
MBGL_DEFINE_UNIFORM_MATRIX(double, 2, u_antialiasingmatrix);
} // namespace uniforms

struct LineColorUniforms : gl::Uniforms<
    uniforms::u_matrix,
    uniforms::u_opacity,
    uniforms::u_linewidth,
    uniforms::u_gapwidth,
    uniforms::u_blur,
    uniforms::u_offset,
    uniforms::u_antialiasing,
    uniforms::u_antialiasingmatrix,
    uniforms::u_ratio,
    uniforms::u_extra,
    uniforms::u_color>
{
    static Values values(const style::LinePaintProperties&,
                         float pixelRatio,
                         const RenderTile&,
                         const TransformState&);
};

struct LineSDFUniforms : gl::Uniforms<
    uniforms::u_matrix,
    uniforms::u_opacity,
    uniforms::u_linewidth,
    uniforms::u_gapwidth,
    uniforms::u_blur,
    uniforms::u_offset,
    uniforms::u_antialiasing,
    uniforms::u_antialiasingmatrix,
    uniforms::u_ratio,
    uniforms::u_extra,
    uniforms::u_color,
    uniforms::u_patternscale_a,
    uniforms::u_patternscale_b,
    uniforms::u_tex_y_a,
    uniforms::u_tex_y_b,
    uniforms::u_mix,
    uniforms::u_sdfgamma,
    uniforms::u_image>
{
    static Values values(const style::LinePaintProperties&,
                         float pixelRatio,
                         const RenderTile&,
                         const TransformState&,
                         const LinePatternPos& posA,
                         const LinePatternPos& posB,
                         float dashLineWidth,
                         float atlasWidth);
};

struct LinePatternUniforms : gl::Uniforms<
    uniforms::u_matrix,
    uniforms::u_opacity,
    uniforms::u_linewidth,
    uniforms::u_gapwidth,
    uniforms::u_blur,
    uniforms::u_offset,
    uniforms::u_antialiasing,
    uniforms::u_antialiasingmatrix,
    uniforms::u_ratio,
    uniforms::u_extra,
    uniforms::u_pattern_tl_a,
    uniforms::u_pattern_br_a,
    uniforms::u_pattern_tl_b,
    uniforms::u_pattern_br_b,
    uniforms::u_pattern_size_a,
    uniforms::u_pattern_size_b,
    uniforms::u_fade,
    uniforms::u_image>
{
    static Values values(const style::LinePaintProperties&,
                         float pixelRatio,
                         const RenderTile&,
                         const TransformState&,
                         const SpriteAtlasPosition& posA,
                         const SpriteAtlasPosition& posB);
};

} // namespace mbgl
