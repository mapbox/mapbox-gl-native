// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLRasterStyleLayer.h"
#import <mbgl/style/layers/raster_layer.hpp>

@interface MGLRasterStyleLayer()
@property (nonatomic) mbgl::style::RasterLayer *layer;
@end

@implementation MGLRasterStyleLayer

#pragma mark - Accessing the Paint Attributes

- (void)setRasterOpacity:(id <MGLStyleAttributeValue>)rasterOpacity
{
    self.layer->setRasterOpacity(rasterOpacity.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)rasterOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getRasterOpacity()];
}
- (void)setRasterHueRotate:(id <MGLStyleAttributeValue>)rasterHueRotate
{
    self.layer->setRasterHueRotate(rasterHueRotate.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)rasterHueRotate
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getRasterHueRotate()];
}
- (void)setRasterBrightnessMin:(id <MGLStyleAttributeValue>)rasterBrightnessMin
{
    self.layer->setRasterBrightnessMin(rasterBrightnessMin.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)rasterBrightnessMin
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getRasterBrightnessMin()];
}
- (void)setRasterBrightnessMax:(id <MGLStyleAttributeValue>)rasterBrightnessMax
{
    self.layer->setRasterBrightnessMax(rasterBrightnessMax.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)rasterBrightnessMax
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getRasterBrightnessMax()];
}
- (void)setRasterSaturation:(id <MGLStyleAttributeValue>)rasterSaturation
{
    self.layer->setRasterSaturation(rasterSaturation.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)rasterSaturation
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getRasterSaturation()];
}
- (void)setRasterContrast:(id <MGLStyleAttributeValue>)rasterContrast
{
    self.layer->setRasterContrast(rasterContrast.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)rasterContrast
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getRasterContrast()];
}
- (void)setRasterFadeDuration:(id <MGLStyleAttributeValue>)rasterFadeDuration
{
    self.layer->setRasterFadeDuration(rasterFadeDuration.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)rasterFadeDuration
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getRasterFadeDuration()];
}

@end
