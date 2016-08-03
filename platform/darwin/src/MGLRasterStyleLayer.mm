// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLRasterStyleLayer.h"

#include <mbgl/style/layers/raster_layer.hpp>

@interface MGLRasterStyleLayer ()

@property (nonatomic) mbgl::style::RasterLayer *layer;
@property (nonatomic, readwrite) NSString *layerID;
@property (nonatomic, readwrite) NSString *sourceID;

@end

@implementation MGLRasterStyleLayer

@synthesize mapView;

- (instancetype)initWithLayerID:(NSString *)layerID sourceID:(NSString *)sourceID {
    self = [super init];
    if (self == nil) return nil;
    _layerID = layerID;
    _sourceID = sourceID;
    _layer = new mbgl::style::RasterLayer(layerID.UTF8String, sourceID.UTF8String);
    return self;
}

#pragma mark - Accessing the Paint Attributes

- (void)setRasterOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterOpacity {
    self.layer->setRasterOpacity(rasterOpacity.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterOpacity {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getRasterOpacity()];
}

- (void)setRasterHueRotate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterHueRotate {
    self.layer->setRasterHueRotate(rasterHueRotate.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterHueRotate {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getRasterHueRotate()];
}

- (void)setRasterBrightnessMin:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterBrightnessMin {
    self.layer->setRasterBrightnessMin(rasterBrightnessMin.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterBrightnessMin {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getRasterBrightnessMin()];
}

- (void)setRasterBrightnessMax:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterBrightnessMax {
    self.layer->setRasterBrightnessMax(rasterBrightnessMax.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterBrightnessMax {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getRasterBrightnessMax()];
}

- (void)setRasterSaturation:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterSaturation {
    self.layer->setRasterSaturation(rasterSaturation.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterSaturation {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getRasterSaturation()];
}

- (void)setRasterContrast:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterContrast {
    self.layer->setRasterContrast(rasterContrast.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterContrast {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getRasterContrast()];
}

- (void)setRasterFadeDuration:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)rasterFadeDuration {
    self.layer->setRasterFadeDuration(rasterFadeDuration.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)rasterFadeDuration {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getRasterFadeDuration()];
}

@end
