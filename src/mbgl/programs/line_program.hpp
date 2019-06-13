#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/programs/textures.hpp>
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
MBGL_DEFINE_UNIFORM_SCALAR(float, ratio);
MBGL_DEFINE_UNIFORM_SCALAR(float, tex_y_a);
MBGL_DEFINE_UNIFORM_SCALAR(float, tex_y_b);
MBGL_DEFINE_UNIFORM_SCALAR(float, sdfgamma);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, patternscale_a);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, patternscale_b);
MBGL_DEFINE_UNIFORM_VECTOR(float, 2, units_to_pixels);
} // namespace uniforms

using LineLayoutAttributes = TypeList<
    attributes::pos_normal,
    attributes::data<uint8_t, 4>>;

class LineProgram : public Program<
    LineProgram,
    gfx::PrimitiveType::Triangle,
    LineLayoutAttributes,
    TypeList<
        uniforms::matrix,
        uniforms::ratio,
        uniforms::units_to_pixels,
        uniforms::device_pixel_ratio>,
    TypeList<>,
    style::LinePaintProperties>
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
                static_cast<int16_t>((p.x * 2) | (round ? 1 : 0)),
                static_cast<int16_t>((p.y * 2) | (up ? 1 : 0))
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
    layoutUniformValues(const style::LinePaintProperties::PossiblyEvaluated&,
                        const RenderTile&,
                        const TransformState&,
                        const std::array<float, 2>& pixelsToGLUnits,
                        const float pixelRatio);
};

class LinePatternProgram : public Program<
    LinePatternProgram,
    gfx::PrimitiveType::Triangle,
    LineLayoutAttributes,
    TypeList<
        uniforms::matrix,
        uniforms::ratio,
        uniforms::units_to_pixels,
        uniforms::device_pixel_ratio,
        uniforms::scale,
        uniforms::texsize,
        uniforms::fade>,
    TypeList<
        textures::image>,
    style::LinePaintProperties>
{
public:
    using Program::Program;

    static LayoutUniformValues
    layoutUniformValues(const style::LinePaintProperties::PossiblyEvaluated&,
                        const RenderTile&,
                        const TransformState&,
                        const std::array<float, 2>& pixelsToGLUnits,
                        const float pixelRatio,
                        Size atlasSize,
                        const CrossfadeParameters& crossfade);
};

class LineSDFProgram : public Program<
    LineSDFProgram,
    gfx::PrimitiveType::Triangle,
    LineLayoutAttributes,
    TypeList<
        uniforms::matrix,
        uniforms::ratio,
        uniforms::units_to_pixels,
        uniforms::device_pixel_ratio,
        uniforms::patternscale_a,
        uniforms::patternscale_b,
        uniforms::tex_y_a,
        uniforms::tex_y_b,
        uniforms::mix,
        uniforms::sdfgamma>,
    TypeList<
        textures::image>,
    style::LinePaintProperties>
{
public:
    using Program::Program;

    static LayoutUniformValues
    layoutUniformValues(const style::LinePaintProperties::PossiblyEvaluated&,
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
    LineGradientProgram,
    gfx::PrimitiveType::Triangle,
    LineLayoutAttributes,
    TypeList<
        uniforms::matrix,
        uniforms::ratio,
        uniforms::units_to_pixels,
        uniforms::device_pixel_ratio>,
    TypeList<
        textures::image>,
    style::LinePaintProperties>
{
public:
    using Program::Program;

    static LayoutUniformValues
    layoutUniformValues(const style::LinePaintProperties::PossiblyEvaluated&,
                        const RenderTile&,
                        const TransformState&,
                        const std::array<float, 2>& pixelsToGLUnits,
                        const float pixelRatio);
};

using LineLayoutVertex = LineProgram::LayoutVertex;
using LineAttributes = LineProgram::AttributeList;

class LineLayerPrograms final : public LayerTypePrograms {
public:
    LineLayerPrograms(gfx::Context& context, const ProgramParameters& programParameters)
        : line(context, programParameters),
          lineGradient(context, programParameters),
          lineSDF(context, programParameters),
          linePattern(context, programParameters) {}
    LineProgram line;
    LineGradientProgram lineGradient;
    LineSDFProgram lineSDF;
    LinePatternProgram linePattern;
};

} // namespace mbgl
