#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/fill_program.hpp>
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

        auto draw = [&] (uint8_t sublayer,
                         auto& program,
                         const auto& drawMode,
                         const auto& vertexBuffer,
                         const auto& indexBuffer,
                         const auto& segments) {
            program.draw(
                context,
                drawMode,
                depthModeForSublayer(sublayer, gl::DepthMode::ReadWrite),
                stencilModeForClipping(tile.clip),
                colorModeForRenderPass(),
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
                vertexBuffer,
                indexBuffer,
                segments
            );
        };

        draw(0,
             parameters.programs.fillPattern,
             gl::Triangles(),
             *bucket.vertexBuffer,
             *bucket.triangleIndexBuffer,
             bucket.triangleSegments);

        if (!properties.fillAntialias.value || !properties.fillOutlineColor.isUndefined()) {
            return;
        }

        draw(2,
             parameters.programs.fillOutlinePattern,
             gl::Lines { 2.0f },
             *bucket.vertexBuffer,
             *bucket.lineIndexBuffer,
             bucket.lineSegments);
    } else {
        auto draw = [&] (uint8_t sublayer,
                         auto& program,
                         Color outlineColor,
                         const auto& drawMode,
                         const auto& vertexBuffer,
                         const auto& indexBuffer,
                         const auto& segments) {
            program.draw(
                context,
                drawMode,
                depthModeForSublayer(sublayer, gl::DepthMode::ReadWrite),
                stencilModeForClipping(tile.clip),
                colorModeForRenderPass(),
                FillProgram::UniformValues {
                    uniforms::u_matrix::Value{ tile.translatedMatrix(properties.fillTranslate.value,
                                               properties.fillTranslateAnchor.value,
                                               state) },
                    uniforms::u_opacity::Value{ properties.fillOpacity.value },
                    uniforms::u_color::Value{ properties.fillColor.value },
                    uniforms::u_outline_color::Value{ outlineColor },
                    uniforms::u_world::Value{ context.viewport.getCurrentValue().size },
                },
                vertexBuffer,
                indexBuffer,
                segments
            );
        };

        if (properties.fillAntialias.value && !properties.fillOutlineColor.isUndefined() && pass == RenderPass::Translucent) {
            draw(2,
                 parameters.programs.fillOutline,
                 properties.fillOutlineColor.value,
                 gl::Lines { 2.0f },
                 *bucket.vertexBuffer,
                 *bucket.lineIndexBuffer,
                 bucket.lineSegments);
        }

        // Only draw the fill when it's opaque and we're drawing opaque fragments,
        // or when it's translucent and we're drawing translucent fragments.
        if ((properties.fillColor.value.a >= 1.0f && properties.fillOpacity.value >= 1.0f) == (pass == RenderPass::Opaque)) {
            draw(1,
                 parameters.programs.fill,
                 properties.fillOutlineColor.value,
                 gl::Triangles(),
                 *bucket.vertexBuffer,
                 *bucket.triangleIndexBuffer,
                 bucket.triangleSegments);
        }

        if (properties.fillAntialias.value && properties.fillOutlineColor.isUndefined() && pass == RenderPass::Translucent) {
            draw(2,
                 parameters.programs.fillOutline,
                 properties.fillColor.value,
                 gl::Lines { 2.0f },
                 *bucket.vertexBuffer,
                 *bucket.lineIndexBuffer,
                 bucket.lineSegments);
        }
    }
}

} // namespace mbgl
