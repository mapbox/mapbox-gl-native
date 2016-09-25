// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLRasterStyleLayer.h"

#include <mbgl/style/layers/raster_layer.hpp>

@interface MGLRasterStyleLayer ()

@property (nonatomic) mbgl::style::RasterLayer *layer;

@end

@implementation MGLRasterStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    if (self = [super initWithIdentifier:identifier source:source]) {
        _layer = new mbgl::style::RasterLayer(identifier.UTF8String, source.identifier.UTF8String);
    }
    return self;
}

#pragma mark - Accessing the Paint Attributes

- (void)setRasterOpacity:(MGLStyleValue<NSNumber *> *)rasterOpacity {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterOpacity);
    self.layer->setRasterOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterOpacity {
    auto propertyValue = self.layer->getRasterOpacity() ?: self.layer->getDefaultRasterOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterHueRotate:(MGLStyleValue<NSNumber *> *)rasterHueRotate {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterHueRotate);
    self.layer->setRasterHueRotate(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterHueRotate {
    auto propertyValue = self.layer->getRasterHueRotate() ?: self.layer->getDefaultRasterHueRotate();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterBrightnessMin:(MGLStyleValue<NSNumber *> *)rasterBrightnessMin {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterBrightnessMin);
    self.layer->setRasterBrightnessMin(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterBrightnessMin {
    auto propertyValue = self.layer->getRasterBrightnessMin() ?: self.layer->getDefaultRasterBrightnessMin();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterBrightnessMax:(MGLStyleValue<NSNumber *> *)rasterBrightnessMax {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterBrightnessMax);
    self.layer->setRasterBrightnessMax(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterBrightnessMax {
    auto propertyValue = self.layer->getRasterBrightnessMax() ?: self.layer->getDefaultRasterBrightnessMax();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterSaturation:(MGLStyleValue<NSNumber *> *)rasterSaturation {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterSaturation);
    self.layer->setRasterSaturation(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterSaturation {
    auto propertyValue = self.layer->getRasterSaturation() ?: self.layer->getDefaultRasterSaturation();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterContrast:(MGLStyleValue<NSNumber *> *)rasterContrast {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterContrast);
    self.layer->setRasterContrast(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterContrast {
    auto propertyValue = self.layer->getRasterContrast() ?: self.layer->getDefaultRasterContrast();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterFadeDuration:(MGLStyleValue<NSNumber *> *)rasterFadeDuration {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterFadeDuration);
    self.layer->setRasterFadeDuration(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterFadeDuration {
    auto propertyValue = self.layer->getRasterFadeDuration() ?: self.layer->getDefaultRasterFadeDuration();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

@end
