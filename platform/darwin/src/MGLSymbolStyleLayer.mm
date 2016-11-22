// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "MGLMapView_Private.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLSymbolStyleLayer.h"

#include <mbgl/style/layers/symbol_layer.hpp>
namespace mbgl {

    MBGL_DEFINE_ENUM(MGLSymbolPlacement, {
        { MGLSymbolPlacementPoint, "point" },
        { MGLSymbolPlacementLine, "line" },
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

    MBGL_DEFINE_ENUM(MGLTextJustify, {
        { MGLTextJustifyLeft, "left" },
        { MGLTextJustifyCenter, "center" },
        { MGLTextJustifyRight, "right" },
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

    MBGL_DEFINE_ENUM(MGLTextTransform, {
        { MGLTextTransformNone, "none" },
        { MGLTextTransformUppercase, "uppercase" },
        { MGLTextTransformLowercase, "lowercase" },
    });

    MBGL_DEFINE_ENUM(MGLIconTranslateAnchor, {
        { MGLIconTranslateAnchorMap, "map" },
        { MGLIconTranslateAnchorViewport, "viewport" },
    });

    MBGL_DEFINE_ENUM(MGLTextTranslateAnchor, {
        { MGLTextTranslateAnchorMap, "map" },
        { MGLTextTranslateAnchorViewport, "viewport" },
    });

}

@interface MGLSymbolStyleLayer ()

@property (nonatomic) mbgl::style::SymbolLayer *rawLayer;

@end

@implementation MGLSymbolStyleLayer
{
    std::unique_ptr<mbgl::style::SymbolLayer> _pendingLayer;
}

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    if (self = [super initWithIdentifier:identifier source:source]) {
        auto layer = std::make_unique<mbgl::style::SymbolLayer>(identifier.UTF8String, source.identifier.UTF8String);
        _pendingLayer = std::move(layer);
        _rawLayer = _pendingLayer.get();
    }
    return self;
}
- (NSString *)sourceLayerIdentifier
{
    auto layerID = _rawLayer->getSourceLayer();
    return layerID.empty() ? nil : @(layerID.c_str());
}

- (void)setSourceLayerIdentifier:(NSString *)sourceLayerIdentifier
{
    _rawLayer->setSourceLayer(sourceLayerIdentifier.UTF8String ?: "");
}

- (void)setPredicate:(NSPredicate *)predicate
{
    _rawLayer->setFilter(predicate.mgl_filter);
}

- (NSPredicate *)predicate
{
    return [NSPredicate mgl_predicateWithFilter:_rawLayer->getFilter()];
}
#pragma mark -  Adding to and removing from a map view

- (void)addToMapView:(MGLMapView *)mapView
{
    [self addToMapView:mapView belowLayer:nil];
}

- (void)addToMapView:(MGLMapView *)mapView belowLayer:(MGLStyleLayer *)otherLayer
{
    if (otherLayer) {
        const mbgl::optional<std::string> belowLayerId{otherLayer.identifier.UTF8String};
        mapView.mbglMap->addLayer(std::move(_pendingLayer), belowLayerId);
    } else {
        mapView.mbglMap->addLayer(std::move(_pendingLayer));
    }
}

- (void)removeFromMapView:(MGLMapView *)mapView
{
    auto removedLayer = mapView.mbglMap->removeLayer(self.identifier.UTF8String);
    _pendingLayer = std::move(reinterpret_cast<std::unique_ptr<mbgl::style::SymbolLayer> &>(removedLayer));
    _rawLayer = _pendingLayer.get();
}

#pragma mark - Accessing the Layout Attributes

- (void)setSymbolPlacement:(MGLStyleValue<NSValue *> *)symbolPlacement {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::SymbolPlacementType, NSValue *, mbgl::style::SymbolPlacementType, MGLSymbolPlacement>().toEnumPropertyValue(symbolPlacement);
    _rawLayer->setSymbolPlacement(mbglValue);
}

- (MGLStyleValue<NSValue *> *)symbolPlacement {
    auto propertyValue = _rawLayer->getSymbolPlacement() ?: _rawLayer->getDefaultSymbolPlacement();
    return MGLStyleValueTransformer<mbgl::style::SymbolPlacementType, NSValue *, mbgl::style::SymbolPlacementType, MGLSymbolPlacement>().toEnumStyleValue(propertyValue);
}

- (void)setSymbolSpacing:(MGLStyleValue<NSNumber *> *)symbolSpacing {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(symbolSpacing);
    _rawLayer->setSymbolSpacing(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)symbolSpacing {
    auto propertyValue = _rawLayer->getSymbolSpacing() ?: _rawLayer->getDefaultSymbolSpacing();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setSymbolAvoidEdges:(MGLStyleValue<NSNumber *> *)symbolAvoidEdges {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(symbolAvoidEdges);
    _rawLayer->setSymbolAvoidEdges(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)symbolAvoidEdges {
    auto propertyValue = _rawLayer->getSymbolAvoidEdges() ?: _rawLayer->getDefaultSymbolAvoidEdges();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconAllowOverlap:(MGLStyleValue<NSNumber *> *)iconAllowOverlap {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(iconAllowOverlap);
    _rawLayer->setIconAllowOverlap(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconAllowOverlap {
    auto propertyValue = _rawLayer->getIconAllowOverlap() ?: _rawLayer->getDefaultIconAllowOverlap();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconIgnorePlacement:(MGLStyleValue<NSNumber *> *)iconIgnorePlacement {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(iconIgnorePlacement);
    _rawLayer->setIconIgnorePlacement(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconIgnorePlacement {
    auto propertyValue = _rawLayer->getIconIgnorePlacement() ?: _rawLayer->getDefaultIconIgnorePlacement();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconOptional:(MGLStyleValue<NSNumber *> *)iconOptional {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(iconOptional);
    _rawLayer->setIconOptional(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconOptional {
    auto propertyValue = _rawLayer->getIconOptional() ?: _rawLayer->getDefaultIconOptional();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconRotationAlignment:(MGLStyleValue<NSValue *> *)iconRotationAlignment {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLIconRotationAlignment>().toEnumPropertyValue(iconRotationAlignment);
    _rawLayer->setIconRotationAlignment(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconRotationAlignment {
    auto propertyValue = _rawLayer->getIconRotationAlignment() ?: _rawLayer->getDefaultIconRotationAlignment();
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLIconRotationAlignment>().toEnumStyleValue(propertyValue);
}

- (void)setIconSize:(MGLStyleValue<NSNumber *> *)iconSize {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconSize);
    _rawLayer->setIconSize(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconSize {
    auto propertyValue = _rawLayer->getIconSize() ?: _rawLayer->getDefaultIconSize();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconTextFit:(MGLStyleValue<NSValue *> *)iconTextFit {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::IconTextFitType, NSValue *, mbgl::style::IconTextFitType, MGLIconTextFit>().toEnumPropertyValue(iconTextFit);
    _rawLayer->setIconTextFit(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconTextFit {
    auto propertyValue = _rawLayer->getIconTextFit() ?: _rawLayer->getDefaultIconTextFit();
    return MGLStyleValueTransformer<mbgl::style::IconTextFitType, NSValue *, mbgl::style::IconTextFitType, MGLIconTextFit>().toEnumStyleValue(propertyValue);
}

- (void)setIconTextFitPadding:(MGLStyleValue<NSValue *> *)iconTextFitPadding {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 4>, NSValue *>().toPropertyValue(iconTextFitPadding);
    _rawLayer->setIconTextFitPadding(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconTextFitPadding {
    auto propertyValue = _rawLayer->getIconTextFitPadding() ?: _rawLayer->getDefaultIconTextFitPadding();
    return MGLStyleValueTransformer<std::array<float, 4>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setIconImage:(MGLStyleValue<NSString *> *)iconImage {
    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(iconImage);
    _rawLayer->setIconImage(mbglValue);
}

- (MGLStyleValue<NSString *> *)iconImage {
    auto propertyValue = _rawLayer->getIconImage() ?: _rawLayer->getDefaultIconImage();
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

- (void)setIconRotate:(MGLStyleValue<NSNumber *> *)iconRotate {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconRotate);
    _rawLayer->setIconRotate(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconRotate {
    auto propertyValue = _rawLayer->getIconRotate() ?: _rawLayer->getDefaultIconRotate();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconPadding:(MGLStyleValue<NSNumber *> *)iconPadding {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconPadding);
    _rawLayer->setIconPadding(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconPadding {
    auto propertyValue = _rawLayer->getIconPadding() ?: _rawLayer->getDefaultIconPadding();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconKeepUpright:(MGLStyleValue<NSNumber *> *)iconKeepUpright {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(iconKeepUpright);
    _rawLayer->setIconKeepUpright(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconKeepUpright {
    auto propertyValue = _rawLayer->getIconKeepUpright() ?: _rawLayer->getDefaultIconKeepUpright();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconOffset:(MGLStyleValue<NSValue *> *)iconOffset {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(iconOffset);
    _rawLayer->setIconOffset(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconOffset {
    auto propertyValue = _rawLayer->getIconOffset() ?: _rawLayer->getDefaultIconOffset();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextPitchAlignment:(MGLStyleValue<NSValue *> *)textPitchAlignment {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLTextPitchAlignment>().toEnumPropertyValue(textPitchAlignment);
    _rawLayer->setTextPitchAlignment(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textPitchAlignment {
    auto propertyValue = _rawLayer->getTextPitchAlignment() ?: _rawLayer->getDefaultTextPitchAlignment();
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLTextPitchAlignment>().toEnumStyleValue(propertyValue);
}

- (void)setTextRotationAlignment:(MGLStyleValue<NSValue *> *)textRotationAlignment {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLTextRotationAlignment>().toEnumPropertyValue(textRotationAlignment);
    _rawLayer->setTextRotationAlignment(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textRotationAlignment {
    auto propertyValue = _rawLayer->getTextRotationAlignment() ?: _rawLayer->getDefaultTextRotationAlignment();
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *, mbgl::style::AlignmentType, MGLTextRotationAlignment>().toEnumStyleValue(propertyValue);
}

- (void)setTextField:(MGLStyleValue<NSString *> *)textField {
    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(textField);
    _rawLayer->setTextField(mbglValue);
}

- (MGLStyleValue<NSString *> *)textField {
    auto propertyValue = _rawLayer->getTextField() ?: _rawLayer->getDefaultTextField();
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

- (void)setTextFont:(MGLStyleValue<NSArray<NSString *> *> *)textFont {
    auto mbglValue = MGLStyleValueTransformer<std::vector<std::string>, NSArray<NSString *> *, std::string>().toPropertyValue(textFont);
    _rawLayer->setTextFont(mbglValue);
}

- (MGLStyleValue<NSArray<NSString *> *> *)textFont {
    auto propertyValue = _rawLayer->getTextFont() ?: _rawLayer->getDefaultTextFont();
    return MGLStyleValueTransformer<std::vector<std::string>, NSArray<NSString *> *, std::string>().toStyleValue(propertyValue);
}

- (void)setTextSize:(MGLStyleValue<NSNumber *> *)textSize {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textSize);
    _rawLayer->setTextSize(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textSize {
    auto propertyValue = _rawLayer->getTextSize() ?: _rawLayer->getDefaultTextSize();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextMaxWidth:(MGLStyleValue<NSNumber *> *)textMaxWidth {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textMaxWidth);
    _rawLayer->setTextMaxWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textMaxWidth {
    auto propertyValue = _rawLayer->getTextMaxWidth() ?: _rawLayer->getDefaultTextMaxWidth();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextLineHeight:(MGLStyleValue<NSNumber *> *)textLineHeight {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textLineHeight);
    _rawLayer->setTextLineHeight(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textLineHeight {
    auto propertyValue = _rawLayer->getTextLineHeight() ?: _rawLayer->getDefaultTextLineHeight();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextLetterSpacing:(MGLStyleValue<NSNumber *> *)textLetterSpacing {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textLetterSpacing);
    _rawLayer->setTextLetterSpacing(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textLetterSpacing {
    auto propertyValue = _rawLayer->getTextLetterSpacing() ?: _rawLayer->getDefaultTextLetterSpacing();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextJustify:(MGLStyleValue<NSValue *> *)textJustify {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TextJustifyType, NSValue *, mbgl::style::TextJustifyType, MGLTextJustify>().toEnumPropertyValue(textJustify);
    _rawLayer->setTextJustify(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textJustify {
    auto propertyValue = _rawLayer->getTextJustify() ?: _rawLayer->getDefaultTextJustify();
    return MGLStyleValueTransformer<mbgl::style::TextJustifyType, NSValue *, mbgl::style::TextJustifyType, MGLTextJustify>().toEnumStyleValue(propertyValue);
}

- (void)setTextAnchor:(MGLStyleValue<NSValue *> *)textAnchor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TextAnchorType, NSValue *, mbgl::style::TextAnchorType, MGLTextAnchor>().toEnumPropertyValue(textAnchor);
    _rawLayer->setTextAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textAnchor {
    auto propertyValue = _rawLayer->getTextAnchor() ?: _rawLayer->getDefaultTextAnchor();
    return MGLStyleValueTransformer<mbgl::style::TextAnchorType, NSValue *, mbgl::style::TextAnchorType, MGLTextAnchor>().toEnumStyleValue(propertyValue);
}

- (void)setTextMaxAngle:(MGLStyleValue<NSNumber *> *)textMaxAngle {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textMaxAngle);
    _rawLayer->setTextMaxAngle(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textMaxAngle {
    auto propertyValue = _rawLayer->getTextMaxAngle() ?: _rawLayer->getDefaultTextMaxAngle();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextRotate:(MGLStyleValue<NSNumber *> *)textRotate {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textRotate);
    _rawLayer->setTextRotate(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textRotate {
    auto propertyValue = _rawLayer->getTextRotate() ?: _rawLayer->getDefaultTextRotate();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextPadding:(MGLStyleValue<NSNumber *> *)textPadding {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textPadding);
    _rawLayer->setTextPadding(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textPadding {
    auto propertyValue = _rawLayer->getTextPadding() ?: _rawLayer->getDefaultTextPadding();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextKeepUpright:(MGLStyleValue<NSNumber *> *)textKeepUpright {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(textKeepUpright);
    _rawLayer->setTextKeepUpright(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textKeepUpright {
    auto propertyValue = _rawLayer->getTextKeepUpright() ?: _rawLayer->getDefaultTextKeepUpright();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextTransform:(MGLStyleValue<NSValue *> *)textTransform {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TextTransformType, NSValue *, mbgl::style::TextTransformType, MGLTextTransform>().toEnumPropertyValue(textTransform);
    _rawLayer->setTextTransform(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textTransform {
    auto propertyValue = _rawLayer->getTextTransform() ?: _rawLayer->getDefaultTextTransform();
    return MGLStyleValueTransformer<mbgl::style::TextTransformType, NSValue *, mbgl::style::TextTransformType, MGLTextTransform>().toEnumStyleValue(propertyValue);
}

- (void)setTextOffset:(MGLStyleValue<NSValue *> *)textOffset {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(textOffset);
    _rawLayer->setTextOffset(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textOffset {
    auto propertyValue = _rawLayer->getTextOffset() ?: _rawLayer->getDefaultTextOffset();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextAllowOverlap:(MGLStyleValue<NSNumber *> *)textAllowOverlap {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(textAllowOverlap);
    _rawLayer->setTextAllowOverlap(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textAllowOverlap {
    auto propertyValue = _rawLayer->getTextAllowOverlap() ?: _rawLayer->getDefaultTextAllowOverlap();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextIgnorePlacement:(MGLStyleValue<NSNumber *> *)textIgnorePlacement {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(textIgnorePlacement);
    _rawLayer->setTextIgnorePlacement(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textIgnorePlacement {
    auto propertyValue = _rawLayer->getTextIgnorePlacement() ?: _rawLayer->getDefaultTextIgnorePlacement();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextOptional:(MGLStyleValue<NSNumber *> *)textOptional {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(textOptional);
    _rawLayer->setTextOptional(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textOptional {
    auto propertyValue = _rawLayer->getTextOptional() ?: _rawLayer->getDefaultTextOptional();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

#pragma mark - Accessing the Paint Attributes

- (void)setIconOpacity:(MGLStyleValue<NSNumber *> *)iconOpacity {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconOpacity);
    _rawLayer->setIconOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconOpacity {
    auto propertyValue = _rawLayer->getIconOpacity() ?: _rawLayer->getDefaultIconOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconColor:(MGLStyleValue<MGLColor *> *)iconColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(iconColor);
    _rawLayer->setIconColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)iconColor {
    auto propertyValue = _rawLayer->getIconColor() ?: _rawLayer->getDefaultIconColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setIconHaloColor:(MGLStyleValue<MGLColor *> *)iconHaloColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(iconHaloColor);
    _rawLayer->setIconHaloColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)iconHaloColor {
    auto propertyValue = _rawLayer->getIconHaloColor() ?: _rawLayer->getDefaultIconHaloColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setIconHaloWidth:(MGLStyleValue<NSNumber *> *)iconHaloWidth {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconHaloWidth);
    _rawLayer->setIconHaloWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconHaloWidth {
    auto propertyValue = _rawLayer->getIconHaloWidth() ?: _rawLayer->getDefaultIconHaloWidth();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconHaloBlur:(MGLStyleValue<NSNumber *> *)iconHaloBlur {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconHaloBlur);
    _rawLayer->setIconHaloBlur(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconHaloBlur {
    auto propertyValue = _rawLayer->getIconHaloBlur() ?: _rawLayer->getDefaultIconHaloBlur();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconTranslate:(MGLStyleValue<NSValue *> *)iconTranslate {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(iconTranslate);
    _rawLayer->setIconTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconTranslate {
    auto propertyValue = _rawLayer->getIconTranslate() ?: _rawLayer->getDefaultIconTranslate();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setIconTranslateAnchor:(MGLStyleValue<NSValue *> *)iconTranslateAnchor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLIconTranslateAnchor>().toEnumPropertyValue(iconTranslateAnchor);
    _rawLayer->setIconTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconTranslateAnchor {
    auto propertyValue = _rawLayer->getIconTranslateAnchor() ?: _rawLayer->getDefaultIconTranslateAnchor();
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLIconTranslateAnchor>().toEnumStyleValue(propertyValue);
}

- (void)setTextOpacity:(MGLStyleValue<NSNumber *> *)textOpacity {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textOpacity);
    _rawLayer->setTextOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textOpacity {
    auto propertyValue = _rawLayer->getTextOpacity() ?: _rawLayer->getDefaultTextOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextColor:(MGLStyleValue<MGLColor *> *)textColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(textColor);
    _rawLayer->setTextColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)textColor {
    auto propertyValue = _rawLayer->getTextColor() ?: _rawLayer->getDefaultTextColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setTextHaloColor:(MGLStyleValue<MGLColor *> *)textHaloColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(textHaloColor);
    _rawLayer->setTextHaloColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)textHaloColor {
    auto propertyValue = _rawLayer->getTextHaloColor() ?: _rawLayer->getDefaultTextHaloColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setTextHaloWidth:(MGLStyleValue<NSNumber *> *)textHaloWidth {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textHaloWidth);
    _rawLayer->setTextHaloWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textHaloWidth {
    auto propertyValue = _rawLayer->getTextHaloWidth() ?: _rawLayer->getDefaultTextHaloWidth();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextHaloBlur:(MGLStyleValue<NSNumber *> *)textHaloBlur {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textHaloBlur);
    _rawLayer->setTextHaloBlur(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textHaloBlur {
    auto propertyValue = _rawLayer->getTextHaloBlur() ?: _rawLayer->getDefaultTextHaloBlur();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextTranslate:(MGLStyleValue<NSValue *> *)textTranslate {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(textTranslate);
    _rawLayer->setTextTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textTranslate {
    auto propertyValue = _rawLayer->getTextTranslate() ?: _rawLayer->getDefaultTextTranslate();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextTranslateAnchor:(MGLStyleValue<NSValue *> *)textTranslateAnchor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLTextTranslateAnchor>().toEnumPropertyValue(textTranslateAnchor);
    _rawLayer->setTextTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textTranslateAnchor {
    auto propertyValue = _rawLayer->getTextTranslateAnchor() ?: _rawLayer->getDefaultTextTranslateAnchor();
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLTextTranslateAnchor>().toEnumStyleValue(propertyValue);
}


@end
