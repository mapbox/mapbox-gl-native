#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/renderer/fill_extrusion_bucket.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {
namespace style {

void FillExtrusionLayer::Impl::cascade(const CascadeParameters& parameters) {
    paint.cascade(parameters);
}

bool FillExtrusionLayer::Impl::evaluate(const PropertyEvaluationParameters& parameters) {
    paint.evaluate(parameters);

    passes = (paint.evaluated.get<FillExtrusionOpacity>() > 0) ? RenderPass::Translucent
                                                               : RenderPass::None;

    return paint.hasTransition();
}

std::unique_ptr<Bucket>
FillExtrusionLayer::Impl::createBucket(const BucketParameters& parameters,
                                       const std::vector<const Layer*>& layers) const {
    return std::make_unique<FillExtrusionBucket>(parameters, layers);
}

bool FillExtrusionLayer::Impl::queryIntersectsFeature(const GeometryCoordinates& queryGeometry,
                                                      const GeometryTileFeature& feature,
                                                      const float,
                                                      const float bearing,
                                                      const float pixelsToTileUnits) const {

    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
        queryGeometry, paint.evaluated.get<FillExtrusionTranslate>(),
        paint.evaluated.get<FillExtrusionTranslateAnchor>(), bearing, pixelsToTileUnits);

    return util::polygonIntersectsMultiPolygon(translatedQueryGeometry.value_or(queryGeometry),
                                               feature.getGeometries());
}

} // namespace style
} // namespace mbgl
