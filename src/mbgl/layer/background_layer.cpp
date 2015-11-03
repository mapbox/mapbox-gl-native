#include <mbgl/layer/background_layer.hpp>
#include <mbgl/style/property_parsing.hpp>

namespace mbgl {

void BackgroundLayer::parsePaints(const JSVal& layer) {
    paints.parseEach(layer, [&] (ClassProperties& paint, const JSVal& value) {
        parseProperty<Function<float>>("background-opacity", PropertyKey::BackgroundOpacity, paint, value);
        parseProperty<Function<Color>>("background-color", PropertyKey::BackgroundColor, paint, value);
        parseProperty<Function<Faded<std::string>>>("background-pattern", PropertyKey::BackgroundImage, paint, value, "background-pattern-transition");
    });
}

void BackgroundLayer::recalculate(const StyleCalculationParameters& parameters) {
    paints.removeExpiredTransitions(parameters.now);

    paints.calculateTransitioned(PropertyKey::BackgroundOpacity, properties.opacity, parameters);
    paints.calculateTransitioned(PropertyKey::BackgroundColor, properties.color, parameters);
    paints.calculate(PropertyKey::BackgroundImage, properties.image, parameters);

    passes = properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

std::unique_ptr<Bucket> BackgroundLayer::createBucket(StyleBucketParameters&) const {
    return nullptr;
}

}
