#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/render_line_layer.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/line_program.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>

namespace mbgl {

using namespace style;

void Painter::renderLine(PaintParameters& parameters,
                         LineBucket& bucket,
                         const RenderLineLayer& layer,
                         const RenderTile& tile) {
    if (pass == RenderPass::Opaque) {
        return;
    }

    const LinePaintProperties::Evaluated& properties = layer.evaluated;

    auto draw = [&] (auto& program, auto&& uniformValues) {
        program.get(properties).draw(
            context,
            gl::Triangles(),
            depthModeForSublayer(0, gl::DepthMode::ReadOnly),
            stencilModeForClipping(tile.clip),
            colorModeForRenderPass(),
            std::move(uniformValues),
            *bucket.vertexBuffer,
            *bucket.indexBuffer,
            bucket.segments,
            bucket.paintPropertyBinders.at(layer.getID()),
            properties,
            state.getZoom(),
            layer.getID()
        );
    };

    if (!properties.get<LineDasharray>().from.empty()) {
        const LinePatternCap cap = bucket.layout.get<LineCap>() == LineCapType::Round
            ? LinePatternCap::Round : LinePatternCap::Square;
        LinePatternPos posA = lineAtlas->getDashPosition(properties.get<LineDasharray>().from, cap);
        LinePatternPos posB = lineAtlas->getDashPosition(properties.get<LineDasharray>().to, cap);

        lineAtlas->bind(context, 0);

        draw(parameters.programs.lineSDF,
             LineSDFProgram::uniformValues(
                 properties,
                 frame.pixelRatio,
                 tile,
                 state,
                 pixelsToGLUnits,
                 posA,
                 posB,
                 layer.dashLineWidth,
                 lineAtlas->getSize().width));

    } else if (!properties.get<LinePattern>().from.empty()) {
        optional<SpriteAtlasElement> posA = spriteAtlas->getPattern(properties.get<LinePattern>().from);
        optional<SpriteAtlasElement> posB = spriteAtlas->getPattern(properties.get<LinePattern>().to);

        if (!posA || !posB)
            return;

        spriteAtlas->bind(true, context, 0);

        draw(parameters.programs.linePattern,
             LinePatternProgram::uniformValues(
                 properties,
                 tile,
                 state,
                 pixelsToGLUnits,
                 *posA,
                 *posB));

    } else {
        draw(parameters.programs.line,
             LineProgram::uniformValues(
                 properties,
                 tile,
                 state,
                 pixelsToGLUnits));
    }
}

} // namespace mbgl
