// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLPrivateAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLFillExtrusionStyleLayer.h"
#import "MGLLoggingConfiguration_Private.h"
#import "MGLFillExtrusionStyleLayer_Private.h"

#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/transition_options.hpp>


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
    MGLLogDebug(@"Initializing %@ with identifier: %@ source: %@", NSStringFromClass([self class]), identifier, source);
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

- (void)setFillExtrusionBase:(NSExpression *)fillExtrusionBase {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillExtrusionBase: %@", fillExtrusionBase);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(fillExtrusionBase, true);
    self.rawLayer->setFillExtrusionBase(mbglValue);
}

- (NSExpression *)fillExtrusionBase {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionBase();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultFillExtrusionBase();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setFillExtrusionBaseTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillExtrusionBaseTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setFillExtrusionBaseTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)fillExtrusionBaseTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillExtrusionBaseTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setFillExtrusionColor:(NSExpression *)fillExtrusionColor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillExtrusionColor: %@", fillExtrusionColor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(fillExtrusionColor, true);
    self.rawLayer->setFillExtrusionColor(mbglValue);
}

- (NSExpression *)fillExtrusionColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionColor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultFillExtrusionColor();
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(propertyValue);
}

- (void)setFillExtrusionColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillExtrusionColorTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setFillExtrusionColorTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)fillExtrusionColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillExtrusionColorTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setFillExtrusionHasVerticalGradient:(NSExpression *)fillExtrusionHasVerticalGradient {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillExtrusionHasVerticalGradient: %@", fillExtrusionHasVerticalGradient);

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(fillExtrusionHasVerticalGradient, false);
    self.rawLayer->setFillExtrusionVerticalGradient(mbglValue);
}

- (NSExpression *)fillExtrusionHasVerticalGradient {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionVerticalGradient();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultFillExtrusionVerticalGradient();
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toExpression(propertyValue);
}

- (void)setFillExtrusionVerticalGradient:(NSExpression *)fillExtrusionVerticalGradient {
}

- (NSExpression *)fillExtrusionVerticalGradient {
    return self.fillExtrusionHasVerticalGradient;
}

- (void)setFillExtrusionHeight:(NSExpression *)fillExtrusionHeight {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillExtrusionHeight: %@", fillExtrusionHeight);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(fillExtrusionHeight, true);
    self.rawLayer->setFillExtrusionHeight(mbglValue);
}

- (NSExpression *)fillExtrusionHeight {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionHeight();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultFillExtrusionHeight();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setFillExtrusionHeightTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillExtrusionHeightTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setFillExtrusionHeightTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)fillExtrusionHeightTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillExtrusionHeightTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setFillExtrusionOpacity:(NSExpression *)fillExtrusionOpacity {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillExtrusionOpacity: %@", fillExtrusionOpacity);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(fillExtrusionOpacity, false);
    self.rawLayer->setFillExtrusionOpacity(mbglValue);
}

- (NSExpression *)fillExtrusionOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionOpacity();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultFillExtrusionOpacity();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setFillExtrusionOpacityTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillExtrusionOpacityTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setFillExtrusionOpacityTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)fillExtrusionOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillExtrusionOpacityTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setFillExtrusionPattern:(NSExpression *)fillExtrusionPattern {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillExtrusionPattern: %@", fillExtrusionPattern);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::expression::Image, NSString *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::expression::Image>>(fillExtrusionPattern, true);
    self.rawLayer->setFillExtrusionPattern(mbglValue);
}

- (NSExpression *)fillExtrusionPattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionPattern();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultFillExtrusionPattern();
    }
    return MGLStyleValueTransformer<mbgl::style::expression::Image, NSString *>().toExpression(propertyValue);
}

- (void)setFillExtrusionPatternTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillExtrusionPatternTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setFillExtrusionPatternTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)fillExtrusionPatternTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillExtrusionPatternTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setFillExtrusionTranslation:(NSExpression *)fillExtrusionTranslation {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillExtrusionTranslation: %@", fillExtrusionTranslation);

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue<mbgl::style::PropertyValue<std::array<float, 2>>>(fillExtrusionTranslation, false);
    self.rawLayer->setFillExtrusionTranslate(mbglValue);
}

- (NSExpression *)fillExtrusionTranslation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionTranslate();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultFillExtrusionTranslate();
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toExpression(propertyValue);
}

- (void)setFillExtrusionTranslationTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillExtrusionTranslationTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setFillExtrusionTranslateTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)fillExtrusionTranslationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getFillExtrusionTranslateTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setFillExtrusionTranslate:(NSExpression *)fillExtrusionTranslate {
}

- (NSExpression *)fillExtrusionTranslate {
    return self.fillExtrusionTranslation;
}

- (void)setFillExtrusionTranslationAnchor:(NSExpression *)fillExtrusionTranslationAnchor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting fillExtrusionTranslationAnchor: %@", fillExtrusionTranslationAnchor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillExtrusionTranslationAnchor>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType>>(fillExtrusionTranslationAnchor, false);
    self.rawLayer->setFillExtrusionTranslateAnchor(mbglValue);
}

- (NSExpression *)fillExtrusionTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionTranslateAnchor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultFillExtrusionTranslateAnchor();
    }
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillExtrusionTranslationAnchor>().toExpression(propertyValue);
}

- (void)setFillExtrusionTranslateAnchor:(NSExpression *)fillExtrusionTranslateAnchor {
}

- (NSExpression *)fillExtrusionTranslateAnchor {
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

namespace mbgl {

MGLStyleLayer* FillExtrusionStyleLayerPeerFactory::createPeer(style::Layer* rawLayer) {
    return [[MGLFillExtrusionStyleLayer alloc] initWithRawLayer:rawLayer];
}

}  // namespace mbgl
