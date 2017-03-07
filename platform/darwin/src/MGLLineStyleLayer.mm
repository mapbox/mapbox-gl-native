// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "MGLMapView_Private.h"
#import "NSPredicate+MGLAdditions.h"
#import "NSDate+MGLAdditions.h"
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
    if (self.rawLayer != mapView.mbglMap->getLayer(self.identifier.UTF8String)) {
        return;
    }

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

    auto propertyValue = self.rawLayer->getLineCap();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::LineCapType, NSValue *, mbgl::style::LineCapType, MGLLineCap>().toEnumStyleValue(self.rawLayer->getDefaultLineCap());
    }
    return MGLStyleValueTransformer<mbgl::style::LineCapType, NSValue *, mbgl::style::LineCapType, MGLLineCap>().toEnumStyleValue(propertyValue);
}

- (void)setLineJoin:(MGLStyleValue<NSValue *> *)lineJoin {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::LineJoinType, NSValue *, mbgl::style::LineJoinType, MGLLineJoin>().toEnumPropertyValue(lineJoin);
    self.rawLayer->setLineJoin(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineJoin {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineJoin();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::LineJoinType, NSValue *, mbgl::style::LineJoinType, MGLLineJoin>().toEnumStyleValue(self.rawLayer->getDefaultLineJoin());
    }
    return MGLStyleValueTransformer<mbgl::style::LineJoinType, NSValue *, mbgl::style::LineJoinType, MGLLineJoin>().toEnumStyleValue(propertyValue);
}

- (void)setLineMiterLimit:(MGLStyleValue<NSNumber *> *)lineMiterLimit {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(lineMiterLimit);
    self.rawLayer->setLineMiterLimit(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineMiterLimit {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineMiterLimit();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultLineMiterLimit());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineRoundLimit:(MGLStyleValue<NSNumber *> *)lineRoundLimit {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(lineRoundLimit);
    self.rawLayer->setLineRoundLimit(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineRoundLimit {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineRoundLimit();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultLineRoundLimit());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

#pragma mark - Accessing the Paint Attributes

+ (NSArray *)transitionKeys
{
    NSMutableArray *transitionKeys = [NSMutableArray array];
    [transitionKeys addObject:@"lineBlur"];
    [transitionKeys addObject:@"lineColor"];
    [transitionKeys addObject:@"lineDashPattern"];
    [transitionKeys addObject:@"lineDasharray"];
    [transitionKeys addObject:@"lineGapWidth"];
    [transitionKeys addObject:@"lineOffset"];
    [transitionKeys addObject:@"lineOpacity"];
    [transitionKeys addObject:@"linePattern"];
    [transitionKeys addObject:@"lineTranslation"];
    [transitionKeys addObject:@"lineTranslate"];
    [transitionKeys addObject:@"lineTranslationAnchor"];
    [transitionKeys addObject:@"lineTranslateAnchor"];
    [transitionKeys addObject:@"lineWidth"];

    return transitionKeys;
}

- (void)setLineBlur:(MGLStyleValue<NSNumber *> *)lineBlur {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(lineBlur);
    self.rawLayer->setLineBlur(mbglValue);
}

- (void)mbx_setLineBlurTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLineBlurTransition(options);
}

- (MGLStyleValue<NSNumber *> *)lineBlur {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineBlur();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultLineBlur());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (NSValue *)mbx_getLineBlurTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineBlurTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    NSValue *transitionValue = [NSValue value:&transition withObjCType:@encode(MGLTransition)];
    return transitionValue;
}

- (void)setLineColor:(MGLStyleValue<MGLColor *> *)lineColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenPropertyValue(lineColor);
    self.rawLayer->setLineColor(mbglValue);
}

- (void)mbx_setLineColorTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLineColorTransition(options);
}

- (MGLStyleValue<MGLColor *> *)lineColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(self.rawLayer->getDefaultLineColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(propertyValue);
}

- (NSValue *)mbx_getLineColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineColorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    NSValue *transitionValue = [NSValue value:&transition withObjCType:@encode(MGLTransition)];
    return transitionValue;
}

- (void)setLineDashPattern:(MGLStyleValue<NSArray<NSNumber *> *> *)lineDashPattern {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toPropertyValue(lineDashPattern);
    self.rawLayer->setLineDasharray(mbglValue);
}

- (void)mbx_setLineDashPatternTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLineDasharrayTransition(options);
}

- (MGLStyleValue<NSArray<NSNumber *> *> *)lineDashPattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineDasharray();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toStyleValue(self.rawLayer->getDefaultLineDasharray());
    }
    return MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toStyleValue(propertyValue);
}

- (NSValue *)mbx_getLineDashPatternTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineDasharrayTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    NSValue *transitionValue = [NSValue value:&transition withObjCType:@encode(MGLTransition)];
    return transitionValue;
}

- (void)setLineDasharray:(MGLStyleValue<NSArray<NSNumber *> *> *)lineDasharray {
}

- (MGLStyleValue<NSArray<NSNumber *> *> *)lineDasharray {
    return self.lineDashPattern;
}

- (NSValue *)mbx_getLineDasharray {
    return [self mbx_getLineDashPatternTransition];
}

- (void)setLineGapWidth:(MGLStyleValue<NSNumber *> *)lineGapWidth {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(lineGapWidth);
    self.rawLayer->setLineGapWidth(mbglValue);
}

- (void)mbx_setLineGapWidthTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLineGapWidthTransition(options);
}

- (MGLStyleValue<NSNumber *> *)lineGapWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineGapWidth();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultLineGapWidth());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (NSValue *)mbx_getLineGapWidthTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineGapWidthTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    NSValue *transitionValue = [NSValue value:&transition withObjCType:@encode(MGLTransition)];
    return transitionValue;
}

- (void)setLineOffset:(MGLStyleValue<NSNumber *> *)lineOffset {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(lineOffset);
    self.rawLayer->setLineOffset(mbglValue);
}

- (void)mbx_setLineOffsetTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLineOffsetTransition(options);
}

- (MGLStyleValue<NSNumber *> *)lineOffset {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineOffset();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultLineOffset());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (NSValue *)mbx_getLineOffsetTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineOffsetTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    NSValue *transitionValue = [NSValue value:&transition withObjCType:@encode(MGLTransition)];
    return transitionValue;
}

- (void)setLineOpacity:(MGLStyleValue<NSNumber *> *)lineOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(lineOpacity);
    self.rawLayer->setLineOpacity(mbglValue);
}

- (void)mbx_setLineOpacityTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLineOpacityTransition(options);
}

- (MGLStyleValue<NSNumber *> *)lineOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineOpacity();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultLineOpacity());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (NSValue *)mbx_getLineOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineOpacityTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    NSValue *transitionValue = [NSValue value:&transition withObjCType:@encode(MGLTransition)];
    return transitionValue;
}

- (void)setLinePattern:(MGLStyleValue<NSString *> *)linePattern {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(linePattern);
    self.rawLayer->setLinePattern(mbglValue);
}

- (void)mbx_setLinePatternTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLinePatternTransition(options);
}

- (MGLStyleValue<NSString *> *)linePattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLinePattern();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(self.rawLayer->getDefaultLinePattern());
    }
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

- (NSValue *)mbx_getLinePatternTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLinePatternTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    NSValue *transitionValue = [NSValue value:&transition withObjCType:@encode(MGLTransition)];
    return transitionValue;
}

- (void)setLineTranslation:(MGLStyleValue<NSValue *> *)lineTranslation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toInterpolatablePropertyValue(lineTranslation);
    self.rawLayer->setLineTranslate(mbglValue);
}

- (void)mbx_setLineTranslationTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLineTranslateTransition(options);
}

- (MGLStyleValue<NSValue *> *)lineTranslation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineTranslate();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(self.rawLayer->getDefaultLineTranslate());
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (NSValue *)mbx_getLineTranslationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineTranslateTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    NSValue *transitionValue = [NSValue value:&transition withObjCType:@encode(MGLTransition)];
    return transitionValue;
}

- (void)setLineTranslate:(MGLStyleValue<NSValue *> *)lineTranslate {
}

- (MGLStyleValue<NSValue *> *)lineTranslate {
    return self.lineTranslation;
}

- (NSValue *)mbx_getLineTranslate {
    return [self mbx_getLineTranslationTransition];
}

- (void)setLineTranslationAnchor:(MGLStyleValue<NSValue *> *)lineTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLLineTranslationAnchor>().toEnumPropertyValue(lineTranslationAnchor);
    self.rawLayer->setLineTranslateAnchor(mbglValue);
}

- (void)mbx_setLineTranslationAnchorTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLineTranslateAnchorTransition(options);
}

- (MGLStyleValue<NSValue *> *)lineTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineTranslateAnchor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLLineTranslationAnchor>().toEnumStyleValue(self.rawLayer->getDefaultLineTranslateAnchor());
    }
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLLineTranslationAnchor>().toEnumStyleValue(propertyValue);
}

- (NSValue *)mbx_getLineTranslationAnchorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineTranslateAnchorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    NSValue *transitionValue = [NSValue value:&transition withObjCType:@encode(MGLTransition)];
    return transitionValue;
}

- (void)setLineTranslateAnchor:(MGLStyleValue<NSValue *> *)lineTranslateAnchor {
}

- (MGLStyleValue<NSValue *> *)lineTranslateAnchor {
    return self.lineTranslationAnchor;
}

- (NSValue *)mbx_getLineTranslateAnchor {
    return [self mbx_getLineTranslationAnchorTransition];
}

- (void)setLineWidth:(MGLStyleValue<NSNumber *> *)lineWidth {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(lineWidth);
    self.rawLayer->setLineWidth(mbglValue);
}

- (void)mbx_setLineWidthTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLineWidthTransition(options);
}

- (MGLStyleValue<NSNumber *> *)lineWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineWidth();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultLineWidth());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (NSValue *)mbx_getLineWidthTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineWidthTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    NSValue *transitionValue = [NSValue value:&transition withObjCType:@encode(MGLTransition)];
    return transitionValue;
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
