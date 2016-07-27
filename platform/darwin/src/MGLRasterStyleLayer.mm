// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLRasterStyleLayer.h"
#import <mbgl/style/layers/raster_layer.hpp>

@interface MGLRasterStyleLayer() {
    mbgl::style::RasterLayer *rasterLayer;
}
@end

@implementation MGLRasterStyleLayer

- (void)setLayer:(mbgl::style::Layer *)layer
{
    rasterLayer = reinterpret_cast<mbgl::style::RasterLayer *>(layer);
}

#pragma mark - Accessing the Paint Attributes

- (void)setRasterOpacity:(id <MGLStyleAttributeValue>)rasterOpacity
{
    rasterLayer->setRasterOpacity(rasterOpacity.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)rasterOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:rasterLayer->getRasterOpacity()];
}
- (void)setRasterHueRotate:(id <MGLStyleAttributeValue>)rasterHueRotate
{
    rasterLayer->setRasterHueRotate(rasterHueRotate.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)rasterHueRotate
{
    return [NSNumber mbgl_numberWithPropertyValue:rasterLayer->getRasterHueRotate()];
}
- (void)setRasterBrightnessMin:(id <MGLStyleAttributeValue>)rasterBrightnessMin
{
    rasterLayer->setRasterBrightnessMin(rasterBrightnessMin.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)rasterBrightnessMin
{
    return [NSNumber mbgl_numberWithPropertyValue:rasterLayer->getRasterBrightnessMin()];
}
- (void)setRasterBrightnessMax:(id <MGLStyleAttributeValue>)rasterBrightnessMax
{
    rasterLayer->setRasterBrightnessMax(rasterBrightnessMax.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)rasterBrightnessMax
{
    return [NSNumber mbgl_numberWithPropertyValue:rasterLayer->getRasterBrightnessMax()];
}
- (void)setRasterSaturation:(id <MGLStyleAttributeValue>)rasterSaturation
{
    rasterLayer->setRasterSaturation(rasterSaturation.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)rasterSaturation
{
    return [NSNumber mbgl_numberWithPropertyValue:rasterLayer->getRasterSaturation()];
}
- (void)setRasterContrast:(id <MGLStyleAttributeValue>)rasterContrast
{
    rasterLayer->setRasterContrast(rasterContrast.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)rasterContrast
{
    return [NSNumber mbgl_numberWithPropertyValue:rasterLayer->getRasterContrast()];
}
- (void)setRasterFadeDuration:(id <MGLStyleAttributeValue>)rasterFadeDuration
{
    rasterLayer->setRasterFadeDuration(rasterFadeDuration.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)rasterFadeDuration
{
    return [NSNumber mbgl_numberWithPropertyValue:rasterLayer->getRasterFadeDuration()];
}

@end
