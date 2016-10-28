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

    passes = (paint.evaluated.get<CircleRadius>().constantOr(1) > 0
           && paint.evaluated.get<CircleColor>().constantOr(Color::black()).a > 0
           && paint.evaluated.get<CircleOpacity>().constantOr(1) > 0)
        ? RenderPass::Translucent : RenderPass::None;

    return paint.hasTransition();
}

std::unique_ptr<Bucket> CircleLayer::Impl::createBucket(const BucketParameters& parameters, const std::vector<const Layer*>& layers) const {
    return std::make_unique<CircleBucket>(parameters, layers);
}

float CircleLayer::Impl::getQueryRadius() const {
    const std::array<float, 2>& translate = paint.evaluated.get<CircleTranslate>();
    return paint.evaluated.get<CircleRadius>().constantOr(CircleRadius::defaultValue())
        + util::length(translate[0], translate[1]);
}

bool CircleLayer::Impl::queryIntersectsGeometry(
        const GeometryCoordinates& queryGeometry,
        const GeometryCollection& geometry,
        const float bearing,
        const float pixelsToTileUnits) const {

    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry, paint.evaluated.get<CircleTranslate>(), paint.evaluated.get<CircleTranslateAnchor>(), bearing, pixelsToTileUnits);

    auto circleRadius = paint.evaluated.get<CircleRadius>().constantOr(CircleRadius::defaultValue()) * pixelsToTileUnits;

    return util::polygonIntersectsBufferedMultiPoint(
            translatedQueryGeometry.value_or(queryGeometry), geometry, circleRadius);
}

} // namespace style
} // namespace mbgl
