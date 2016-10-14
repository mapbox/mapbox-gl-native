// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleAttributeValue.h"
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

- (void)setRasterOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterOpacity {
    self.layer->setRasterOpacity(rasterOpacity.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterOpacity {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getRasterOpacity() ?: self.layer->getDefaultRasterOpacity()];
}

- (void)setRasterHueRotate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterHueRotate {
    self.layer->setRasterHueRotate(rasterHueRotate.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterHueRotate {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getRasterHueRotate() ?: self.layer->getDefaultRasterHueRotate()];
}

- (void)setRasterBrightnessMin:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterBrightnessMin {
    self.layer->setRasterBrightnessMin(rasterBrightnessMin.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterBrightnessMin {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getRasterBrightnessMin() ?: self.layer->getDefaultRasterBrightnessMin()];
}

- (void)setRasterBrightnessMax:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterBrightnessMax {
    self.layer->setRasterBrightnessMax(rasterBrightnessMax.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterBrightnessMax {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getRasterBrightnessMax() ?: self.layer->getDefaultRasterBrightnessMax()];
}

- (void)setRasterSaturation:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterSaturation {
    self.layer->setRasterSaturation(rasterSaturation.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterSaturation {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getRasterSaturation() ?: self.layer->getDefaultRasterSaturation()];
}

- (void)setRasterContrast:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterContrast {
    self.layer->setRasterContrast(rasterContrast.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterContrast {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getRasterContrast() ?: self.layer->getDefaultRasterContrast()];
}

- (void)setRasterFadeDuration:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterFadeDuration {
    self.layer->setRasterFadeDuration(rasterFadeDuration.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterFadeDuration {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getRasterFadeDuration() ?: self.layer->getDefaultRasterFadeDuration()];
}

@end
