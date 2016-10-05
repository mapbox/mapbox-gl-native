// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleAttributeValue.h"
#import "MGLFillStyleLayer.h"

#include <mbgl/style/layers/fill_layer.hpp>

@interface MGLFillStyleLayer ()

@property (nonatomic) mbgl::style::FillLayer *layer;

@end

@implementation MGLFillStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    if (self = [super initWithIdentifier:identifier source:source]) {
        _layer = new mbgl::style::FillLayer(identifier.UTF8String, source.identifier.UTF8String);
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

#pragma mark - Accessing the Paint Attributes

- (void)setFillAntialias:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillAntialias {
    self.layer->setFillAntialias(fillAntialias.mbgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)fillAntialias {
    return [MGLStyleAttribute mbgl_boolWithPropertyValueBool:self.layer->getFillAntialias() ?: self.layer->getDefaultFillAntialias()];
}

- (void)setFillOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillOpacity {
    self.layer->setFillOpacity(fillOpacity.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)fillOpacity {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getFillOpacity() ?: self.layer->getDefaultFillOpacity()];
}

- (void)setFillColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillColor {
    self.layer->setFillColor(fillColor.mbgl_colorPropertyValue);
}

- (id <MGLStyleAttributeValue>)fillColor {
    return [MGLStyleAttribute mbgl_colorWithPropertyValueColor:self.layer->getFillColor() ?: self.layer->getDefaultFillColor()];
}

- (void)setFillOutlineColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillOutlineColor {
    self.layer->setFillOutlineColor(fillOutlineColor.mbgl_colorPropertyValue);
}

- (id <MGLStyleAttributeValue>)fillOutlineColor {
    return [MGLStyleAttribute mbgl_colorWithPropertyValueColor:self.layer->getFillOutlineColor() ?: self.layer->getDefaultFillOutlineColor()];
}

- (void)setFillTranslate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillTranslate {
    self.layer->setFillTranslate(fillTranslate.mbgl_offsetPropertyValue);
}

- (id <MGLStyleAttributeValue>)fillTranslate {
    return [MGLStyleAttribute mbgl_offsetWithPropertyValueOffset:self.layer->getFillTranslate() ?: self.layer->getDefaultFillTranslate()];
}

- (void)setFillTranslateAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillTranslateAnchor {
    MGLSetEnumProperty(fillTranslateAnchor, FillTranslateAnchor, TranslateAnchorType, MGLFillTranslateAnchor);
}

- (id <MGLStyleAttributeValue>)fillTranslateAnchor {
    MGLGetEnumProperty(FillTranslateAnchor, TranslateAnchorType, MGLFillTranslateAnchor);
}

- (void)setFillPattern:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillPattern {
    self.layer->setFillPattern(fillPattern.mbgl_stringPropertyValue);
}

- (id <MGLStyleAttributeValue>)fillPattern {
    return [MGLStyleAttribute mbgl_stringWithPropertyValueString:self.layer->getFillPattern() ?: self.layer->getDefaultFillPattern()];
}

@end
