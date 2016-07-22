// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-ios`.

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
    // TODO:
    return 0;
}
- (void)setRasterHueRotate:(CGFloat)rasterHueRotate
{
    rasterLayer->setRasterHueRotate(rasterHueRotate);
}
- (CGFloat)rasterHueRotate
{
    // TODO:
    return 0;
}
- (void)setRasterBrightnessMin:(CGFloat)rasterBrightnessMin
{
    rasterLayer->setRasterBrightnessMin(rasterBrightnessMin);
}
- (CGFloat)rasterBrightnessMin
{
    // TODO:
    return 0;
}
- (void)setRasterBrightnessMax:(CGFloat)rasterBrightnessMax
{
    rasterLayer->setRasterBrightnessMax(rasterBrightnessMax);
}
- (CGFloat)rasterBrightnessMax
{
    // TODO:
    return 0;
}
- (void)setRasterSaturation:(CGFloat)rasterSaturation
{
    rasterLayer->setRasterSaturation(rasterSaturation);
}
- (CGFloat)rasterSaturation
{
    // TODO:
    return 0;
}
- (void)setRasterContrast:(CGFloat)rasterContrast
{
    rasterLayer->setRasterContrast(rasterContrast);
}
- (CGFloat)rasterContrast
{
    // TODO:
    return 0;
}
- (void)setRasterFadeDuration:(CGFloat)rasterFadeDuration
{
    rasterLayer->setRasterFadeDuration(rasterFadeDuration);
}
- (CGFloat)rasterFadeDuration
{
    // TODO:
    return 0;
}

@end