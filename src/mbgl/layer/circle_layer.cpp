#include <mbgl/layer/circle_layer.hpp>
#include <mbgl/style/style_bucket_parameters.hpp>
#include <mbgl/renderer/circle_bucket.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> CircleLayer::clone() const {
    return std::make_unique<CircleLayer>(*this);
}

void CircleLayer::parsePaints(const JSValue& layer) {
    paint.circleRadius.parse("circle-radius", layer);
    paint.circleColor.parse("circle-color", layer);
    paint.circleOpacity.parse("circle-opacity", layer);
    paint.circleTranslate.parse("circle-translate", layer);
    paint.circleTranslateAnchor.parse("circle-translate-anchor", layer);
    paint.circleBlur.parse("circle-blur", layer);
}

void CircleLayer::cascade(const StyleCascadeParameters& parameters) {
    paint.circleRadius.cascade(parameters);
    paint.circleColor.cascade(parameters);
    paint.circleOpacity.cascade(parameters);
    paint.circleTranslate.cascade(parameters);
    paint.circleTranslateAnchor.cascade(parameters);
    paint.circleBlur.cascade(parameters);
}

bool CircleLayer::recalculate(const StyleCalculationParameters& parameters) {
    bool hasTransitions = false;

    hasTransitions |= paint.circleRadius.calculate(parameters);
    hasTransitions |= paint.circleColor.calculate(parameters);
    hasTransitions |= paint.circleOpacity.calculate(parameters);
    hasTransitions |= paint.circleTranslate.calculate(parameters);
    hasTransitions |= paint.circleTranslateAnchor.calculate(parameters);
    hasTransitions |= paint.circleBlur.calculate(parameters);

    passes = (paint.circleRadius > 0 && paint.circleColor.value[3] > 0 && paint.circleOpacity > 0)
        ? RenderPass::Translucent : RenderPass::None;

    return hasTransitions;
}

std::unique_ptr<Bucket> CircleLayer::createBucket(StyleBucketParameters& parameters) const {
    auto bucket = std::make_unique<CircleBucket>(parameters.mode);

    auto& name = bucketName();
    parameters.eachFilteredFeature(filter, [&] (const auto& feature, std::size_t index, const std::string& layerName) {
        auto geometries = feature.getGeometries();
        bucket->addGeometry(geometries);
        parameters.featureIndex.insert(geometries, index, layerName, name);
    });

    return std::move(bucket);
}

float CircleLayer::getQueryRadius() const {
    const std::array<float, 2>& translate = paint.circleTranslate;
    return paint.circleRadius + util::length(translate[0], translate[1]);
}

bool CircleLayer::queryIntersectsGeometry(
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

} // namespace mbgl
