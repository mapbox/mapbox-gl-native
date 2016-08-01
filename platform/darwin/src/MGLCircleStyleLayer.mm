// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLCircleStyleLayer.h"
#import <mbgl/style/layers/circle_layer.hpp>

@interface MGLCircleStyleLayer()
@property (nonatomic) mbgl::style::CircleLayer *layer;
@property (nonatomic, readwrite) NSString *layerID;
@property (nonatomic, readwrite) NSString *sourceID;
@end

@implementation MGLCircleStyleLayer

@synthesize mapView;

- (instancetype)initWithLayerID:(NSString *)layerID sourceID:(NSString *)sourceID
{
    self = [super init];
    if (self == nil) return nil;
    _layerID = layerID;
    _sourceID = sourceID;
    _layer = new mbgl::style::CircleLayer(layerID.UTF8String, sourceID.UTF8String);
    return self;
}

#pragma mark - Accessing the Paint Attributes

- (void)setCircleRadius:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circleRadius
{
    self.layer->setCircleRadius(circleRadius.mbgl_floatPropertyValue);
}
- (id <MGLStyleAttributeValue>)circleRadius
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getCircleRadius()];
}
- (void)setCircleColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circleColor
{
    self.layer->setCircleColor(circleColor.mbgl_colorPropertyValue);
}
- (id <MGLStyleAttributeValue>)circleColor
{
    return [MGLColor mbgl_propertyValue:self.layer->getCircleColor()];
}
- (void)setCircleBlur:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circleBlur
{
    self.layer->setCircleBlur(circleBlur.mbgl_floatPropertyValue);
}
- (id <MGLStyleAttributeValue>)circleBlur
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getCircleBlur()];
}
- (void)setCircleOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circleOpacity
{
    self.layer->setCircleOpacity(circleOpacity.mbgl_floatPropertyValue);
}
- (id <MGLStyleAttributeValue>)circleOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getCircleOpacity()];
}
- (void)setCircleTranslate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circleTranslate
{
    self.layer->setCircleTranslate(circleTranslate.mbgl_offsetPropertyValue);
}
- (id <MGLStyleAttributeValue>)circleTranslate
{
    return @[];
}
- (void)setCircleTranslateAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circleTranslateAnchor
{
    // TODO: setterEnum
}
- (id <MGLStyleAttributeValue>)circleTranslateAnchor
{
    auto rawValue = self.layer->getCircleTranslateAnchor();
    const char *type = @encode(MGLCircleStyleLayerCircleTranslateAnchor);
    return [NSValue value:&rawValue withObjCType:type];
}
- (void)setCirclePitchScale:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circlePitchScale
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
