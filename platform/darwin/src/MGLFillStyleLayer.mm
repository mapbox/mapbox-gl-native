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
@property (nonatomic, readwrite) NSString *layerIdentifier;
@property (nonatomic, readwrite) NSString *sourceIdentifier;
@property (nonatomic, readwrite) NSString *sourceLayerIdentifier;

@end

@implementation MGLFillStyleLayer

@synthesize mapView;


- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier source:(MGLSource *)source
{
    if (self = [super init]) {
        _layerIdentifier = layerIdentifier;
        _sourceIdentifier = source.sourceIdentifier;
        _layer = new mbgl::style::FillLayer(layerIdentifier.UTF8String, source.sourceIdentifier.UTF8String);
    }
    return self;
}

- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier source:(MGLSource *)source sourceLayer:(NSString *)sourceLayer
{
    if (self = [super init]) {
        _layerIdentifier = layerIdentifier;
        _sourceIdentifier = source.sourceIdentifier;
        _layer = new mbgl::style::FillLayer(layerIdentifier.UTF8String, source.sourceIdentifier.UTF8String);
        _layer->setSourceLayer(sourceLayer.UTF8String);
    }
    return self;
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
    MGLSetEnumProperty(fillTranslateAnchor, FillTranslateAnchor, TranslateAnchorType, MGLFillStyleLayerFillTranslateAnchor);
}

- (id <MGLStyleAttributeValue>)fillTranslateAnchor {
    MGLGetEnumProperty(FillTranslateAnchor, TranslateAnchorType, MGLFillStyleLayerFillTranslateAnchor);
}

- (void)setFillPattern:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillPattern {
    self.layer->setFillPattern(fillPattern.mbgl_stringPropertyValue);
}

- (id <MGLStyleAttributeValue>)fillPattern {
    return [MGLStyleAttribute mbgl_stringWithPropertyValueString:self.layer->getFillPattern() ?: self.layer->getDefaultFillPattern()];
}

@end
