// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "MGLMapView_Private.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLLineStyleLayer.h"

#include <mbgl/style/layers/line_layer.hpp>

@interface MGLLineStyleLayer ()

@property (nonatomic) mbgl::style::LineLayer *rawLayer;

@end

@implementation MGLLineStyleLayer
{
    std::unique_ptr<mbgl::style::LineLayer> _pendingLayer;
}

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    if (self = [super initWithIdentifier:identifier source:source]) {
        auto layer = std::make_unique<mbgl::style::LineLayer>(identifier.UTF8String, source.identifier.UTF8String);
        _pendingLayer = std::move(layer);
        self.rawLayer = _pendingLayer.get();
    }
    return self;
}


- (NSString *)sourceLayerIdentifier
{
    auto layerID = self.rawLayer->getSourceLayer();
    return layerID.empty() ? nil : @(layerID.c_str());
}

- (void)setSourceLayerIdentifier:(NSString *)sourceLayerIdentifier
{
    self.rawLayer->setSourceLayer(sourceLayerIdentifier.UTF8String ?: "");
}

- (void)setPredicate:(NSPredicate *)predicate
{
    self.rawLayer->setFilter(predicate.mgl_filter);
}

- (NSPredicate *)predicate
{
    return [NSPredicate mgl_predicateWithFilter:self.rawLayer->getFilter()];
}

#pragma mark - Accessing the Layout Attributes

- (void)setLineCap:(MGLStyleValue<NSValue *> *)lineCap {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::LineCapType, NSValue *>().toPropertyValue(lineCap);
    self.rawLayer->setLineCap(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineCap {
    auto propertyValue = self.rawLayer->getLineCap() ?: self.rawLayer->getDefaultLineCap();
    return MGLStyleValueTransformer<mbgl::style::LineCapType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setLineJoin:(MGLStyleValue<NSValue *> *)lineJoin {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::LineJoinType, NSValue *>().toPropertyValue(lineJoin);
    self.rawLayer->setLineJoin(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineJoin {
    auto propertyValue = self.rawLayer->getLineJoin() ?: self.rawLayer->getDefaultLineJoin();
    return MGLStyleValueTransformer<mbgl::style::LineJoinType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setLineMiterLimit:(MGLStyleValue<NSNumber *> *)lineMiterLimit {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineMiterLimit);
    self.rawLayer->setLineMiterLimit(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineMiterLimit {
    auto propertyValue = self.rawLayer->getLineMiterLimit() ?: self.rawLayer->getDefaultLineMiterLimit();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineRoundLimit:(MGLStyleValue<NSNumber *> *)lineRoundLimit {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineRoundLimit);
    self.rawLayer->setLineRoundLimit(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineRoundLimit {
    auto propertyValue = self.rawLayer->getLineRoundLimit() ?: self.rawLayer->getDefaultLineRoundLimit();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

#pragma mark - Accessing the Paint Attributes

- (void)setLineOpacity:(MGLStyleValue<NSNumber *> *)lineOpacity {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineOpacity);
    self.rawLayer->setLineOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineOpacity {
    auto propertyValue = self.rawLayer->getLineOpacity() ?: self.rawLayer->getDefaultLineOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineColor:(MGLStyleValue<MGLColor *> *)lineColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(lineColor);
    self.rawLayer->setLineColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)lineColor {
    auto propertyValue = self.rawLayer->getLineColor() ?: self.rawLayer->getDefaultLineColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setLineTranslate:(MGLStyleValue<NSValue *> *)lineTranslate {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(lineTranslate);
    self.rawLayer->setLineTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineTranslate {
    auto propertyValue = self.rawLayer->getLineTranslate() ?: self.rawLayer->getDefaultLineTranslate();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setLineTranslateAnchor:(MGLStyleValue<NSValue *> *)lineTranslateAnchor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toPropertyValue(lineTranslateAnchor);
    self.rawLayer->setLineTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineTranslateAnchor {
    auto propertyValue = self.rawLayer->getLineTranslateAnchor() ?: self.rawLayer->getDefaultLineTranslateAnchor();
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setLineWidth:(MGLStyleValue<NSNumber *> *)lineWidth {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineWidth);
    self.rawLayer->setLineWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineWidth {
    auto propertyValue = self.rawLayer->getLineWidth() ?: self.rawLayer->getDefaultLineWidth();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineGapWidth:(MGLStyleValue<NSNumber *> *)lineGapWidth {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineGapWidth);
    self.rawLayer->setLineGapWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineGapWidth {
    auto propertyValue = self.rawLayer->getLineGapWidth() ?: self.rawLayer->getDefaultLineGapWidth();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineOffset:(MGLStyleValue<NSNumber *> *)lineOffset {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineOffset);
    self.rawLayer->setLineOffset(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineOffset {
    auto propertyValue = self.rawLayer->getLineOffset() ?: self.rawLayer->getDefaultLineOffset();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineBlur:(MGLStyleValue<NSNumber *> *)lineBlur {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineBlur);
    self.rawLayer->setLineBlur(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineBlur {
    auto propertyValue = self.rawLayer->getLineBlur() ?: self.rawLayer->getDefaultLineBlur();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineDasharray:(MGLStyleValue<NSArray<NSNumber *> *> *)lineDasharray {
    auto mbglValue = MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toPropertyValue(lineDasharray);
    self.rawLayer->setLineDasharray(mbglValue);
}

- (MGLStyleValue<NSArray<NSNumber *> *> *)lineDasharray {
    auto propertyValue = self.rawLayer->getLineDasharray() ?: self.rawLayer->getDefaultLineDasharray();
    return MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toStyleValue(propertyValue);
}

- (void)setLinePattern:(MGLStyleValue<NSString *> *)linePattern {
    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(linePattern);
    self.rawLayer->setLinePattern(mbglValue);
}

- (MGLStyleValue<NSString *> *)linePattern {
    auto propertyValue = self.rawLayer->getLinePattern() ?: self.rawLayer->getDefaultLinePattern();
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}


#pragma mark - Add style layer to map

- (void)addToMapView:(MGLMapView *)mapView
{
    [self addToMapView:mapView belowLayer:nil];
}

- (void)addToMapView:(MGLMapView *)mapView belowLayer:(MGLStyleLayer *)otherLayer
{
    if (otherLayer) {
        const mbgl::optional<std::string> belowLayerId{otherLayer.identifier.UTF8String};
        mapView.mbglMap->addLayer(std::move(_pendingLayer), belowLayerId);
    } else {
        mapView.mbglMap->addLayer(std::move(_pendingLayer));
    }
}

@end
