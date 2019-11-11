// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLPrivateAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLFillStyleLayer.h"
#import "MGLLoggingConfiguration_Private.h"
#import "MGLFillStyleLayer_Private.h"

#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/transition_options.hpp>


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
    MGLLogDebug(@"Initializing %@ with identifier: %@ source: %@", NSStringFromClass([self class]), identifier, source);
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
    MGLLogDebug(@"Setting sourceLayerIdentifier: %@", sourceLayerIdentifier);

    self.rawLayer->setSourceLayer(sourceLayerIdentifier.UTF8String ?: "");
}

- (void)setPredicate:(NSPredicate *)predicate
{
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting predicate: %@", predicate);

    self.rawLayer->setFilter(predicate ? predicate.mgl_filter : mbgl::style::Filter());
}

- (NSPredicate *)predicate
{
    MGLAssertStyleLayerIsValid();

    return [NSPredicate mgl_predicateWithFilter:self.rawLayer->getFilter()];
}

#pragma mark - Accessing the Paint Attributes

- (void)setFillAntialiased:(NSExpression *)fillAntialiased {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillAntialiased: %@", fillAntialiased);

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(fillAntialiased, false);
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
    MGLLogDebug(@"Setting fillColor: %@", fillColor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(fillColor, true);
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
    MGLLogDebug(@"Setting fillColorTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setFillColorTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)fillColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillColorTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setFillOpacity:(NSExpression *)fillOpacity {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillOpacity: %@", fillOpacity);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(fillOpacity, true);
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
    MGLLogDebug(@"Setting fillOpacityTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setFillOpacityTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)fillOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillOpacityTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setFillOutlineColor:(NSExpression *)fillOutlineColor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillOutlineColor: %@", fillOutlineColor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(fillOutlineColor, true);
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
    MGLLogDebug(@"Setting fillOutlineColorTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setFillOutlineColorTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)fillOutlineColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillOutlineColorTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setFillPattern:(NSExpression *)fillPattern {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillPattern: %@", fillPattern);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::expression::Image, NSString *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::expression::Image>>(fillPattern, true);
    self.rawLayer->setFillPattern(mbglValue);
}

- (NSExpression *)fillPattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillPattern();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultFillPattern();
    }
    return MGLStyleValueTransformer<mbgl::style::expression::Image, NSString *>().toExpression(propertyValue);
}

- (void)setFillPatternTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillPatternTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setFillPatternTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)fillPatternTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillPatternTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setFillTranslation:(NSExpression *)fillTranslation {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillTranslation: %@", fillTranslation);

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue<mbgl::style::PropertyValue<std::array<float, 2>>>(fillTranslation, false);
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
    MGLLogDebug(@"Setting fillTranslationTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setFillTranslateTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)fillTranslationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillTranslateTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setFillTranslate:(NSExpression *)fillTranslate {
}

- (NSExpression *)fillTranslate {
    return self.fillTranslation;
}

- (void)setFillTranslationAnchor:(NSExpression *)fillTranslationAnchor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillTranslationAnchor: %@", fillTranslationAnchor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillTranslationAnchor>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType>>(fillTranslationAnchor, false);
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

namespace mbgl {

MGLStyleLayer* FillStyleLayerPeerFactory::createPeer(style::Layer* rawLayer) {
    return [[MGLFillStyleLayer alloc] initWithRawLayer:rawLayer];
}

}  // namespace mbgl
