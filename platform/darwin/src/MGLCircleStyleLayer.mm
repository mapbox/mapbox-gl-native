// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

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
    return circleLayer->getCircleRadius().asConstant();
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
    return circleLayer->getCircleBlur().asConstant();
}
- (void)setCircleOpacity:(CGFloat)circleOpacity
{
    circleLayer->setCircleOpacity(circleOpacity);
}
- (CGFloat)circleOpacity
{
    return circleLayer->getCircleOpacity().asConstant();
}
- (void)setCircleTranslate:(NSArray*)circleTranslate
{
    // TODO: setterArray
}
- (NSArray *)circleTranslate
{
    return @[]; // TODO: getterArray
}
- (void)setCircleTranslateAnchor:(MGLCircleStyleLayerCircleTranslateAnchor)circleTranslateAnchor
{
    // TODO: setterEnum
}
- (MGLCircleStyleLayerCircleTranslateAnchor)circleTranslateAnchor
{
    return MGLCircleStyleLayerCircleTranslateAnchorMap;
}
- (void)setCirclePitchScale:(MGLCircleStyleLayerCirclePitchScale)circlePitchScale
{
    // TODO: setterEnum
}
- (MGLCircleStyleLayerCirclePitchScale)circlePitchScale
{
    return MGLCircleStyleLayerCirclePitchScaleMap;
}

@end
