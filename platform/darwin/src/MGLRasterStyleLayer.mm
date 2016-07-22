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
    return 0; // TODO: getterNumber
}
- (void)setRasterHueRotate:(CGFloat)rasterHueRotate
{
    rasterLayer->setRasterHueRotate(rasterHueRotate);
}
- (CGFloat)rasterHueRotate
{
    return 0; // TODO: getterNumber
}
- (void)setRasterBrightnessMin:(CGFloat)rasterBrightnessMin
{
    rasterLayer->setRasterBrightnessMin(rasterBrightnessMin);
}
- (CGFloat)rasterBrightnessMin
{
    return 0; // TODO: getterNumber
}
- (void)setRasterBrightnessMax:(CGFloat)rasterBrightnessMax
{
    rasterLayer->setRasterBrightnessMax(rasterBrightnessMax);
}
- (CGFloat)rasterBrightnessMax
{
    return 0; // TODO: getterNumber
}
- (void)setRasterSaturation:(CGFloat)rasterSaturation
{
    rasterLayer->setRasterSaturation(rasterSaturation);
}
- (CGFloat)rasterSaturation
{
    return 0; // TODO: getterNumber
}
- (void)setRasterContrast:(CGFloat)rasterContrast
{
    rasterLayer->setRasterContrast(rasterContrast);
}
- (CGFloat)rasterContrast
{
    return 0; // TODO: getterNumber
}
- (void)setRasterFadeDuration:(CGFloat)rasterFadeDuration
{
    rasterLayer->setRasterFadeDuration(rasterFadeDuration);
}
- (CGFloat)rasterFadeDuration
{
    return 0; // TODO: getterNumber
}

@end
