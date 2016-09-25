// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLSymbolStyleLayer.h"

#include <mbgl/style/layers/symbol_layer.hpp>

@interface MGLSymbolStyleLayer ()

@property (nonatomic) mbgl::style::SymbolLayer *layer;

@end

@implementation MGLSymbolStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    if (self = [super initWithIdentifier:identifier source:source]) {
        _layer = new mbgl::style::SymbolLayer(identifier.UTF8String, source.identifier.UTF8String);
    }
    return self;
}

- (NSString *)sourceLayerIdentifier
{
    auto layerID = self.layer->getSourceLayer();
    return layerID.empty() ? nil : @(layerID.c_str());
}

- (void)setSourceLayerIdentifier:(NSString *)sourceLayerIdentifier
{
    self.layer->setSourceLayer(sourceLayerIdentifier.UTF8String ?: "");
}

- (void)setPredicate:(NSPredicate *)predicate
{
    self.layer->setFilter(predicate.mgl_filter);
}

- (NSPredicate *)predicate
{
    return [NSPredicate mgl_predicateWithFilter:self.layer->getFilter()];
}

#pragma mark - Accessing the Layout Attributes

- (void)setSymbolPlacement:(MGLStyleValue<NSValue *> *)symbolPlacement {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::SymbolPlacementType, NSValue *>().toPropertyValue(symbolPlacement);
    self.layer->setSymbolPlacement(mbglValue);
}

- (MGLStyleValue<NSValue *> *)symbolPlacement {
    auto propertyValue = self.layer->getSymbolPlacement() ?: self.layer->getDefaultSymbolPlacement();
    return MGLStyleValueTransformer<mbgl::style::SymbolPlacementType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setSymbolSpacing:(MGLStyleValue<NSNumber *> *)symbolSpacing {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(symbolSpacing);
    self.layer->setSymbolSpacing(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)symbolSpacing {
    auto propertyValue = self.layer->getSymbolSpacing() ?: self.layer->getDefaultSymbolSpacing();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setSymbolAvoidEdges:(MGLStyleValue<NSNumber *> *)symbolAvoidEdges {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(symbolAvoidEdges);
    self.layer->setSymbolAvoidEdges(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)symbolAvoidEdges {
    auto propertyValue = self.layer->getSymbolAvoidEdges() ?: self.layer->getDefaultSymbolAvoidEdges();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconAllowOverlap:(MGLStyleValue<NSNumber *> *)iconAllowOverlap {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(iconAllowOverlap);
    self.layer->setIconAllowOverlap(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconAllowOverlap {
    auto propertyValue = self.layer->getIconAllowOverlap() ?: self.layer->getDefaultIconAllowOverlap();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconIgnorePlacement:(MGLStyleValue<NSNumber *> *)iconIgnorePlacement {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(iconIgnorePlacement);
    self.layer->setIconIgnorePlacement(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconIgnorePlacement {
    auto propertyValue = self.layer->getIconIgnorePlacement() ?: self.layer->getDefaultIconIgnorePlacement();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconOptional:(MGLStyleValue<NSNumber *> *)iconOptional {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(iconOptional);
    self.layer->setIconOptional(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconOptional {
    auto propertyValue = self.layer->getIconOptional() ?: self.layer->getDefaultIconOptional();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconRotationAlignment:(MGLStyleValue<NSValue *> *)iconRotationAlignment {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *>().toPropertyValue(iconRotationAlignment);
    self.layer->setIconRotationAlignment(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconRotationAlignment {
    auto propertyValue = self.layer->getIconRotationAlignment() ?: self.layer->getDefaultIconRotationAlignment();
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setIconSize:(MGLStyleValue<NSNumber *> *)iconSize {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconSize);
    self.layer->setIconSize(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconSize {
    auto propertyValue = self.layer->getIconSize() ?: self.layer->getDefaultIconSize();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconTextFit:(MGLStyleValue<NSValue *> *)iconTextFit {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::IconTextFitType, NSValue *>().toPropertyValue(iconTextFit);
    self.layer->setIconTextFit(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconTextFit {
    auto propertyValue = self.layer->getIconTextFit() ?: self.layer->getDefaultIconTextFit();
    return MGLStyleValueTransformer<mbgl::style::IconTextFitType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setIconTextFitPadding:(MGLStyleValue<NSValue *> *)iconTextFitPadding {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 4>, NSValue *>().toPropertyValue(iconTextFitPadding);
    self.layer->setIconTextFitPadding(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconTextFitPadding {
    auto propertyValue = self.layer->getIconTextFitPadding() ?: self.layer->getDefaultIconTextFitPadding();
    return MGLStyleValueTransformer<std::array<float, 4>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setIconImage:(MGLStyleValue<NSString *> *)iconImage {
    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(iconImage);
    self.layer->setIconImage(mbglValue);
}

- (MGLStyleValue<NSString *> *)iconImage {
    auto propertyValue = self.layer->getIconImage() ?: self.layer->getDefaultIconImage();
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

- (void)setIconRotate:(MGLStyleValue<NSNumber *> *)iconRotate {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconRotate);
    self.layer->setIconRotate(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconRotate {
    auto propertyValue = self.layer->getIconRotate() ?: self.layer->getDefaultIconRotate();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconPadding:(MGLStyleValue<NSNumber *> *)iconPadding {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconPadding);
    self.layer->setIconPadding(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconPadding {
    auto propertyValue = self.layer->getIconPadding() ?: self.layer->getDefaultIconPadding();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconKeepUpright:(MGLStyleValue<NSNumber *> *)iconKeepUpright {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(iconKeepUpright);
    self.layer->setIconKeepUpright(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconKeepUpright {
    auto propertyValue = self.layer->getIconKeepUpright() ?: self.layer->getDefaultIconKeepUpright();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconOffset:(MGLStyleValue<NSValue *> *)iconOffset {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(iconOffset);
    self.layer->setIconOffset(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconOffset {
    auto propertyValue = self.layer->getIconOffset() ?: self.layer->getDefaultIconOffset();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextPitchAlignment:(MGLStyleValue<NSValue *> *)textPitchAlignment {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *>().toPropertyValue(textPitchAlignment);
    self.layer->setTextPitchAlignment(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textPitchAlignment {
    auto propertyValue = self.layer->getTextPitchAlignment() ?: self.layer->getDefaultTextPitchAlignment();
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextRotationAlignment:(MGLStyleValue<NSValue *> *)textRotationAlignment {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *>().toPropertyValue(textRotationAlignment);
    self.layer->setTextRotationAlignment(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textRotationAlignment {
    auto propertyValue = self.layer->getTextRotationAlignment() ?: self.layer->getDefaultTextRotationAlignment();
    return MGLStyleValueTransformer<mbgl::style::AlignmentType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextField:(MGLStyleValue<NSString *> *)textField {
    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(textField);
    self.layer->setTextField(mbglValue);
}

- (MGLStyleValue<NSString *> *)textField {
    auto propertyValue = self.layer->getTextField() ?: self.layer->getDefaultTextField();
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

- (void)setTextFont:(MGLStyleValue<NSArray<NSString *> *> *)textFont {
    auto mbglValue = MGLStyleValueTransformer<std::vector<std::string>, NSArray<NSString *> *, std::string>().toPropertyValue(textFont);
    self.layer->setTextFont(mbglValue);
}

- (MGLStyleValue<NSArray<NSString *> *> *)textFont {
    auto propertyValue = self.layer->getTextFont() ?: self.layer->getDefaultTextFont();
    return MGLStyleValueTransformer<std::vector<std::string>, NSArray<NSString *> *, std::string>().toStyleValue(propertyValue);
}

- (void)setTextSize:(MGLStyleValue<NSNumber *> *)textSize {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textSize);
    self.layer->setTextSize(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textSize {
    auto propertyValue = self.layer->getTextSize() ?: self.layer->getDefaultTextSize();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextMaxWidth:(MGLStyleValue<NSNumber *> *)textMaxWidth {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textMaxWidth);
    self.layer->setTextMaxWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textMaxWidth {
    auto propertyValue = self.layer->getTextMaxWidth() ?: self.layer->getDefaultTextMaxWidth();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextLineHeight:(MGLStyleValue<NSNumber *> *)textLineHeight {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textLineHeight);
    self.layer->setTextLineHeight(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textLineHeight {
    auto propertyValue = self.layer->getTextLineHeight() ?: self.layer->getDefaultTextLineHeight();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextLetterSpacing:(MGLStyleValue<NSNumber *> *)textLetterSpacing {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textLetterSpacing);
    self.layer->setTextLetterSpacing(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textLetterSpacing {
    auto propertyValue = self.layer->getTextLetterSpacing() ?: self.layer->getDefaultTextLetterSpacing();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextJustify:(MGLStyleValue<NSValue *> *)textJustify {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TextJustifyType, NSValue *>().toPropertyValue(textJustify);
    self.layer->setTextJustify(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textJustify {
    auto propertyValue = self.layer->getTextJustify() ?: self.layer->getDefaultTextJustify();
    return MGLStyleValueTransformer<mbgl::style::TextJustifyType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextAnchor:(MGLStyleValue<NSValue *> *)textAnchor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TextAnchorType, NSValue *>().toPropertyValue(textAnchor);
    self.layer->setTextAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textAnchor {
    auto propertyValue = self.layer->getTextAnchor() ?: self.layer->getDefaultTextAnchor();
    return MGLStyleValueTransformer<mbgl::style::TextAnchorType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextMaxAngle:(MGLStyleValue<NSNumber *> *)textMaxAngle {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textMaxAngle);
    self.layer->setTextMaxAngle(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textMaxAngle {
    auto propertyValue = self.layer->getTextMaxAngle() ?: self.layer->getDefaultTextMaxAngle();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextRotate:(MGLStyleValue<NSNumber *> *)textRotate {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textRotate);
    self.layer->setTextRotate(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textRotate {
    auto propertyValue = self.layer->getTextRotate() ?: self.layer->getDefaultTextRotate();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextPadding:(MGLStyleValue<NSNumber *> *)textPadding {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textPadding);
    self.layer->setTextPadding(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textPadding {
    auto propertyValue = self.layer->getTextPadding() ?: self.layer->getDefaultTextPadding();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextKeepUpright:(MGLStyleValue<NSNumber *> *)textKeepUpright {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(textKeepUpright);
    self.layer->setTextKeepUpright(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textKeepUpright {
    auto propertyValue = self.layer->getTextKeepUpright() ?: self.layer->getDefaultTextKeepUpright();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextTransform:(MGLStyleValue<NSValue *> *)textTransform {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TextTransformType, NSValue *>().toPropertyValue(textTransform);
    self.layer->setTextTransform(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textTransform {
    auto propertyValue = self.layer->getTextTransform() ?: self.layer->getDefaultTextTransform();
    return MGLStyleValueTransformer<mbgl::style::TextTransformType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextOffset:(MGLStyleValue<NSValue *> *)textOffset {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(textOffset);
    self.layer->setTextOffset(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textOffset {
    auto propertyValue = self.layer->getTextOffset() ?: self.layer->getDefaultTextOffset();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextAllowOverlap:(MGLStyleValue<NSNumber *> *)textAllowOverlap {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(textAllowOverlap);
    self.layer->setTextAllowOverlap(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textAllowOverlap {
    auto propertyValue = self.layer->getTextAllowOverlap() ?: self.layer->getDefaultTextAllowOverlap();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextIgnorePlacement:(MGLStyleValue<NSNumber *> *)textIgnorePlacement {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(textIgnorePlacement);
    self.layer->setTextIgnorePlacement(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textIgnorePlacement {
    auto propertyValue = self.layer->getTextIgnorePlacement() ?: self.layer->getDefaultTextIgnorePlacement();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextOptional:(MGLStyleValue<NSNumber *> *)textOptional {
    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(textOptional);
    self.layer->setTextOptional(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textOptional {
    auto propertyValue = self.layer->getTextOptional() ?: self.layer->getDefaultTextOptional();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

#pragma mark - Accessing the Paint Attributes

- (void)setIconOpacity:(MGLStyleValue<NSNumber *> *)iconOpacity {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconOpacity);
    self.layer->setIconOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconOpacity {
    auto propertyValue = self.layer->getIconOpacity() ?: self.layer->getDefaultIconOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconColor:(MGLStyleValue<MGLColor *> *)iconColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(iconColor);
    self.layer->setIconColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)iconColor {
    auto propertyValue = self.layer->getIconColor() ?: self.layer->getDefaultIconColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setIconHaloColor:(MGLStyleValue<MGLColor *> *)iconHaloColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(iconHaloColor);
    self.layer->setIconHaloColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)iconHaloColor {
    auto propertyValue = self.layer->getIconHaloColor() ?: self.layer->getDefaultIconHaloColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setIconHaloWidth:(MGLStyleValue<NSNumber *> *)iconHaloWidth {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconHaloWidth);
    self.layer->setIconHaloWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconHaloWidth {
    auto propertyValue = self.layer->getIconHaloWidth() ?: self.layer->getDefaultIconHaloWidth();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconHaloBlur:(MGLStyleValue<NSNumber *> *)iconHaloBlur {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(iconHaloBlur);
    self.layer->setIconHaloBlur(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)iconHaloBlur {
    auto propertyValue = self.layer->getIconHaloBlur() ?: self.layer->getDefaultIconHaloBlur();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setIconTranslate:(MGLStyleValue<NSValue *> *)iconTranslate {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(iconTranslate);
    self.layer->setIconTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconTranslate {
    auto propertyValue = self.layer->getIconTranslate() ?: self.layer->getDefaultIconTranslate();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setIconTranslateAnchor:(MGLStyleValue<NSValue *> *)iconTranslateAnchor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toPropertyValue(iconTranslateAnchor);
    self.layer->setIconTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)iconTranslateAnchor {
    auto propertyValue = self.layer->getIconTranslateAnchor() ?: self.layer->getDefaultIconTranslateAnchor();
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextOpacity:(MGLStyleValue<NSNumber *> *)textOpacity {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textOpacity);
    self.layer->setTextOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textOpacity {
    auto propertyValue = self.layer->getTextOpacity() ?: self.layer->getDefaultTextOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextColor:(MGLStyleValue<MGLColor *> *)textColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(textColor);
    self.layer->setTextColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)textColor {
    auto propertyValue = self.layer->getTextColor() ?: self.layer->getDefaultTextColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setTextHaloColor:(MGLStyleValue<MGLColor *> *)textHaloColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(textHaloColor);
    self.layer->setTextHaloColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)textHaloColor {
    auto propertyValue = self.layer->getTextHaloColor() ?: self.layer->getDefaultTextHaloColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setTextHaloWidth:(MGLStyleValue<NSNumber *> *)textHaloWidth {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textHaloWidth);
    self.layer->setTextHaloWidth(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textHaloWidth {
    auto propertyValue = self.layer->getTextHaloWidth() ?: self.layer->getDefaultTextHaloWidth();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextHaloBlur:(MGLStyleValue<NSNumber *> *)textHaloBlur {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(textHaloBlur);
    self.layer->setTextHaloBlur(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)textHaloBlur {
    auto propertyValue = self.layer->getTextHaloBlur() ?: self.layer->getDefaultTextHaloBlur();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setTextTranslate:(MGLStyleValue<NSValue *> *)textTranslate {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(textTranslate);
    self.layer->setTextTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textTranslate {
    auto propertyValue = self.layer->getTextTranslate() ?: self.layer->getDefaultTextTranslate();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setTextTranslateAnchor:(MGLStyleValue<NSValue *> *)textTranslateAnchor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toPropertyValue(textTranslateAnchor);
    self.layer->setTextTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)textTranslateAnchor {
    auto propertyValue = self.layer->getTextTranslateAnchor() ?: self.layer->getDefaultTextTranslateAnchor();
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toStyleValue(propertyValue);
}

@end
