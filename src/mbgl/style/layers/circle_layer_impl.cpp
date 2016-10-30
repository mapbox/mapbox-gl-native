#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/style/bucket_parameters.hpp>
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

    passes = (paint.evaluated.get<CircleRadius>() > 0 && paint.evaluated.get<CircleColor>().a > 0 && paint.evaluated.get<CircleOpacity>() > 0)
        ? RenderPass::Translucent : RenderPass::None;

    return paint.hasTransition();
}

std::unique_ptr<Bucket> CircleLayer::Impl::createBucket(BucketParameters& parameters) const {
    auto bucket = std::make_unique<CircleBucket>(parameters.mode);

    auto& name = bucketName();
    parameters.eachFilteredFeature(filter, [&] (const auto& feature, std::size_t index, const std::string& layerName) {
        auto geometries = feature.getGeometries();
        bucket->addGeometry(geometries);
        parameters.featureIndex.insert(geometries, index, layerName, name);
    });

    return std::move(bucket);
}

float CircleLayer::Impl::getQueryRadius() const {
    const std::array<float, 2>& translate = paint.evaluated.get<CircleTranslate>();
    return paint.evaluated.get<CircleRadius>() + util::length(translate[0], translate[1]);
}

bool CircleLayer::Impl::queryIntersectsGeometry(
        const GeometryCoordinates& queryGeometry,
        const GeometryCollection& geometry,
        const float bearing,
        const float pixelsToTileUnits) const {

    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry, paint.evaluated.get<CircleTranslate>(), paint.evaluated.get<CircleTranslateAnchor>(), bearing, pixelsToTileUnits);

    auto circleRadius = paint.evaluated.get<CircleRadius>() * pixelsToTileUnits;

    return util::polygonIntersectsBufferedMultiPoint(
            translatedQueryGeometry.value_or(queryGeometry), geometry, circleRadius);
}

} // namespace style
} // namespace mbgl
