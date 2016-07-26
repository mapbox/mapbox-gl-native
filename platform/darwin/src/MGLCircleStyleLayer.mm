// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLStyleAttribute.hpp"
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

- (void)setCircleRadius:(id <MGLStyleAttributeValue>)circleRadius
{
    // circleLayer->setCircleRadius(circleRadius.number.floatValue);
}
- (id <MGLStyleAttributeValue>)circleRadius
{
    return [[MGLStyleAttribute alloc] init]; //return circleLayer->getCircleRadius().asConstant();
}
- (void)setCircleColor:(id <MGLStyleAttributeValue>)circleColor
{
    circleLayer->setCircleColor([MGLStyleAttribute colorPropertyValueWith:circleColor]);
}
- (id <MGLStyleAttributeValue>)circleColor
{
    return [[MGLStyleAttribute alloc] init]; //return [MGLColor mbgl_color:circleLayer->getCircleColor().asConstant()];
}
- (void)setCircleBlur:(id <MGLStyleAttributeValue>)circleBlur
{
    // circleLayer->setCircleBlur(circleBlur.number.floatValue);
}
- (id <MGLStyleAttributeValue>)circleBlur
{
    return [[MGLStyleAttribute alloc] init]; //return circleLayer->getCircleBlur().asConstant();
}
- (void)setCircleOpacity:(id <MGLStyleAttributeValue>)circleOpacity
{
    // circleLayer->setCircleOpacity(circleOpacity.number.floatValue);
}
- (id <MGLStyleAttributeValue>)circleOpacity
{
    return [[MGLStyleAttribute alloc] init]; //return circleLayer->getCircleOpacity().asConstant();
}
- (void)setCircleTranslate:(id <MGLStyleAttributeValue>)circleTranslate
{
    // TODO: setterArray
}
- (id <MGLStyleAttributeValue>)circleTranslate
{
    return [[MGLStyleAttribute alloc] init]; //return @[]; // TODO: getterArray
}
- (void)setCircleTranslateAnchor:(id <MGLStyleAttributeValue>)circleTranslateAnchor
{
    // TODO: setterEnum
}
- (id <MGLStyleAttributeValue>)circleTranslateAnchor
{
    return [[MGLStyleAttribute alloc] init]; //return MGLCircleStyleLayerCircleTranslateAnchorMap;
}
- (void)setCirclePitchScale:(id <MGLStyleAttributeValue>)circlePitchScale
{
    // TODO: setterEnum
}
- (id <MGLStyleAttributeValue>)circlePitchScale
{
    return [[MGLStyleAttribute alloc] init]; //return MGLCircleStyleLayerCirclePitchScaleMap;
}

@end
