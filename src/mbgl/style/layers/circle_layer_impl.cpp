#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/renderer/circle_bucket.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {
namespace style {

void CircleLayer::Impl::cascade(const CascadeParameters& parameters) {
    paint.cascade(parameters);
}

bool CircleLayer::Impl::evaluate(const PropertyEvaluationParameters& parameters) {
    paint.evaluate(parameters);

    passes = ((paint.evaluated.get<CircleRadius>().constantOr(1) > 0 ||
               paint.evaluated.get<CircleStrokeWidth>().constantOr(1) > 0)
           && (paint.evaluated.get<CircleColor>().constantOr(Color::black()).a > 0 ||
               paint.evaluated.get<CircleStrokeColor>().constantOr(Color::black()).a > 0)
           && (paint.evaluated.get<CircleOpacity>().constantOr(1) > 0 ||
               paint.evaluated.get<CircleStrokeOpacity>().constantOr(1) > 0))
        ? RenderPass::Translucent : RenderPass::None;

    return paint.hasTransition();
}

std::unique_ptr<Bucket> CircleLayer::Impl::createBucket(const BucketParameters& parameters, const std::vector<const Layer*>& layers) const {
    return std::make_unique<CircleBucket>(parameters, layers);
}

bool CircleLayer::Impl::queryIntersectsFeature(
        const GeometryCoordinates& queryGeometry,
        const GeometryTileFeature& feature,
        const float zoom,
        const float bearing,
        const float pixelsToTileUnits) const {

    // Translate query geometry
    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry, paint.evaluated.get<CircleTranslate>(), paint.evaluated.get<CircleTranslateAnchor>(), bearing, pixelsToTileUnits);

    // Evaluate function
    auto circleRadius = paint.evaluated.get<CircleRadius>()
                                .evaluate(feature, zoom, CircleRadius::defaultValue())
                        * pixelsToTileUnits;

    // Test intersection
    return util::polygonIntersectsBufferedMultiPoint(translatedQueryGeometry.value_or(queryGeometry), feature.getGeometries(), circleRadius);
}

} // namespace style
} // namespace mbgl
