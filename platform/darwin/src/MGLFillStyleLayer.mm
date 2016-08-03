// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLFillStyleLayer.h"

#include <mbgl/style/layers/fill_layer.hpp>

@interface MGLFillStyleLayer ()

@property (nonatomic) mbgl::style::FillLayer *layer;
@property (nonatomic, readwrite) NSString *layerIdentifier;
@property (nonatomic, readwrite) NSString *sourceIdentifier;

@end

@implementation MGLFillStyleLayer

@synthesize mapView;

- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier sourceIdentifier:(NSString *)sourceIdentifier {
    if (self = [super init]) {
        _layerIdentifier = layerIdentifier;
        _sourceIdentifier = sourceIdentifier;
        _layer = new mbgl::style::FillLayer(layerIdentifier.UTF8String, sourceIdentifier.UTF8String);
    }
    return self;
}

#pragma mark - Accessing the Paint Attributes

- (void)setFillAntialias:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillAntialias {
    self.layer->setFillAntialias(fillAntialias.mbgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)fillAntialias {
    return [MGLStyleAttribute mbgl_boolPropertyValueWith:self.layer->getFillAntialias()];
}

- (void)setFillOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillOpacity {
    self.layer->setFillOpacity(fillOpacity.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)fillOpacity {
    return [MGLStyleAttribute mbgl_numberPropertyValueWith:self.layer->getFillOpacity()];
}

- (void)setFillColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillColor {
    self.layer->setFillColor(fillColor.mbgl_colorPropertyValue);
}

- (id <MGLStyleAttributeValue>)fillColor {
    return [MGLStyleAttribute mbgl_colorPropertyValueWith:self.layer->getFillColor()];
}

- (void)setFillOutlineColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillOutlineColor {
    self.layer->setFillOutlineColor(fillOutlineColor.mbgl_colorPropertyValue);
}

- (id <MGLStyleAttributeValue>)fillOutlineColor {
    return [MGLStyleAttribute mbgl_colorPropertyValueWith:self.layer->getFillOutlineColor()];
}

- (void)setFillTranslate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillTranslate {
    self.layer->setFillTranslate(fillTranslate.mbgl_offsetPropertyValue);
}

- (id <MGLStyleAttributeValue>)fillTranslate {
    return [NSArray mbgl_offsetPropertyValue:self.layer->getFillTranslate()];
}

- (void)setFillTranslateAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillTranslateAnchor {
    MGLSetEnumProperty(fillTranslateAnchor, FillTranslateAnchor, TranslateAnchorType, MGLFillStyleLayerFillTranslateAnchor);
}

- (id <MGLStyleAttributeValue>)fillTranslateAnchor {
    auto rawValue = self.layer->getFillTranslateAnchor();
    const char *type = @encode(MGLFillStyleLayerFillTranslateAnchor);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setFillPattern:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillPattern {
    self.layer->setFillPattern(fillPattern.mbgl_stringPropertyValue);
}

- (id <MGLStyleAttributeValue>)fillPattern {
    return [NSString mbgl_stringWithPropertyValue:self.layer->getFillPattern()];
}

@end
