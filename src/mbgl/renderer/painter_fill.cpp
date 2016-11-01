#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/shader/fill_uniforms.hpp>
#include <mbgl/util/convert.hpp>

namespace mbgl {

using namespace style;

void Painter::renderFill(PaintParameters& parameters,
                         FillBucket& bucket,
                         const FillLayer& layer,
                         const RenderTile& tile) {
    const FillPaintProperties& properties = layer.impl->paint;

    if (!properties.fillPattern.value.from.empty()) {
        if (pass != RenderPass::Translucent) {
            return;
        }

        optional<SpriteAtlasPosition> imagePosA = spriteAtlas->getPosition(
            properties.fillPattern.value.from, SpritePatternMode::Repeating);
        optional<SpriteAtlasPosition> imagePosB = spriteAtlas->getPosition(
            properties.fillPattern.value.to, SpritePatternMode::Repeating);

        if (!imagePosA || !imagePosB) {
            return;
        }

        spriteAtlas->bind(true, context, 0);

        auto draw = [&] (uint8_t sublayer, auto& shader, const auto& subject) {
            context.draw({
                depthModeForSublayer(sublayer, gl::DepthMode::ReadWrite),
                stencilModeForClipping(tile.clip),
                colorModeForRenderPass(),
                shader,
                FillPatternUniforms::values(
                    tile.translatedMatrix(properties.fillTranslate.value,
                                          properties.fillTranslateAnchor.value,
                                          state),
                    properties.fillOpacity.value,
                    context.viewport.getCurrentValue().size,
                    *imagePosA,
                    *imagePosB,
                    properties.fillPattern.value,
                    tile.id,
                    state
                ),
                subject
            });
        };

        draw(0,
             parameters.shaders.fillPattern,
             gl::Segmented<gl::Triangles>(
                 *bucket.vertexBuffer,
                 *bucket.triangleIndexBuffer,
                 bucket.triangleSegments));

        if (!properties.fillAntialias.value || !properties.fillOutlineColor.isUndefined()) {
            return;
        }

        draw(2,
             parameters.shaders.fillOutlinePattern,
             gl::Segmented<gl::Lines>(
                 *bucket.vertexBuffer,
                 *bucket.lineIndexBuffer,
                 bucket.lineSegments,
                 2.0f));
    } else {
        auto draw = [&] (uint8_t sublayer, auto& shader, Color outlineColor, const auto& subject) {
            context.draw({
                depthModeForSublayer(sublayer, gl::DepthMode::ReadWrite),
                stencilModeForClipping(tile.clip),
                colorModeForRenderPass(),
                shader,
                FillColorUniforms::values(
                    tile.translatedMatrix(properties.fillTranslate.value,
                                          properties.fillTranslateAnchor.value,
                                          state),
                    properties.fillOpacity.value,
                    properties.fillColor.value,
                    outlineColor,
                    context.viewport.getCurrentValue().size
                ),
                subject
            });
        };

        if (properties.fillAntialias.value && !properties.fillOutlineColor.isUndefined() && pass == RenderPass::Translucent) {
            draw(2,
                 parameters.shaders.fillOutline,
                 properties.fillOutlineColor.value,
                 gl::Segmented<gl::Lines>(
                     *bucket.vertexBuffer,
                     *bucket.lineIndexBuffer,
                     bucket.lineSegments,
                     2.0f));
        }

        // Only draw the fill when it's opaque and we're drawing opaque fragments,
        // or when it's translucent and we're drawing translucent fragments.
        if ((properties.fillColor.value.a >= 1.0f && properties.fillOpacity.value >= 1.0f) == (pass == RenderPass::Opaque)) {
            draw(1,
                 parameters.shaders.fill,
                 properties.fillOutlineColor.value,
                 gl::Segmented<gl::Triangles>(
                     *bucket.vertexBuffer,
                     *bucket.triangleIndexBuffer,
                     bucket.triangleSegments));
        }

        if (properties.fillAntialias.value && properties.fillOutlineColor.isUndefined() && pass == RenderPass::Translucent) {
            draw(2,
                 parameters.shaders.fillOutline,
                 properties.fillColor.value,
                 gl::Segmented<gl::Lines>(
                     *bucket.vertexBuffer,
                     *bucket.lineIndexBuffer,
                     bucket.lineSegments,
                     2.0f));
        }
    }
}

} // namespace mbgl
