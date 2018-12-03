// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLPrivateAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLCircleStyleLayer.h"
#import "MGLLoggingConfiguration_Private.h"
#import "MGLCircleStyleLayer_Private.h"

#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/transition_options.hpp>


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
    MGLLogDebug(@"Initializing %@ with identifier: %@ source: %@", NSStringFromClass([self class]), identifier, source);
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

- (void)setCircleBlur:(NSExpression *)circleBlur {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleBlur: %@", circleBlur);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(circleBlur, true);
    self.rawLayer->setCircleBlur(mbglValue);
}

- (NSExpression *)circleBlur {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleBlur();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultCircleBlur();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setCircleBlurTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleBlurTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setCircleBlurTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)circleBlurTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getCircleBlurTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setCircleColor:(NSExpression *)circleColor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleColor: %@", circleColor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(circleColor, true);
    self.rawLayer->setCircleColor(mbglValue);
}

- (NSExpression *)circleColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleColor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultCircleColor();
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(propertyValue);
}

- (void)setCircleColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleColorTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setCircleColorTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)circleColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getCircleColorTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setCircleOpacity:(NSExpression *)circleOpacity {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleOpacity: %@", circleOpacity);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(circleOpacity, true);
    self.rawLayer->setCircleOpacity(mbglValue);
}

- (NSExpression *)circleOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleOpacity();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultCircleOpacity();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setCircleOpacityTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleOpacityTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setCircleOpacityTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)circleOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getCircleOpacityTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setCirclePitchAlignment:(NSExpression *)circlePitchAlignment {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circlePitchAlignment: %@", circlePitchAlignment);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLCirclePitchAlignment>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::AlignmentType>>(circlePitchAlignment, false);
    self.rawLayer->setCirclePitchAlignment(mbglValue);
}

- (NSExpression *)circlePitchAlignment {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCirclePitchAlignment();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultCirclePitchAlignment();
    }
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLCirclePitchAlignment>().toExpression(propertyValue);
}

- (void)setCircleRadius:(NSExpression *)circleRadius {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleRadius: %@", circleRadius);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(circleRadius, true);
    self.rawLayer->setCircleRadius(mbglValue);
}

- (NSExpression *)circleRadius {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleRadius();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultCircleRadius();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setCircleRadiusTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleRadiusTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setCircleRadiusTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)circleRadiusTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getCircleRadiusTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setCircleScaleAlignment:(NSExpression *)circleScaleAlignment {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleScaleAlignment: %@", circleScaleAlignment);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::CirclePitchScaleType, NSValue *, mbgl::style::CirclePitchScaleType, MGLCircleScaleAlignment>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::CirclePitchScaleType>>(circleScaleAlignment, false);
    self.rawLayer->setCirclePitchScale(mbglValue);
}

- (NSExpression *)circleScaleAlignment {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCirclePitchScale();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultCirclePitchScale();
    }
    return MGLStyleValueTransformer<mbgl::style::CirclePitchScaleType, NSValue *, mbgl::style::CirclePitchScaleType, MGLCircleScaleAlignment>().toExpression(propertyValue);
}

- (void)setCirclePitchScale:(NSExpression *)circlePitchScale {
}

- (NSExpression *)circlePitchScale {
    return self.circleScaleAlignment;
}

- (void)setCircleStrokeColor:(NSExpression *)circleStrokeColor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleStrokeColor: %@", circleStrokeColor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(circleStrokeColor, true);
    self.rawLayer->setCircleStrokeColor(mbglValue);
}

- (NSExpression *)circleStrokeColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleStrokeColor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultCircleStrokeColor();
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(propertyValue);
}

- (void)setCircleStrokeColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleStrokeColorTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setCircleStrokeColorTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)circleStrokeColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getCircleStrokeColorTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setCircleStrokeOpacity:(NSExpression *)circleStrokeOpacity {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleStrokeOpacity: %@", circleStrokeOpacity);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(circleStrokeOpacity, true);
    self.rawLayer->setCircleStrokeOpacity(mbglValue);
}

- (NSExpression *)circleStrokeOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleStrokeOpacity();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultCircleStrokeOpacity();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setCircleStrokeOpacityTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleStrokeOpacityTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setCircleStrokeOpacityTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)circleStrokeOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getCircleStrokeOpacityTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setCircleStrokeWidth:(NSExpression *)circleStrokeWidth {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleStrokeWidth: %@", circleStrokeWidth);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(circleStrokeWidth, true);
    self.rawLayer->setCircleStrokeWidth(mbglValue);
}

- (NSExpression *)circleStrokeWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleStrokeWidth();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultCircleStrokeWidth();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setCircleStrokeWidthTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleStrokeWidthTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setCircleStrokeWidthTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)circleStrokeWidthTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getCircleStrokeWidthTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setCircleTranslation:(NSExpression *)circleTranslation {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleTranslation: %@", circleTranslation);

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue<mbgl::style::PropertyValue<std::array<float, 2>>>(circleTranslation, false);
    self.rawLayer->setCircleTranslate(mbglValue);
}

- (NSExpression *)circleTranslation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleTranslate();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultCircleTranslate();
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toExpression(propertyValue);
}

- (void)setCircleTranslationTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleTranslationTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setCircleTranslateTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)circleTranslationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getCircleTranslateTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setCircleTranslate:(NSExpression *)circleTranslate {
}

- (NSExpression *)circleTranslate {
    return self.circleTranslation;
}

- (void)setCircleTranslationAnchor:(NSExpression *)circleTranslationAnchor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting circleTranslationAnchor: %@", circleTranslationAnchor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLCircleTranslationAnchor>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType>>(circleTranslationAnchor, false);
    self.rawLayer->setCircleTranslateAnchor(mbglValue);
}

- (NSExpression *)circleTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleTranslateAnchor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultCircleTranslateAnchor();
    }
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLCircleTranslationAnchor>().toExpression(propertyValue);
}

- (void)setCircleTranslateAnchor:(NSExpression *)circleTranslateAnchor {
}

- (NSExpression *)circleTranslateAnchor {
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

namespace mbgl {

MGLStyleLayer* CircleStyleLayerPeerFactory::createPeer(style::Layer* rawLayer) {
    return [[MGLCircleStyleLayer alloc] initWithRawLayer:rawLayer];
}

}  // namespace mbgl
