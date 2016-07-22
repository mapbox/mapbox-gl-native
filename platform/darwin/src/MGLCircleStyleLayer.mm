// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-ios`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLCircleStyleLayer.h"
#import <mbgl/style/layers/circle_layer.hpp>

@interface MGLCircleStyleLayer() {
    mbgl::style::CircleLayer *circleLayer;
}
@end

@implementation MGLCircleStyleLayer

- (void)setLayer:(mbgl::style::Layer *)layer
{
    circleLayer = reinterpret_cast<mbgl::style::CircleLayer *>(layer);
}

#pragma mark - Accessing the Paint Attributes

- (void)setCircleRadius:(CGFloat)circleRadius
{
    circleLayer->setCircleRadius(circleRadius);
}
- (CGFloat)circleRadius
{
    // TODO:
    return 0;
}
- (void)setCircleColor:(MGLColor*)circleColor
{
    circleLayer->setCircleColor(circleColor.mbgl_color);
}
- (MGLColor *)circleColor
{
    return [MGLColor mbgl_color:circleLayer->getCircleColor().asConstant()];
}
- (void)setCircleBlur:(CGFloat)circleBlur
{
    circleLayer->setCircleBlur(circleBlur);
}
- (CGFloat)circleBlur
{
    // TODO:
    return 0;
}
- (void)setCircleOpacity:(CGFloat)circleOpacity
{
    circleLayer->setCircleOpacity(circleOpacity);
}
- (CGFloat)circleOpacity
{
    // TODO:
    return 0;
}
- (void)setCircleTranslate:(NSArray*)circleTranslate
{
}
- (NSArray *)circleTranslate
{
    // TODO:
    return @[];
}
- (void)setCircleTranslateAnchor:(MGLCircleStyleLayerCircleTranslateAnchor)circleTranslateAnchor
{
}
- (MGLCircleStyleLayerCircleTranslateAnchor)circleTranslateAnchor
{
    // TODO:
    return MGLCircleStyleLayerCircleTranslateAnchorMap;
}
- (void)setCirclePitchScale:(MGLCircleStyleLayerCirclePitchScale)circlePitchScale
{
}
- (MGLCircleStyleLayerCirclePitchScale)circlePitchScale
{
    // TODO:
    return MGLCircleStyleLayerCirclePitchScaleMap;
}

@end