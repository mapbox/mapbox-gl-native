// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "MGLMapView_Private.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLLineStyleLayer.h"

#include <mbgl/style/layers/line_layer.hpp>

namespace mbgl {

    MBGL_DEFINE_ENUM(MGLLineCap, {
        { MGLLineCapButt, "butt" },
        { MGLLineCapRound, "round" },
        { MGLLineCapSquare, "square" },
    });

    MBGL_DEFINE_ENUM(MGLLineJoin, {
        { MGLLineJoinBevel, "bevel" },
        { MGLLineJoinRound, "round" },
        { MGLLineJoinMiter, "miter" },
    });

    MBGL_DEFINE_ENUM(MGLLineTranslateAnchor, {
        { MGLLineTranslateAnchorMap, "map" },
        { MGLLineTranslateAnchorViewport, "viewport" },
    });

}

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
        _rawLayer = _pendingLayer.get();
    }
    return self;
}
- (NSString *)sourceLayerIdentifier
{
    auto layerID = _rawLayer->getSourceLayer();
    return layerID.empty() ? nil : @(layerID.c_str());
}

- (void)setSourceLayerIdentifier:(NSString *)sourceLayerIdentifier
{
    _rawLayer->setSourceLayer(sourceLayerIdentifier.UTF8String ?: "");
}

- (void)setPredicate:(NSPredicate *)predicate
{
    _rawLayer->setFilter(predicate.mgl_filter);
}

- (NSPredicate *)predicate
{
    return [NSPredicate mgl_predicateWithFilter:_rawLayer->getFilter()];
}
#pragma mark -  Adding to and removing from a map view

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

- (void)removeFromMapView:(MGLMapView *)mapView
{
    auto removedLayer = mapView.mbglMap->removeLayer(self.identifier.UTF8String);
    _pendingLayer = std::move(reinterpret_cast<std::unique_ptr<mbgl::style::LineLayer> &>(removedLayer));
    _rawLayer = _pendingLayer.get();
}

#pragma mark - Accessing the Layout Attributes

- (void)setLineCap:(MGLStyleValue<NSValue *> *)lineCap {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::LineCapType,
    NSValue *,
    mbgl::style::LineCapType,
    MGLLineCap>().toEnumPropertyValue(lineCap);
    _rawLayer->setLineCap(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineCap {
    auto propertyValue = _rawLayer->getLineCap() ?: _rawLayer->getDefaultLineCap();
    return MGLStyleValueTransformer<mbgl::style::LineCapType, NSValue *, mbgl::style::LineCapType, MGLLineCap>().toEnumStyleValue(propertyValue);
}

- (void)setLineJoin:(MGLStyleValue<NSValue *> *)lineJoin {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::LineJoinType,
    NSValue *,
    mbgl::style::LineJoinType,
    MGLLineJoin>().toEnumPropertyValue(lineJoin);
    _rawLayer->setLineJoin(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineJoin {
    auto propertyValue = _rawLayer->getLineJoin() ?: _rawLayer->getDefaultLineJoin();
    return MGLStyleValueTransformer<mbgl::style::LineJoinType, NSValue *, mbgl::style::LineJoinType, MGLLineJoin>().toEnumStyleValue(propertyValue);
}

- (void)setLineMiterLimit:(MGLStyleValue<NSNumber *> *)lineMiterLimit {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineMiterLimit);
    _rawLayer->setLineMiterLimit(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineMiterLimit {
    auto propertyValue = _rawLayer->getLineMiterLimit() ?: _rawLayer->getDefaultLineMiterLimit();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineRoundLimit:(MGLStyleValue<NSNumber *> *)lineRoundLimit {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineRoundLimit);
    _rawLayer->setLineRoundLimit(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineRoundLimit {
    auto propertyValue = _rawLayer->getLineRoundLimit() ?: _rawLayer->getDefaultLineRoundLimit();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

#pragma mark - Accessing the Paint Attributes

- (void)setLineOpacity:(MGLStyleValue<NSNumber *> *)lineOpacity {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineOpacity);
    _rawLayer->setLineOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineOpacity {
    auto propertyValue = _rawLayer->getLineOpacity() ?: _rawLayer->getDefaultLineOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineColor:(MGLStyleValue<MGLColor *> *)lineColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(lineColor);
    _rawLayer->setLineColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)lineColor {
    auto propertyValue = _rawLayer->getLineColor() ?: _rawLayer->getDefaultLineColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setLineTranslate:(MGLStyleValue<NSValue *> *)lineTranslate {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(lineTranslate);
    _rawLayer->setLineTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineTranslate {
    auto propertyValue = _rawLayer->getLineTranslate() ?: _rawLayer->getDefaultLineTranslate();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setLineTranslateAnchor:(MGLStyleValue<NSValue *> *)lineTranslateAnchor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType,
    NSValue *,
    mbgl::style::TranslateAnchorType,
    MGLLineTranslateAnchor>().toEnumPropertyValue(lineTranslateAnchor);
    _rawLayer->setLineTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineTranslateAnchor {
    auto propertyValue = _rawLayer->getLineTranslateAnchor() ?: _rawLayer->getDefaultLineTranslateAnchor();
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLLineTranslateAnchor>().toEnumStyleValue(propertyValue);
}

- (void)setLineWidth:(MGLStyleValue<NSNumber *> *)lineWidth {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineWidth);
    _rawLayer->setLineWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineWidth {
    auto propertyValue = _rawLayer->getLineWidth() ?: _rawLayer->getDefaultLineWidth();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineGapWidth:(MGLStyleValue<NSNumber *> *)lineGapWidth {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineGapWidth);
    _rawLayer->setLineGapWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineGapWidth {
    auto propertyValue = _rawLayer->getLineGapWidth() ?: _rawLayer->getDefaultLineGapWidth();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineOffset:(MGLStyleValue<NSNumber *> *)lineOffset {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineOffset);
    _rawLayer->setLineOffset(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineOffset {
    auto propertyValue = _rawLayer->getLineOffset() ?: _rawLayer->getDefaultLineOffset();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineBlur:(MGLStyleValue<NSNumber *> *)lineBlur {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineBlur);
    _rawLayer->setLineBlur(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineBlur {
    auto propertyValue = _rawLayer->getLineBlur() ?: _rawLayer->getDefaultLineBlur();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineDasharray:(MGLStyleValue<NSArray<NSNumber *> *> *)lineDasharray {
    auto mbglValue = MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toPropertyValue(lineDasharray);
    _rawLayer->setLineDasharray(mbglValue);
}

- (MGLStyleValue<NSArray<NSNumber *> *> *)lineDasharray {
    auto propertyValue = _rawLayer->getLineDasharray() ?: _rawLayer->getDefaultLineDasharray();
    return MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toStyleValue(propertyValue);
}

- (void)setLinePattern:(MGLStyleValue<NSString *> *)linePattern {
    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(linePattern);
    _rawLayer->setLinePattern(mbglValue);
}

- (MGLStyleValue<NSString *> *)linePattern {
    auto propertyValue = _rawLayer->getLinePattern() ?: _rawLayer->getDefaultLinePattern();
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}


@end
