#include <mbgl/renderer/layers/render_fill_extrusion_layer.hpp>
#include <mbgl/renderer/buckets/fill_extrusion_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/renderer/renderer_backend.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {

using namespace style;

RenderFillExtrusionLayer::RenderFillExtrusionLayer(Immutable<style::FillExtrusionLayer::Impl> _impl)
    : RenderLayer(style::LayerType::FillExtrusion, _impl),
      unevaluated(impl().paint.untransitioned()) {
}

const style::FillExtrusionLayer::Impl& RenderFillExtrusionLayer::impl() const {
    return static_cast<const style::FillExtrusionLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderFillExtrusionLayer::createBucket(const BucketParameters& parameters, const std::vector<const RenderLayer*>& layers) const {
    return std::make_unique<FillExtrusionBucket>(parameters, layers);
}

void RenderFillExtrusionLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transitioned(parameters, std::move(unevaluated));
}

void RenderFillExtrusionLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);

    passes = (evaluated.get<style::FillExtrusionOpacity>() > 0)
                 ? (RenderPass::Translucent | RenderPass::Pass3D)
                 : RenderPass::None;
}

bool RenderFillExtrusionLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

void RenderFillExtrusionLayer::render(PaintParameters& parameters, RenderSource*) {
    if (parameters.pass == RenderPass::Opaque) {
        return;
    }

    if (parameters.pass == RenderPass::Pass3D) {
        const auto& size = parameters.staticData.backendSize;

        if (!renderTexture || renderTexture->getSize() != size) {
            renderTexture = OffscreenTexture(parameters.context, size, *parameters.staticData.depthRenderbuffer);
        }

        renderTexture->bind();

        optional<float> depthClearValue = {};
        if (parameters.staticData.depthRenderbuffer->needsClearing()) depthClearValue = 1.0;
        // Flag the depth buffer as no longer needing to be cleared for the remainder of this pass.
        parameters.staticData.depthRenderbuffer->shouldClear(false);

        parameters.context.setStencilMode(gl::StencilMode::disabled());
        parameters.context.clear(Color{ 0.0f, 0.0f, 0.0f, 0.0f }, depthClearValue, {});

        if (evaluated.get<FillExtrusionPattern>().from.empty()) {
            for (const RenderTile& tile : renderTiles) {
                assert(dynamic_cast<FillExtrusionBucket*>(tile.tile.getBucket(*baseImpl)));
                FillExtrusionBucket& bucket =
                    *reinterpret_cast<FillExtrusionBucket*>(tile.tile.getBucket(*baseImpl));

                parameters.programs.fillExtrusion.get(evaluated).draw(
                    parameters.context, gl::Triangles(),
                    parameters.depthModeFor3D(gl::DepthMode::ReadWrite),
                    gl::StencilMode::disabled(), parameters.colorModeForRenderPass(),
                    FillExtrusionUniforms::values(
                        tile.translatedClipMatrix(evaluated.get<FillExtrusionTranslate>(),
                                                  evaluated.get<FillExtrusionTranslateAnchor>(),
                                                  parameters.state),
                        parameters.state, parameters.evaluatedLight),
                    *bucket.vertexBuffer, *bucket.indexBuffer, bucket.triangleSegments,
                    bucket.paintPropertyBinders.at(getID()), evaluated, parameters.state.getZoom(),
                    getID());
            }
        } else {
            optional<ImagePosition> imagePosA =
                parameters.imageManager.getPattern(evaluated.get<FillExtrusionPattern>().from);
            optional<ImagePosition> imagePosB =
                parameters.imageManager.getPattern(evaluated.get<FillExtrusionPattern>().to);

            if (!imagePosA || !imagePosB) {
                return;
            }

            parameters.imageManager.bind(parameters.context, 0);

            for (const RenderTile& tile : renderTiles) {
                assert(dynamic_cast<FillExtrusionBucket*>(tile.tile.getBucket(*baseImpl)));
                FillExtrusionBucket& bucket =
                    *reinterpret_cast<FillExtrusionBucket*>(tile.tile.getBucket(*baseImpl));

                parameters.programs.fillExtrusionPattern.get(evaluated).draw(
                    parameters.context, gl::Triangles(),
                    parameters.depthModeFor3D(gl::DepthMode::ReadWrite),
                    gl::StencilMode::disabled(), parameters.colorModeForRenderPass(),
                    FillExtrusionPatternUniforms::values(
                        tile.translatedClipMatrix(evaluated.get<FillExtrusionTranslate>(),
                                                  evaluated.get<FillExtrusionTranslateAnchor>(),
                                                  parameters.state),
                        parameters.imageManager.getPixelSize(), *imagePosA, *imagePosB,
                        evaluated.get<FillExtrusionPattern>(), tile.id, parameters.state,
                        -std::pow(2, tile.id.canonical.z) / util::tileSize / 8.0f,
                        parameters.evaluatedLight),
                    *bucket.vertexBuffer, *bucket.indexBuffer, bucket.triangleSegments,
                    bucket.paintPropertyBinders.at(getID()), evaluated, parameters.state.getZoom(),
                    getID());
            }
        }

    } else if (parameters.pass == RenderPass::Translucent) {
        parameters.context.bindTexture(renderTexture->getTexture());

        const auto& size = parameters.staticData.backendSize;

        mat4 viewportMat;
        matrix::ortho(viewportMat, 0, size.width, size.height, 0, 0, 1);

        const Properties<>::PossiblyEvaluated properties;

        parameters.programs.extrusionTexture.draw(
            parameters.context, gl::Triangles(), gl::DepthMode::disabled(),
            gl::StencilMode::disabled(), parameters.colorModeForRenderPass(),
            ExtrusionTextureProgram::UniformValues{
                uniforms::u_matrix::Value{ viewportMat }, uniforms::u_world::Value{ size },
                uniforms::u_image::Value{ 0 },
                uniforms::u_opacity::Value{ evaluated.get<FillExtrusionOpacity>() } },
            parameters.staticData.extrusionTextureVertexBuffer,
            parameters.staticData.quadTriangleIndexBuffer,
            parameters.staticData.extrusionTextureSegments,
            ExtrusionTextureProgram::PaintPropertyBinders{ properties, 0 }, properties,
            parameters.state.getZoom(), getID());
    }
}

bool RenderFillExtrusionLayer::queryIntersectsFeature(
        const GeometryCoordinates& queryGeometry,
        const GeometryTileFeature& feature,
        const float,
        const float bearing,
        const float pixelsToTileUnits) const {

    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry,
            evaluated.get<style::FillExtrusionTranslate>(),
            evaluated.get<style::FillExtrusionTranslateAnchor>(),
            bearing,
            pixelsToTileUnits);

    return util::polygonIntersectsMultiPolygon(translatedQueryGeometry.value_or(queryGeometry), feature.getGeometries());
}

} // namespace mbgl
