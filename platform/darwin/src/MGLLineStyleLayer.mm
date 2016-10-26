// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
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

- (void)setLineCap:(MGLStyleValue<NSValue *> *)lineCap {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::LineCapType, NSValue *>().toPropertyValue(lineCap);
    self.layer->setLineCap(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineCap {
    auto propertyValue = self.layer->getLineCap() ?: self.layer->getDefaultLineCap();
    return MGLStyleValueTransformer<mbgl::style::LineCapType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setLineJoin:(MGLStyleValue<NSValue *> *)lineJoin {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::LineJoinType, NSValue *>().toPropertyValue(lineJoin);
    self.layer->setLineJoin(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineJoin {
    auto propertyValue = self.layer->getLineJoin() ?: self.layer->getDefaultLineJoin();
    return MGLStyleValueTransformer<mbgl::style::LineJoinType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setLineMiterLimit:(MGLStyleValue<NSNumber *> *)lineMiterLimit {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineMiterLimit);
    self.layer->setLineMiterLimit(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineMiterLimit {
    auto propertyValue = self.layer->getLineMiterLimit() ?: self.layer->getDefaultLineMiterLimit();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineRoundLimit:(MGLStyleValue<NSNumber *> *)lineRoundLimit {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineRoundLimit);
    self.layer->setLineRoundLimit(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineRoundLimit {
    auto propertyValue = self.layer->getLineRoundLimit() ?: self.layer->getDefaultLineRoundLimit();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

#pragma mark - Accessing the Paint Attributes

- (void)setLineOpacity:(MGLStyleValue<NSNumber *> *)lineOpacity {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineOpacity);
    self.layer->setLineOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineOpacity {
    auto propertyValue = self.layer->getLineOpacity() ?: self.layer->getDefaultLineOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineColor:(MGLStyleValue<MGLColor *> *)lineColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(lineColor);
    self.layer->setLineColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)lineColor {
    auto propertyValue = self.layer->getLineColor() ?: self.layer->getDefaultLineColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setLineTranslate:(MGLStyleValue<NSValue *> *)lineTranslate {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(lineTranslate);
    self.layer->setLineTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineTranslate {
    auto propertyValue = self.layer->getLineTranslate() ?: self.layer->getDefaultLineTranslate();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setLineTranslateAnchor:(MGLStyleValue<NSValue *> *)lineTranslateAnchor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toPropertyValue(lineTranslateAnchor);
    self.layer->setLineTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineTranslateAnchor {
    auto propertyValue = self.layer->getLineTranslateAnchor() ?: self.layer->getDefaultLineTranslateAnchor();
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setLineWidth:(MGLStyleValue<NSNumber *> *)lineWidth {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineWidth);
    self.layer->setLineWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineWidth {
    auto propertyValue = self.layer->getLineWidth() ?: self.layer->getDefaultLineWidth();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineGapWidth:(MGLStyleValue<NSNumber *> *)lineGapWidth {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineGapWidth);
    self.layer->setLineGapWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineGapWidth {
    auto propertyValue = self.layer->getLineGapWidth() ?: self.layer->getDefaultLineGapWidth();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineOffset:(MGLStyleValue<NSNumber *> *)lineOffset {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineOffset);
    self.layer->setLineOffset(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineOffset {
    auto propertyValue = self.layer->getLineOffset() ?: self.layer->getDefaultLineOffset();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineBlur:(MGLStyleValue<NSNumber *> *)lineBlur {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineBlur);
    self.layer->setLineBlur(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineBlur {
    auto propertyValue = self.layer->getLineBlur() ?: self.layer->getDefaultLineBlur();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineDasharray:(MGLStyleValue<NSArray<NSNumber *> *> *)lineDasharray {
    auto mbglValue = MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toPropertyValue(lineDasharray);
    self.layer->setLineDasharray(mbglValue);
}

- (MGLStyleValue<NSArray<NSNumber *> *> *)lineDasharray {
    auto propertyValue = self.layer->getLineDasharray() ?: self.layer->getDefaultLineDasharray();
    return MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toStyleValue(propertyValue);
}

- (void)setLinePattern:(MGLStyleValue<NSString *> *)linePattern {
    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(linePattern);
    self.layer->setLinePattern(mbglValue);
}

- (MGLStyleValue<NSString *> *)linePattern {
    auto propertyValue = self.layer->getLinePattern() ?: self.layer->getDefaultLinePattern();
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

@end
