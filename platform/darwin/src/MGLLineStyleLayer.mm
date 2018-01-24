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

- (void)setLineCap:(NSExpression *)lineCap {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::LineCapType, NSValue *, mbgl::style::LineCapType, MGLLineCap>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::LineCapType>>(lineCap);
    self.rawLayer->setLineCap(mbglValue);
}

- (NSExpression *)lineCap {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineCap();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineCap();
    }
    return MGLStyleValueTransformer<mbgl::style::LineCapType, NSValue *, mbgl::style::LineCapType, MGLLineCap>().toExpression(propertyValue);
}

- (void)setLineJoin:(NSExpression *)lineJoin {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::LineJoinType, NSValue *, mbgl::style::LineJoinType, MGLLineJoin>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<mbgl::style::LineJoinType>>(lineJoin);
    self.rawLayer->setLineJoin(mbglValue);
}

- (NSExpression *)lineJoin {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineJoin();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineJoin();
    }
    return MGLStyleValueTransformer<mbgl::style::LineJoinType, NSValue *, mbgl::style::LineJoinType, MGLLineJoin>().toExpression(propertyValue);
}

- (void)setLineMiterLimit:(NSExpression *)lineMiterLimit {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(lineMiterLimit);
    self.rawLayer->setLineMiterLimit(mbglValue);
}

- (NSExpression *)lineMiterLimit {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineMiterLimit();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineMiterLimit();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setLineRoundLimit:(NSExpression *)lineRoundLimit {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(lineRoundLimit);
    self.rawLayer->setLineRoundLimit(mbglValue);
}

- (NSExpression *)lineRoundLimit {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineRoundLimit();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineRoundLimit();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

#pragma mark - Accessing the Paint Attributes

- (void)setLineBlur:(NSExpression *)lineBlur {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(lineBlur);
    self.rawLayer->setLineBlur(mbglValue);
}

- (NSExpression *)lineBlur {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineBlur();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineBlur();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
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

- (void)setLineColor:(NSExpression *)lineColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<mbgl::Color>>(lineColor);
    self.rawLayer->setLineColor(mbglValue);
}

- (NSExpression *)lineColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineColor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineColor();
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(propertyValue);
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

- (void)setLineDashPattern:(NSExpression *)lineDashPattern {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toPropertyValue<mbgl::style::PropertyValue<std::vector<float>>>(lineDashPattern);
    self.rawLayer->setLineDasharray(mbglValue);
}

- (NSExpression *)lineDashPattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineDasharray();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineDasharray();
    }
    return MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toExpression(propertyValue);
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

- (void)setLineDasharray:(NSExpression *)lineDasharray {
}

- (NSExpression *)lineDasharray {
    return self.lineDashPattern;
}

- (void)setLineGapWidth:(NSExpression *)lineGapWidth {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(lineGapWidth);
    self.rawLayer->setLineGapWidth(mbglValue);
}

- (NSExpression *)lineGapWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineGapWidth();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineGapWidth();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
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

- (void)setLineOffset:(NSExpression *)lineOffset {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(lineOffset);
    self.rawLayer->setLineOffset(mbglValue);
}

- (NSExpression *)lineOffset {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineOffset();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineOffset();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
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

- (void)setLineOpacity:(NSExpression *)lineOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(lineOpacity);
    self.rawLayer->setLineOpacity(mbglValue);
}

- (NSExpression *)lineOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineOpacity();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineOpacity();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
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

- (void)setLinePattern:(NSExpression *)linePattern {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue<mbgl::style::PropertyValue<std::string>>(linePattern);
    self.rawLayer->setLinePattern(mbglValue);
}

- (NSExpression *)linePattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLinePattern();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLinePattern();
    }
    return MGLStyleValueTransformer<std::string, NSString *>().toExpression(propertyValue);
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

- (void)setLineTranslation:(NSExpression *)lineTranslation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue<mbgl::style::PropertyValue<std::array<float, 2>>>(lineTranslation);
    self.rawLayer->setLineTranslate(mbglValue);
}

- (NSExpression *)lineTranslation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineTranslate();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineTranslate();
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toExpression(propertyValue);
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

- (void)setLineTranslate:(NSExpression *)lineTranslate {
}

- (NSExpression *)lineTranslate {
    return self.lineTranslation;
}

- (void)setLineTranslationAnchor:(NSExpression *)lineTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLLineTranslationAnchor>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType>>(lineTranslationAnchor);
    self.rawLayer->setLineTranslateAnchor(mbglValue);
}

- (NSExpression *)lineTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineTranslateAnchor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineTranslateAnchor();
    }
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLLineTranslationAnchor>().toExpression(propertyValue);
}

- (void)setLineTranslateAnchor:(NSExpression *)lineTranslateAnchor {
}

- (NSExpression *)lineTranslateAnchor {
    return self.lineTranslationAnchor;
}

- (void)setLineWidth:(NSExpression *)lineWidth {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(lineWidth);
    self.rawLayer->setLineWidth(mbglValue);
}

- (NSExpression *)lineWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineWidth();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineWidth();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
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
