#include <mbgl/layer/background_layer.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> BackgroundLayer::clone() const {
    return std::make_unique<BackgroundLayer>(*this);
}

void BackgroundLayer::parsePaints(const JSValue& layer) {
    paint.backgroundOpacity.parse("background-opacity", layer);
    paint.backgroundColor.parse("background-color", layer);
    paint.backgroundPattern.parse("background-pattern", layer);
}

void BackgroundLayer::cascade(const StyleCascadeParameters& parameters) {
    paint.backgroundOpacity.cascade(parameters);
    paint.backgroundColor.cascade(parameters);
    paint.backgroundPattern.cascade(parameters);
}

bool BackgroundLayer::recalculate(const StyleCalculationParameters& parameters) {
    bool hasTransitions = false;

    hasTransitions |= paint.backgroundOpacity.calculate(parameters);
    hasTransitions |= paint.backgroundColor.calculate(parameters);
    hasTransitions |= paint.backgroundPattern.calculate(parameters);

    passes = paint.backgroundOpacity > 0 ? RenderPass::Translucent : RenderPass::None;

    return hasTransitions;
}

std::unique_ptr<Bucket> BackgroundLayer::createBucket(StyleBucketParameters&) const {
    return nullptr;
}

} // namespace mbgl
