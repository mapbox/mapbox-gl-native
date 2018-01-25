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

#pragma mark - Accessing the Paint Attributes

- (void)setHillshadeAccentColor:(NSExpression *)hillshadeAccentColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(hillshadeAccentColor);
    self.rawLayer->setHillshadeAccentColor(mbglValue);
}

- (NSExpression *)hillshadeAccentColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getHillshadeAccentColor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultHillshadeAccentColor();
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(propertyValue);
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

- (void)setHillshadeExaggeration:(NSExpression *)hillshadeExaggeration {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(hillshadeExaggeration);
    self.rawLayer->setHillshadeExaggeration(mbglValue);
}

- (NSExpression *)hillshadeExaggeration {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getHillshadeExaggeration();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultHillshadeExaggeration();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
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

- (void)setHillshadeHighlightColor:(NSExpression *)hillshadeHighlightColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(hillshadeHighlightColor);
    self.rawLayer->setHillshadeHighlightColor(mbglValue);
}

- (NSExpression *)hillshadeHighlightColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getHillshadeHighlightColor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultHillshadeHighlightColor();
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(propertyValue);
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

- (void)setHillshadeIlluminationAnchor:(NSExpression *)hillshadeIlluminationAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::HillshadeIlluminationAnchorType, NSValue *, mbgl::style::HillshadeIlluminationAnchorType, MGLHillshadeIlluminationAnchor>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::HillshadeIlluminationAnchorType>>(hillshadeIlluminationAnchor);
    self.rawLayer->setHillshadeIlluminationAnchor(mbglValue);
}

- (NSExpression *)hillshadeIlluminationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getHillshadeIlluminationAnchor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultHillshadeIlluminationAnchor();
    }
    return MGLStyleValueTransformer<mbgl::style::HillshadeIlluminationAnchorType, NSValue *, mbgl::style::HillshadeIlluminationAnchorType, MGLHillshadeIlluminationAnchor>().toExpression(propertyValue);
}

- (void)setHillshadeIlluminationDirection:(NSExpression *)hillshadeIlluminationDirection {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(hillshadeIlluminationDirection);
    self.rawLayer->setHillshadeIlluminationDirection(mbglValue);
}

- (NSExpression *)hillshadeIlluminationDirection {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getHillshadeIlluminationDirection();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultHillshadeIlluminationDirection();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setHillshadeShadowColor:(NSExpression *)hillshadeShadowColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(hillshadeShadowColor);
    self.rawLayer->setHillshadeShadowColor(mbglValue);
}

- (NSExpression *)hillshadeShadowColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getHillshadeShadowColor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultHillshadeShadowColor();
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(propertyValue);
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
