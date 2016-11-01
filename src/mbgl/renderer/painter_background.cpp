#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/background_layer_impl.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/shader/fill_uniforms.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/util/tile_cover.hpp>

namespace mbgl {

using namespace style;

void Painter::renderBackground(PaintParameters& parameters, const BackgroundLayer& layer) {
    // Note that for bottommost layers without a pattern, the background color is drawn with
    // glClear rather than this method.
    const BackgroundPaintProperties& properties = layer.impl->paint;

    if (!properties.backgroundPattern.value.to.empty()) {
        optional<SpriteAtlasPosition> imagePosA = spriteAtlas->getPosition(
            properties.backgroundPattern.value.from, SpritePatternMode::Repeating);
        optional<SpriteAtlasPosition> imagePosB = spriteAtlas->getPosition(
            properties.backgroundPattern.value.to, SpritePatternMode::Repeating);

        if (!imagePosA || !imagePosB)
            return;

        spriteAtlas->bind(true, context, 0);

        for (const auto& tileID : util::tileCover(state, state.getIntegerZoom())) {
            context.draw({
                depthModeForSublayer(0, gl::DepthMode::ReadOnly),
                gl::StencilMode::disabled(),
                colorModeForRenderPass(),
                parameters.shaders.fillPattern,
                FillPatternUniforms::values(
                    matrixForTile(tileID),
                    properties.backgroundOpacity.value,
                    context.viewport.getCurrentValue().size,
                    *imagePosA,
                    *imagePosB,
                    properties.backgroundPattern.value,
                    tileID,
                    state
                ),
                gl::Unindexed<gl::TriangleStrip>(tileTriangleVertexBuffer)
            });
        }
    } else {
        for (const auto& tileID : util::tileCover(state, state.getIntegerZoom())) {
            context.draw({
                depthModeForSublayer(0, gl::DepthMode::ReadOnly),
                gl::StencilMode::disabled(),
                colorModeForRenderPass(),
                parameters.shaders.fill,
                FillColorUniforms::values(
                    matrixForTile(tileID),
                    properties.backgroundOpacity.value,
                    properties.backgroundColor.value,
                    properties.backgroundColor.value,
                    context.viewport.getCurrentValue().size
                ),
                gl::Unindexed<gl::TriangleStrip>(tileTriangleVertexBuffer)
            });
        }
    }
}

} // namespace mbgl
