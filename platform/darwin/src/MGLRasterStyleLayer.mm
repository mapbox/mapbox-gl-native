// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "NSNumber+MGLStyleAttributeAdditions_Private.hpp"
#import "NSArray+MGLStyleAttributeAdditions_Private.hpp"
#import "NSString+MGLStyleAttributeAdditions_Private.hpp"
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
    // rasterLayer->setRasterOpacity(rasterOpacity.number.floatValue);
}
- (id <MGLStyleAttributeValue>)rasterOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:rasterLayer->getRasterOpacity()];
}
- (void)setRasterHueRotate:(id <MGLStyleAttributeValue>)rasterHueRotate
{
    // rasterLayer->setRasterHueRotate(rasterHueRotate.number.floatValue);
}
- (id <MGLStyleAttributeValue>)rasterHueRotate
{
    return [NSNumber mbgl_numberWithPropertyValue:rasterLayer->getRasterHueRotate()];
}
- (void)setRasterBrightnessMin:(id <MGLStyleAttributeValue>)rasterBrightnessMin
{
    // rasterLayer->setRasterBrightnessMin(rasterBrightnessMin.number.floatValue);
}
- (id <MGLStyleAttributeValue>)rasterBrightnessMin
{
    return [NSNumber mbgl_numberWithPropertyValue:rasterLayer->getRasterBrightnessMin()];
}
- (void)setRasterBrightnessMax:(id <MGLStyleAttributeValue>)rasterBrightnessMax
{
    // rasterLayer->setRasterBrightnessMax(rasterBrightnessMax.number.floatValue);
}
- (id <MGLStyleAttributeValue>)rasterBrightnessMax
{
    return [NSNumber mbgl_numberWithPropertyValue:rasterLayer->getRasterBrightnessMax()];
}
- (void)setRasterSaturation:(id <MGLStyleAttributeValue>)rasterSaturation
{
    // rasterLayer->setRasterSaturation(rasterSaturation.number.floatValue);
}
- (id <MGLStyleAttributeValue>)rasterSaturation
{
    return [NSNumber mbgl_numberWithPropertyValue:rasterLayer->getRasterSaturation()];
}
- (void)setRasterContrast:(id <MGLStyleAttributeValue>)rasterContrast
{
    // rasterLayer->setRasterContrast(rasterContrast.number.floatValue);
}
- (id <MGLStyleAttributeValue>)rasterContrast
{
    return [NSNumber mbgl_numberWithPropertyValue:rasterLayer->getRasterContrast()];
}
- (void)setRasterFadeDuration:(id <MGLStyleAttributeValue>)rasterFadeDuration
{
    // rasterLayer->setRasterFadeDuration(rasterFadeDuration.number.floatValue);
}
- (id <MGLStyleAttributeValue>)rasterFadeDuration
{
    return [NSNumber mbgl_numberWithPropertyValue:rasterLayer->getRasterFadeDuration()];
}

@end
