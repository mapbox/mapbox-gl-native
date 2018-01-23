// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLHillshadeStyleLayer.h"

#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/layers/hillshade_layer.hpp>

namespace mbgl {

    MBGL_DEFINE_ENUM(MGLHillshadeIlluminationAnchor, {
        { MGLHillshadeIlluminationAnchorMap, "map" },
        { MGLHillshadeIlluminationAnchorViewport, "viewport" },
    });

}

@interface MGLHillshadeStyleLayer ()

@property (nonatomic, readonly) mbgl::style::HillshadeLayer *rawLayer;

@end

@implementation MGLHillshadeStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    auto layer = std::make_unique<mbgl::style::HillshadeLayer>(identifier.UTF8String, source.identifier.UTF8String);
    return self = [super initWithPendingLayer:std::move(layer)];
}

- (mbgl::style::HillshadeLayer *)rawLayer
{
    return (mbgl::style::HillshadeLayer *)super.rawLayer;
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

- (void)setHillshadeAccentColor:(MGLStyleValue<MGLColor *> *)hillshadeAccentColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toInterpolatablePropertyValue(hillshadeAccentColor);
    self.rawLayer->setHillshadeAccentColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)hillshadeAccentColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getHillshadeAccentColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(self.rawLayer->getDefaultHillshadeAccentColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setHillshadeAccentColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setHillshadeAccentColorTransition(options);
}

- (MGLTransition)hillshadeAccentColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getHillshadeAccentColorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setHillshadeExaggeration:(MGLStyleValue<NSNumber *> *)hillshadeExaggeration {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(hillshadeExaggeration);
    self.rawLayer->setHillshadeExaggeration(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)hillshadeExaggeration {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getHillshadeExaggeration();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultHillshadeExaggeration());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setHillshadeExaggerationTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setHillshadeExaggerationTransition(options);
}

- (MGLTransition)hillshadeExaggerationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getHillshadeExaggerationTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setHillshadeHighlightColor:(MGLStyleValue<MGLColor *> *)hillshadeHighlightColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toInterpolatablePropertyValue(hillshadeHighlightColor);
    self.rawLayer->setHillshadeHighlightColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)hillshadeHighlightColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getHillshadeHighlightColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(self.rawLayer->getDefaultHillshadeHighlightColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setHillshadeHighlightColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setHillshadeHighlightColorTransition(options);
}

- (MGLTransition)hillshadeHighlightColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getHillshadeHighlightColorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setHillshadeIlluminationAnchor:(MGLStyleValue<NSValue *> *)hillshadeIlluminationAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::HillshadeIlluminationAnchorType, NSValue *, mbgl::style::HillshadeIlluminationAnchorType, MGLHillshadeIlluminationAnchor>().toEnumPropertyValue(hillshadeIlluminationAnchor);
    self.rawLayer->setHillshadeIlluminationAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)hillshadeIlluminationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getHillshadeIlluminationAnchor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::HillshadeIlluminationAnchorType, NSValue *, mbgl::style::HillshadeIlluminationAnchorType, MGLHillshadeIlluminationAnchor>().toEnumStyleValue(self.rawLayer->getDefaultHillshadeIlluminationAnchor());
    }
    return MGLStyleValueTransformer<mbgl::style::HillshadeIlluminationAnchorType, NSValue *, mbgl::style::HillshadeIlluminationAnchorType, MGLHillshadeIlluminationAnchor>().toEnumStyleValue(propertyValue);
}

- (void)setHillshadeIlluminationDirection:(MGLStyleValue<NSNumber *> *)hillshadeIlluminationDirection {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(hillshadeIlluminationDirection);
    self.rawLayer->setHillshadeIlluminationDirection(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)hillshadeIlluminationDirection {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getHillshadeIlluminationDirection();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultHillshadeIlluminationDirection());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setHillshadeIlluminationDirectionTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setHillshadeIlluminationDirectionTransition(options);
}

- (MGLTransition)hillshadeIlluminationDirectionTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getHillshadeIlluminationDirectionTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setHillshadeShadowColor:(MGLStyleValue<MGLColor *> *)hillshadeShadowColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toInterpolatablePropertyValue(hillshadeShadowColor);
    self.rawLayer->setHillshadeShadowColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)hillshadeShadowColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getHillshadeShadowColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(self.rawLayer->getDefaultHillshadeShadowColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setHillshadeShadowColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setHillshadeShadowColorTransition(options);
}

- (MGLTransition)hillshadeShadowColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getHillshadeShadowColorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

@end

@implementation NSValue (MGLHillshadeStyleLayerAdditions)

+ (NSValue *)valueWithMGLHillshadeIlluminationAnchor:(MGLHillshadeIlluminationAnchor)hillshadeIlluminationAnchor {
    return [NSValue value:&hillshadeIlluminationAnchor withObjCType:@encode(MGLHillshadeIlluminationAnchor)];
}

- (MGLHillshadeIlluminationAnchor)MGLHillshadeIlluminationAnchorValue {
    MGLHillshadeIlluminationAnchor hillshadeIlluminationAnchor;
    [self getValue:&hillshadeIlluminationAnchor];
    return hillshadeIlluminationAnchor;
}

@end
