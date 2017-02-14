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

        optional<SpriteAtlasElement> imagePosA = spriteAtlas->getImage(
            properties.get<FillPattern>().from, SpritePatternMode::Repeating);
        optional<SpriteAtlasElement> imagePosB = spriteAtlas->getImage(
            properties.get<FillPattern>().to, SpritePatternMode::Repeating);

        if (!imagePosA || !imagePosB) {
            return;
        }

        spriteAtlas->bind(true, context, 0);

        auto draw = [&] (uint8_t sublayer,
                         auto& program,
                         const auto& drawMode,
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
                    context.viewport.getCurrentValue().size,
                    *imagePosA,
                    *imagePosB,
                    properties.get<FillPattern>(),
                    tile.id,
                    state
                ),
                *bucket.vertexBuffer,
                indexBuffer,
                segments,
                bucket.paintPropertyBinders.at(layer.getID()),
                properties,
                state.getZoom()
            );
        };

        draw(0,
             parameters.programs.fillPattern,
             gl::Triangles(),
             *bucket.triangleIndexBuffer,
             bucket.triangleSegments);

        if (!properties.get<FillAntialias>() || !layer.impl->paint.unevaluated.get<FillOutlineColor>().isUndefined()) {
            return;
        }

        draw(2,
             parameters.programs.fillOutlinePattern,
             gl::Lines { 2.0f },
             *bucket.lineIndexBuffer,
             bucket.lineSegments);
    } else {
        auto draw = [&] (uint8_t sublayer,
                         auto& program,
                         const auto& drawMode,
                         const auto& indexBuffer,
                         const auto& segments) {
            program.draw(
                context,
                drawMode,
                depthModeForSublayer(sublayer, gl::DepthMode::ReadWrite),
                stencilModeForClipping(tile.clip),
                colorModeForRenderPass(),
                FillProgram::UniformValues {
                    uniforms::u_matrix::Value{
                        tile.translatedMatrix(properties.get<FillTranslate>(),
                                              properties.get<FillTranslateAnchor>(),
                                              state)
                    },
                    uniforms::u_world::Value{ context.viewport.getCurrentValue().size },
                },
                *bucket.vertexBuffer,
                indexBuffer,
                segments,
                bucket.paintPropertyBinders.at(layer.getID()),
                properties,
                state.getZoom()
            );
        };

        if (properties.get<FillAntialias>() && !layer.impl->paint.unevaluated.get<FillOutlineColor>().isUndefined() && pass == RenderPass::Translucent) {
            draw(2,
                 parameters.programs.fillOutline,
                 gl::Lines { 2.0f },
                 *bucket.lineIndexBuffer,
                 bucket.lineSegments);
        }

        // Only draw the fill when it's opaque and we're drawing opaque fragments,
        // or when it's translucent and we're drawing translucent fragments.
        if ((properties.get<FillColor>().constantOr(Color()).a >= 1.0f
          && properties.get<FillOpacity>().constantOr(0) >= 1.0f) == (pass == RenderPass::Opaque)) {
            draw(1,
                 parameters.programs.fill,
                 gl::Triangles(),
                 *bucket.triangleIndexBuffer,
                 bucket.triangleSegments);
        }

        if (properties.get<FillAntialias>() && layer.impl->paint.unevaluated.get<FillOutlineColor>().isUndefined() && pass == RenderPass::Translucent) {
            draw(2,
                 parameters.programs.fillOutline,
                 gl::Lines { 2.0f },
                 *bucket.lineIndexBuffer,
                 bucket.lineSegments);
        }
    }
}

} // namespace mbgl
