#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/style/bucket_parameters.hpp>
#include <mbgl/renderer/circle_bucket.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {
namespace style {

void CircleLayer::Impl::parsePaints(const JSValue& layer) {
    paint.parse(layer);
}

void CircleLayer::Impl::cascade(const CascadeParameters& parameters) {
    paint.cascade(parameters);
}

bool CircleLayer::Impl::recalculate(const CalculationParameters& parameters) {
    bool hasTransitions = paint.recalculate(parameters);

    passes = (paint.circleRadius > 0 && paint.circleColor.value.a > 0 && paint.circleOpacity > 0)
        ? RenderPass::Translucent : RenderPass::None;

    return hasTransitions;
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
    const std::array<float, 2>& translate = paint.circleTranslate;
    return paint.circleRadius + util::length(translate[0], translate[1]);
}

bool CircleLayer::Impl::queryIntersectsGeometry(
        const GeometryCollection& queryGeometry,
        const GeometryCollection& geometry,
        const float bearing,
        const float pixelsToTileUnits) const {

    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry, paint.circleTranslate, paint.circleTranslateAnchor, bearing, pixelsToTileUnits);

    auto circleRadius = paint.circleRadius * pixelsToTileUnits;

    return util::multiPolygonIntersectsBufferedMultiPoint(
            translatedQueryGeometry.value_or(queryGeometry), geometry, circleRadius);
}

} // namespace style
} // namespace mbgl
