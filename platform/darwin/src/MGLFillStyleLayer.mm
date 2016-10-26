// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
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

- (void)setFillAntialias:(MGLStyleValue<NSNumber *> *)fillAntialias {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(fillAntialias);
    self.layer->setFillAntialias(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)fillAntialias {
    auto propertyValue = self.layer->getFillAntialias() ?: self.layer->getDefaultFillAntialias();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setFillOpacity:(MGLStyleValue<NSNumber *> *)fillOpacity {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(fillOpacity);
    self.layer->setFillOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)fillOpacity {
    auto propertyValue = self.layer->getFillOpacity() ?: self.layer->getDefaultFillOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setFillColor:(MGLStyleValue<MGLColor *> *)fillColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(fillColor);
    self.layer->setFillColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)fillColor {
    auto propertyValue = self.layer->getFillColor() ?: self.layer->getDefaultFillColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setFillOutlineColor:(MGLStyleValue<MGLColor *> *)fillOutlineColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(fillOutlineColor);
    self.layer->setFillOutlineColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)fillOutlineColor {
    auto propertyValue = self.layer->getFillOutlineColor() ?: self.layer->getDefaultFillOutlineColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setFillTranslate:(MGLStyleValue<NSValue *> *)fillTranslate {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(fillTranslate);
    self.layer->setFillTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)fillTranslate {
    auto propertyValue = self.layer->getFillTranslate() ?: self.layer->getDefaultFillTranslate();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setFillTranslateAnchor:(MGLStyleValue<NSValue *> *)fillTranslateAnchor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toPropertyValue(fillTranslateAnchor);
    self.layer->setFillTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)fillTranslateAnchor {
    auto propertyValue = self.layer->getFillTranslateAnchor() ?: self.layer->getDefaultFillTranslateAnchor();
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setFillPattern:(MGLStyleValue<NSString *> *)fillPattern {
    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(fillPattern);
    self.layer->setFillPattern(mbglValue);
}

- (MGLStyleValue<NSString *> *)fillPattern {
    auto propertyValue = self.layer->getFillPattern() ?: self.layer->getDefaultFillPattern();
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

@end
