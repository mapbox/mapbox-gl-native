// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLCircleStyleLayer.h"

#include <mbgl/style/layers/circle_layer.hpp>

@interface MGLCircleStyleLayer ()

@property (nonatomic) mbgl::style::CircleLayer *layer;
@property (nonatomic, readwrite) NSString *layerIdentifier;
@property (nonatomic, readwrite) NSString *sourceIdentifier;

@end

@implementation MGLCircleStyleLayer

@synthesize mapView;

- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier sourceIdentifier:(NSString *)sourceIdentifier {
    if (self = [super init]) {
        _layerIdentifier = layerIdentifier;
        _sourceIdentifier = sourceIdentifier;
        _layer = new mbgl::style::CircleLayer(layerIdentifier.UTF8String, sourceIdentifier.UTF8String);
    }
    return self;
}

#pragma mark - Accessing the Paint Attributes

- (void)setCircleRadius:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circleRadius {
    self.layer->setCircleRadius(circleRadius.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)circleRadius {
    return [MGLStyleAttribute mbgl_numberPropertyValueWith:self.layer->getCircleRadius()];
}

- (void)setCircleColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circleColor {
    self.layer->setCircleColor(circleColor.mbgl_colorPropertyValue);
}

- (id <MGLStyleAttributeValue>)circleColor {
    return [MGLStyleAttribute mbgl_colorPropertyValueWith:self.layer->getCircleColor()];
}

- (void)setCircleBlur:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circleBlur {
    self.layer->setCircleBlur(circleBlur.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)circleBlur {
    return [MGLStyleAttribute mbgl_numberPropertyValueWith:self.layer->getCircleBlur()];
}

- (void)setCircleOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circleOpacity {
    self.layer->setCircleOpacity(circleOpacity.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)circleOpacity {
    return [MGLStyleAttribute mbgl_numberPropertyValueWith:self.layer->getCircleOpacity()];
}

- (void)setCircleTranslate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circleTranslate {
    self.layer->setCircleTranslate(circleTranslate.mbgl_offsetPropertyValue);
}

- (id <MGLStyleAttributeValue>)circleTranslate {
    return [MGLStyleAttribute mbgl_offsetPropertyValueWith:self.layer->getCircleTranslate()];
}

- (void)setCircleTranslateAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circleTranslateAnchor {
    MGLSetEnumProperty(circleTranslateAnchor, CircleTranslateAnchor, TranslateAnchorType, MGLCircleStyleLayerCircleTranslateAnchor);
}

- (id <MGLStyleAttributeValue>)circleTranslateAnchor {
    auto rawValue = self.layer->getCircleTranslateAnchor();
    const char *type = @encode(MGLCircleStyleLayerCircleTranslateAnchor);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setCirclePitchScale:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circlePitchScale {
    MGLSetEnumProperty(circlePitchScale, CirclePitchScale, CirclePitchScaleType, MGLCircleStyleLayerCirclePitchScale);
}

- (id <MGLStyleAttributeValue>)circlePitchScale {
    auto rawValue = self.layer->getCirclePitchScale();
    const char *type = @encode(MGLCircleStyleLayerCirclePitchScale);
    return [NSValue value:&rawValue withObjCType:type];
}

@end
