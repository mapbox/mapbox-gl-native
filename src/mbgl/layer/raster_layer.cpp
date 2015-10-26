#include <mbgl/layer/raster_layer.hpp>
#include <mbgl/style/property_parsing.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {

std::unique_ptr<StyleLayer> RasterLayer::clone() const {
    std::unique_ptr<RasterLayer> result = std::make_unique<RasterLayer>();
    result->copy(*this);
    result->paints.paints = paints.paints;
    return std::move(result);
}

void RasterLayer::parsePaints(const JSVal& layer) {
    paints.parseEach(layer, [&] (ClassProperties& paint, const JSVal& value) {
        parseProperty<Function<float>>("raster-opacity", PropertyKey::RasterOpacity, paint, value);
        parseProperty<PropertyTransition>("raster-opacity-transition", PropertyKey::RasterOpacity, paint, value);
        parseProperty<Function<float>>("raster-hue-rotate", PropertyKey::RasterHueRotate, paint, value);
        parseProperty<PropertyTransition>("raster-hue-rotate-transition", PropertyKey::RasterHueRotate, paint, value);
        parseProperty<Function<float>>("raster-brightness-min", PropertyKey::RasterBrightnessLow, paint, value);
        parseProperty<Function<float>>("raster-brightness-max", PropertyKey::RasterBrightnessHigh, paint, value);
        parseProperty<PropertyTransition>("raster-brightness-transition", PropertyKey::RasterBrightness, paint, value);
        parseProperty<Function<float>>("raster-saturation", PropertyKey::RasterSaturation, paint, value);
        parseProperty<PropertyTransition>("raster-saturation-transition", PropertyKey::RasterSaturation, paint, value);
        parseProperty<Function<float>>("raster-contrast", PropertyKey::RasterContrast, paint, value);
        parseProperty<PropertyTransition>("raster-contrast-transition", PropertyKey::RasterContrast, paint, value);
        parseProperty<Function<float>>("raster-fade-duration", PropertyKey::RasterFade, paint, value);
        parseProperty<PropertyTransition>("raster-fade-duration-transition", PropertyKey::RasterFade, paint, value);
    });
}

void RasterLayer::cascade(const StyleCascadeParameters& parameters) {
    paints.cascade(parameters);
}

bool RasterLayer::hasTransitions() const {
    return paints.hasTransitions();
}

void RasterLayer::recalculate(const StyleCalculationParameters& parameters) {
    paints.removeExpiredTransitions(parameters.now);

    paints.calculateTransitioned(PropertyKey::RasterOpacity, properties.opacity, parameters);
    paints.calculateTransitioned(PropertyKey::RasterHueRotate, properties.hue_rotate, parameters);
    paints.calculateTransitioned(PropertyKey::RasterBrightnessLow, properties.brightness[0], parameters);
    paints.calculateTransitioned(PropertyKey::RasterBrightnessHigh, properties.brightness[1], parameters);
    paints.calculateTransitioned(PropertyKey::RasterSaturation, properties.saturation, parameters);
    paints.calculateTransitioned(PropertyKey::RasterContrast, properties.contrast, parameters);
    paints.calculateTransitioned(PropertyKey::RasterFade, properties.fade, parameters);

    passes = properties.isVisible() ? RenderPass::Translucent : RenderPass::None;
}

std::unique_ptr<Bucket> RasterLayer::createBucket(StyleBucketParameters&) const {
    return nullptr;
}

}
