// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
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

- (void)setRasterOpacity:(CGFloat)rasterOpacity
{
    rasterLayer->setRasterOpacity(rasterOpacity);
}
- (CGFloat)rasterOpacity
{
    return rasterLayer->getRasterOpacity().asConstant();
}
- (void)setRasterHueRotate:(CGFloat)rasterHueRotate
{
    rasterLayer->setRasterHueRotate(rasterHueRotate);
}
- (CGFloat)rasterHueRotate
{
    return rasterLayer->getRasterHueRotate().asConstant();
}
- (void)setRasterBrightnessMin:(CGFloat)rasterBrightnessMin
{
    rasterLayer->setRasterBrightnessMin(rasterBrightnessMin);
}
- (CGFloat)rasterBrightnessMin
{
    return rasterLayer->getRasterBrightnessMin().asConstant();
}
- (void)setRasterBrightnessMax:(CGFloat)rasterBrightnessMax
{
    rasterLayer->setRasterBrightnessMax(rasterBrightnessMax);
}
- (CGFloat)rasterBrightnessMax
{
    return rasterLayer->getRasterBrightnessMax().asConstant();
}
- (void)setRasterSaturation:(CGFloat)rasterSaturation
{
    rasterLayer->setRasterSaturation(rasterSaturation);
}
- (CGFloat)rasterSaturation
{
    return rasterLayer->getRasterSaturation().asConstant();
}
- (void)setRasterContrast:(CGFloat)rasterContrast
{
    rasterLayer->setRasterContrast(rasterContrast);
}
- (CGFloat)rasterContrast
{
    return rasterLayer->getRasterContrast().asConstant();
}
- (void)setRasterFadeDuration:(CGFloat)rasterFadeDuration
{
    rasterLayer->setRasterFadeDuration(rasterFadeDuration);
}
- (CGFloat)rasterFadeDuration
{
    return rasterLayer->getRasterFadeDuration().asConstant();
}

@end
