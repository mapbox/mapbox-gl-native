#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/buckets/fill_extrusion_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/layers/render_fill_extrusion_layer.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/convert.hpp>

namespace mbgl {

using namespace style;

void Painter::renderFillExtrusion(PaintParameters& parameters,
                                  FillExtrusionBucket& bucket,
                                  const RenderFillExtrusionLayer& layer,
                                  const RenderTile& tile) {
    const FillExtrusionPaintProperties::PossiblyEvaluated& properties = layer.evaluated;

    if (pass == RenderPass::Opaque) {
        return;
    }

    if (!properties.get<FillExtrusionPattern>().from.empty()) {
        optional<ImagePosition> imagePosA = imageManager->getPattern(properties.get<FillExtrusionPattern>().from);
        optional<ImagePosition> imagePosB = imageManager->getPattern(properties.get<FillExtrusionPattern>().to);

        if (!imagePosA || !imagePosB) {
            return;
        }

        imageManager->bind(context, 0);

        parameters.programs.fillExtrusionPattern.get(properties).draw(
            context,
            gl::Triangles(),
            depthModeForSublayer(0, gl::DepthMode::ReadWrite),
            gl::StencilMode::disabled(),
            colorModeForRenderPass(),
            FillExtrusionPatternUniforms::values(
                tile.translatedClipMatrix(properties.get<FillExtrusionTranslate>(),
                                          properties.get<FillExtrusionTranslateAnchor>(),
                                          state),
                imageManager->getPixelSize(),
                *imagePosA,
                *imagePosB,
                properties.get<FillExtrusionPattern>(),
                tile.id,
                state,
                -std::pow(2, tile.id.canonical.z) / util::tileSize / 8.0f,
                evaluatedLight
            ),
            *bucket.vertexBuffer,
            *bucket.indexBuffer,
            bucket.triangleSegments,
            bucket.paintPropertyBinders.at(layer.getID()),
            properties,
            state.getZoom(),
            layer.getID());

    } else {
        parameters.programs.fillExtrusion.get(properties).draw(
            context,
            gl::Triangles(),
            depthModeForSublayer(0, gl::DepthMode::ReadWrite),
            gl::StencilMode::disabled(),
            colorModeForRenderPass(),
            FillExtrusionUniforms::values(
                tile.translatedClipMatrix(properties.get<FillExtrusionTranslate>(),
                                          properties.get<FillExtrusionTranslateAnchor>(),
                                          state),
                state,
                evaluatedLight
            ),
            *bucket.vertexBuffer,
            *bucket.indexBuffer,
            bucket.triangleSegments,
            bucket.paintPropertyBinders.at(layer.getID()),
            properties,
            state.getZoom(),
            layer.getID());
    };
}

} // namespace mbgl
