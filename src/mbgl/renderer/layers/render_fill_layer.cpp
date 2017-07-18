#include <mbgl/renderer/layers/render_fill_layer.hpp>
#include <mbgl/renderer/buckets/fill_bucket.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {

using namespace style;

RenderFillLayer::RenderFillLayer(Immutable<style::FillLayer::Impl> _impl)
    : RenderLayer(style::LayerType::Fill, _impl),
      unevaluated(impl().paint.untransitioned()) {
}

const style::FillLayer::Impl& RenderFillLayer::impl() const {
    return static_cast<const style::FillLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderFillLayer::createBucket(const BucketParameters& parameters, const std::vector<const RenderLayer*>& layers) const {
    return std::make_unique<FillBucket>(parameters, layers);
}

void RenderFillLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transitioned(parameters, std::move(unevaluated));
}

void RenderFillLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);

    if (unevaluated.get<style::FillOutlineColor>().isUndefined()) {
        evaluated.get<style::FillOutlineColor>() = evaluated.get<style::FillColor>();
    }

    passes = RenderPass::None;

    if (evaluated.get<style::FillAntialias>()) {
        passes |= RenderPass::Translucent;
    }

    if (!unevaluated.get<style::FillPattern>().isUndefined()
        || evaluated.get<style::FillColor>().constantOr(Color()).a < 1.0f
        || evaluated.get<style::FillOpacity>().constantOr(0) < 1.0f) {
        passes |= RenderPass::Translucent;
    } else {
        passes |= RenderPass::Opaque;
    }
}

bool RenderFillLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

void RenderFillLayer::render(Painter& painter, PaintParameters& parameters, RenderSource*) {
    if (evaluated.get<FillPattern>().from.empty()) {
        for (const RenderTile& tile : renderTiles) {
            assert(dynamic_cast<FillBucket*>(tile.tile.getBucket(*baseImpl)));
            FillBucket& bucket = *reinterpret_cast<FillBucket*>(tile.tile.getBucket(*baseImpl));

            auto draw = [&] (uint8_t sublayer,
                             auto& program,
                             const auto& drawMode,
                             const auto& indexBuffer,
                             const auto& segments) {
                program.get(evaluated).draw(
                    painter.context,
                    drawMode,
                    painter.depthModeForSublayer(sublayer, gl::DepthMode::ReadWrite),
                    painter.stencilModeForClipping(tile.clip),
                    painter.colorModeForRenderPass(),
                    FillProgram::UniformValues {
                        uniforms::u_matrix::Value{
                            tile.translatedMatrix(evaluated.get<FillTranslate>(),
                                                  evaluated.get<FillTranslateAnchor>(),
                                                  painter.state)
                        },
                        uniforms::u_world::Value{ painter.context.viewport.getCurrentValue().size },
                    },
                    *bucket.vertexBuffer,
                    indexBuffer,
                    segments,
                    bucket.paintPropertyBinders.at(getID()),
                    evaluated,
                    painter.state.getZoom(),
                    getID()
                );
            };

            if (evaluated.get<FillAntialias>() && !unevaluated.get<FillOutlineColor>().isUndefined() && painter.pass == RenderPass::Translucent) {
                draw(2,
                     parameters.programs.fillOutline,
                     gl::Lines { 2.0f },
                     *bucket.lineIndexBuffer,
                     bucket.lineSegments);
            }

            // Only draw the fill when it's opaque and we're drawing opaque fragments,
            // or when it's translucent and we're drawing translucent fragments.
            if ((evaluated.get<FillColor>().constantOr(Color()).a >= 1.0f
              && evaluated.get<FillOpacity>().constantOr(0) >= 1.0f) == (painter.pass == RenderPass::Opaque)) {
                draw(1,
                     parameters.programs.fill,
                     gl::Triangles(),
                     *bucket.triangleIndexBuffer,
                     bucket.triangleSegments);
            }

            if (evaluated.get<FillAntialias>() && unevaluated.get<FillOutlineColor>().isUndefined() && painter.pass == RenderPass::Translucent) {
                draw(2,
                     parameters.programs.fillOutline,
                     gl::Lines { 2.0f },
                     *bucket.lineIndexBuffer,
                     bucket.lineSegments);
            }
        }
    } else {
        if (painter.pass != RenderPass::Translucent) {
            return;
        }

        optional<ImagePosition> imagePosA = painter.imageManager->getPattern(evaluated.get<FillPattern>().from);
        optional<ImagePosition> imagePosB = painter.imageManager->getPattern(evaluated.get<FillPattern>().to);

        if (!imagePosA || !imagePosB) {
            return;
        }

        painter.imageManager->bind(painter.context, 0);

        for (const RenderTile& tile : renderTiles) {
            assert(dynamic_cast<FillBucket*>(tile.tile.getBucket(*baseImpl)));
            FillBucket& bucket = *reinterpret_cast<FillBucket*>(tile.tile.getBucket(*baseImpl));

            auto draw = [&] (uint8_t sublayer,
                             auto& program,
                             const auto& drawMode,
                             const auto& indexBuffer,
                             const auto& segments) {
                program.get(evaluated).draw(
                    painter.context,
                    drawMode,
                    painter.depthModeForSublayer(sublayer, gl::DepthMode::ReadWrite),
                    painter.stencilModeForClipping(tile.clip),
                    painter.colorModeForRenderPass(),
                    FillPatternUniforms::values(
                        tile.translatedMatrix(evaluated.get<FillTranslate>(),
                                              evaluated.get<FillTranslateAnchor>(),
                                              painter.state),
                        painter.context.viewport.getCurrentValue().size,
                        painter.imageManager->getPixelSize(),
                        *imagePosA,
                        *imagePosB,
                        evaluated.get<FillPattern>(),
                        tile.id,
                        painter.state
                    ),
                    *bucket.vertexBuffer,
                    indexBuffer,
                    segments,
                    bucket.paintPropertyBinders.at(getID()),
                    evaluated,
                    painter.state.getZoom(),
                    getID()
                );
            };

            draw(0,
                 parameters.programs.fillPattern,
                 gl::Triangles(),
                 *bucket.triangleIndexBuffer,
                 bucket.triangleSegments);

            if (!evaluated.get<FillAntialias>() || !unevaluated.get<FillOutlineColor>().isUndefined()) {
                continue;
            }

            draw(2,
                 parameters.programs.fillOutlinePattern,
                 gl::Lines { 2.0f },
                 *bucket.lineIndexBuffer,
                 bucket.lineSegments);
        }
    }
}

bool RenderFillLayer::queryIntersectsFeature(
        const GeometryCoordinates& queryGeometry,
        const GeometryTileFeature& feature,
        const float,
        const float bearing,
        const float pixelsToTileUnits) const {

    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry,
            evaluated.get<style::FillTranslate>(),
            evaluated.get<style::FillTranslateAnchor>(),
            bearing,
            pixelsToTileUnits);

    return util::polygonIntersectsMultiPolygon(translatedQueryGeometry.value_or(queryGeometry), feature.getGeometries());
}


} // namespace mbgl
