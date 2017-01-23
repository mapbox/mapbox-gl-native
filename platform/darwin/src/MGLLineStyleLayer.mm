// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "MGLMapView_Private.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLLineStyleLayer.h"

#include <mbgl/map/map.hpp>
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

    MBGL_DEFINE_ENUM(MGLLineTranslationAnchor, {
        { MGLLineTranslationAnchorMap, "map" },
        { MGLLineTranslationAnchorViewport, "viewport" },
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
        self.rawLayer = _pendingLayer.get();
    }
    return self;
}

- (mbgl::style::LineLayer *)rawLayer
{
    return (mbgl::style::LineLayer *)super.rawLayer;
}

- (void)setRawLayer:(mbgl::style::LineLayer *)rawLayer
{
    super.rawLayer = rawLayer;
}

- (NSString *)sourceIdentifier
{
    MGLAssertStyleLayerIsValid();
    
    return @(self.rawLayer->getSourceID().c_str());
}

- (NSString *)sourceLayerIdentifier
{
    MGLAssertStyleLayerIsValid();

    auto layerID = self.rawLayer->getSourceLayer();
    return layerID.empty() ? nil : @(layerID.c_str());
}

- (void)setSourceLayerIdentifier:(NSString *)sourceLayerIdentifier
{
    MGLAssertStyleLayerIsValid();

    self.rawLayer->setSourceLayer(sourceLayerIdentifier.UTF8String ?: "");
}

- (void)setPredicate:(NSPredicate *)predicate
{
    MGLAssertStyleLayerIsValid();

    self.rawLayer->setFilter(predicate ? predicate.mgl_filter : mbgl::style::NullFilter());
}

- (NSPredicate *)predicate
{
    MGLAssertStyleLayerIsValid();

    return [NSPredicate mgl_predicateWithFilter:self.rawLayer->getFilter()];
}

#pragma mark - Adding to and removing from a map view

- (void)addToMapView:(MGLMapView *)mapView belowLayer:(MGLStyleLayer *)otherLayer
{
    if (_pendingLayer == nullptr) {
        [NSException raise:@"MGLRedundantLayerException"
            format:@"This instance %@ was already added to %@. Adding the same layer instance " \
                    "to the style more than once is invalid.", self, mapView.style];
    }

    if (otherLayer) {
        const mbgl::optional<std::string> belowLayerId{otherLayer.identifier.UTF8String};
        mapView.mbglMap->addLayer(std::move(_pendingLayer), belowLayerId);
    } else {
        mapView.mbglMap->addLayer(std::move(_pendingLayer));
    }
}

- (void)removeFromMapView:(MGLMapView *)mapView
{
    _pendingLayer = nullptr;
    self.rawLayer = nullptr;

    auto removedLayer = mapView.mbglMap->removeLayer(self.identifier.UTF8String);
    if (!removedLayer) {
        return;
    }

    mbgl::style::LineLayer *layer = dynamic_cast<mbgl::style::LineLayer *>(removedLayer.get());
    if (!layer) {
        return;
    }

    removedLayer.release();

    _pendingLayer = std::unique_ptr<mbgl::style::LineLayer>(layer);
    self.rawLayer = _pendingLayer.get();
}

#pragma mark - Accessing the Layout Attributes

- (void)setLineCap:(MGLStyleValue<NSValue *> *)lineCap {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::LineCapType, NSValue *, mbgl::style::LineCapType, MGLLineCap>().toEnumPropertyValue(lineCap);
    self.rawLayer->setLineCap(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineCap {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineCap() ?: self.rawLayer->getDefaultLineCap();
    return MGLStyleValueTransformer<mbgl::style::LineCapType, NSValue *, mbgl::style::LineCapType, MGLLineCap>().toEnumStyleValue(propertyValue);
}

- (void)setLineJoin:(MGLStyleValue<NSValue *> *)lineJoin {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::LineJoinType, NSValue *, mbgl::style::LineJoinType, MGLLineJoin>().toEnumPropertyValue(lineJoin);
    self.rawLayer->setLineJoin(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineJoin {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineJoin() ?: self.rawLayer->getDefaultLineJoin();
    return MGLStyleValueTransformer<mbgl::style::LineJoinType, NSValue *, mbgl::style::LineJoinType, MGLLineJoin>().toEnumStyleValue(propertyValue);
}

- (void)setLineMiterLimit:(MGLStyleValue<NSNumber *> *)lineMiterLimit {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineMiterLimit);
    self.rawLayer->setLineMiterLimit(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineMiterLimit {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineMiterLimit() ?: self.rawLayer->getDefaultLineMiterLimit();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineRoundLimit:(MGLStyleValue<NSNumber *> *)lineRoundLimit {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineRoundLimit);
    self.rawLayer->setLineRoundLimit(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineRoundLimit {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineRoundLimit() ?: self.rawLayer->getDefaultLineRoundLimit();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

#pragma mark - Accessing the Paint Attributes

- (void)setLineBlur:(MGLStyleValue<NSNumber *> *)lineBlur {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineBlur);
    self.rawLayer->setLineBlur(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineBlur {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineBlur() ?: self.rawLayer->getDefaultLineBlur();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineColor:(MGLStyleValue<MGLColor *> *)lineColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(lineColor);
    self.rawLayer->setLineColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)lineColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineColor() ?: self.rawLayer->getDefaultLineColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setLineDashPattern:(MGLStyleValue<NSArray<NSNumber *> *> *)lineDashPattern {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toPropertyValue(lineDashPattern);
    self.rawLayer->setLineDasharray(mbglValue);
}

- (MGLStyleValue<NSArray<NSNumber *> *> *)lineDashPattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineDasharray() ?: self.rawLayer->getDefaultLineDasharray();
    return MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toStyleValue(propertyValue);
}

- (void)setLineDasharray:(MGLStyleValue<NSArray<NSNumber *> *> *)lineDasharray {
}

- (MGLStyleValue<NSArray<NSNumber *> *> *)lineDasharray {
    return self.lineDashPattern;
}

- (void)setLineGapWidth:(MGLStyleValue<NSNumber *> *)lineGapWidth {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineGapWidth);
    self.rawLayer->setLineGapWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineGapWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineGapWidth() ?: self.rawLayer->getDefaultLineGapWidth();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineOffset:(MGLStyleValue<NSNumber *> *)lineOffset {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineOffset);
    self.rawLayer->setLineOffset(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineOffset {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineOffset() ?: self.rawLayer->getDefaultLineOffset();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineOpacity:(MGLStyleValue<NSNumber *> *)lineOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineOpacity);
    self.rawLayer->setLineOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineOpacity() ?: self.rawLayer->getDefaultLineOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLinePattern:(MGLStyleValue<NSString *> *)linePattern {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(linePattern);
    self.rawLayer->setLinePattern(mbglValue);
}

- (MGLStyleValue<NSString *> *)linePattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLinePattern() ?: self.rawLayer->getDefaultLinePattern();
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

- (void)setLineTranslation:(MGLStyleValue<NSValue *> *)lineTranslation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(lineTranslation);
    self.rawLayer->setLineTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineTranslation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineTranslate() ?: self.rawLayer->getDefaultLineTranslate();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setLineTranslate:(MGLStyleValue<NSValue *> *)lineTranslate {
}

- (MGLStyleValue<NSValue *> *)lineTranslate {
    return self.lineTranslation;
}

- (void)setLineTranslationAnchor:(MGLStyleValue<NSValue *> *)lineTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLLineTranslationAnchor>().toEnumPropertyValue(lineTranslationAnchor);
    self.rawLayer->setLineTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineTranslateAnchor() ?: self.rawLayer->getDefaultLineTranslateAnchor();
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLLineTranslationAnchor>().toEnumStyleValue(propertyValue);
}

- (void)setLineTranslateAnchor:(MGLStyleValue<NSValue *> *)lineTranslateAnchor {
}

- (MGLStyleValue<NSValue *> *)lineTranslateAnchor {
    return self.lineTranslationAnchor;
}

- (void)setLineWidth:(MGLStyleValue<NSNumber *> *)lineWidth {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(lineWidth);
    self.rawLayer->setLineWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineWidth() ?: self.rawLayer->getDefaultLineWidth();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}


@end

@implementation NSValue (MGLLineStyleLayerAdditions)

+ (NSValue *)valueWithMGLLineCap:(MGLLineCap)lineCap {
    return [NSValue value:&lineCap withObjCType:@encode(MGLLineCap)];
}

- (MGLLineCap)MGLLineCapValue {
    MGLLineCap lineCap;
    [self getValue:&lineCap];
    return lineCap;
}

+ (NSValue *)valueWithMGLLineJoin:(MGLLineJoin)lineJoin {
    return [NSValue value:&lineJoin withObjCType:@encode(MGLLineJoin)];
}

- (MGLLineJoin)MGLLineJoinValue {
    MGLLineJoin lineJoin;
    [self getValue:&lineJoin];
    return lineJoin;
}

+ (NSValue *)valueWithMGLLineTranslationAnchor:(MGLLineTranslationAnchor)lineTranslationAnchor {
    return [NSValue value:&lineTranslationAnchor withObjCType:@encode(MGLLineTranslationAnchor)];
}

- (MGLLineTranslationAnchor)MGLLineTranslationAnchorValue {
    MGLLineTranslationAnchor lineTranslationAnchor;
    [self getValue:&lineTranslationAnchor];
    return lineTranslationAnchor;
}

@end
