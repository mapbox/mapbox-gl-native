// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLFillStyleLayer.h"

#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/layers/fill_layer.hpp>

namespace mbgl {

    MBGL_DEFINE_ENUM(MGLFillTranslationAnchor, {
        { MGLFillTranslationAnchorMap, "map" },
        { MGLFillTranslationAnchorViewport, "viewport" },
    });

}

@interface MGLFillStyleLayer ()

@property (nonatomic, readonly) mbgl::style::FillLayer *rawLayer;

@end

@implementation MGLFillStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    auto layer = std::make_unique<mbgl::style::FillLayer>(identifier.UTF8String, source.identifier.UTF8String);
    return self = [super initWithPendingLayer:std::move(layer)];
}

- (mbgl::style::FillLayer *)rawLayer
{
    return (mbgl::style::FillLayer *)super.rawLayer;
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

- (void)setFillAntialiased:(MGLStyleValue<NSNumber *> *)fillAntialiased {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(fillAntialiased);
    self.rawLayer->setFillAntialias(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)isFillAntialiased {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillAntialias();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(self.rawLayer->getDefaultFillAntialias());
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setFillAntialias:(MGLStyleValue<NSNumber *> *)fillAntialias {
}

- (MGLStyleValue<NSNumber *> *)fillAntialias {
    return self.isFillAntialiased;
}

- (void)setFillColor:(MGLStyleValue<MGLColor *> *)fillColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenPropertyValue(fillColor);
    self.rawLayer->setFillColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)fillColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(self.rawLayer->getDefaultFillColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setFillColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillColorTransition(options);
}

- (MGLTransition)fillColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillColorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setFillOpacity:(MGLStyleValue<NSNumber *> *)fillOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(fillOpacity);
    self.rawLayer->setFillOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)fillOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillOpacity();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultFillOpacity());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setFillOpacityTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillOpacityTransition(options);
}

- (MGLTransition)fillOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillOpacityTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setFillOutlineColor:(MGLStyleValue<MGLColor *> *)fillOutlineColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenPropertyValue(fillOutlineColor);
    self.rawLayer->setFillOutlineColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)fillOutlineColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillOutlineColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(self.rawLayer->getDefaultFillOutlineColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setFillOutlineColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillOutlineColorTransition(options);
}

- (MGLTransition)fillOutlineColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillOutlineColorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setFillPattern:(MGLStyleValue<NSString *> *)fillPattern {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(fillPattern);
    self.rawLayer->setFillPattern(mbglValue);
}

- (MGLStyleValue<NSString *> *)fillPattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillPattern();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(self.rawLayer->getDefaultFillPattern());
    }
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

- (void)setFillPatternTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillPatternTransition(options);
}

- (MGLTransition)fillPatternTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillPatternTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setFillTranslation:(MGLStyleValue<NSValue *> *)fillTranslation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toInterpolatablePropertyValue(fillTranslation);
    self.rawLayer->setFillTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)fillTranslation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillTranslate();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(self.rawLayer->getDefaultFillTranslate());
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setFillTranslationTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillTranslateTransition(options);
}

- (MGLTransition)fillTranslationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillTranslateTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setFillTranslate:(MGLStyleValue<NSValue *> *)fillTranslate {
}

- (MGLStyleValue<NSValue *> *)fillTranslate {
    return self.fillTranslation;
}

- (void)setFillTranslationAnchor:(MGLStyleValue<NSValue *> *)fillTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillTranslationAnchor>().toEnumPropertyValue(fillTranslationAnchor);
    self.rawLayer->setFillTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)fillTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillTranslateAnchor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillTranslationAnchor>().toEnumStyleValue(self.rawLayer->getDefaultFillTranslateAnchor());
    }
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillTranslationAnchor>().toEnumStyleValue(propertyValue);
}

- (void)setFillTranslateAnchor:(MGLStyleValue<NSValue *> *)fillTranslateAnchor {
}

- (MGLStyleValue<NSValue *> *)fillTranslateAnchor {
    return self.fillTranslationAnchor;
}

@end

@implementation NSValue (MGLFillStyleLayerAdditions)

+ (NSValue *)valueWithMGLFillTranslationAnchor:(MGLFillTranslationAnchor)fillTranslationAnchor {
    return [NSValue value:&fillTranslationAnchor withObjCType:@encode(MGLFillTranslationAnchor)];
}

- (MGLFillTranslationAnchor)MGLFillTranslationAnchorValue {
    MGLFillTranslationAnchor fillTranslationAnchor;
    [self getValue:&fillTranslationAnchor];
    return fillTranslationAnchor;
}

@end
