#include <mbgl/renderer/render_circle_layer.hpp>
#include <mbgl/renderer/circle_bucket.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/intersection_tests.hpp>

namespace mbgl {

RenderCircleLayer::RenderCircleLayer(const style::CircleLayer::Impl& _impl)
        : RenderLayer(style::LayerType::Circle, _impl),
          impl(&_impl) {
}

std::unique_ptr<RenderLayer> RenderCircleLayer::clone() const {
    return std::make_unique<RenderCircleLayer>(*this);
}

std::unique_ptr<Bucket> RenderCircleLayer::createBucket(const BucketParameters& parameters, const std::vector<const RenderLayer*>& layers) const {
    return std::make_unique<CircleBucket>(parameters, layers);
}

void RenderCircleLayer::cascade(const CascadeParameters& parameters) {
    unevaluated = impl->cascading.cascade(parameters, std::move(unevaluated));
}

void RenderCircleLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);

    passes = ((evaluated.get<style::CircleRadius>().constantOr(1) > 0 ||
               evaluated.get<style::CircleStrokeWidth>().constantOr(1) > 0)
              && (evaluated.get<style::CircleColor>().constantOr(Color::black()).a > 0 ||
                  evaluated.get<style::CircleStrokeColor>().constantOr(Color::black()).a > 0)
              && (evaluated.get<style::CircleOpacity>().constantOr(1) > 0 ||
                  evaluated.get<style::CircleStrokeOpacity>().constantOr(1) > 0))
             ? RenderPass::Translucent : RenderPass::None;
}

bool RenderCircleLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

bool RenderCircleLayer::queryIntersectsFeature(
        const GeometryCoordinates& queryGeometry,
        const GeometryTileFeature& feature,
        const float zoom,
        const float bearing,
        const float pixelsToTileUnits) const {

    // Translate query geometry
    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry,
            evaluated.get<style::CircleTranslate>(),
            evaluated.get<style::CircleTranslateAnchor>(),
            bearing,
            pixelsToTileUnits);

    // Evaluate function
    auto circleRadius = evaluated.get<style::CircleRadius>()
                                .evaluate(feature, zoom, style::CircleRadius::defaultValue())
                        * pixelsToTileUnits;

    // Test intersection
    return util::polygonIntersectsBufferedMultiPoint(translatedQueryGeometry.value_or(queryGeometry), feature.getGeometries(), circleRadius);
}

}
