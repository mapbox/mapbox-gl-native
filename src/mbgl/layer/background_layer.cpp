#include <mbgl/layer/background_layer.hpp>
#include <mbgl/style/property_parsing.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> BackgroundLayer::clone() const {
    std::unique_ptr<BackgroundLayer> result = std::make_unique<BackgroundLayer>();
    result->copy(*this);
    result->layout = layout;
    result->paints.paints = paints.paints;
    return std::move(result);
}

void BackgroundLayer::parsePaints(const JSVal& layer) {
    paints.parseEach(layer, [&] (ClassProperties& paint, const JSVal& value) {
        parseProperty<Function<float>>("background-opacity", PropertyKey::BackgroundOpacity, paint, value);
        parseProperty<Function<Color>>("background-color", PropertyKey::BackgroundColor, paint, value);
        parseProperty<Function<Faded<std::string>>>("background-pattern", PropertyKey::BackgroundImage, paint, value);
    });
}

void BackgroundLayer::cascade(const StyleCascadeParameters& parameters) {
    paints.cascade(parameters);
}

bool BackgroundLayer::hasTransitions() const {
    return paints.hasTransitions();
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
