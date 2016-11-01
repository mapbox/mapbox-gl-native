#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/line_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/shader/line_uniforms.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/geometry/line_atlas.hpp>

namespace mbgl {

using namespace style;

void Painter::renderLine(PaintParameters& parameters,
                         LineBucket& bucket,
                         const LineLayer& layer,
                         const RenderTile& tile) {
    if (pass == RenderPass::Opaque) {
        return;
    }

    const auto& properties = layer.impl->paint;

    auto draw = [&] (auto& shader, auto&& uniformValues) {
        context.draw({
            depthModeForSublayer(0, gl::DepthMode::ReadOnly),
            stencilModeForClipping(tile.clip),
            colorModeForRenderPass(),
            shader,
            std::move(uniformValues),
            gl::Segmented<gl::Triangles>(
                *bucket.vertexBuffer,
                *bucket.indexBuffer,
                bucket.segments
            )
        });
    };

    if (!properties.lineDasharray.value.from.empty()) {
        const LinePatternCap cap = bucket.layout.lineCap == LineCapType::Round
            ? LinePatternCap::Round : LinePatternCap::Square;
        LinePatternPos posA = lineAtlas->getDashPosition(properties.lineDasharray.value.from, cap);
        LinePatternPos posB = lineAtlas->getDashPosition(properties.lineDasharray.value.to, cap);

        lineAtlas->bind(context, 0);

        draw(parameters.shaders.lineSDF,
             LineSDFUniforms::values(
                 properties,
                 frame.pixelRatio,
                 tile,
                 state,
                 posA,
                 posB,
                 layer.impl->dashLineWidth,
                 lineAtlas->getSize().width));

    } else if (!properties.linePattern.value.from.empty()) {
        optional<SpriteAtlasPosition> posA = spriteAtlas->getPosition(
            properties.linePattern.value.from, SpritePatternMode::Repeating);
        optional<SpriteAtlasPosition> posB = spriteAtlas->getPosition(
            properties.linePattern.value.to, SpritePatternMode::Repeating);

        if (!posA || !posB)
            return;

        spriteAtlas->bind(true, context, 0);

        draw(parameters.shaders.linePattern,
             LinePatternUniforms::values(
                 properties,
                 frame.pixelRatio,
                 tile,
                 state,
                 *posA,
                 *posB));

    } else {
        draw(parameters.shaders.line,
             LineColorUniforms::values(
                 properties,
                 frame.pixelRatio,
                 tile,
                 state));
    }
}

} // namespace mbgl
