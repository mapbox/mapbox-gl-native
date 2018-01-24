// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLSymbolStyleLayer.h"

#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>

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

#pragma mark - Accessing the Layout Attributes

- (void)setIconAllowsOverlap:(NSExpression *)iconAllowsOverlap {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(iconAllowsOverlap);
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

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::SymbolAnchorType, NSValue *, mbgl::style::SymbolAnchorType, MGLIconAnchor>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<mbgl::style::SymbolAnchorType>>(iconAnchor);
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

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(iconIgnoresPlacement);
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

    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<std::string>>(iconImageName);
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

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<std::array<float, 2>>>(iconOffset);
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

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(iconOptional);
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

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(iconPadding);
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

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLIconPitchAlignment>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::AlignmentType>>(iconPitchAlignment);
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

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(iconRotation);
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

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLIconRotationAlignment>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::AlignmentType>>(iconRotationAlignment);
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

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(iconScale);
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

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::IconTextFitType, NSValue *, mbgl::style::IconTextFitType, MGLIconTextFit>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::IconTextFitType>>(iconTextFit);
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

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 4>, NSValue *>().toPropertyValue<mbgl::style::PropertyValue<std::array<float, 4>>>(iconTextFitPadding);
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

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(keepsIconUpright);
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

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(keepsTextUpright);
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

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(maximumTextAngle);
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

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(maximumTextWidth);
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

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(symbolAvoidsEdges);
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

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::SymbolPlacementType, NSValue *, mbgl::style::SymbolPlacementType, MGLSymbolPlacement>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::SymbolPlacementType>>(symbolPlacement);
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

- (void)setSymbolSpacing:(NSExpression *)symbolSpacing {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(symbolSpacing);
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

- (void)setText:(NSExpression *)text {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<std::string>>(text);
    self.rawLayer->setTextField(mbglValue);
}

- (NSExpression *)text {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextField();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultTextField();
    }
    return MGLStyleValueTransformer<std::string, NSString *>().toExpression(propertyValue);
}

- (void)setTextField:(NSExpression *)textField {
}

- (NSExpression *)textField {
    return self.text;
}

- (void)setTextAllowsOverlap:(NSExpression *)textAllowsOverlap {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(textAllowsOverlap);
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

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::SymbolAnchorType, NSValue *, mbgl::style::SymbolAnchorType, MGLTextAnchor>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<mbgl::style::SymbolAnchorType>>(textAnchor);
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

    auto mbglValue = MGLStyleValueTransformer<std::vector<std::string>, NSArray<NSString *> *, std::string>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<std::vector<std::string>>>(textFontNames);
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

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(textFontSize);
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

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(textIgnoresPlacement);
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

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TextJustifyType, NSValue *, mbgl::style::TextJustifyType, MGLTextJustification>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<mbgl::style::TextJustifyType>>(textJustification);
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

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(textLetterSpacing);
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

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(textLineHeight);
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

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<std::array<float, 2>>>(textOffset);
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

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<bool>>(textOptional);
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

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(textPadding);
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

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLTextPitchAlignment>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::AlignmentType>>(textPitchAlignment);
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

- (void)setTextRotation:(NSExpression *)textRotation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(textRotation);
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

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLTextRotationAlignment>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::AlignmentType>>(textRotationAlignment);
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

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TextTransformType, NSValue *, mbgl::style::TextTransformType, MGLTextTransform>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<mbgl::style::TextTransformType>>(textTransform);
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

#pragma mark - Accessing the Paint Attributes

- (void)setIconColor:(NSExpression *)iconColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<mbgl::Color>>(iconColor);
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

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setIconColorTransition(options);
}

- (MGLTransition)iconColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getIconColorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setIconHaloBlur:(NSExpression *)iconHaloBlur {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(iconHaloBlur);
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

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setIconHaloBlurTransition(options);
}

- (MGLTransition)iconHaloBlurTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getIconHaloBlurTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setIconHaloColor:(NSExpression *)iconHaloColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<mbgl::Color>>(iconHaloColor);
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

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setIconHaloColorTransition(options);
}

- (MGLTransition)iconHaloColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getIconHaloColorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setIconHaloWidth:(NSExpression *)iconHaloWidth {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(iconHaloWidth);
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

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setIconHaloWidthTransition(options);
}

- (MGLTransition)iconHaloWidthTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getIconHaloWidthTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setIconOpacity:(NSExpression *)iconOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(iconOpacity);
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

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setIconOpacityTransition(options);
}

- (MGLTransition)iconOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getIconOpacityTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setIconTranslation:(NSExpression *)iconTranslation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue<mbgl::style::PropertyValue<std::array<float, 2>>>(iconTranslation);
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

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setIconTranslateTransition(options);
}

- (MGLTransition)iconTranslationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getIconTranslateTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setIconTranslate:(NSExpression *)iconTranslate {
}

- (NSExpression *)iconTranslate {
    return self.iconTranslation;
}

- (void)setIconTranslationAnchor:(NSExpression *)iconTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLIconTranslationAnchor>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType>>(iconTranslationAnchor);
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

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<mbgl::Color>>(textColor);
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

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setTextColorTransition(options);
}

- (MGLTransition)textColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getTextColorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setTextHaloBlur:(NSExpression *)textHaloBlur {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(textHaloBlur);
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

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setTextHaloBlurTransition(options);
}

- (MGLTransition)textHaloBlurTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getTextHaloBlurTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setTextHaloColor:(NSExpression *)textHaloColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<mbgl::Color>>(textHaloColor);
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

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setTextHaloColorTransition(options);
}

- (MGLTransition)textHaloColorTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getTextHaloColorTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setTextHaloWidth:(NSExpression *)textHaloWidth {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(textHaloWidth);
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

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setTextHaloWidthTransition(options);
}

- (MGLTransition)textHaloWidthTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getTextHaloWidthTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setTextOpacity:(NSExpression *)textOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(textOpacity);
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

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setTextOpacityTransition(options);
}

- (MGLTransition)textOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getTextOpacityTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setTextTranslation:(NSExpression *)textTranslation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue<mbgl::style::PropertyValue<std::array<float, 2>>>(textTranslation);
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

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setTextTranslateTransition(options);
}

- (MGLTransition)textTranslationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getTextTranslateTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setTextTranslate:(NSExpression *)textTranslate {
}

- (NSExpression *)textTranslate {
    return self.textTranslation;
}

- (void)setTextTranslationAnchor:(NSExpression *)textTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLTextTranslationAnchor>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType>>(textTranslationAnchor);
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
