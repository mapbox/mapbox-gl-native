// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleAttributeValue.h"
#import "MGLLineStyleLayer.h"

#include <mbgl/style/layers/line_layer.hpp>

@interface MGLLineStyleLayer ()

@property (nonatomic) mbgl::style::LineLayer *layer;

@end

@implementation MGLLineStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    if (self = [super initWithIdentifier:identifier source:source]) {
        _layer = new mbgl::style::LineLayer(identifier.UTF8String, source.identifier.UTF8String);
    }
    return self;
}

- (NSString *)sourceLayerIdentifier
{
    auto layerID = self.layer->getSourceLayer();
    return layerID.empty() ? nil : @(layerID.c_str());
}

- (void)setSourceLayerIdentifier:(NSString *)sourceLayerIdentifier
{
    self.layer->setSourceLayer(sourceLayerIdentifier.UTF8String ?: "");
}

- (void)setPredicate:(NSPredicate *)predicate
{
    self.layer->setFilter(predicate.mgl_filter);
}

- (NSPredicate *)predicate
{
    return [NSPredicate mgl_predicateWithFilter:self.layer->getFilter()];
}

#pragma mark - Accessing the Layout Attributes

- (void)setLineCap:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineCap {
    MGLSetEnumProperty(lineCap, LineCap, LineCapType, MGLLineCap);
}

- (id <MGLStyleAttributeValue>)lineCap {
    MGLGetEnumProperty(LineCap, LineCapType, MGLLineCap);
}

- (void)setLineJoin:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineJoin {
    MGLSetEnumProperty(lineJoin, LineJoin, LineJoinType, MGLLineJoin);
}

- (id <MGLStyleAttributeValue>)lineJoin {
    MGLGetEnumProperty(LineJoin, LineJoinType, MGLLineJoin);
}

- (void)setLineMiterLimit:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineMiterLimit {
    self.layer->setLineMiterLimit(lineMiterLimit.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineMiterLimit {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getLineMiterLimit() ?: self.layer->getDefaultLineMiterLimit()];
}

- (void)setLineRoundLimit:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineRoundLimit {
    self.layer->setLineRoundLimit(lineRoundLimit.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineRoundLimit {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getLineRoundLimit() ?: self.layer->getDefaultLineRoundLimit()];
}

#pragma mark - Accessing the Paint Attributes

- (void)setLineOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineOpacity {
    self.layer->setLineOpacity(lineOpacity.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineOpacity {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getLineOpacity() ?: self.layer->getDefaultLineOpacity()];
}

- (void)setLineColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineColor {
    self.layer->setLineColor(lineColor.mbgl_colorPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineColor {
    return [MGLStyleAttribute mbgl_colorWithPropertyValueColor:self.layer->getLineColor() ?: self.layer->getDefaultLineColor()];
}

- (void)setLineTranslate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineTranslate {
    self.layer->setLineTranslate(lineTranslate.mbgl_offsetPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineTranslate {
    return [MGLStyleAttribute mbgl_offsetWithPropertyValueOffset:self.layer->getLineTranslate() ?: self.layer->getDefaultLineTranslate()];
}

- (void)setLineTranslateAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineTranslateAnchor {
    MGLSetEnumProperty(lineTranslateAnchor, LineTranslateAnchor, TranslateAnchorType, MGLLineTranslateAnchor);
}

- (id <MGLStyleAttributeValue>)lineTranslateAnchor {
    MGLGetEnumProperty(LineTranslateAnchor, TranslateAnchorType, MGLLineTranslateAnchor);
}

- (void)setLineWidth:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineWidth {
    self.layer->setLineWidth(lineWidth.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineWidth {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getLineWidth() ?: self.layer->getDefaultLineWidth()];
}

- (void)setLineGapWidth:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineGapWidth {
    self.layer->setLineGapWidth(lineGapWidth.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineGapWidth {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getLineGapWidth() ?: self.layer->getDefaultLineGapWidth()];
}

- (void)setLineOffset:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineOffset {
    self.layer->setLineOffset(lineOffset.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineOffset {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getLineOffset() ?: self.layer->getDefaultLineOffset()];
}

- (void)setLineBlur:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineBlur {
    self.layer->setLineBlur(lineBlur.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineBlur {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getLineBlur() ?: self.layer->getDefaultLineBlur()];
}

- (void)setLineDasharray:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineDasharray {
    self.layer->setLineDasharray(lineDasharray.mbgl_numberArrayPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineDasharray {
    return [MGLStyleAttribute mbgl_numberArrayWithPropertyValueNumberArray:self.layer->getLineDasharray() ?: self.layer->getDefaultLineDasharray()];
}

- (void)setLinePattern:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)linePattern {
    self.layer->setLinePattern(linePattern.mbgl_stringPropertyValue);
}

- (id <MGLStyleAttributeValue>)linePattern {
    return [MGLStyleAttribute mbgl_stringWithPropertyValueString:self.layer->getLinePattern() ?: self.layer->getDefaultLinePattern()];
}

@end
