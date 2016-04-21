#include <mbgl/layer/raster_layer.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> RasterLayer::clone() const {
    return std::make_unique<RasterLayer>(*this);
}

void RasterLayer::parsePaints(const JSValue& layer) {
    paint.rasterOpacity.parse("raster-opacity", layer);
    paint.rasterHueRotate.parse("raster-hue-rotate", layer);
    paint.rasterBrightnessMin.parse("raster-brightness-min", layer);
    paint.rasterBrightnessMax.parse("raster-brightness-max", layer);
    paint.rasterSaturation.parse("raster-saturation", layer);
    paint.rasterContrast.parse("raster-contrast", layer);
    paint.rasterFadeDuration.parse("raster-fade-duration", layer);
}

void RasterLayer::cascade(const StyleCascadeParameters& parameters) {
    paint.rasterOpacity.cascade(parameters);
    paint.rasterHueRotate.cascade(parameters);
    paint.rasterBrightnessMin.cascade(parameters);
    paint.rasterBrightnessMax.cascade(parameters);
    paint.rasterSaturation.cascade(parameters);
    paint.rasterContrast.cascade(parameters);
    paint.rasterFadeDuration.cascade(parameters);
}

bool RasterLayer::recalculate(const StyleCalculationParameters& parameters) {
    bool hasTransitions = false;

    hasTransitions |= paint.rasterOpacity.calculate(parameters);
    hasTransitions |= paint.rasterHueRotate.calculate(parameters);
    hasTransitions |= paint.rasterBrightnessMin.calculate(parameters);
    hasTransitions |= paint.rasterBrightnessMax.calculate(parameters);
    hasTransitions |= paint.rasterSaturation.calculate(parameters);
    hasTransitions |= paint.rasterContrast.calculate(parameters);
    hasTransitions |= paint.rasterFadeDuration.calculate(parameters);

    passes = paint.rasterOpacity > 0 ? RenderPass::Translucent : RenderPass::None;

    return hasTransitions;
}

std::unique_ptr<Bucket> RasterLayer::createBucket(StyleBucketParameters&) const {
    return nullptr;
}

} // namespace mbgl
