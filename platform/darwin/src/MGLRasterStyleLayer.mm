// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLStyleAttribute.hpp"
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
    return [[MGLStyleAttribute alloc] init]; //return rasterLayer->getRasterOpacity().asConstant();
}
- (void)setRasterHueRotate:(id <MGLStyleAttributeValue>)rasterHueRotate
{
    // rasterLayer->setRasterHueRotate(rasterHueRotate.number.floatValue);
}
- (id <MGLStyleAttributeValue>)rasterHueRotate
{
    return [[MGLStyleAttribute alloc] init]; //return rasterLayer->getRasterHueRotate().asConstant();
}
- (void)setRasterBrightnessMin:(id <MGLStyleAttributeValue>)rasterBrightnessMin
{
    // rasterLayer->setRasterBrightnessMin(rasterBrightnessMin.number.floatValue);
}
- (id <MGLStyleAttributeValue>)rasterBrightnessMin
{
    return [[MGLStyleAttribute alloc] init]; //return rasterLayer->getRasterBrightnessMin().asConstant();
}
- (void)setRasterBrightnessMax:(id <MGLStyleAttributeValue>)rasterBrightnessMax
{
    // rasterLayer->setRasterBrightnessMax(rasterBrightnessMax.number.floatValue);
}
- (id <MGLStyleAttributeValue>)rasterBrightnessMax
{
    return [[MGLStyleAttribute alloc] init]; //return rasterLayer->getRasterBrightnessMax().asConstant();
}
- (void)setRasterSaturation:(id <MGLStyleAttributeValue>)rasterSaturation
{
    // rasterLayer->setRasterSaturation(rasterSaturation.number.floatValue);
}
- (id <MGLStyleAttributeValue>)rasterSaturation
{
    return [[MGLStyleAttribute alloc] init]; //return rasterLayer->getRasterSaturation().asConstant();
}
- (void)setRasterContrast:(id <MGLStyleAttributeValue>)rasterContrast
{
    // rasterLayer->setRasterContrast(rasterContrast.number.floatValue);
}
- (id <MGLStyleAttributeValue>)rasterContrast
{
    return [[MGLStyleAttribute alloc] init]; //return rasterLayer->getRasterContrast().asConstant();
}
- (void)setRasterFadeDuration:(id <MGLStyleAttributeValue>)rasterFadeDuration
{
    // rasterLayer->setRasterFadeDuration(rasterFadeDuration.number.floatValue);
}
- (id <MGLStyleAttributeValue>)rasterFadeDuration
{
    return [[MGLStyleAttribute alloc] init]; //return rasterLayer->getRasterFadeDuration().asConstant();
}

@end
