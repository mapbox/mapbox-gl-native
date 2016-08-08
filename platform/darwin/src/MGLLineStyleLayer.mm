// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLLineStyleLayer.h"

#include <mbgl/style/layers/line_layer.hpp>

@interface MGLLineStyleLayer ()

@property (nonatomic) mbgl::style::LineLayer *layer;
@property (nonatomic, readwrite) NSString *layerIdentifier;
@property (nonatomic, readwrite) NSString *sourceIdentifier;

@end

@implementation MGLLineStyleLayer

@synthesize mapView;

- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier sourceIdentifier:(NSString *)sourceIdentifier {
    if (self = [super init]) {
        _layerIdentifier = layerIdentifier;
        _sourceIdentifier = sourceIdentifier;
        _layer = new mbgl::style::LineLayer(layerIdentifier.UTF8String, sourceIdentifier.UTF8String);
    }
    return self;
}

#pragma mark - Accessing the Layout Attributes

- (void)setLineCap:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineCap {
    MGLSetEnumProperty(lineCap, LineCap, LineCapType, MGLLineStyleLayerLineCap);
}

- (id <MGLStyleAttributeValue>)lineCap {
    MGLGetEnumProperty(LineCap, LineCapType, MGLLineStyleLayerLineCap);
}

- (void)setLineJoin:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineJoin {
    MGLSetEnumProperty(lineJoin, LineJoin, LineJoinType, MGLLineStyleLayerLineJoin);
}

- (id <MGLStyleAttributeValue>)lineJoin {
    MGLGetEnumProperty(LineJoin, LineJoinType, MGLLineStyleLayerLineJoin);
}

- (void)setLineMiterLimit:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineMiterLimit {
    self.layer->setLineMiterLimit(lineMiterLimit.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineMiterLimit {
    return [MGLStyleAttribute mbgl_numberPropertyValueWith:self.layer->getLineMiterLimit()];
}

- (void)setLineRoundLimit:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineRoundLimit {
    self.layer->setLineRoundLimit(lineRoundLimit.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineRoundLimit {
    return [MGLStyleAttribute mbgl_numberPropertyValueWith:self.layer->getLineRoundLimit()];
}

#pragma mark - Accessing the Paint Attributes

- (void)setLineOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineOpacity {
    self.layer->setLineOpacity(lineOpacity.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineOpacity {
    return [MGLStyleAttribute mbgl_numberPropertyValueWith:self.layer->getLineOpacity()];
}

- (void)setLineColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineColor {
    self.layer->setLineColor(lineColor.mbgl_colorPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineColor {
    return [MGLStyleAttribute mbgl_colorPropertyValueWith:self.layer->getLineColor()];
}

- (void)setLineTranslate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineTranslate {
    self.layer->setLineTranslate(lineTranslate.mbgl_offsetPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineTranslate {
    return [MGLStyleAttribute mbgl_offsetPropertyValueWith:self.layer->getLineTranslate()];
}

- (void)setLineTranslateAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineTranslateAnchor {
    MGLSetEnumProperty(lineTranslateAnchor, LineTranslateAnchor, TranslateAnchorType, MGLLineStyleLayerLineTranslateAnchor);
}

- (id <MGLStyleAttributeValue>)lineTranslateAnchor {
    MGLGetEnumProperty(LineTranslateAnchor, TranslateAnchorType, MGLLineStyleLayerLineTranslateAnchor);
}

- (void)setLineWidth:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineWidth {
    self.layer->setLineWidth(lineWidth.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineWidth {
    return [MGLStyleAttribute mbgl_numberPropertyValueWith:self.layer->getLineWidth()];
}

- (void)setLineGapWidth:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineGapWidth {
    self.layer->setLineGapWidth(lineGapWidth.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineGapWidth {
    return [MGLStyleAttribute mbgl_numberPropertyValueWith:self.layer->getLineGapWidth()];
}

- (void)setLineOffset:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineOffset {
    self.layer->setLineOffset(lineOffset.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineOffset {
    return [MGLStyleAttribute mbgl_numberPropertyValueWith:self.layer->getLineOffset()];
}

- (void)setLineBlur:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineBlur {
    self.layer->setLineBlur(lineBlur.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineBlur {
    return [MGLStyleAttribute mbgl_numberPropertyValueWith:self.layer->getLineBlur()];
}

- (void)setLineDasharray:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineDasharray {
    self.layer->setLineDasharray(lineDasharray.mbgl_numberArrayPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineDasharray {
    return [MGLStyleAttribute mbgl_numberArrayPropertyValueWith:self.layer->getLineDasharray()];
}

- (void)setLinePattern:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)linePattern {
    self.layer->setLinePattern(linePattern.mbgl_stringPropertyValue);
}

- (id <MGLStyleAttributeValue>)linePattern {
    return [MGLStyleAttribute mbgl_stringPropertyValueWith:self.layer->getLinePattern()];
}

@end
