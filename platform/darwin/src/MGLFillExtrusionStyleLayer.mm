// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLFillExtrusionStyleLayer.h"

#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>

namespace mbgl {

    MBGL_DEFINE_ENUM(MGLFillExtrusionTranslationAnchor, {
        { MGLFillExtrusionTranslationAnchorMap, "map" },
        { MGLFillExtrusionTranslationAnchorViewport, "viewport" },
    });

}

@interface MGLFillExtrusionStyleLayer ()

@property (nonatomic, readonly) mbgl::style::FillExtrusionLayer *rawLayer;

@end

@implementation MGLFillExtrusionStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    auto layer = std::make_unique<mbgl::style::FillExtrusionLayer>(identifier.UTF8String, source.identifier.UTF8String);
    return self = [super initWithPendingLayer:std::move(layer)];
}

- (mbgl::style::FillExtrusionLayer *)rawLayer
{
    return (mbgl::style::FillExtrusionLayer *)super.rawLayer;
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

- (void)setFillExtrusionBase:(MGLStyleValue<NSNumber *> *)fillExtrusionBase {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(fillExtrusionBase);
    self.rawLayer->setFillExtrusionBase(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)fillExtrusionBase {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionBase();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultFillExtrusionBase());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setFillExtrusionBaseTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillExtrusionBaseTransition(options);
}

- (MGLTransition)fillExtrusionBaseTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillExtrusionBaseTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setFillExtrusionColor:(MGLStyleValue<MGLColor *> *)fillExtrusionColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenPropertyValue(fillExtrusionColor);
    self.rawLayer->setFillExtrusionColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)fillExtrusionColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(self.rawLayer->getDefaultFillExtrusionColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setFillExtrusionColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillExtrusionColorTransition(options);
}

- (MGLTransition)fillExtrusionColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillExtrusionColorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setFillExtrusionHeight:(MGLStyleValue<NSNumber *> *)fillExtrusionHeight {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(fillExtrusionHeight);
    self.rawLayer->setFillExtrusionHeight(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)fillExtrusionHeight {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionHeight();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultFillExtrusionHeight());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setFillExtrusionHeightTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillExtrusionHeightTransition(options);
}

- (MGLTransition)fillExtrusionHeightTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillExtrusionHeightTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setFillExtrusionOpacity:(MGLStyleValue<NSNumber *> *)fillExtrusionOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(fillExtrusionOpacity);
    self.rawLayer->setFillExtrusionOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)fillExtrusionOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionOpacity();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultFillExtrusionOpacity());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setFillExtrusionOpacityTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillExtrusionOpacityTransition(options);
}

- (MGLTransition)fillExtrusionOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillExtrusionOpacityTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setFillExtrusionPattern:(MGLStyleValue<NSString *> *)fillExtrusionPattern {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(fillExtrusionPattern);
    self.rawLayer->setFillExtrusionPattern(mbglValue);
}

- (MGLStyleValue<NSString *> *)fillExtrusionPattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionPattern();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(self.rawLayer->getDefaultFillExtrusionPattern());
    }
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

- (void)setFillExtrusionPatternTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillExtrusionPatternTransition(options);
}

- (MGLTransition)fillExtrusionPatternTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillExtrusionPatternTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setFillExtrusionTranslation:(MGLStyleValue<NSValue *> *)fillExtrusionTranslation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toInterpolatablePropertyValue(fillExtrusionTranslation);
    self.rawLayer->setFillExtrusionTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)fillExtrusionTranslation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionTranslate();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(self.rawLayer->getDefaultFillExtrusionTranslate());
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setFillExtrusionTranslationTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillExtrusionTranslateTransition(options);
}

- (MGLTransition)fillExtrusionTranslationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillExtrusionTranslateTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setFillExtrusionTranslate:(MGLStyleValue<NSValue *> *)fillExtrusionTranslate {
}

- (MGLStyleValue<NSValue *> *)fillExtrusionTranslate {
    return self.fillExtrusionTranslation;
}

- (void)setFillExtrusionTranslationAnchor:(MGLStyleValue<NSValue *> *)fillExtrusionTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillExtrusionTranslationAnchor>().toEnumPropertyValue(fillExtrusionTranslationAnchor);
    self.rawLayer->setFillExtrusionTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)fillExtrusionTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionTranslateAnchor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillExtrusionTranslationAnchor>().toEnumStyleValue(self.rawLayer->getDefaultFillExtrusionTranslateAnchor());
    }
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillExtrusionTranslationAnchor>().toEnumStyleValue(propertyValue);
}

- (void)setFillExtrusionTranslateAnchor:(MGLStyleValue<NSValue *> *)fillExtrusionTranslateAnchor {
}

- (MGLStyleValue<NSValue *> *)fillExtrusionTranslateAnchor {
    return self.fillExtrusionTranslationAnchor;
}

@end

@implementation NSValue (MGLFillExtrusionStyleLayerAdditions)

+ (NSValue *)valueWithMGLFillExtrusionTranslationAnchor:(MGLFillExtrusionTranslationAnchor)fillExtrusionTranslationAnchor {
    return [NSValue value:&fillExtrusionTranslationAnchor withObjCType:@encode(MGLFillExtrusionTranslationAnchor)];
}

- (MGLFillExtrusionTranslationAnchor)MGLFillExtrusionTranslationAnchorValue {
    MGLFillExtrusionTranslationAnchor fillExtrusionTranslationAnchor;
    [self getValue:&fillExtrusionTranslationAnchor];
    return fillExtrusionTranslationAnchor;
}

@end
