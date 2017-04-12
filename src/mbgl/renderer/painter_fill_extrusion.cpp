#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/fill_extrusion_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/sprite/sprite_atlas.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/convert.hpp>

namespace mbgl {

using namespace style;

void Painter::renderFillExtrusion(PaintParameters& parameters,
                                  FillExtrusionBucket& bucket,
                                  const FillExtrusionLayer& layer,
                                  const RenderTile& tile,
                                  const Style& style) {
    const FillExtrusionPaintProperties::Evaluated& properties = layer.impl->paint.evaluated;

    if (pass == RenderPass::Opaque) {
        return;
    }

    const auto light = style.light.evaluated;

    if (!properties.get<FillExtrusionPattern>().from.empty()) {
        optional<SpriteAtlasElement> imagePosA =
            spriteAtlas->getPattern(properties.get<FillExtrusionPattern>().from);
        optional<SpriteAtlasElement> imagePosB =
            spriteAtlas->getPattern(properties.get<FillExtrusionPattern>().to);

        if (!imagePosA || !imagePosB) {
            return;
        }

        spriteAtlas->bind(true, context, 0);

        parameters.programs.fillExtrusionPattern.draw(
            context,
            gl::Triangles(),
            depthModeForSublayer(0, gl::DepthMode::ReadWrite),
            gl::StencilMode::disabled(),
            colorModeForRenderPass(),
            FillExtrusionPatternUniforms::values(
                tile.translatedMatrix(properties.get<FillExtrusionTranslate>(),
                                      properties.get<FillExtrusionTranslateAnchor>(),
                                      state),
                *imagePosA,
                *imagePosB,
                properties.get<FillExtrusionPattern>(),
                tile.id,
                state,
                -std::pow(2, tile.id.canonical.z) / util::tileSize / 8.0f,
                light
            ),
            *bucket.vertexBuffer,
            *bucket.indexBuffer,
            bucket.triangleSegments,
            bucket.paintPropertyBinders.at(layer.getID()),
            properties,
            state.getZoom());

    } else {
        parameters.programs.fillExtrusion.draw(
            context,
            gl::Triangles(),
            depthModeForSublayer(0, gl::DepthMode::ReadWrite),
            gl::StencilMode::disabled(),
            colorModeForRenderPass(),
            FillExtrusionUniforms::values(
                tile.translatedMatrix(properties.get<FillExtrusionTranslate>(),
                                      properties.get<FillExtrusionTranslateAnchor>(),
                                      state),
                state,
                light
            ),
            *bucket.vertexBuffer,
            *bucket.indexBuffer,
            bucket.triangleSegments,
            bucket.paintPropertyBinders.at(layer.getID()),
            properties,
            state.getZoom());
    };
}

} // namespace mbgl
