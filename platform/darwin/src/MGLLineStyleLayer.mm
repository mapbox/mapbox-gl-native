// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLLineStyleLayer.h"

#include <mbgl/style/transition_options.hpp>
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

@property (nonatomic, readonly) mbgl::style::LineLayer *rawLayer;

@end

@implementation MGLLineStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    auto layer = std::make_unique<mbgl::style::LineLayer>(identifier.UTF8String, source.identifier.UTF8String);
    return self = [super initWithPendingLayer:std::move(layer)];
}

- (mbgl::style::LineLayer *)rawLayer
{
    return (mbgl::style::LineLayer *)super.rawLayer;
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

- (void)setLineBlur:(MGLStyleValue<NSNumber *> *)lineBlur {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(lineBlur);
    self.rawLayer->setLineBlur(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineBlur {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineBlur();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultLineBlur());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setLineBlurTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLineBlurTransition(options);
}

- (MGLTransition)lineBlurTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineBlurTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setLineColor:(MGLStyleValue<MGLColor *> *)lineColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenPropertyValue(lineColor);
    self.rawLayer->setLineColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)lineColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(self.rawLayer->getDefaultLineColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setLineColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLineColorTransition(options);
}

- (MGLTransition)lineColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineColorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setLineDashPattern:(MGLStyleValue<NSArray<NSNumber *> *> *)lineDashPattern {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toPropertyValue(lineDashPattern);
    self.rawLayer->setLineDasharray(mbglValue);
}

- (MGLStyleValue<NSArray<NSNumber *> *> *)lineDashPattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineDasharray();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toStyleValue(self.rawLayer->getDefaultLineDasharray());
    }
    return MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toStyleValue(propertyValue);
}

- (void)setLineDashPatternTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLineDasharrayTransition(options);
}

- (MGLTransition)lineDashPatternTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineDasharrayTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setLineDasharray:(MGLStyleValue<NSArray<NSNumber *> *> *)lineDasharray {
}

- (MGLStyleValue<NSArray<NSNumber *> *> *)lineDasharray {
    return self.lineDashPattern;
}

- (void)setLineGapWidth:(MGLStyleValue<NSNumber *> *)lineGapWidth {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(lineGapWidth);
    self.rawLayer->setLineGapWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineGapWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineGapWidth();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultLineGapWidth());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setLineGapWidthTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLineGapWidthTransition(options);
}

- (MGLTransition)lineGapWidthTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineGapWidthTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setLineOffset:(MGLStyleValue<NSNumber *> *)lineOffset {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(lineOffset);
    self.rawLayer->setLineOffset(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineOffset {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineOffset();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultLineOffset());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setLineOffsetTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLineOffsetTransition(options);
}

- (MGLTransition)lineOffsetTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineOffsetTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setLineOpacity:(MGLStyleValue<NSNumber *> *)lineOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(lineOpacity);
    self.rawLayer->setLineOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineOpacity();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultLineOpacity());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setLineOpacityTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLineOpacityTransition(options);
}

- (MGLTransition)lineOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineOpacityTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setLinePattern:(MGLStyleValue<NSString *> *)linePattern {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(linePattern);
    self.rawLayer->setLinePattern(mbglValue);
}

- (MGLStyleValue<NSString *> *)linePattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLinePattern();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(self.rawLayer->getDefaultLinePattern());
    }
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

- (void)setLinePatternTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLinePatternTransition(options);
}

- (MGLTransition)linePatternTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLinePatternTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setLineTranslation:(MGLStyleValue<NSValue *> *)lineTranslation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toInterpolatablePropertyValue(lineTranslation);
    self.rawLayer->setLineTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)lineTranslation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineTranslate();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(self.rawLayer->getDefaultLineTranslate());
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setLineTranslationTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLineTranslateTransition(options);
}

- (MGLTransition)lineTranslationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineTranslateTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
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

    auto propertyValue = self.rawLayer->getLineTranslateAnchor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLLineTranslationAnchor>().toEnumStyleValue(self.rawLayer->getDefaultLineTranslateAnchor());
    }
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLLineTranslationAnchor>().toEnumStyleValue(propertyValue);
}

- (void)setLineTranslateAnchor:(MGLStyleValue<NSValue *> *)lineTranslateAnchor {
}

- (MGLStyleValue<NSValue *> *)lineTranslateAnchor {
    return self.lineTranslationAnchor;
}

- (void)setLineWidth:(MGLStyleValue<NSNumber *> *)lineWidth {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(lineWidth);
    self.rawLayer->setLineWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)lineWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineWidth();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultLineWidth());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setLineWidthTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setLineWidthTransition(options);
}

- (MGLTransition)lineWidthTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineWidthTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
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
