// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLPrivateAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLHillshadeStyleLayer.h"
#import "MGLLoggingConfiguration_Private.h"
#import "MGLHillshadeStyleLayer_Private.h"

#include <mbgl/style/layers/hillshade_layer.hpp>
#include <mbgl/style/transition_options.hpp>


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
    MGLLogDebug(@"Initializing %@ with identifier: %@ source: %@", NSStringFromClass([self class]), identifier, source);
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
    MGLLogDebug(@"Setting hillshadeAccentColor: %@", hillshadeAccentColor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(hillshadeAccentColor, false);
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
    MGLLogDebug(@"Setting hillshadeAccentColorTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setHillshadeAccentColorTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)hillshadeAccentColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getHillshadeAccentColorTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setHillshadeExaggeration:(NSExpression *)hillshadeExaggeration {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting hillshadeExaggeration: %@", hillshadeExaggeration);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(hillshadeExaggeration, false);
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
    MGLLogDebug(@"Setting hillshadeExaggerationTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setHillshadeExaggerationTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)hillshadeExaggerationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getHillshadeExaggerationTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setHillshadeHighlightColor:(NSExpression *)hillshadeHighlightColor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting hillshadeHighlightColor: %@", hillshadeHighlightColor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(hillshadeHighlightColor, false);
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
    MGLLogDebug(@"Setting hillshadeHighlightColorTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setHillshadeHighlightColorTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)hillshadeHighlightColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getHillshadeHighlightColorTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setHillshadeIlluminationAnchor:(NSExpression *)hillshadeIlluminationAnchor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting hillshadeIlluminationAnchor: %@", hillshadeIlluminationAnchor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::HillshadeIlluminationAnchorType, NSValue *, mbgl::style::HillshadeIlluminationAnchorType, MGLHillshadeIlluminationAnchor>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::HillshadeIlluminationAnchorType>>(hillshadeIlluminationAnchor, false);
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
    MGLLogDebug(@"Setting hillshadeIlluminationDirection: %@", hillshadeIlluminationDirection);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(hillshadeIlluminationDirection, false);
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
    MGLLogDebug(@"Setting hillshadeShadowColor: %@", hillshadeShadowColor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(hillshadeShadowColor, false);
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
    MGLLogDebug(@"Setting hillshadeShadowColorTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setHillshadeShadowColorTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)hillshadeShadowColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getHillshadeShadowColorTransition();

    return MGLTransitionFromOptions(transitionOptions);
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

namespace mbgl {

MGLStyleLayer* HillshadeStyleLayerPeerFactory::createPeer(style::Layer* rawLayer) {
    return [[MGLHillshadeStyleLayer alloc] initWithRawLayer:rawLayer];
}

}  // namespace mbgl
