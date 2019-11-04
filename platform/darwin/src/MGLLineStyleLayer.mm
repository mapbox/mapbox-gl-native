// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLPrivateAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLLineStyleLayer.h"
#import "MGLLoggingConfiguration_Private.h"
#import "MGLLineStyleLayer_Private.h"

#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/transition_options.hpp>


namespace mbgl {

    MBGL_DEFINE_ENUM(MGLLineCap, {
        { MGLLineCapButt, "butt" },
        { MGLLineCapRound, "round" },
        { MGLLineCapSquare, "square" },
    });

    MBGL_DEFINE_ENUM(MGLLineJoin, {
        { MGLLineJoinBevel, "bevel" },
        { MGLLineJoinRound, "round" },
        { MGLLineJoinMiter, "miter" },
    });

    MBGL_DEFINE_ENUM(MGLLineTranslationAnchor, {
        { MGLLineTranslationAnchorMap, "map" },
        { MGLLineTranslationAnchorViewport, "viewport" },
    });

}

@interface MGLLineStyleLayer ()

@property (nonatomic, readonly) mbgl::style::LineLayer *rawLayer;

@end

@implementation MGLLineStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    MGLLogDebug(@"Initializing %@ with identifier: %@ source: %@", NSStringFromClass([self class]), identifier, source);
    auto layer = std::make_unique<mbgl::style::LineLayer>(identifier.UTF8String, source.identifier.UTF8String);
    return self = [super initWithPendingLayer:std::move(layer)];
}

- (mbgl::style::LineLayer *)rawLayer
{
    return (mbgl::style::LineLayer *)super.rawLayer;
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

#pragma mark - Accessing the Layout Attributes

- (void)setLineCap:(NSExpression *)lineCap {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineCap: %@", lineCap);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::LineCapType, NSValue *, mbgl::style::LineCapType, MGLLineCap>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::LineCapType>>(lineCap, false);
    self.rawLayer->setLineCap(mbglValue);
}

- (NSExpression *)lineCap {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineCap();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineCap();
    }
    return MGLStyleValueTransformer<mbgl::style::LineCapType, NSValue *, mbgl::style::LineCapType, MGLLineCap>().toExpression(propertyValue);
}

- (void)setLineJoin:(NSExpression *)lineJoin {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineJoin: %@", lineJoin);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::LineJoinType, NSValue *, mbgl::style::LineJoinType, MGLLineJoin>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::LineJoinType>>(lineJoin, true);
    self.rawLayer->setLineJoin(mbglValue);
}

- (NSExpression *)lineJoin {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineJoin();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineJoin();
    }
    return MGLStyleValueTransformer<mbgl::style::LineJoinType, NSValue *, mbgl::style::LineJoinType, MGLLineJoin>().toExpression(propertyValue);
}

- (void)setLineMiterLimit:(NSExpression *)lineMiterLimit {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineMiterLimit: %@", lineMiterLimit);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(lineMiterLimit, false);
    self.rawLayer->setLineMiterLimit(mbglValue);
}

- (NSExpression *)lineMiterLimit {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineMiterLimit();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineMiterLimit();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setLineRoundLimit:(NSExpression *)lineRoundLimit {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineRoundLimit: %@", lineRoundLimit);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(lineRoundLimit, false);
    self.rawLayer->setLineRoundLimit(mbglValue);
}

- (NSExpression *)lineRoundLimit {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineRoundLimit();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineRoundLimit();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

#pragma mark - Accessing the Paint Attributes

- (void)setLineBlur:(NSExpression *)lineBlur {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineBlur: %@", lineBlur);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(lineBlur, true);
    self.rawLayer->setLineBlur(mbglValue);
}

- (NSExpression *)lineBlur {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineBlur();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineBlur();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setLineBlurTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineBlurTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setLineBlurTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)lineBlurTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineBlurTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setLineColor:(NSExpression *)lineColor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineColor: %@", lineColor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(lineColor, true);
    self.rawLayer->setLineColor(mbglValue);
}

- (NSExpression *)lineColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineColor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineColor();
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(propertyValue);
}

- (void)setLineColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineColorTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setLineColorTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)lineColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineColorTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setLineDashPattern:(NSExpression *)lineDashPattern {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineDashPattern: %@", lineDashPattern);

    auto mbglValue = MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toPropertyValue<mbgl::style::PropertyValue<std::vector<float>>>(lineDashPattern, false);
    self.rawLayer->setLineDasharray(mbglValue);
}

- (NSExpression *)lineDashPattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineDasharray();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineDasharray();
    }
    return MGLStyleValueTransformer<std::vector<float>, NSArray<NSNumber *> *, float>().toExpression(propertyValue);
}

- (void)setLineDashPatternTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineDashPatternTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setLineDasharrayTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)lineDashPatternTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineDasharrayTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setLineDasharray:(NSExpression *)lineDasharray {
}

- (NSExpression *)lineDasharray {
    return self.lineDashPattern;
}

- (void)setLineGapWidth:(NSExpression *)lineGapWidth {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineGapWidth: %@", lineGapWidth);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(lineGapWidth, true);
    self.rawLayer->setLineGapWidth(mbglValue);
}

- (NSExpression *)lineGapWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineGapWidth();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineGapWidth();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setLineGapWidthTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineGapWidthTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setLineGapWidthTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)lineGapWidthTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineGapWidthTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setLineGradient:(NSExpression *)lineGradient {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineGradient: %@", lineGradient);

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::ColorRampPropertyValue>(lineGradient);
    self.rawLayer->setLineGradient(mbglValue);
}

- (NSExpression *)lineGradient {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineGradient();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineGradient();
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(propertyValue);
}

- (void)setLineOffset:(NSExpression *)lineOffset {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineOffset: %@", lineOffset);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(lineOffset, true);
    self.rawLayer->setLineOffset(mbglValue);
}

- (NSExpression *)lineOffset {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineOffset();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineOffset();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setLineOffsetTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineOffsetTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setLineOffsetTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)lineOffsetTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineOffsetTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setLineOpacity:(NSExpression *)lineOpacity {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineOpacity: %@", lineOpacity);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(lineOpacity, true);
    self.rawLayer->setLineOpacity(mbglValue);
}

- (NSExpression *)lineOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineOpacity();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineOpacity();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setLineOpacityTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineOpacityTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setLineOpacityTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)lineOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineOpacityTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setLinePattern:(NSExpression *)linePattern {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting linePattern: %@", linePattern);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::expression::Image, NSString *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::expression::Image>>(linePattern, true);
    self.rawLayer->setLinePattern(mbglValue);
}

- (NSExpression *)linePattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLinePattern();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLinePattern();
    }
    return MGLStyleValueTransformer<mbgl::style::expression::Image, NSString *>().toExpression(propertyValue);
}

- (void)setLinePatternTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting linePatternTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setLinePatternTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)linePatternTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLinePatternTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setLineTranslation:(NSExpression *)lineTranslation {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineTranslation: %@", lineTranslation);

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue<mbgl::style::PropertyValue<std::array<float, 2>>>(lineTranslation, false);
    self.rawLayer->setLineTranslate(mbglValue);
}

- (NSExpression *)lineTranslation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineTranslate();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineTranslate();
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toExpression(propertyValue);
}

- (void)setLineTranslationTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineTranslationTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setLineTranslateTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)lineTranslationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineTranslateTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setLineTranslate:(NSExpression *)lineTranslate {
}

- (NSExpression *)lineTranslate {
    return self.lineTranslation;
}

- (void)setLineTranslationAnchor:(NSExpression *)lineTranslationAnchor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineTranslationAnchor: %@", lineTranslationAnchor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLLineTranslationAnchor>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType>>(lineTranslationAnchor, false);
    self.rawLayer->setLineTranslateAnchor(mbglValue);
}

- (NSExpression *)lineTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineTranslateAnchor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineTranslateAnchor();
    }
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLLineTranslationAnchor>().toExpression(propertyValue);
}

- (void)setLineTranslateAnchor:(NSExpression *)lineTranslateAnchor {
}

- (NSExpression *)lineTranslateAnchor {
    return self.lineTranslationAnchor;
}

- (void)setLineWidth:(NSExpression *)lineWidth {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineWidth: %@", lineWidth);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(lineWidth, true);
    self.rawLayer->setLineWidth(mbglValue);
}

- (NSExpression *)lineWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getLineWidth();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultLineWidth();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setLineWidthTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting lineWidthTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setLineWidthTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)lineWidthTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getLineWidthTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

@end

@implementation NSValue (MGLLineStyleLayerAdditions)

+ (NSValue *)valueWithMGLLineCap:(MGLLineCap)lineCap {
    return [NSValue value:&lineCap withObjCType:@encode(MGLLineCap)];
}

- (MGLLineCap)MGLLineCapValue {
    MGLLineCap lineCap;
    [self getValue:&lineCap];
    return lineCap;
}

+ (NSValue *)valueWithMGLLineJoin:(MGLLineJoin)lineJoin {
    return [NSValue value:&lineJoin withObjCType:@encode(MGLLineJoin)];
}

- (MGLLineJoin)MGLLineJoinValue {
    MGLLineJoin lineJoin;
    [self getValue:&lineJoin];
    return lineJoin;
}

+ (NSValue *)valueWithMGLLineTranslationAnchor:(MGLLineTranslationAnchor)lineTranslationAnchor {
    return [NSValue value:&lineTranslationAnchor withObjCType:@encode(MGLLineTranslationAnchor)];
}

- (MGLLineTranslationAnchor)MGLLineTranslationAnchorValue {
    MGLLineTranslationAnchor lineTranslationAnchor;
    [self getValue:&lineTranslationAnchor];
    return lineTranslationAnchor;
}

@end

namespace mbgl {

MGLStyleLayer* LineStyleLayerPeerFactory::createPeer(style::Layer* rawLayer) {
    return [[MGLLineStyleLayer alloc] initWithRawLayer:rawLayer];
}

}  // namespace mbgl
