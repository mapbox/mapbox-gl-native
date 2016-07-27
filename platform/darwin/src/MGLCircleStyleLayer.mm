// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLCircleStyleLayer.h"
#import <mbgl/style/layers/circle_layer.hpp>

@interface MGLCircleStyleLayer()
@property (nonatomic) mbgl::style::CircleLayer *layer;
@end

@implementation MGLCircleStyleLayer

#pragma mark - Accessing the Paint Attributes

- (void)setCircleRadius:(id <MGLStyleAttributeValue>)circleRadius
{
    self.layer->setCircleRadius(circleRadius.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)circleRadius
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getCircleRadius()];
}
- (void)setCircleColor:(id <MGLStyleAttributeValue>)circleColor
{
    self.layer->setCircleColor(circleColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)circleColor
{
    return [MGLColor mbgl_propertyValue:self.layer->getCircleColor()];
}
- (void)setCircleBlur:(id <MGLStyleAttributeValue>)circleBlur
{
    self.layer->setCircleBlur(circleBlur.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)circleBlur
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getCircleBlur()];
}
- (void)setCircleOpacity:(id <MGLStyleAttributeValue>)circleOpacity
{
    self.layer->setCircleOpacity(circleOpacity.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)circleOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getCircleOpacity()];
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
    auto rawValue = self.layer->getCircleTranslateAnchor();
    const char *type = @encode(MGLCircleStyleLayerCircleTranslateAnchor);
    return [NSValue value:&rawValue withObjCType:type];
}
- (void)setCirclePitchScale:(id <MGLStyleAttributeValue>)circlePitchScale
{
    // TODO: setterEnum
}
- (id <MGLStyleAttributeValue>)circlePitchScale
{
    auto rawValue = self.layer->getCirclePitchScale();
    const char *type = @encode(MGLCircleStyleLayerCirclePitchScale);
    return [NSValue value:&rawValue withObjCType:type];
}

@end
