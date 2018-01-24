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

- (void)setFillAntialiased:(NSExpression *)fillAntialiased {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(fillAntialiased);
    self.rawLayer->setFillAntialias(mbglValue);
}

- (NSExpression *)isFillAntialiased {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillAntialias();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultFillAntialias();
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toExpression(propertyValue);
}

- (void)setFillAntialias:(NSExpression *)fillAntialias {
}

- (NSExpression *)fillAntialias {
    return self.isFillAntialiased;
}

- (void)setFillColor:(NSExpression *)fillColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<mbgl::Color>>(fillColor);
    self.rawLayer->setFillColor(mbglValue);
}

- (NSExpression *)fillColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillColor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultFillColor();
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(propertyValue);
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

- (void)setFillOpacity:(NSExpression *)fillOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(fillOpacity);
    self.rawLayer->setFillOpacity(mbglValue);
}

- (NSExpression *)fillOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillOpacity();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultFillOpacity();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
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

- (void)setFillOutlineColor:(NSExpression *)fillOutlineColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<mbgl::Color>>(fillOutlineColor);
    self.rawLayer->setFillOutlineColor(mbglValue);
}

- (NSExpression *)fillOutlineColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillOutlineColor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultFillOutlineColor();
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(propertyValue);
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

- (void)setFillPattern:(NSExpression *)fillPattern {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue<mbgl::style::PropertyValue<std::string>>(fillPattern);
    self.rawLayer->setFillPattern(mbglValue);
}

- (NSExpression *)fillPattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillPattern();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultFillPattern();
    }
    return MGLStyleValueTransformer<std::string, NSString *>().toExpression(propertyValue);
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

- (void)setFillTranslation:(NSExpression *)fillTranslation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue<mbgl::style::PropertyValue<std::array<float, 2>>>(fillTranslation);
    self.rawLayer->setFillTranslate(mbglValue);
}

- (NSExpression *)fillTranslation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillTranslate();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultFillTranslate();
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toExpression(propertyValue);
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

- (void)setFillTranslate:(NSExpression *)fillTranslate {
}

- (NSExpression *)fillTranslate {
    return self.fillTranslation;
}

- (void)setFillTranslationAnchor:(NSExpression *)fillTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillTranslationAnchor>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType>>(fillTranslationAnchor);
    self.rawLayer->setFillTranslateAnchor(mbglValue);
}

- (NSExpression *)fillTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillTranslateAnchor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultFillTranslateAnchor();
    }
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillTranslationAnchor>().toExpression(propertyValue);
}

- (void)setFillTranslateAnchor:(NSExpression *)fillTranslateAnchor {
}

- (NSExpression *)fillTranslateAnchor {
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
