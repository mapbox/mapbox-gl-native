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
@property (nonatomic, readwrite) NSString *layerIdentifier;
@property (nonatomic, readwrite) NSString *sourceIdentifier;
@property (nonatomic, readwrite) NSString *sourceLayerIdentifier;

@end

@implementation MGLRasterStyleLayer

@synthesize mapView;


- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier source:(MGLSource *)source
{
    if (self = [super init]) {
        _layerIdentifier = layerIdentifier;
        _sourceIdentifier = source.sourceIdentifier;
        _layer = new mbgl::style::RasterLayer(layerIdentifier.UTF8String, source.sourceIdentifier.UTF8String);
    }
    return self;
}


#pragma mark - Accessing the Paint Attributes

- (void)setRasterOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterOpacity {
    self.layer->setRasterOpacity(rasterOpacity.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterOpacity {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getRasterOpacity() ?: self.layer->getDefaultRasterOpacity()];
}

- (void)setRasterHueRotate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterHueRotate {
    self.layer->setRasterHueRotate(rasterHueRotate.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterHueRotate {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getRasterHueRotate() ?: self.layer->getDefaultRasterHueRotate()];
}

- (void)setRasterBrightnessMin:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterBrightnessMin {
    self.layer->setRasterBrightnessMin(rasterBrightnessMin.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterBrightnessMin {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getRasterBrightnessMin() ?: self.layer->getDefaultRasterBrightnessMin()];
}

- (void)setRasterBrightnessMax:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterBrightnessMax {
    self.layer->setRasterBrightnessMax(rasterBrightnessMax.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterBrightnessMax {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getRasterBrightnessMax() ?: self.layer->getDefaultRasterBrightnessMax()];
}

- (void)setRasterSaturation:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterSaturation {
    self.layer->setRasterSaturation(rasterSaturation.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterSaturation {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getRasterSaturation() ?: self.layer->getDefaultRasterSaturation()];
}

- (void)setRasterContrast:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterContrast {
    self.layer->setRasterContrast(rasterContrast.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterContrast {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getRasterContrast() ?: self.layer->getDefaultRasterContrast()];
}

- (void)setRasterFadeDuration:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterFadeDuration {
    self.layer->setRasterFadeDuration(rasterFadeDuration.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterFadeDuration {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getRasterFadeDuration() ?: self.layer->getDefaultRasterFadeDuration()];
}

@end
