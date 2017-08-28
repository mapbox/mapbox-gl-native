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

- (void)setIconAllowsOverlap:(MGLStyleValue<NSNumber *> *)iconAllowsOverlap {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(iconAllowsOverlap);
    self.rawLayer->setIconAllowOverlap(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconAllowsOverlap {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconAllowOverlap();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(self.rawLayer->getDefaultIconAllowOverlap());
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconAllowOverlap:(MGLStyleValue<NSNumber *> *)iconAllowOverlap {
}

- (MGLStyleValue<NSNumber *> *)iconAllowOverlap {
    return self.iconAllowsOverlap;
}

- (void)setIconAnchor:(MGLStyleValue<NSValue *> *)iconAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::SymbolAnchorType, NSValue *, mbgl::style::SymbolAnchorType, MGLIconAnchor>().toDataDrivenPropertyValue(iconAnchor);
    self.rawLayer->setIconAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconAnchor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::SymbolAnchorType, NSValue *, mbgl::style::SymbolAnchorType, MGLIconAnchor>().toDataDrivenStyleValue(self.rawLayer->getDefaultIconAnchor());
    }
    return MGLStyleValueTransformer<mbgl::style::SymbolAnchorType, NSValue *, mbgl::style::SymbolAnchorType, MGLIconAnchor>().toDataDrivenStyleValue(propertyValue);
}

- (void)setIconIgnoresPlacement:(MGLStyleValue<NSNumber *> *)iconIgnoresPlacement {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(iconIgnoresPlacement);
    self.rawLayer->setIconIgnorePlacement(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconIgnoresPlacement {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconIgnorePlacement();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(self.rawLayer->getDefaultIconIgnorePlacement());
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconIgnorePlacement:(MGLStyleValue<NSNumber *> *)iconIgnorePlacement {
}

- (MGLStyleValue<NSNumber *> *)iconIgnorePlacement {
    return self.iconIgnoresPlacement;
}

- (void)setIconImageName:(MGLStyleValue<NSString *> *)iconImageName {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toDataDrivenPropertyValue(iconImageName);
    self.rawLayer->setIconImage(mbglValue);
}

- (MGLStyleValue<NSString *> *)iconImageName {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconImage();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::string, NSString *>().toDataDrivenStyleValue(self.rawLayer->getDefaultIconImage());
    }
    return MGLStyleValueTransformer<std::string, NSString *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setIconImage:(MGLStyleValue<NSString *> *)iconImage {
}

- (MGLStyleValue<NSString *> *)iconImage {
    return self.iconImageName;
}

- (void)setIconOffset:(MGLStyleValue<NSValue *> *)iconOffset {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toDataDrivenPropertyValue(iconOffset);
    self.rawLayer->setIconOffset(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconOffset {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconOffset();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toDataDrivenStyleValue(self.rawLayer->getDefaultIconOffset());
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setIconOptional:(MGLStyleValue<NSNumber *> *)iconOptional {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(iconOptional);
    self.rawLayer->setIconOptional(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)isIconOptional {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconOptional();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(self.rawLayer->getDefaultIconOptional());
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconPadding:(MGLStyleValue<NSNumber *> *)iconPadding {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(iconPadding);
    self.rawLayer->setIconPadding(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconPadding {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconPadding();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultIconPadding());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconPitchAlignment:(MGLStyleValue<NSValue *> *)iconPitchAlignment {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLIconPitchAlignment>().toEnumPropertyValue(iconPitchAlignment);
    self.rawLayer->setIconPitchAlignment(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconPitchAlignment {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconPitchAlignment();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLIconPitchAlignment>().toEnumStyleValue(self.rawLayer->getDefaultIconPitchAlignment());
    }
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLIconPitchAlignment>().toEnumStyleValue(propertyValue);
}

- (void)setIconRotation:(MGLStyleValue<NSNumber *> *)iconRotation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(iconRotation);
    self.rawLayer->setIconRotate(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconRotation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconRotate();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultIconRotate());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setIconRotate:(MGLStyleValue<NSNumber *> *)iconRotate {
}

- (MGLStyleValue<NSNumber *> *)iconRotate {
    return self.iconRotation;
}

- (void)setIconRotationAlignment:(MGLStyleValue<NSValue *> *)iconRotationAlignment {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLIconRotationAlignment>().toEnumPropertyValue(iconRotationAlignment);
    self.rawLayer->setIconRotationAlignment(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconRotationAlignment {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconRotationAlignment();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLIconRotationAlignment>().toEnumStyleValue(self.rawLayer->getDefaultIconRotationAlignment());
    }
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLIconRotationAlignment>().toEnumStyleValue(propertyValue);
}

- (void)setIconScale:(MGLStyleValue<NSNumber *> *)iconScale {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(iconScale);
    self.rawLayer->setIconSize(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconScale {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconSize();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultIconSize());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setIconSize:(MGLStyleValue<NSNumber *> *)iconSize {
}

- (MGLStyleValue<NSNumber *> *)iconSize {
    return self.iconScale;
}

- (void)setIconTextFit:(MGLStyleValue<NSValue *> *)iconTextFit {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::IconTextFitType, NSValue *, mbgl::style::IconTextFitType, MGLIconTextFit>().toEnumPropertyValue(iconTextFit);
    self.rawLayer->setIconTextFit(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconTextFit {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconTextFit();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::IconTextFitType, NSValue *, mbgl::style::IconTextFitType, MGLIconTextFit>().toEnumStyleValue(self.rawLayer->getDefaultIconTextFit());
    }
    return MGLStyleValueTransformer<mbgl::style::IconTextFitType, NSValue *, mbgl::style::IconTextFitType, MGLIconTextFit>().toEnumStyleValue(propertyValue);
}

- (void)setIconTextFitPadding:(MGLStyleValue<NSValue *> *)iconTextFitPadding {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 4>, NSValue *>().toInterpolatablePropertyValue(iconTextFitPadding);
    self.rawLayer->setIconTextFitPadding(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconTextFitPadding {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconTextFitPadding();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::array<float, 4>, NSValue *>().toStyleValue(self.rawLayer->getDefaultIconTextFitPadding());
    }
    return MGLStyleValueTransformer<std::array<float, 4>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setKeepsIconUpright:(MGLStyleValue<NSNumber *> *)keepsIconUpright {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(keepsIconUpright);
    self.rawLayer->setIconKeepUpright(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)keepsIconUpright {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconKeepUpright();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(self.rawLayer->getDefaultIconKeepUpright());
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconKeepUpright:(MGLStyleValue<NSNumber *> *)iconKeepUpright {
}

- (MGLStyleValue<NSNumber *> *)iconKeepUpright {
    return self.keepsIconUpright;
}

- (void)setKeepsTextUpright:(MGLStyleValue<NSNumber *> *)keepsTextUpright {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(keepsTextUpright);
    self.rawLayer->setTextKeepUpright(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)keepsTextUpright {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextKeepUpright();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(self.rawLayer->getDefaultTextKeepUpright());
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextKeepUpright:(MGLStyleValue<NSNumber *> *)textKeepUpright {
}

- (MGLStyleValue<NSNumber *> *)textKeepUpright {
    return self.keepsTextUpright;
}

- (void)setMaximumTextAngle:(MGLStyleValue<NSNumber *> *)maximumTextAngle {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(maximumTextAngle);
    self.rawLayer->setTextMaxAngle(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)maximumTextAngle {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextMaxAngle();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultTextMaxAngle());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextMaxAngle:(MGLStyleValue<NSNumber *> *)textMaxAngle {
}

- (MGLStyleValue<NSNumber *> *)textMaxAngle {
    return self.maximumTextAngle;
}

- (void)setMaximumTextWidth:(MGLStyleValue<NSNumber *> *)maximumTextWidth {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(maximumTextWidth);
    self.rawLayer->setTextMaxWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)maximumTextWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextMaxWidth();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultTextMaxWidth());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextMaxWidth:(MGLStyleValue<NSNumber *> *)textMaxWidth {
}

- (MGLStyleValue<NSNumber *> *)textMaxWidth {
    return self.maximumTextWidth;
}

- (void)setSymbolAvoidsEdges:(MGLStyleValue<NSNumber *> *)symbolAvoidsEdges {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(symbolAvoidsEdges);
    self.rawLayer->setSymbolAvoidEdges(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)symbolAvoidsEdges {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getSymbolAvoidEdges();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(self.rawLayer->getDefaultSymbolAvoidEdges());
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setSymbolAvoidEdges:(MGLStyleValue<NSNumber *> *)symbolAvoidEdges {
}

- (MGLStyleValue<NSNumber *> *)symbolAvoidEdges {
    return self.symbolAvoidsEdges;
}

- (void)setSymbolPlacement:(MGLStyleValue<NSValue *> *)symbolPlacement {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::SymbolPlacementType, NSValue *, mbgl::style::SymbolPlacementType, MGLSymbolPlacement>().toEnumPropertyValue(symbolPlacement);
    self.rawLayer->setSymbolPlacement(mbglValue);
}

- (MGLStyleValue<NSValue *> *)symbolPlacement {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getSymbolPlacement();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::SymbolPlacementType, NSValue *, mbgl::style::SymbolPlacementType, MGLSymbolPlacement>().toEnumStyleValue(self.rawLayer->getDefaultSymbolPlacement());
    }
    return MGLStyleValueTransformer<mbgl::style::SymbolPlacementType, NSValue *, mbgl::style::SymbolPlacementType, MGLSymbolPlacement>().toEnumStyleValue(propertyValue);
}

- (void)setSymbolSpacing:(MGLStyleValue<NSNumber *> *)symbolSpacing {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(symbolSpacing);
    self.rawLayer->setSymbolSpacing(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)symbolSpacing {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getSymbolSpacing();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultSymbolSpacing());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setText:(MGLStyleValue<NSString *> *)text {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toDataDrivenPropertyValue(text);
    self.rawLayer->setTextField(mbglValue);
}

- (MGLStyleValue<NSString *> *)text {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextField();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::string, NSString *>().toDataDrivenStyleValue(self.rawLayer->getDefaultTextField());
    }
    return MGLStyleValueTransformer<std::string, NSString *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setTextField:(MGLStyleValue<NSString *> *)textField {
}

- (MGLStyleValue<NSString *> *)textField {
    return self.text;
}

- (void)setTextAllowsOverlap:(MGLStyleValue<NSNumber *> *)textAllowsOverlap {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(textAllowsOverlap);
    self.rawLayer->setTextAllowOverlap(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textAllowsOverlap {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextAllowOverlap();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(self.rawLayer->getDefaultTextAllowOverlap());
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextAllowOverlap:(MGLStyleValue<NSNumber *> *)textAllowOverlap {
}

- (MGLStyleValue<NSNumber *> *)textAllowOverlap {
    return self.textAllowsOverlap;
}

- (void)setTextAnchor:(MGLStyleValue<NSValue *> *)textAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::SymbolAnchorType, NSValue *, mbgl::style::SymbolAnchorType, MGLTextAnchor>().toDataDrivenPropertyValue(textAnchor);
    self.rawLayer->setTextAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextAnchor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::SymbolAnchorType, NSValue *, mbgl::style::SymbolAnchorType, MGLTextAnchor>().toDataDrivenStyleValue(self.rawLayer->getDefaultTextAnchor());
    }
    return MGLStyleValueTransformer<mbgl::style::SymbolAnchorType, NSValue *, mbgl::style::SymbolAnchorType, MGLTextAnchor>().toDataDrivenStyleValue(propertyValue);
}

- (void)setTextFontNames:(MGLStyleValue<NSArray<NSString *> *> *)textFontNames {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::vector<std::string>, NSArray<NSString *> *, std::string>().toPropertyValue(textFontNames);
    self.rawLayer->setTextFont(mbglValue);
}

- (MGLStyleValue<NSArray<NSString *> *> *)textFontNames {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextFont();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::vector<std::string>, NSArray<NSString *> *, std::string>().toStyleValue(self.rawLayer->getDefaultTextFont());
    }
    return MGLStyleValueTransformer<std::vector<std::string>, NSArray<NSString *> *, std::string>().toStyleValue(propertyValue);
}

- (void)setTextFont:(MGLStyleValue<NSArray<NSString *> *> *)textFont {
}

- (MGLStyleValue<NSArray<NSString *> *> *)textFont {
    return self.textFontNames;
}

- (void)setTextFontSize:(MGLStyleValue<NSNumber *> *)textFontSize {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(textFontSize);
    self.rawLayer->setTextSize(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textFontSize {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextSize();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultTextSize());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setTextSize:(MGLStyleValue<NSNumber *> *)textSize {
}

- (MGLStyleValue<NSNumber *> *)textSize {
    return self.textFontSize;
}

- (void)setTextIgnoresPlacement:(MGLStyleValue<NSNumber *> *)textIgnoresPlacement {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(textIgnoresPlacement);
    self.rawLayer->setTextIgnorePlacement(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textIgnoresPlacement {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextIgnorePlacement();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(self.rawLayer->getDefaultTextIgnorePlacement());
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextIgnorePlacement:(MGLStyleValue<NSNumber *> *)textIgnorePlacement {
}

- (MGLStyleValue<NSNumber *> *)textIgnorePlacement {
    return self.textIgnoresPlacement;
}

- (void)setTextJustification:(MGLStyleValue<NSValue *> *)textJustification {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TextJustifyType, NSValue *, mbgl::style::TextJustifyType, MGLTextJustification>().toDataDrivenPropertyValue(textJustification);
    self.rawLayer->setTextJustify(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textJustification {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextJustify();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::TextJustifyType, NSValue *, mbgl::style::TextJustifyType, MGLTextJustification>().toDataDrivenStyleValue(self.rawLayer->getDefaultTextJustify());
    }
    return MGLStyleValueTransformer<mbgl::style::TextJustifyType, NSValue *, mbgl::style::TextJustifyType, MGLTextJustification>().toDataDrivenStyleValue(propertyValue);
}

- (void)setTextJustify:(MGLStyleValue<NSValue *> *)textJustify {
}

- (MGLStyleValue<NSValue *> *)textJustify {
    return self.textJustification;
}

- (void)setTextLetterSpacing:(MGLStyleValue<NSNumber *> *)textLetterSpacing {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(textLetterSpacing);
    self.rawLayer->setTextLetterSpacing(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textLetterSpacing {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextLetterSpacing();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultTextLetterSpacing());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextLineHeight:(MGLStyleValue<NSNumber *> *)textLineHeight {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(textLineHeight);
    self.rawLayer->setTextLineHeight(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textLineHeight {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextLineHeight();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultTextLineHeight());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextOffset:(MGLStyleValue<NSValue *> *)textOffset {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toDataDrivenPropertyValue(textOffset);
    self.rawLayer->setTextOffset(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textOffset {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextOffset();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toDataDrivenStyleValue(self.rawLayer->getDefaultTextOffset());
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setTextOptional:(MGLStyleValue<NSNumber *> *)textOptional {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(textOptional);
    self.rawLayer->setTextOptional(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)isTextOptional {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextOptional();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(self.rawLayer->getDefaultTextOptional());
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextPadding:(MGLStyleValue<NSNumber *> *)textPadding {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(textPadding);
    self.rawLayer->setTextPadding(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textPadding {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextPadding();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultTextPadding());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextPitchAlignment:(MGLStyleValue<NSValue *> *)textPitchAlignment {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLTextPitchAlignment>().toEnumPropertyValue(textPitchAlignment);
    self.rawLayer->setTextPitchAlignment(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textPitchAlignment {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextPitchAlignment();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLTextPitchAlignment>().toEnumStyleValue(self.rawLayer->getDefaultTextPitchAlignment());
    }
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLTextPitchAlignment>().toEnumStyleValue(propertyValue);
}

- (void)setTextRotation:(MGLStyleValue<NSNumber *> *)textRotation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(textRotation);
    self.rawLayer->setTextRotate(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textRotation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextRotate();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultTextRotate());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setTextRotate:(MGLStyleValue<NSNumber *> *)textRotate {
}

- (MGLStyleValue<NSNumber *> *)textRotate {
    return self.textRotation;
}

- (void)setTextRotationAlignment:(MGLStyleValue<NSValue *> *)textRotationAlignment {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLTextRotationAlignment>().toEnumPropertyValue(textRotationAlignment);
    self.rawLayer->setTextRotationAlignment(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textRotationAlignment {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextRotationAlignment();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLTextRotationAlignment>().toEnumStyleValue(self.rawLayer->getDefaultTextRotationAlignment());
    }
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLTextRotationAlignment>().toEnumStyleValue(propertyValue);
}

- (void)setTextTransform:(MGLStyleValue<NSValue *> *)textTransform {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TextTransformType, NSValue *, mbgl::style::TextTransformType, MGLTextTransform>().toDataDrivenPropertyValue(textTransform);
    self.rawLayer->setTextTransform(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textTransform {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextTransform();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::TextTransformType, NSValue *, mbgl::style::TextTransformType, MGLTextTransform>().toDataDrivenStyleValue(self.rawLayer->getDefaultTextTransform());
    }
    return MGLStyleValueTransformer<mbgl::style::TextTransformType, NSValue *, mbgl::style::TextTransformType, MGLTextTransform>().toDataDrivenStyleValue(propertyValue);
}

#pragma mark - Accessing the Paint Attributes

- (void)setIconColor:(MGLStyleValue<MGLColor *> *)iconColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenPropertyValue(iconColor);
    self.rawLayer->setIconColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)iconColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(self.rawLayer->getDefaultIconColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(propertyValue);
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

- (void)setIconHaloBlur:(MGLStyleValue<NSNumber *> *)iconHaloBlur {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(iconHaloBlur);
    self.rawLayer->setIconHaloBlur(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconHaloBlur {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconHaloBlur();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultIconHaloBlur());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
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

- (void)setIconHaloColor:(MGLStyleValue<MGLColor *> *)iconHaloColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenPropertyValue(iconHaloColor);
    self.rawLayer->setIconHaloColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)iconHaloColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconHaloColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(self.rawLayer->getDefaultIconHaloColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(propertyValue);
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

- (void)setIconHaloWidth:(MGLStyleValue<NSNumber *> *)iconHaloWidth {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(iconHaloWidth);
    self.rawLayer->setIconHaloWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconHaloWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconHaloWidth();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultIconHaloWidth());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
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

- (void)setIconOpacity:(MGLStyleValue<NSNumber *> *)iconOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(iconOpacity);
    self.rawLayer->setIconOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconOpacity();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultIconOpacity());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
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

- (void)setIconTranslation:(MGLStyleValue<NSValue *> *)iconTranslation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toInterpolatablePropertyValue(iconTranslation);
    self.rawLayer->setIconTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconTranslation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconTranslate();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(self.rawLayer->getDefaultIconTranslate());
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
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

- (void)setIconTranslate:(MGLStyleValue<NSValue *> *)iconTranslate {
}

- (MGLStyleValue<NSValue *> *)iconTranslate {
    return self.iconTranslation;
}

- (void)setIconTranslationAnchor:(MGLStyleValue<NSValue *> *)iconTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLIconTranslationAnchor>().toEnumPropertyValue(iconTranslationAnchor);
    self.rawLayer->setIconTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getIconTranslateAnchor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLIconTranslationAnchor>().toEnumStyleValue(self.rawLayer->getDefaultIconTranslateAnchor());
    }
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLIconTranslationAnchor>().toEnumStyleValue(propertyValue);
}

- (void)setIconTranslateAnchor:(MGLStyleValue<NSValue *> *)iconTranslateAnchor {
}

- (MGLStyleValue<NSValue *> *)iconTranslateAnchor {
    return self.iconTranslationAnchor;
}

- (void)setTextColor:(MGLStyleValue<MGLColor *> *)textColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenPropertyValue(textColor);
    self.rawLayer->setTextColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)textColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(self.rawLayer->getDefaultTextColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(propertyValue);
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

- (void)setTextHaloBlur:(MGLStyleValue<NSNumber *> *)textHaloBlur {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(textHaloBlur);
    self.rawLayer->setTextHaloBlur(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textHaloBlur {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextHaloBlur();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultTextHaloBlur());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
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

- (void)setTextHaloColor:(MGLStyleValue<MGLColor *> *)textHaloColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenPropertyValue(textHaloColor);
    self.rawLayer->setTextHaloColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)textHaloColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextHaloColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(self.rawLayer->getDefaultTextHaloColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(propertyValue);
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

- (void)setTextHaloWidth:(MGLStyleValue<NSNumber *> *)textHaloWidth {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(textHaloWidth);
    self.rawLayer->setTextHaloWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textHaloWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextHaloWidth();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultTextHaloWidth());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
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

- (void)setTextOpacity:(MGLStyleValue<NSNumber *> *)textOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(textOpacity);
    self.rawLayer->setTextOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextOpacity();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultTextOpacity());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
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

- (void)setTextTranslation:(MGLStyleValue<NSValue *> *)textTranslation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toInterpolatablePropertyValue(textTranslation);
    self.rawLayer->setTextTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textTranslation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextTranslate();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(self.rawLayer->getDefaultTextTranslate());
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
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

- (void)setTextTranslate:(MGLStyleValue<NSValue *> *)textTranslate {
}

- (MGLStyleValue<NSValue *> *)textTranslate {
    return self.textTranslation;
}

- (void)setTextTranslationAnchor:(MGLStyleValue<NSValue *> *)textTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLTextTranslationAnchor>().toEnumPropertyValue(textTranslationAnchor);
    self.rawLayer->setTextTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getTextTranslateAnchor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLTextTranslationAnchor>().toEnumStyleValue(self.rawLayer->getDefaultTextTranslateAnchor());
    }
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLTextTranslationAnchor>().toEnumStyleValue(propertyValue);
}

- (void)setTextTranslateAnchor:(MGLStyleValue<NSValue *> *)textTranslateAnchor {
}

- (MGLStyleValue<NSValue *> *)textTranslateAnchor {
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
