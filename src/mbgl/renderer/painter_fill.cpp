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
    const FillPaintProperties::Evaluated& properties = layer.impl->paint.evaluated;

    if (!properties.get<FillPattern>().from.empty()) {
        if (pass != RenderPass::Translucent) {
            return;
        }

        optional<SpriteAtlasPosition> imagePosA = spriteAtlas->getPosition(
            properties.get<FillPattern>().from, SpritePatternMode::Repeating);
        optional<SpriteAtlasPosition> imagePosB = spriteAtlas->getPosition(
            properties.get<FillPattern>().to, SpritePatternMode::Repeating);

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
                    tile.translatedMatrix(properties.get<FillTranslate>(),
                                          properties.get<FillTranslateAnchor>(),
                                          state),
                    properties.get<FillOpacity>(),
                    context.viewport.getCurrentValue().size,
                    *imagePosA,
                    *imagePosB,
                    properties.get<FillPattern>(),
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

        if (!properties.get<FillAntialias>() || !layer.impl->paint.unevaluated.get<FillOutlineColor>().isUndefined()) {
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
                    uniforms::u_matrix::Value{ tile.translatedMatrix(properties.get<FillTranslate>(),
                                               properties.get<FillTranslateAnchor>(),
                                               state) },
                    uniforms::u_opacity::Value{ properties.get<FillOpacity>() },
                    uniforms::u_color::Value{ properties.get<FillColor>() },
                    uniforms::u_outline_color::Value{ outlineColor },
                    uniforms::u_world::Value{ context.viewport.getCurrentValue().size },
                },
                vertexBuffer,
                indexBuffer,
                segments
            );
        };

        if (properties.get<FillAntialias>() && !layer.impl->paint.unevaluated.get<FillOutlineColor>().isUndefined() && pass == RenderPass::Translucent) {
            draw(2,
                 parameters.programs.fillOutline,
                 properties.get<FillOutlineColor>(),
                 gl::Lines { 2.0f },
                 *bucket.vertexBuffer,
                 *bucket.lineIndexBuffer,
                 bucket.lineSegments);
        }

        // Only draw the fill when it's opaque and we're drawing opaque fragments,
        // or when it's translucent and we're drawing translucent fragments.
        if ((properties.get<FillColor>().a >= 1.0f && properties.get<FillOpacity>() >= 1.0f) == (pass == RenderPass::Opaque)) {
            draw(1,
                 parameters.programs.fill,
                 properties.get<FillOutlineColor>(),
                 gl::Triangles(),
                 *bucket.vertexBuffer,
                 *bucket.triangleIndexBuffer,
                 bucket.triangleSegments);
        }

        if (properties.get<FillAntialias>() && layer.impl->paint.unevaluated.get<FillOutlineColor>().isUndefined() && pass == RenderPass::Translucent) {
            draw(2,
                 parameters.programs.fillOutline,
                 properties.get<FillColor>(),
                 gl::Lines { 2.0f },
                 *bucket.vertexBuffer,
                 *bucket.lineIndexBuffer,
                 bucket.lineSegments);
        }
    }
}

} // namespace mbgl
