#include <mbgl/renderer/render_fill_extrusion_layer.hpp>
#include <mbgl/renderer/fill_extrusion_bucket.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {

RenderFillExtrusionLayer::RenderFillExtrusionLayer(const style::FillExtrusionLayer::Impl& _impl)
        : RenderLayer(style::LayerType::FillExtrusion, _impl),
          impl(&_impl) {
}

std::unique_ptr<RenderLayer> RenderFillExtrusionLayer::clone() const {
    return std::make_unique<RenderFillExtrusionLayer>(*this);
}

std::unique_ptr<Bucket> RenderFillExtrusionLayer::createBucket(const BucketParameters& parameters, const std::vector<const RenderLayer*>& layers) const {
    return std::make_unique<FillExtrusionBucket>(parameters, layers);
}

void RenderFillExtrusionLayer::cascade(const CascadeParameters& parameters) {
    unevaluated = impl->cascading.cascade(parameters, std::move(unevaluated));
}

void RenderFillExtrusionLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);

    passes = (evaluated.get<style::FillExtrusionOpacity>() > 0) ? RenderPass::Translucent
                                                         : RenderPass::None;
}

bool RenderFillExtrusionLayer::hasTransition() const {
    return unevaluated.hasTransition();
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
