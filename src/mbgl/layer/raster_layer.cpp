#include <mbgl/layer/raster_layer.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> RasterLayer::clone() const {
    std::unique_ptr<RasterLayer> result = std::make_unique<RasterLayer>();
    result->copy(*this);
    result->paint = paint;
    return std::move(result);
}

void RasterLayer::parsePaints(const JSVal& layer) {
    paint.opacity.parse("raster-opacity", layer);
    paint.hueRotate.parse("raster-hue-rotate", layer);
    paint.brightnessMin.parse("raster-brightness-min", layer);
    paint.brightnessMax.parse("raster-brightness-max", layer);
    paint.saturation.parse("raster-saturation", layer);
    paint.contrast.parse("raster-contrast", layer);
    paint.fadeDuration.parse("raster-fade-duration", layer);
}

void RasterLayer::cascade(const StyleCascadeParameters& parameters) {
    paint.opacity.cascade(parameters);
    paint.hueRotate.cascade(parameters);
    paint.brightnessMin.cascade(parameters);
    paint.brightnessMax.cascade(parameters);
    paint.saturation.cascade(parameters);
    paint.contrast.cascade(parameters);
    paint.fadeDuration.cascade(parameters);
}

bool RasterLayer::recalculate(const StyleCalculationParameters& parameters) {
    bool hasTransitions = false;

    hasTransitions |= paint.opacity.calculate(parameters);
    hasTransitions |= paint.hueRotate.calculate(parameters);
    hasTransitions |= paint.brightnessMin.calculate(parameters);
    hasTransitions |= paint.brightnessMax.calculate(parameters);
    hasTransitions |= paint.saturation.calculate(parameters);
    hasTransitions |= paint.contrast.calculate(parameters);
    hasTransitions |= paint.fadeDuration.calculate(parameters);

    passes = paint.opacity > 0 ? RenderPass::Translucent : RenderPass::None;

    return hasTransitions;
}

std::unique_ptr<Bucket> RasterLayer::createBucket(StyleBucketParameters&) const {
    return nullptr;
}

}
