// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLPrivateAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLSymbolStyleLayer.h"
#import "MGLLoggingConfiguration_Private.h"
#import "MGLSymbolStyleLayer_Private.h"

#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/transition_options.hpp>


namespace mbgl {

    MBGL_DEFINE_ENUM(MGLIconAnchor, {
        { MGLIconAnchorCenter, "center" },
        { MGLIconAnchorLeft, "left" },
        { MGLIconAnchorRight, "right" },
        { MGLIconAnchorTop, "top" },
        { MGLIconAnchorBottom, "bottom" },
        { MGLIconAnchorTopLeft, "top-left" },
        { MGLIconAnchorTopRight, "top-right" },
        { MGLIconAnchorBottomLeft, "bottom-left" },
        { MGLIconAnchorBottomRight, "bottom-right" },
    });

    MBGL_DEFINE_ENUM(MGLIconPitchAlignment, {
        { MGLIconPitchAlignmentMap, "map" },
        { MGLIconPitchAlignmentViewport, "viewport" },
        { MGLIconPitchAlignmentAuto, "auto" },
    });

    MBGL_DEFINE_ENUM(MGLIconRotationAlignment, {
        { MGLIconRotationAlignmentMap, "map" },
        { MGLIconRotationAlignmentViewport, "viewport" },
        { MGLIconRotationAlignmentAuto, "auto" },
    });

    MBGL_DEFINE_ENUM(MGLIconTextFit, {
        { MGLIconTextFitNone, "none" },
        { MGLIconTextFitWidth, "width" },
        { MGLIconTextFitHeight, "height" },
        { MGLIconTextFitBoth, "both" },
    });

    MBGL_DEFINE_ENUM(MGLSymbolPlacement, {
        { MGLSymbolPlacementPoint, "point" },
        { MGLSymbolPlacementLine, "line" },
        { MGLSymbolPlacementLineCenter, "line-center" },
    });

    MBGL_DEFINE_ENUM(MGLSymbolZOrder, {
        { MGLSymbolZOrderAuto, "auto" },
        { MGLSymbolZOrderViewportY, "viewport-y" },
        { MGLSymbolZOrderSource, "source" },
    });

    MBGL_DEFINE_ENUM(MGLTextAnchor, {
        { MGLTextAnchorCenter, "center" },
        { MGLTextAnchorLeft, "left" },
        { MGLTextAnchorRight, "right" },
        { MGLTextAnchorTop, "top" },
        { MGLTextAnchorBottom, "bottom" },
        { MGLTextAnchorTopLeft, "top-left" },
        { MGLTextAnchorTopRight, "top-right" },
        { MGLTextAnchorBottomLeft, "bottom-left" },
        { MGLTextAnchorBottomRight, "bottom-right" },
    });

    MBGL_DEFINE_ENUM(MGLTextJustification, {
        { MGLTextJustificationAuto, "auto" },
        { MGLTextJustificationLeft, "left" },
        { MGLTextJustificationCenter, "center" },
        { MGLTextJustificationRight, "right" },
    });

    MBGL_DEFINE_ENUM(MGLTextPitchAlignment, {
        { MGLTextPitchAlignmentMap, "map" },
        { MGLTextPitchAlignmentViewport, "viewport" },
        { MGLTextPitchAlignmentAuto, "auto" },
    });

    MBGL_DEFINE_ENUM(MGLTextRotationAlignment, {
        { MGLTextRotationAlignmentMap, "map" },
        { MGLTextRotationAlignmentViewport, "viewport" },
        { MGLTextRotationAlignmentAuto, "auto" },
    });

    MBGL_DEFINE_ENUM(MGLTextTransform, {
        { MGLTextTransformNone, "none" },
        { MGLTextTransformUppercase, "uppercase" },
        { MGLTextTransformLowercase, "lowercase" },
    });

    MBGL_DEFINE_ENUM(MGLTextWritingModes, {
        { MGLTextWritingModesHorizontal, "horizontal" },
        { MGLTextWritingModesVertical, "vertical" },
    });

    MBGL_DEFINE_ENUM(MGLIconTranslationAnchor, {
        { MGLIconTranslationAnchorMap, "map" },
        { MGLIconTranslationAnchorViewport, "viewport" },
    });

    MBGL_DEFINE_ENUM(MGLTextTranslationAnchor, {
        { MGLTextTranslationAnchorMap, "map" },
        { MGLTextTranslationAnchorViewport, "viewport" },
    });

}

@interface MGLSymbolStyleLayer ()

@property (nonatomic, readonly) mbgl::style::SymbolLayer *rawLayer;

@end

@implementation MGLSymbolStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    MGLLogDebug(@"Initializing %@ with identifier: %@ source: %@", NSStringFromClass([self class]), identifier, source);
    auto layer = std::make_unique<mbgl::style::SymbolLayer>(identifier.UTF8String, source.identifier.UTF8String);
    return self = [super initWithPendingLayer:std::move(layer)];
}

- (mbgl::style::SymbolLayer *)rawLayer
{
    return (mbgl::style::SymbolLayer *)super.rawLayer;
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

- (void)setIconAllowsOverlap:(NSExpression *)iconAllowsOverlap {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconAllowsOverlap: %@", iconAllowsOverlap);

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(iconAllowsOverlap, false);
    self.rawLayer->setIconAllowOverlap(mbglValue);
}

- (NSExpression *)iconAllowsOverlap {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconAllowOverlap();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconAllowOverlap();
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toExpression(propertyValue);
}

- (void)setIconAllowOverlap:(NSExpression *)iconAllowOverlap {
}

- (NSExpression *)iconAllowOverlap {
    return self.iconAllowsOverlap;
}

- (void)setIconAnchor:(NSExpression *)iconAnchor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconAnchor: %@", iconAnchor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::SymbolAnchorType, NSValue *, mbgl::style::SymbolAnchorType, MGLIconAnchor>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::SymbolAnchorType>>(iconAnchor, true);
    self.rawLayer->setIconAnchor(mbglValue);
}

- (NSExpression *)iconAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconAnchor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconAnchor();
    }
    return MGLStyleValueTransformer<mbgl::style::SymbolAnchorType, NSValue *, mbgl::style::SymbolAnchorType, MGLIconAnchor>().toExpression(propertyValue);
}

- (void)setIconIgnoresPlacement:(NSExpression *)iconIgnoresPlacement {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconIgnoresPlacement: %@", iconIgnoresPlacement);

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(iconIgnoresPlacement, false);
    self.rawLayer->setIconIgnorePlacement(mbglValue);
}

- (NSExpression *)iconIgnoresPlacement {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconIgnorePlacement();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconIgnorePlacement();
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toExpression(propertyValue);
}

- (void)setIconIgnorePlacement:(NSExpression *)iconIgnorePlacement {
}

- (NSExpression *)iconIgnorePlacement {
    return self.iconIgnoresPlacement;
}

- (void)setIconImageName:(NSExpression *)iconImageName {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconImageName: %@", iconImageName);

    if (iconImageName && iconImageName.expressionType == NSConstantValueExpressionType) {
        std::string string = ((NSString *)iconImageName.constantValue).UTF8String;
        if (mbgl::style::conversion::hasTokens(string)) {
            self.rawLayer->setIconImage(mbgl::style::PropertyValue<std::string>(
                mbgl::style::conversion::convertTokenStringToExpression(string)));
            return;
        }
    }
    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue<mbgl::style::PropertyValue<std::string>>(iconImageName, true);
    self.rawLayer->setIconImage(mbglValue);
}

- (NSExpression *)iconImageName {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconImage();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconImage();
    }
    return MGLStyleValueTransformer<std::string, NSString *>().toExpression(propertyValue);
}

- (void)setIconImage:(NSExpression *)iconImage {
}

- (NSExpression *)iconImage {
    return self.iconImageName;
}

- (void)setIconOffset:(NSExpression *)iconOffset {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconOffset: %@", iconOffset);

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue<mbgl::style::PropertyValue<std::array<float, 2>>>(iconOffset, true);
    self.rawLayer->setIconOffset(mbglValue);
}

- (NSExpression *)iconOffset {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconOffset();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconOffset();
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toExpression(propertyValue);
}

- (void)setIconOptional:(NSExpression *)iconOptional {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconOptional: %@", iconOptional);

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(iconOptional, false);
    self.rawLayer->setIconOptional(mbglValue);
}

- (NSExpression *)isIconOptional {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconOptional();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconOptional();
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toExpression(propertyValue);
}

- (void)setIconPadding:(NSExpression *)iconPadding {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconPadding: %@", iconPadding);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(iconPadding, false);
    self.rawLayer->setIconPadding(mbglValue);
}

- (NSExpression *)iconPadding {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconPadding();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconPadding();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setIconPitchAlignment:(NSExpression *)iconPitchAlignment {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconPitchAlignment: %@", iconPitchAlignment);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLIconPitchAlignment>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::AlignmentType>>(iconPitchAlignment, false);
    self.rawLayer->setIconPitchAlignment(mbglValue);
}

- (NSExpression *)iconPitchAlignment {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconPitchAlignment();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconPitchAlignment();
    }
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLIconPitchAlignment>().toExpression(propertyValue);
}

- (void)setIconRotation:(NSExpression *)iconRotation {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconRotation: %@", iconRotation);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(iconRotation, true);
    self.rawLayer->setIconRotate(mbglValue);
}

- (NSExpression *)iconRotation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconRotate();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconRotate();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setIconRotate:(NSExpression *)iconRotate {
}

- (NSExpression *)iconRotate {
    return self.iconRotation;
}

- (void)setIconRotationAlignment:(NSExpression *)iconRotationAlignment {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconRotationAlignment: %@", iconRotationAlignment);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLIconRotationAlignment>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::AlignmentType>>(iconRotationAlignment, false);
    self.rawLayer->setIconRotationAlignment(mbglValue);
}

- (NSExpression *)iconRotationAlignment {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconRotationAlignment();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconRotationAlignment();
    }
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLIconRotationAlignment>().toExpression(propertyValue);
}

- (void)setIconScale:(NSExpression *)iconScale {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconScale: %@", iconScale);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(iconScale, true);
    self.rawLayer->setIconSize(mbglValue);
}

- (NSExpression *)iconScale {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconSize();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconSize();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setIconSize:(NSExpression *)iconSize {
}

- (NSExpression *)iconSize {
    return self.iconScale;
}

- (void)setIconTextFit:(NSExpression *)iconTextFit {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconTextFit: %@", iconTextFit);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::IconTextFitType, NSValue *, mbgl::style::IconTextFitType, MGLIconTextFit>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::IconTextFitType>>(iconTextFit, false);
    self.rawLayer->setIconTextFit(mbglValue);
}

- (NSExpression *)iconTextFit {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconTextFit();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconTextFit();
    }
    return MGLStyleValueTransformer<mbgl::style::IconTextFitType, NSValue *, mbgl::style::IconTextFitType, MGLIconTextFit>().toExpression(propertyValue);
}

- (void)setIconTextFitPadding:(NSExpression *)iconTextFitPadding {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconTextFitPadding: %@", iconTextFitPadding);

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 4>, NSValue *>().toPropertyValue<mbgl::style::PropertyValue<std::array<float, 4>>>(iconTextFitPadding, false);
    self.rawLayer->setIconTextFitPadding(mbglValue);
}

- (NSExpression *)iconTextFitPadding {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconTextFitPadding();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconTextFitPadding();
    }
    return MGLStyleValueTransformer<std::array<float, 4>, NSValue *>().toExpression(propertyValue);
}

- (void)setKeepsIconUpright:(NSExpression *)keepsIconUpright {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting keepsIconUpright: %@", keepsIconUpright);

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(keepsIconUpright, false);
    self.rawLayer->setIconKeepUpright(mbglValue);
}

- (NSExpression *)keepsIconUpright {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconKeepUpright();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconKeepUpright();
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toExpression(propertyValue);
}

- (void)setIconKeepUpright:(NSExpression *)iconKeepUpright {
}

- (NSExpression *)iconKeepUpright {
    return self.keepsIconUpright;
}

- (void)setKeepsTextUpright:(NSExpression *)keepsTextUpright {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting keepsTextUpright: %@", keepsTextUpright);

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(keepsTextUpright, false);
    self.rawLayer->setTextKeepUpright(mbglValue);
}

- (NSExpression *)keepsTextUpright {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextKeepUpright();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextKeepUpright();
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toExpression(propertyValue);
}

- (void)setTextKeepUpright:(NSExpression *)textKeepUpright {
}

- (NSExpression *)textKeepUpright {
    return self.keepsTextUpright;
}

- (void)setMaximumTextAngle:(NSExpression *)maximumTextAngle {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting maximumTextAngle: %@", maximumTextAngle);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(maximumTextAngle, false);
    self.rawLayer->setTextMaxAngle(mbglValue);
}

- (NSExpression *)maximumTextAngle {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextMaxAngle();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextMaxAngle();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setTextMaxAngle:(NSExpression *)textMaxAngle {
}

- (NSExpression *)textMaxAngle {
    return self.maximumTextAngle;
}

- (void)setMaximumTextWidth:(NSExpression *)maximumTextWidth {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting maximumTextWidth: %@", maximumTextWidth);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(maximumTextWidth, true);
    self.rawLayer->setTextMaxWidth(mbglValue);
}

- (NSExpression *)maximumTextWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextMaxWidth();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextMaxWidth();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setTextMaxWidth:(NSExpression *)textMaxWidth {
}

- (NSExpression *)textMaxWidth {
    return self.maximumTextWidth;
}

- (void)setSymbolAvoidsEdges:(NSExpression *)symbolAvoidsEdges {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting symbolAvoidsEdges: %@", symbolAvoidsEdges);

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(symbolAvoidsEdges, false);
    self.rawLayer->setSymbolAvoidEdges(mbglValue);
}

- (NSExpression *)symbolAvoidsEdges {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getSymbolAvoidEdges();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultSymbolAvoidEdges();
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toExpression(propertyValue);
}

- (void)setSymbolAvoidEdges:(NSExpression *)symbolAvoidEdges {
}

- (NSExpression *)symbolAvoidEdges {
    return self.symbolAvoidsEdges;
}

- (void)setSymbolPlacement:(NSExpression *)symbolPlacement {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting symbolPlacement: %@", symbolPlacement);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::SymbolPlacementType, NSValue *, mbgl::style::SymbolPlacementType, MGLSymbolPlacement>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::SymbolPlacementType>>(symbolPlacement, false);
    self.rawLayer->setSymbolPlacement(mbglValue);
}

- (NSExpression *)symbolPlacement {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getSymbolPlacement();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultSymbolPlacement();
    }
    return MGLStyleValueTransformer<mbgl::style::SymbolPlacementType, NSValue *, mbgl::style::SymbolPlacementType, MGLSymbolPlacement>().toExpression(propertyValue);
}

- (void)setSymbolSortKey:(NSExpression *)symbolSortKey {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting symbolSortKey: %@", symbolSortKey);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(symbolSortKey, true);
    self.rawLayer->setSymbolSortKey(mbglValue);
}

- (NSExpression *)symbolSortKey {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getSymbolSortKey();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultSymbolSortKey();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setSymbolSpacing:(NSExpression *)symbolSpacing {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting symbolSpacing: %@", symbolSpacing);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(symbolSpacing, false);
    self.rawLayer->setSymbolSpacing(mbglValue);
}

- (NSExpression *)symbolSpacing {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getSymbolSpacing();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultSymbolSpacing();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setSymbolZOrder:(NSExpression *)symbolZOrder {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting symbolZOrder: %@", symbolZOrder);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::SymbolZOrderType, NSValue *, mbgl::style::SymbolZOrderType, MGLSymbolZOrder>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::SymbolZOrderType>>(symbolZOrder, false);
    self.rawLayer->setSymbolZOrder(mbglValue);
}

- (NSExpression *)symbolZOrder {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getSymbolZOrder();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultSymbolZOrder();
    }
    return MGLStyleValueTransformer<mbgl::style::SymbolZOrderType, NSValue *, mbgl::style::SymbolZOrderType, MGLSymbolZOrder>().toExpression(propertyValue);
}

- (void)setText:(NSExpression *)text {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting text: %@", text);

    if (text && text.expressionType == NSConstantValueExpressionType) {
        std::string string = ((NSString *)text.constantValue).UTF8String;
        if (mbgl::style::conversion::hasTokens(string)) {
            self.rawLayer->setTextField(mbgl::style::PropertyValue<mbgl::style::expression::Formatted>(
                mbgl::style::conversion::convertTokenStringToFormatExpression(string)));
            return;
        }
    }
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::expression::Formatted, NSString *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::expression::Formatted>>(text, true);
    self.rawLayer->setTextField(mbglValue);
}

- (NSExpression *)text {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextField();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextField();
    }
    return MGLStyleValueTransformer<mbgl::style::expression::Formatted, NSString *>().toExpression(propertyValue);
}

- (void)setTextField:(NSExpression *)textField {
}

- (NSExpression *)textField {
    return self.text;
}

- (void)setTextAllowsOverlap:(NSExpression *)textAllowsOverlap {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textAllowsOverlap: %@", textAllowsOverlap);

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(textAllowsOverlap, false);
    self.rawLayer->setTextAllowOverlap(mbglValue);
}

- (NSExpression *)textAllowsOverlap {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextAllowOverlap();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextAllowOverlap();
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toExpression(propertyValue);
}

- (void)setTextAllowOverlap:(NSExpression *)textAllowOverlap {
}

- (NSExpression *)textAllowOverlap {
    return self.textAllowsOverlap;
}

- (void)setTextAnchor:(NSExpression *)textAnchor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textAnchor: %@", textAnchor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::SymbolAnchorType, NSValue *, mbgl::style::SymbolAnchorType, MGLTextAnchor>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::SymbolAnchorType>>(textAnchor, true);
    self.rawLayer->setTextAnchor(mbglValue);
}

- (NSExpression *)textAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextAnchor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextAnchor();
    }
    return MGLStyleValueTransformer<mbgl::style::SymbolAnchorType, NSValue *, mbgl::style::SymbolAnchorType, MGLTextAnchor>().toExpression(propertyValue);
}

- (void)setTextFontNames:(NSExpression *)textFontNames {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textFontNames: %@", textFontNames);

    auto mbglValue = MGLStyleValueTransformer<std::vector<std::string>, NSArray<NSString *> *, std::string>().toPropertyValue<mbgl::style::PropertyValue<std::vector<std::string>>>(textFontNames, true);
    self.rawLayer->setTextFont(mbglValue);
}

- (NSExpression *)textFontNames {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextFont();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextFont();
    }
    return MGLStyleValueTransformer<std::vector<std::string>, NSArray<NSString *> *, std::string>().toExpression(propertyValue);
}

- (void)setTextFont:(NSExpression *)textFont {
}

- (NSExpression *)textFont {
    return self.textFontNames;
}

- (void)setTextFontSize:(NSExpression *)textFontSize {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textFontSize: %@", textFontSize);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(textFontSize, true);
    self.rawLayer->setTextSize(mbglValue);
}

- (NSExpression *)textFontSize {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextSize();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextSize();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setTextSize:(NSExpression *)textSize {
}

- (NSExpression *)textSize {
    return self.textFontSize;
}

- (void)setTextIgnoresPlacement:(NSExpression *)textIgnoresPlacement {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textIgnoresPlacement: %@", textIgnoresPlacement);

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(textIgnoresPlacement, false);
    self.rawLayer->setTextIgnorePlacement(mbglValue);
}

- (NSExpression *)textIgnoresPlacement {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextIgnorePlacement();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextIgnorePlacement();
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toExpression(propertyValue);
}

- (void)setTextIgnorePlacement:(NSExpression *)textIgnorePlacement {
}

- (NSExpression *)textIgnorePlacement {
    return self.textIgnoresPlacement;
}

- (void)setTextJustification:(NSExpression *)textJustification {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textJustification: %@", textJustification);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TextJustifyType, NSValue *, mbgl::style::TextJustifyType, MGLTextJustification>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::TextJustifyType>>(textJustification, true);
    self.rawLayer->setTextJustify(mbglValue);
}

- (NSExpression *)textJustification {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextJustify();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextJustify();
    }
    return MGLStyleValueTransformer<mbgl::style::TextJustifyType, NSValue *, mbgl::style::TextJustifyType, MGLTextJustification>().toExpression(propertyValue);
}

- (void)setTextJustify:(NSExpression *)textJustify {
}

- (NSExpression *)textJustify {
    return self.textJustification;
}

- (void)setTextLetterSpacing:(NSExpression *)textLetterSpacing {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textLetterSpacing: %@", textLetterSpacing);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(textLetterSpacing, true);
    self.rawLayer->setTextLetterSpacing(mbglValue);
}

- (NSExpression *)textLetterSpacing {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextLetterSpacing();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextLetterSpacing();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setTextLineHeight:(NSExpression *)textLineHeight {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textLineHeight: %@", textLineHeight);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(textLineHeight, false);
    self.rawLayer->setTextLineHeight(mbglValue);
}

- (NSExpression *)textLineHeight {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextLineHeight();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextLineHeight();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setTextOffset:(NSExpression *)textOffset {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textOffset: %@", textOffset);

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue<mbgl::style::PropertyValue<std::array<float, 2>>>(textOffset, true);
    self.rawLayer->setTextOffset(mbglValue);
}

- (NSExpression *)textOffset {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextOffset();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextOffset();
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toExpression(propertyValue);
}

- (void)setTextOptional:(NSExpression *)textOptional {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textOptional: %@", textOptional);

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(textOptional, false);
    self.rawLayer->setTextOptional(mbglValue);
}

- (NSExpression *)isTextOptional {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextOptional();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextOptional();
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toExpression(propertyValue);
}

- (void)setTextPadding:(NSExpression *)textPadding {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textPadding: %@", textPadding);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(textPadding, false);
    self.rawLayer->setTextPadding(mbglValue);
}

- (NSExpression *)textPadding {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextPadding();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextPadding();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setTextPitchAlignment:(NSExpression *)textPitchAlignment {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textPitchAlignment: %@", textPitchAlignment);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLTextPitchAlignment>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::AlignmentType>>(textPitchAlignment, false);
    self.rawLayer->setTextPitchAlignment(mbglValue);
}

- (NSExpression *)textPitchAlignment {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextPitchAlignment();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextPitchAlignment();
    }
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLTextPitchAlignment>().toExpression(propertyValue);
}

- (void)setTextRadialOffset:(NSExpression *)textRadialOffset {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textRadialOffset: %@", textRadialOffset);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(textRadialOffset, true);
    self.rawLayer->setTextRadialOffset(mbglValue);
}

- (NSExpression *)textRadialOffset {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextRadialOffset();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextRadialOffset();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setTextRotation:(NSExpression *)textRotation {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textRotation: %@", textRotation);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(textRotation, true);
    self.rawLayer->setTextRotate(mbglValue);
}

- (NSExpression *)textRotation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextRotate();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextRotate();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setTextRotate:(NSExpression *)textRotate {
}

- (NSExpression *)textRotate {
    return self.textRotation;
}

- (void)setTextRotationAlignment:(NSExpression *)textRotationAlignment {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textRotationAlignment: %@", textRotationAlignment);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLTextRotationAlignment>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::AlignmentType>>(textRotationAlignment, false);
    self.rawLayer->setTextRotationAlignment(mbglValue);
}

- (NSExpression *)textRotationAlignment {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextRotationAlignment();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextRotationAlignment();
    }
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLTextRotationAlignment>().toExpression(propertyValue);
}

- (void)setTextTransform:(NSExpression *)textTransform {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textTransform: %@", textTransform);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TextTransformType, NSValue *, mbgl::style::TextTransformType, MGLTextTransform>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::TextTransformType>>(textTransform, true);
    self.rawLayer->setTextTransform(mbglValue);
}

- (NSExpression *)textTransform {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextTransform();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextTransform();
    }
    return MGLStyleValueTransformer<mbgl::style::TextTransformType, NSValue *, mbgl::style::TextTransformType, MGLTextTransform>().toExpression(propertyValue);
}

- (void)setTextVariableAnchor:(NSExpression *)textVariableAnchor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textVariableAnchor: %@", textVariableAnchor);

    auto mbglValue = MGLStyleValueTransformer<std::vector<mbgl::style::SymbolAnchorType>, NSArray<NSValue *> *, mbgl::style::SymbolAnchorType, MGLTextAnchor>().toPropertyValue<mbgl::style::PropertyValue<std::vector<mbgl::style::SymbolAnchorType>>>(textVariableAnchor, false);
    self.rawLayer->setTextVariableAnchor(mbglValue);
}

- (NSExpression *)textVariableAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextVariableAnchor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextVariableAnchor();
    }
    return MGLStyleValueTransformer<std::vector<mbgl::style::SymbolAnchorType>, NSArray<NSValue *> *, mbgl::style::SymbolAnchorType, MGLTextAnchor>().toExpression(propertyValue);
}

- (void)setTextWritingModes:(NSExpression *)textWritingModes {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textWritingModes: %@", textWritingModes);

    auto mbglValue = MGLStyleValueTransformer<std::vector<mbgl::style::TextWritingModeType>, NSArray<NSValue *> *, mbgl::style::TextWritingModeType, MGLTextWritingModes>().toPropertyValue<mbgl::style::PropertyValue<std::vector<mbgl::style::TextWritingModeType>>>(textWritingModes, false);
    self.rawLayer->setTextWritingMode(mbglValue);
}

- (NSExpression *)textWritingModes {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextWritingMode();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextWritingMode();
    }
    return MGLStyleValueTransformer<std::vector<mbgl::style::TextWritingModeType>, NSArray<NSValue *> *, mbgl::style::TextWritingModeType, MGLTextWritingModes>().toExpression(propertyValue);
}

- (void)setTextWritingMode:(NSExpression *)textWritingMode {
}

- (NSExpression *)textWritingMode {
    return self.textWritingModes;
}

#pragma mark - Accessing the Paint Attributes

- (void)setIconColor:(NSExpression *)iconColor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconColor: %@", iconColor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(iconColor, true);
    self.rawLayer->setIconColor(mbglValue);
}

- (NSExpression *)iconColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconColor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconColor();
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(propertyValue);
}

- (void)setIconColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconColorTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setIconColorTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)iconColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getIconColorTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setIconHaloBlur:(NSExpression *)iconHaloBlur {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconHaloBlur: %@", iconHaloBlur);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(iconHaloBlur, true);
    self.rawLayer->setIconHaloBlur(mbglValue);
}

- (NSExpression *)iconHaloBlur {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconHaloBlur();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconHaloBlur();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setIconHaloBlurTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconHaloBlurTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setIconHaloBlurTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)iconHaloBlurTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getIconHaloBlurTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setIconHaloColor:(NSExpression *)iconHaloColor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconHaloColor: %@", iconHaloColor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(iconHaloColor, true);
    self.rawLayer->setIconHaloColor(mbglValue);
}

- (NSExpression *)iconHaloColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconHaloColor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconHaloColor();
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(propertyValue);
}

- (void)setIconHaloColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconHaloColorTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setIconHaloColorTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)iconHaloColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getIconHaloColorTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setIconHaloWidth:(NSExpression *)iconHaloWidth {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconHaloWidth: %@", iconHaloWidth);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(iconHaloWidth, true);
    self.rawLayer->setIconHaloWidth(mbglValue);
}

- (NSExpression *)iconHaloWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconHaloWidth();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconHaloWidth();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setIconHaloWidthTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconHaloWidthTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setIconHaloWidthTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)iconHaloWidthTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getIconHaloWidthTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setIconOpacity:(NSExpression *)iconOpacity {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconOpacity: %@", iconOpacity);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(iconOpacity, true);
    self.rawLayer->setIconOpacity(mbglValue);
}

- (NSExpression *)iconOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconOpacity();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconOpacity();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setIconOpacityTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconOpacityTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setIconOpacityTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)iconOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getIconOpacityTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setIconTranslation:(NSExpression *)iconTranslation {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconTranslation: %@", iconTranslation);

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue<mbgl::style::PropertyValue<std::array<float, 2>>>(iconTranslation, false);
    self.rawLayer->setIconTranslate(mbglValue);
}

- (NSExpression *)iconTranslation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconTranslate();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconTranslate();
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toExpression(propertyValue);
}

- (void)setIconTranslationTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconTranslationTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setIconTranslateTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)iconTranslationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getIconTranslateTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setIconTranslate:(NSExpression *)iconTranslate {
}

- (NSExpression *)iconTranslate {
    return self.iconTranslation;
}

- (void)setIconTranslationAnchor:(NSExpression *)iconTranslationAnchor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting iconTranslationAnchor: %@", iconTranslationAnchor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLIconTranslationAnchor>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType>>(iconTranslationAnchor, false);
    self.rawLayer->setIconTranslateAnchor(mbglValue);
}

- (NSExpression *)iconTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconTranslateAnchor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultIconTranslateAnchor();
    }
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLIconTranslationAnchor>().toExpression(propertyValue);
}

- (void)setIconTranslateAnchor:(NSExpression *)iconTranslateAnchor {
}

- (NSExpression *)iconTranslateAnchor {
    return self.iconTranslationAnchor;
}

- (void)setTextColor:(NSExpression *)textColor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textColor: %@", textColor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(textColor, true);
    self.rawLayer->setTextColor(mbglValue);
}

- (NSExpression *)textColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextColor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextColor();
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(propertyValue);
}

- (void)setTextColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textColorTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setTextColorTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)textColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getTextColorTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setTextHaloBlur:(NSExpression *)textHaloBlur {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textHaloBlur: %@", textHaloBlur);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(textHaloBlur, true);
    self.rawLayer->setTextHaloBlur(mbglValue);
}

- (NSExpression *)textHaloBlur {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextHaloBlur();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextHaloBlur();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setTextHaloBlurTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textHaloBlurTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setTextHaloBlurTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)textHaloBlurTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getTextHaloBlurTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setTextHaloColor:(NSExpression *)textHaloColor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textHaloColor: %@", textHaloColor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::PropertyValue<mbgl::Color>>(textHaloColor, true);
    self.rawLayer->setTextHaloColor(mbglValue);
}

- (NSExpression *)textHaloColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextHaloColor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextHaloColor();
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(propertyValue);
}

- (void)setTextHaloColorTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textHaloColorTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setTextHaloColorTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)textHaloColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getTextHaloColorTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setTextHaloWidth:(NSExpression *)textHaloWidth {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textHaloWidth: %@", textHaloWidth);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(textHaloWidth, true);
    self.rawLayer->setTextHaloWidth(mbglValue);
}

- (NSExpression *)textHaloWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextHaloWidth();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextHaloWidth();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setTextHaloWidthTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textHaloWidthTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setTextHaloWidthTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)textHaloWidthTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getTextHaloWidthTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setTextOpacity:(NSExpression *)textOpacity {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textOpacity: %@", textOpacity);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(textOpacity, true);
    self.rawLayer->setTextOpacity(mbglValue);
}

- (NSExpression *)textOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextOpacity();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextOpacity();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setTextOpacityTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textOpacityTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setTextOpacityTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)textOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getTextOpacityTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setTextTranslation:(NSExpression *)textTranslation {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textTranslation: %@", textTranslation);

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue<mbgl::style::PropertyValue<std::array<float, 2>>>(textTranslation, false);
    self.rawLayer->setTextTranslate(mbglValue);
}

- (NSExpression *)textTranslation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextTranslate();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextTranslate();
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toExpression(propertyValue);
}

- (void)setTextTranslationTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textTranslationTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setTextTranslateTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)textTranslationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getTextTranslateTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setTextTranslate:(NSExpression *)textTranslate {
}

- (NSExpression *)textTranslate {
    return self.textTranslation;
}

- (void)setTextTranslationAnchor:(NSExpression *)textTranslationAnchor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting textTranslationAnchor: %@", textTranslationAnchor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLTextTranslationAnchor>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType>>(textTranslationAnchor, false);
    self.rawLayer->setTextTranslateAnchor(mbglValue);
}

- (NSExpression *)textTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextTranslateAnchor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextTranslateAnchor();
    }
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLTextTranslationAnchor>().toExpression(propertyValue);
}

- (void)setTextTranslateAnchor:(NSExpression *)textTranslateAnchor {
}

- (NSExpression *)textTranslateAnchor {
    return self.textTranslationAnchor;
}

@end

@implementation NSValue (MGLSymbolStyleLayerAdditions)

+ (NSValue *)valueWithMGLIconAnchor:(MGLIconAnchor)iconAnchor {
    return [NSValue value:&iconAnchor withObjCType:@encode(MGLIconAnchor)];
}

- (MGLIconAnchor)MGLIconAnchorValue {
    MGLIconAnchor iconAnchor;
    [self getValue:&iconAnchor];
    return iconAnchor;
}

+ (NSValue *)valueWithMGLIconPitchAlignment:(MGLIconPitchAlignment)iconPitchAlignment {
    return [NSValue value:&iconPitchAlignment withObjCType:@encode(MGLIconPitchAlignment)];
}

- (MGLIconPitchAlignment)MGLIconPitchAlignmentValue {
    MGLIconPitchAlignment iconPitchAlignment;
    [self getValue:&iconPitchAlignment];
    return iconPitchAlignment;
}

+ (NSValue *)valueWithMGLIconRotationAlignment:(MGLIconRotationAlignment)iconRotationAlignment {
    return [NSValue value:&iconRotationAlignment withObjCType:@encode(MGLIconRotationAlignment)];
}

- (MGLIconRotationAlignment)MGLIconRotationAlignmentValue {
    MGLIconRotationAlignment iconRotationAlignment;
    [self getValue:&iconRotationAlignment];
    return iconRotationAlignment;
}

+ (NSValue *)valueWithMGLIconTextFit:(MGLIconTextFit)iconTextFit {
    return [NSValue value:&iconTextFit withObjCType:@encode(MGLIconTextFit)];
}

- (MGLIconTextFit)MGLIconTextFitValue {
    MGLIconTextFit iconTextFit;
    [self getValue:&iconTextFit];
    return iconTextFit;
}

+ (NSValue *)valueWithMGLSymbolPlacement:(MGLSymbolPlacement)symbolPlacement {
    return [NSValue value:&symbolPlacement withObjCType:@encode(MGLSymbolPlacement)];
}

- (MGLSymbolPlacement)MGLSymbolPlacementValue {
    MGLSymbolPlacement symbolPlacement;
    [self getValue:&symbolPlacement];
    return symbolPlacement;
}

+ (NSValue *)valueWithMGLSymbolZOrder:(MGLSymbolZOrder)symbolZOrder {
    return [NSValue value:&symbolZOrder withObjCType:@encode(MGLSymbolZOrder)];
}

- (MGLSymbolZOrder)MGLSymbolZOrderValue {
    MGLSymbolZOrder symbolZOrder;
    [self getValue:&symbolZOrder];
    return symbolZOrder;
}

+ (NSValue *)valueWithMGLTextAnchor:(MGLTextAnchor)textAnchor {
    return [NSValue value:&textAnchor withObjCType:@encode(MGLTextAnchor)];
}

- (MGLTextAnchor)MGLTextAnchorValue {
    MGLTextAnchor textAnchor;
    [self getValue:&textAnchor];
    return textAnchor;
}

+ (NSValue *)valueWithMGLTextJustification:(MGLTextJustification)textJustification {
    return [NSValue value:&textJustification withObjCType:@encode(MGLTextJustification)];
}

- (MGLTextJustification)MGLTextJustificationValue {
    MGLTextJustification textJustification;
    [self getValue:&textJustification];
    return textJustification;
}

+ (NSValue *)valueWithMGLTextPitchAlignment:(MGLTextPitchAlignment)textPitchAlignment {
    return [NSValue value:&textPitchAlignment withObjCType:@encode(MGLTextPitchAlignment)];
}

- (MGLTextPitchAlignment)MGLTextPitchAlignmentValue {
    MGLTextPitchAlignment textPitchAlignment;
    [self getValue:&textPitchAlignment];
    return textPitchAlignment;
}

+ (NSValue *)valueWithMGLTextRotationAlignment:(MGLTextRotationAlignment)textRotationAlignment {
    return [NSValue value:&textRotationAlignment withObjCType:@encode(MGLTextRotationAlignment)];
}

- (MGLTextRotationAlignment)MGLTextRotationAlignmentValue {
    MGLTextRotationAlignment textRotationAlignment;
    [self getValue:&textRotationAlignment];
    return textRotationAlignment;
}

+ (NSValue *)valueWithMGLTextTransform:(MGLTextTransform)textTransform {
    return [NSValue value:&textTransform withObjCType:@encode(MGLTextTransform)];
}

- (MGLTextTransform)MGLTextTransformValue {
    MGLTextTransform textTransform;
    [self getValue:&textTransform];
    return textTransform;
}

+ (NSValue *)valueWithMGLTextWritingModes:(MGLTextWritingModes)textWritingModes {
    return [NSValue value:&textWritingModes withObjCType:@encode(MGLTextWritingModes)];
}

- (MGLTextWritingModes)MGLTextWritingModesValue {
    MGLTextWritingModes textWritingModes;
    [self getValue:&textWritingModes];
    return textWritingModes;
}

+ (NSValue *)valueWithMGLIconTranslationAnchor:(MGLIconTranslationAnchor)iconTranslationAnchor {
    return [NSValue value:&iconTranslationAnchor withObjCType:@encode(MGLIconTranslationAnchor)];
}

- (MGLIconTranslationAnchor)MGLIconTranslationAnchorValue {
    MGLIconTranslationAnchor iconTranslationAnchor;
    [self getValue:&iconTranslationAnchor];
    return iconTranslationAnchor;
}

+ (NSValue *)valueWithMGLTextTranslationAnchor:(MGLTextTranslationAnchor)textTranslationAnchor {
    return [NSValue value:&textTranslationAnchor withObjCType:@encode(MGLTextTranslationAnchor)];
}

- (MGLTextTranslationAnchor)MGLTextTranslationAnchorValue {
    MGLTextTranslationAnchor textTranslationAnchor;
    [self getValue:&textTranslationAnchor];
    return textTranslationAnchor;
}

@end

namespace mbgl {

MGLStyleLayer* SymbolStyleLayerPeerFactory::createPeer(style::Layer* rawLayer) {
    return [[MGLSymbolStyleLayer alloc] initWithRawLayer:rawLayer];
}

}  // namespace mbgl
