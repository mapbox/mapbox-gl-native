#include <mbgl/layer/circle_layer.hpp>
#include <mbgl/style/property_parsing.hpp>
#include <mbgl/style/style_bucket_parameters.hpp>
#include <mbgl/renderer/circle_bucket.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> CircleLayer::clone() const {
    std::unique_ptr<CircleLayer> result = std::make_unique<CircleLayer>();
    result->copy(*this);
    result->layout = layout;
    result->paints.paints = paints.paints;
    return std::move(result);
}

void CircleLayer::parsePaints(const JSVal& layer) {
    paints.parseEach(layer, [&] (ClassProperties& paint, const JSVal& value) {
        parseProperty<Function<float>>("circle-radius", PropertyKey::CircleRadius, paint, value);
        parseProperty<Function<Color>>("circle-color", PropertyKey::CircleColor, paint, value);
        parseProperty<Function<float>>("circle-opacity", PropertyKey::CircleOpacity, paint, value);
        parseProperty<Function<std::array<float,2>>>("circle-translate", PropertyKey::CircleTranslate, paint, value);
        parseProperty<Function<TranslateAnchorType>>("circle-translate-anchor", PropertyKey::CircleTranslateAnchor, paint, value);
        parseProperty<Function<float>>("circle-blur", PropertyKey::CircleBlur, paint, value);
    });
}

void CircleLayer::cascade(const StyleCascadeParameters& parameters) {
    paints.cascade(parameters);
}

bool CircleLayer::hasTransitions() const {
    return paints.hasTransitions();
}

void CircleLayer::recalculate(const StyleCalculationParameters& parameters) {
    paints.removeExpiredTransitions(parameters.now);

    paints.calculateTransitioned(PropertyKey::CircleRadius, properties.radius, parameters);
    paints.calculateTransitioned(PropertyKey::CircleColor, properties.color, parameters);
    paints.calculateTransitioned(PropertyKey::CircleOpacity, properties.opacity, parameters);
    paints.calculateTransitioned(PropertyKey::CircleTranslate, properties.translate, parameters);
    paints.calculate(PropertyKey::CircleTranslateAnchor, properties.translateAnchor, parameters);
    paints.calculateTransitioned(PropertyKey::CircleBlur, properties.blur, parameters);

    passes = properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

std::unique_ptr<Bucket> CircleLayer::createBucket(StyleBucketParameters& parameters) const {
    auto bucket = std::make_unique<CircleBucket>();

    parameters.eachFilteredFeature(filter, [&] (const auto& feature) {
        bucket->addGeometry(feature.getGeometries());
    });

    return std::move(bucket);
}

}
