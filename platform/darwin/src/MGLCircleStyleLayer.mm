// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLCircleStyleLayer.h"

#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/layers/circle_layer.hpp>

namespace mbgl {

    MBGL_DEFINE_ENUM(MGLCirclePitchAlignment, {
        { MGLCirclePitchAlignmentMap, "map" },
        { MGLCirclePitchAlignmentViewport, "viewport" },
    });

    MBGL_DEFINE_ENUM(MGLCircleScaleAlignment, {
        { MGLCircleScaleAlignmentMap, "map" },
        { MGLCircleScaleAlignmentViewport, "viewport" },
    });

    MBGL_DEFINE_ENUM(MGLCircleTranslationAnchor, {
        { MGLCircleTranslationAnchorMap, "map" },
        { MGLCircleTranslationAnchorViewport, "viewport" },
    });

}

@interface MGLCircleStyleLayer ()

@property (nonatomic, readonly) mbgl::style::CircleLayer *rawLayer;

@end

@implementation MGLCircleStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    auto layer = std::make_unique<mbgl::style::CircleLayer>(identifier.UTF8String, source.identifier.UTF8String);
    return self = [super initWithPendingLayer:std::move(layer)];
}

- (mbgl::style::CircleLayer *)rawLayer
{
    return (mbgl::style::CircleLayer *)super.rawLayer;
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

#pragma mark - Accessing the Paint Attributes

- (void)setCircleBlur:(MGLStyleValue<NSNumber *> *)circleBlur {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(circleBlur);
    self.rawLayer->setCircleBlur(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)circleBlur {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleBlur();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultCircleBlur());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setCircleBlurTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setCircleBlurTransition(options);
}

- (MGLTransition)circleBlurTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getCircleBlurTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setCircleColor:(MGLStyleValue<MGLColor *> *)circleColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenPropertyValue(circleColor);
    self.rawLayer->setCircleColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)circleColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(self.rawLayer->getDefaultCircleColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setCircleColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setCircleColorTransition(options);
}

- (MGLTransition)circleColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getCircleColorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setCircleOpacity:(MGLStyleValue<NSNumber *> *)circleOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(circleOpacity);
    self.rawLayer->setCircleOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)circleOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleOpacity();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultCircleOpacity());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setCircleOpacityTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setCircleOpacityTransition(options);
}

- (MGLTransition)circleOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getCircleOpacityTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setCirclePitchAlignment:(MGLStyleValue<NSValue *> *)circlePitchAlignment {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLCirclePitchAlignment>().toEnumPropertyValue(circlePitchAlignment);
    self.rawLayer->setCirclePitchAlignment(mbglValue);
}

- (MGLStyleValue<NSValue *> *)circlePitchAlignment {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCirclePitchAlignment();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLCirclePitchAlignment>().toEnumStyleValue(self.rawLayer->getDefaultCirclePitchAlignment());
    }
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLCirclePitchAlignment>().toEnumStyleValue(propertyValue);
}

- (void)setCircleRadius:(MGLStyleValue<NSNumber *> *)circleRadius {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(circleRadius);
    self.rawLayer->setCircleRadius(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)circleRadius {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleRadius();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultCircleRadius());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setCircleRadiusTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setCircleRadiusTransition(options);
}

- (MGLTransition)circleRadiusTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getCircleRadiusTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setCircleScaleAlignment:(MGLStyleValue<NSValue *> *)circleScaleAlignment {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::CirclePitchScaleType, NSValue *, mbgl::style::CirclePitchScaleType, MGLCircleScaleAlignment>().toEnumPropertyValue(circleScaleAlignment);
    self.rawLayer->setCirclePitchScale(mbglValue);
}

- (MGLStyleValue<NSValue *> *)circleScaleAlignment {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCirclePitchScale();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::CirclePitchScaleType, NSValue *, mbgl::style::CirclePitchScaleType, MGLCircleScaleAlignment>().toEnumStyleValue(self.rawLayer->getDefaultCirclePitchScale());
    }
    return MGLStyleValueTransformer<mbgl::style::CirclePitchScaleType, NSValue *, mbgl::style::CirclePitchScaleType, MGLCircleScaleAlignment>().toEnumStyleValue(propertyValue);
}

- (void)setCirclePitchScale:(MGLStyleValue<NSValue *> *)circlePitchScale {
}

- (MGLStyleValue<NSValue *> *)circlePitchScale {
    return self.circleScaleAlignment;
}

- (void)setCircleStrokeColor:(MGLStyleValue<MGLColor *> *)circleStrokeColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenPropertyValue(circleStrokeColor);
    self.rawLayer->setCircleStrokeColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)circleStrokeColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleStrokeColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(self.rawLayer->getDefaultCircleStrokeColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setCircleStrokeColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setCircleStrokeColorTransition(options);
}

- (MGLTransition)circleStrokeColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getCircleStrokeColorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setCircleStrokeOpacity:(MGLStyleValue<NSNumber *> *)circleStrokeOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(circleStrokeOpacity);
    self.rawLayer->setCircleStrokeOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)circleStrokeOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleStrokeOpacity();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultCircleStrokeOpacity());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setCircleStrokeOpacityTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setCircleStrokeOpacityTransition(options);
}

- (MGLTransition)circleStrokeOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getCircleStrokeOpacityTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setCircleStrokeWidth:(MGLStyleValue<NSNumber *> *)circleStrokeWidth {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(circleStrokeWidth);
    self.rawLayer->setCircleStrokeWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)circleStrokeWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleStrokeWidth();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultCircleStrokeWidth());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setCircleStrokeWidthTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setCircleStrokeWidthTransition(options);
}

- (MGLTransition)circleStrokeWidthTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getCircleStrokeWidthTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setCircleTranslation:(MGLStyleValue<NSValue *> *)circleTranslation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toInterpolatablePropertyValue(circleTranslation);
    self.rawLayer->setCircleTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)circleTranslation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleTranslate();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(self.rawLayer->getDefaultCircleTranslate());
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setCircleTranslationTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setCircleTranslateTransition(options);
}

- (MGLTransition)circleTranslationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getCircleTranslateTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setCircleTranslate:(MGLStyleValue<NSValue *> *)circleTranslate {
}

- (MGLStyleValue<NSValue *> *)circleTranslate {
    return self.circleTranslation;
}

- (void)setCircleTranslationAnchor:(MGLStyleValue<NSValue *> *)circleTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLCircleTranslationAnchor>().toEnumPropertyValue(circleTranslationAnchor);
    self.rawLayer->setCircleTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)circleTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleTranslateAnchor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLCircleTranslationAnchor>().toEnumStyleValue(self.rawLayer->getDefaultCircleTranslateAnchor());
    }
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLCircleTranslationAnchor>().toEnumStyleValue(propertyValue);
}

- (void)setCircleTranslateAnchor:(MGLStyleValue<NSValue *> *)circleTranslateAnchor {
}

- (MGLStyleValue<NSValue *> *)circleTranslateAnchor {
    return self.circleTranslationAnchor;
}

@end

@implementation NSValue (MGLCircleStyleLayerAdditions)

+ (NSValue *)valueWithMGLCirclePitchAlignment:(MGLCirclePitchAlignment)circlePitchAlignment {
    return [NSValue value:&circlePitchAlignment withObjCType:@encode(MGLCirclePitchAlignment)];
}

- (MGLCirclePitchAlignment)MGLCirclePitchAlignmentValue {
    MGLCirclePitchAlignment circlePitchAlignment;
    [self getValue:&circlePitchAlignment];
    return circlePitchAlignment;
}

+ (NSValue *)valueWithMGLCircleScaleAlignment:(MGLCircleScaleAlignment)circleScaleAlignment {
    return [NSValue value:&circleScaleAlignment withObjCType:@encode(MGLCircleScaleAlignment)];
}

- (MGLCircleScaleAlignment)MGLCircleScaleAlignmentValue {
    MGLCircleScaleAlignment circleScaleAlignment;
    [self getValue:&circleScaleAlignment];
    return circleScaleAlignment;
}

+ (NSValue *)valueWithMGLCircleTranslationAnchor:(MGLCircleTranslationAnchor)circleTranslationAnchor {
    return [NSValue value:&circleTranslationAnchor withObjCType:@encode(MGLCircleTranslationAnchor)];
}

- (MGLCircleTranslationAnchor)MGLCircleTranslationAnchorValue {
    MGLCircleTranslationAnchor circleTranslationAnchor;
    [self getValue:&circleTranslationAnchor];
    return circleTranslationAnchor;
}

@end
