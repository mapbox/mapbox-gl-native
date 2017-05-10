#include <mbgl/renderer/layers/render_fill_layer.hpp>
#include <mbgl/renderer/buckets/fill_bucket.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {

RenderFillLayer::RenderFillLayer(Immutable<style::FillLayer::Impl> _impl)
    : RenderLayer(style::LayerType::Fill, _impl) {
}

const style::FillLayer::Impl& RenderFillLayer::impl() const {
    return static_cast<const style::FillLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderFillLayer::createBucket(const BucketParameters& parameters, const std::vector<const RenderLayer*>& layers) const {
    return std::make_unique<FillBucket>(parameters, layers);
}

void RenderFillLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transition(parameters, std::move(unevaluated));
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
