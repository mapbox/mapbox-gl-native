#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/shader/line.hpp>
#include <mbgl/shader/line_pattern.hpp>
#include <mbgl/shader/line_sdf.hpp>
#include <mbgl/style/layers/line_layer_properties.hpp>
#include <mbgl/util/geometry.hpp>

#include <cmath>

namespace mbgl {

class RenderTile;
class TransformState;
class LinePatternPos;
class SpriteAtlasPosition;

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(float, u_ratio);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_width);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_gapwidth);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_offset);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_tex_y_a);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_tex_y_b);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_sdfgamma);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_fade);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_patternscale_a);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_patternscale_b);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_gl_units_to_pixels);
} // namespace uniforms

struct LineAttributes : gl::Attributes<
    attributes::a_pos,
    attributes::a_data<4>>
{
    /*
     * @param p vertex position
     * @param e extrude normal
     * @param t texture normal
     * @param dir direction of the line cap (-1/0/1)
     */
    static Vertex vertex(Point<int16_t> p, Point<double> e, Point<bool> t, int8_t dir, int32_t linesofar = 0) {
        return Vertex {
            {
                static_cast<int16_t>((p.x * 2) | t.x),
                static_cast<int16_t>((p.y * 2) | t.y)
            },
            {
                // add 128 to store a byte in an unsigned byte
                static_cast<uint8_t>(::round(extrudeScale * e.x) + 128),
                static_cast<uint8_t>(::round(extrudeScale * e.y) + 128),

                // Encode the -1/0/1 direction value into the first two bits of .z of a_data.
                // Combine it with the lower 6 bits of `linesofar` (shifted by 2 bites to make
                // room for the direction value). The upper 8 bits of `linesofar` are placed in
                // the `w` component. `linesofar` is scaled down by `LINE_DISTANCE_SCALE` so that
                // we can store longer distances while sacrificing precision.

                // Encode the -1/0/1 direction value into .zw coordinates of a_data, which is normally covered
                // by linesofar, so we need to merge them.
                // The z component's first bit, as well as the sign bit is reserved for the direction,
                // so we need to shift the linesofar.
                static_cast<uint8_t>(((dir == 0 ? 0 : (dir < 0 ? -1 : 1 )) + 1) | ((linesofar & 0x3F) << 2)),
                static_cast<uint8_t>(linesofar >> 6)
            }
        };
    }

    /*
     * Scale the extrusion vector so that the normal length is this value.
     * Contains the "texture" normals (-1..1). This is distinct from the extrude
     * normals for line joins, because the x-value remains 0 for the texture
     * normal array, while the extrude normal actually moves the vertex to create
     * the acute/bevelled line join.
     */
    static const int8_t extrudeScale = 63;
};

using LineVertex = LineAttributes::Vertex;

class LineProgram : public Program<
    shaders::line,
    gl::Triangle,
    LineAttributes,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_opacity,
        uniforms::u_width,
        uniforms::u_gapwidth,
        uniforms::u_blur,
        uniforms::u_offset,
        uniforms::u_ratio,
        uniforms::u_gl_units_to_pixels,
        uniforms::u_color>>
{
public:
    using Program::Program;

    static UniformValues uniformValues(const style::LinePaintProperties::Evaluated&,
                                       const RenderTile&,
                                       const TransformState&,
                                       const std::array<float, 2>& pixelsToGLUnits);
};

class LinePatternProgram : public Program<
    shaders::line_pattern,
    gl::Triangle,
    LineAttributes,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_opacity,
        uniforms::u_width,
        uniforms::u_gapwidth,
        uniforms::u_blur,
        uniforms::u_offset,
        uniforms::u_ratio,
        uniforms::u_gl_units_to_pixels,
        uniforms::u_pattern_tl_a,
        uniforms::u_pattern_br_a,
        uniforms::u_pattern_tl_b,
        uniforms::u_pattern_br_b,
        uniforms::u_pattern_size_a,
        uniforms::u_pattern_size_b,
        uniforms::u_fade,
        uniforms::u_image>>
{
public:
    using Program::Program;

    static UniformValues uniformValues(const style::LinePaintProperties::Evaluated&,
                                       const RenderTile&,
                                       const TransformState&,
                                       const std::array<float, 2>& pixelsToGLUnits,
                                       const SpriteAtlasPosition& posA,
                                       const SpriteAtlasPosition& posB);
};

class LineSDFProgram : public Program<
    shaders::line_sdf,
    gl::Triangle,
    LineAttributes,
    gl::Uniforms<
        uniforms::u_matrix,
        uniforms::u_opacity,
        uniforms::u_width,
        uniforms::u_gapwidth,
        uniforms::u_blur,
        uniforms::u_offset,
        uniforms::u_ratio,
        uniforms::u_gl_units_to_pixels,
        uniforms::u_color,
        uniforms::u_patternscale_a,
        uniforms::u_patternscale_b,
        uniforms::u_tex_y_a,
        uniforms::u_tex_y_b,
        uniforms::u_mix,
        uniforms::u_sdfgamma,
        uniforms::u_image>>
{
public:
    using Program::Program;

    static UniformValues uniformValues(const style::LinePaintProperties::Evaluated&,
                                       float pixelRatio,
                                       const RenderTile&,
                                       const TransformState&,
                                       const std::array<float, 2>& pixelsToGLUnits,
                                       const LinePatternPos& posA,
                                       const LinePatternPos& posB,
                                       float dashLineWidth,
                                       float atlasWidth);
};

} // namespace mbgl
