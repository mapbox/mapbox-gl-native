// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLBaseStyleLayer_Private.h"
#import "MGLStyleLayer_Private.h"
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
    [self update];
}

- (id <MGLStyleAttributeValue>)circleRadius {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getCircleRadius() ?: self.layer->getDefaultCircleRadius()];
}

- (void)setCircleColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circleColor {
    self.layer->setCircleColor(circleColor.mbgl_colorPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)circleColor {
    return [MGLStyleAttribute mbgl_colorWithPropertyValueColor:self.layer->getCircleColor() ?: self.layer->getDefaultCircleColor()];
}

- (void)setCircleBlur:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circleBlur {
    self.layer->setCircleBlur(circleBlur.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)circleBlur {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getCircleBlur() ?: self.layer->getDefaultCircleBlur()];
}

- (void)setCircleOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circleOpacity {
    self.layer->setCircleOpacity(circleOpacity.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)circleOpacity {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getCircleOpacity() ?: self.layer->getDefaultCircleOpacity()];
}

- (void)setCircleTranslate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circleTranslate {
    self.layer->setCircleTranslate(circleTranslate.mbgl_offsetPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)circleTranslate {
    return [MGLStyleAttribute mbgl_offsetWithPropertyValueOffset:self.layer->getCircleTranslate() ?: self.layer->getDefaultCircleTranslate()];
}

- (void)setCircleTranslateAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circleTranslateAnchor {
    MGLSetEnumProperty(circleTranslateAnchor, CircleTranslateAnchor, TranslateAnchorType, MGLCircleStyleLayerCircleTranslateAnchor);
    [self update];
}

- (id <MGLStyleAttributeValue>)circleTranslateAnchor {
    MGLGetEnumProperty(CircleTranslateAnchor, TranslateAnchorType, MGLCircleStyleLayerCircleTranslateAnchor);
}

- (void)setCirclePitchScale:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)circlePitchScale {
    MGLSetEnumProperty(circlePitchScale, CirclePitchScale, CirclePitchScaleType, MGLCircleStyleLayerCirclePitchScale);
    [self update];
}

- (id <MGLStyleAttributeValue>)circlePitchScale {
    MGLGetEnumProperty(CirclePitchScale, CirclePitchScaleType, MGLCircleStyleLayerCirclePitchScale);
}

@end
