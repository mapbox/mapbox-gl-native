#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/style/bucket_parameters.hpp>
#include <mbgl/renderer/fill_bucket.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {
namespace style {

void FillLayer::Impl::parsePaints(const JSValue& layer) {
    paint.parse(layer);
}

void FillLayer::Impl::cascade(const CascadeParameters& parameters) {
    paint.cascade(parameters);
}

bool FillLayer::Impl::recalculate(const CalculationParameters& parameters) {
    bool hasTransitions = paint.recalculate(parameters);

    passes = RenderPass::None;

    if (paint.fillAntialias) {
        passes |= RenderPass::Translucent;
    }

    if (!paint.fillPattern.value.from.empty() || (paint.fillColor.value.a * paint.fillOpacity) < 1.0f) {
        passes |= RenderPass::Translucent;
    } else {
        passes |= RenderPass::Opaque;
    }

    return hasTransitions;
}

std::unique_ptr<Bucket> FillLayer::Impl::createBucket(BucketParameters& parameters) const {
    auto bucket = std::make_unique<FillBucket>();

    auto& name = bucketName();
    parameters.eachFilteredFeature(filter, [&] (const auto& feature, std::size_t index, const std::string& layerName) {
        auto geometries = feature.getGeometries();
        bucket->addGeometry(geometries);
        parameters.featureIndex.insert(geometries, index, layerName, name);
    });

    return std::move(bucket);
}

float FillLayer::Impl::getQueryRadius() const {
    const std::array<float, 2>& translate = paint.fillTranslate;
    return util::length(translate[0], translate[1]);
}

bool FillLayer::Impl::queryIntersectsGeometry(
        const GeometryCollection& queryGeometry,
        const GeometryCollection& geometry,
        const float bearing,
        const float pixelsToTileUnits) const {

    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry, paint.fillTranslate, paint.fillTranslateAnchor, bearing, pixelsToTileUnits);

    return util::multiPolygonIntersectsMultiPolygon(translatedQueryGeometry.value_or(queryGeometry), geometry);
}

} // namespace style
} // namespace mbgl
