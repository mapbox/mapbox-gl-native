// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
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
    circleLayer->setCircleRadius(circleRadius.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)circleRadius
{
    return [NSNumber mbgl_numberWithPropertyValue:circleLayer->getCircleRadius()];
}
- (void)setCircleColor:(id <MGLStyleAttributeValue>)circleColor
{
    circleLayer->setCircleColor(circleColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)circleColor
{
    return [MGLColor mbgl_propertyValue:circleLayer->getCircleColor()];
}
- (void)setCircleBlur:(id <MGLStyleAttributeValue>)circleBlur
{
    circleLayer->setCircleBlur(circleBlur.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)circleBlur
{
    return [NSNumber mbgl_numberWithPropertyValue:circleLayer->getCircleBlur()];
}
- (void)setCircleOpacity:(id <MGLStyleAttributeValue>)circleOpacity
{
    circleLayer->setCircleOpacity(circleOpacity.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)circleOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:circleLayer->getCircleOpacity()];
}
- (void)setCircleTranslate:(id <MGLStyleAttributeValue>)circleTranslate
{
    // TODO: setterArray
}
- (id <MGLStyleAttributeValue>)circleTranslate
{
    return @[];
}
- (void)setCircleTranslateAnchor:(id <MGLStyleAttributeValue>)circleTranslateAnchor
{
    // TODO: setterEnum
}
- (id <MGLStyleAttributeValue>)circleTranslateAnchor
{
    auto rawValue = circleLayer->getCircleTranslateAnchor();
    const char *type = @encode(MGLCircleStyleLayerCircleTranslateAnchor);
    return [NSValue value:&rawValue withObjCType:type];
}
- (void)setCirclePitchScale:(id <MGLStyleAttributeValue>)circlePitchScale
{
    // TODO: setterEnum
}
- (id <MGLStyleAttributeValue>)circlePitchScale
{
    auto rawValue = circleLayer->getCirclePitchScale();
    const char *type = @encode(MGLCircleStyleLayerCirclePitchScale);
    return [NSValue value:&rawValue withObjCType:type];
}

@end
