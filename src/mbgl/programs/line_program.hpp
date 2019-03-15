#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/programs/textures.hpp>
#include <mbgl/shaders/line.hpp>
#include <mbgl/shaders/line_gradient.hpp>
#include <mbgl/shaders/line_pattern.hpp>
#include <mbgl/shaders/line_sdf.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/renderer/layers/render_line_layer.hpp>
#include <mbgl/renderer/cross_faded_property_evaluator.hpp>
#include <cmath>

namespace mbgl {

class RenderTile;
class TransformState;
class LinePatternPos;
class ImagePosition;

namespace uniforms {
MBGL_DEFINE_UNIFORM_SCALAR(float, u_ratio);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_tex_y_a);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_tex_y_b);
MBGL_DEFINE_UNIFORM_SCALAR(float, u_sdfgamma);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_patternscale_a);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_patternscale_b);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, u_gl_units_to_pixels);
} // namespace uniforms

using LineLayoutAttributes = TypeList<
    attributes::a_pos_normal,
    attributes::a_data<uint8_t, 4>>;

class LineProgram : public Program<
    shaders::line,
    gfx::PrimitiveType::Triangle,
    LineLayoutAttributes,
    TypeList<
        uniforms::u_matrix,
        uniforms::u_ratio,
        uniforms::u_gl_units_to_pixels>,
    TypeList<>,
    RenderLinePaintProperties>
{
public:
    using Program::Program;

    /*
     * @param p vertex position
     * @param e extrude normal
     * @param round whether the vertex uses a round line cap
     * @param up whether the line normal points up or down
     * @param dir direction of the line cap (-1/0/1)
     */
    static LayoutVertex layoutVertex(Point<int16_t> p, Point<double> e, bool round, bool up, int8_t dir, int32_t linesofar = 0) {
        return LayoutVertex {
            {{
                p.x,
                p.y,
                static_cast<int16_t>(round ? 1 : 0),
                static_cast<int16_t>(up ? 1 : -1)
            }},
            {{
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
            }}
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

    static LayoutUniformValues
    layoutUniformValues(const RenderLinePaintProperties::PossiblyEvaluated&,
                        const RenderTile&,
                        const TransformState&,
                        const std::array<float, 2>& pixelsToGLUnits);
};

class LinePatternProgram : public Program<
    shaders::line_pattern,
    gfx::PrimitiveType::Triangle,
    LineLayoutAttributes,
    TypeList<
        uniforms::u_matrix,
        uniforms::u_ratio,
        uniforms::u_gl_units_to_pixels,
        uniforms::u_scale,
        uniforms::u_texsize,
        uniforms::u_fade>,
    TypeList<
        textures::u_image>,
    RenderLinePaintProperties>
{
public:
    using Program::Program;

    static LayoutUniformValues
    layoutUniformValues(const RenderLinePaintProperties::PossiblyEvaluated&,
                        const RenderTile&,
                        const TransformState&,
                        const std::array<float, 2>& pixelsToGLUnits,
                        Size atlasSize,
                        const CrossfadeParameters& crossfade,
                        const float pixelRatio);
};

class LineSDFProgram : public Program<
    shaders::line_sdf,
    gfx::PrimitiveType::Triangle,
    LineLayoutAttributes,
    TypeList<
        uniforms::u_matrix,
        uniforms::u_ratio,
        uniforms::u_gl_units_to_pixels,
        uniforms::u_patternscale_a,
        uniforms::u_patternscale_b,
        uniforms::u_tex_y_a,
        uniforms::u_tex_y_b,
        uniforms::u_mix,
        uniforms::u_sdfgamma>,
    TypeList<
        textures::u_image>,
    RenderLinePaintProperties>
{
public:
    using Program::Program;

    static LayoutUniformValues
    layoutUniformValues(const RenderLinePaintProperties::PossiblyEvaluated&,
                        float pixelRatio,
                        const RenderTile&,
                        const TransformState&,
                        const std::array<float, 2>& pixelsToGLUnits,
                        const LinePatternPos& posA,
                        const LinePatternPos& posB,
                        const CrossfadeParameters& crossfade,
                        float atlasWidth);
};

class LineGradientProgram : public Program<
    shaders::line_gradient,
    gfx::PrimitiveType::Triangle,
    LineLayoutAttributes,
    TypeList<
        uniforms::u_matrix,
        uniforms::u_ratio,
        uniforms::u_gl_units_to_pixels>,
    TypeList<
        textures::u_image>,
    RenderLinePaintProperties>
{
public:
    using Program::Program;

    static LayoutUniformValues
    layoutUniformValues(const RenderLinePaintProperties::PossiblyEvaluated&,
                        const RenderTile&,
                        const TransformState&,
                        const std::array<float, 2>& pixelsToGLUnits);
};

using LineLayoutVertex = LineProgram::LayoutVertex;
using LineAttributes = LineProgram::AttributeList;

class LineLayerPrograms final : public LayerTypePrograms {
public:
    LineLayerPrograms(gl::Context& context, const ProgramParameters& programParameters)
        : line(context, programParameters),
          lineGradient(context, programParameters),
          lineSDF(context, programParameters),
          linePattern(context, programParameters) {}
    ProgramMap<LineProgram> line;
    ProgramMap<LineGradientProgram> lineGradient;
    ProgramMap<LineSDFProgram> lineSDF;
    ProgramMap<LinePatternProgram> linePattern;
};

} // namespace mbgl
