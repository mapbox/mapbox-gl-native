// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleAttributeValue.h"
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

- (void)setSymbolPlacement:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)symbolPlacement {
    MGLSetEnumProperty(symbolPlacement, SymbolPlacement, SymbolPlacementType, MGLSymbolPlacement);
}

- (id <MGLStyleAttributeValue>)symbolPlacement {
    MGLGetEnumProperty(SymbolPlacement, SymbolPlacementType, MGLSymbolPlacement);
}

- (void)setSymbolSpacing:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)symbolSpacing {
    self.layer->setSymbolSpacing(symbolSpacing.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)symbolSpacing {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getSymbolSpacing() ?: self.layer->getDefaultSymbolSpacing()];
}

- (void)setSymbolAvoidEdges:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)symbolAvoidEdges {
    self.layer->setSymbolAvoidEdges(symbolAvoidEdges.mgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)symbolAvoidEdges {
    return [MGLStyleAttribute mgl_boolWithPropertyValueBool:self.layer->getSymbolAvoidEdges() ?: self.layer->getDefaultSymbolAvoidEdges()];
}

- (void)setIconAllowOverlap:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconAllowOverlap {
    self.layer->setIconAllowOverlap(iconAllowOverlap.mgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconAllowOverlap {
    return [MGLStyleAttribute mgl_boolWithPropertyValueBool:self.layer->getIconAllowOverlap() ?: self.layer->getDefaultIconAllowOverlap()];
}

- (void)setIconIgnorePlacement:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconIgnorePlacement {
    self.layer->setIconIgnorePlacement(iconIgnorePlacement.mgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconIgnorePlacement {
    return [MGLStyleAttribute mgl_boolWithPropertyValueBool:self.layer->getIconIgnorePlacement() ?: self.layer->getDefaultIconIgnorePlacement()];
}

- (void)setIconOptional:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconOptional {
    self.layer->setIconOptional(iconOptional.mgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconOptional {
    return [MGLStyleAttribute mgl_boolWithPropertyValueBool:self.layer->getIconOptional() ?: self.layer->getDefaultIconOptional()];
}

- (void)setIconRotationAlignment:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconRotationAlignment {
    MGLSetEnumProperty(iconRotationAlignment, IconRotationAlignment, AlignmentType, MGLIconRotationAlignment);
}

- (id <MGLStyleAttributeValue>)iconRotationAlignment {
    MGLGetEnumProperty(IconRotationAlignment, AlignmentType, MGLIconRotationAlignment);
}

- (void)setIconSize:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconSize {
    self.layer->setIconSize(iconSize.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconSize {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getIconSize() ?: self.layer->getDefaultIconSize()];
}

- (void)setIconTextFit:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconTextFit {
    MGLSetEnumProperty(iconTextFit, IconTextFit, IconTextFitType, MGLIconTextFit);
}

- (id <MGLStyleAttributeValue>)iconTextFit {
    MGLGetEnumProperty(IconTextFit, IconTextFitType, MGLIconTextFit);
}

- (void)setIconTextFitPadding:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconTextFitPadding {
    self.layer->setIconTextFitPadding(iconTextFitPadding.mgl_paddingPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconTextFitPadding {
    return [MGLStyleAttribute mgl_paddingWithPropertyValuePadding:self.layer->getIconTextFitPadding() ?: self.layer->getDefaultIconTextFitPadding()];
}

- (void)setIconImage:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconImage {
    self.layer->setIconImage(iconImage.mgl_stringPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconImage {
    return [MGLStyleAttribute mgl_stringWithPropertyValueString:self.layer->getIconImage() ?: self.layer->getDefaultIconImage()];
}

- (void)setIconRotate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconRotate {
    self.layer->setIconRotate(iconRotate.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconRotate {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getIconRotate() ?: self.layer->getDefaultIconRotate()];
}

- (void)setIconPadding:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconPadding {
    self.layer->setIconPadding(iconPadding.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconPadding {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getIconPadding() ?: self.layer->getDefaultIconPadding()];
}

- (void)setIconKeepUpright:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconKeepUpright {
    self.layer->setIconKeepUpright(iconKeepUpright.mgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconKeepUpright {
    return [MGLStyleAttribute mgl_boolWithPropertyValueBool:self.layer->getIconKeepUpright() ?: self.layer->getDefaultIconKeepUpright()];
}

- (void)setIconOffset:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconOffset {
    self.layer->setIconOffset(iconOffset.mgl_offsetPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconOffset {
    return [MGLStyleAttribute mgl_offsetWithPropertyValueOffset:self.layer->getIconOffset() ?: self.layer->getDefaultIconOffset()];
}

- (void)setTextPitchAlignment:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textPitchAlignment {
    MGLSetEnumProperty(textPitchAlignment, TextPitchAlignment, AlignmentType, MGLTextPitchAlignment);
}

- (id <MGLStyleAttributeValue>)textPitchAlignment {
    MGLGetEnumProperty(TextPitchAlignment, AlignmentType, MGLTextPitchAlignment);
}

- (void)setTextRotationAlignment:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textRotationAlignment {
    MGLSetEnumProperty(textRotationAlignment, TextRotationAlignment, AlignmentType, MGLTextRotationAlignment);
}

- (id <MGLStyleAttributeValue>)textRotationAlignment {
    MGLGetEnumProperty(TextRotationAlignment, AlignmentType, MGLTextRotationAlignment);
}

- (void)setTextField:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textField {
    self.layer->setTextField(textField.mgl_stringPropertyValue);
}

- (id <MGLStyleAttributeValue>)textField {
    return [MGLStyleAttribute mgl_stringWithPropertyValueString:self.layer->getTextField() ?: self.layer->getDefaultTextField()];
}

- (void)setTextFont:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textFont {
    self.layer->setTextFont(textFont.mgl_stringArrayPropertyValue);
}

- (id <MGLStyleAttributeValue>)textFont {
    return [MGLStyleAttribute mgl_stringArrayWithPropertyValueStringArray:self.layer->getTextFont() ?: self.layer->getDefaultTextFont()];
}

- (void)setTextSize:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textSize {
    self.layer->setTextSize(textSize.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textSize {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getTextSize() ?: self.layer->getDefaultTextSize()];
}

- (void)setTextMaxWidth:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textMaxWidth {
    self.layer->setTextMaxWidth(textMaxWidth.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textMaxWidth {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getTextMaxWidth() ?: self.layer->getDefaultTextMaxWidth()];
}

- (void)setTextLineHeight:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textLineHeight {
    self.layer->setTextLineHeight(textLineHeight.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textLineHeight {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getTextLineHeight() ?: self.layer->getDefaultTextLineHeight()];
}

- (void)setTextLetterSpacing:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textLetterSpacing {
    self.layer->setTextLetterSpacing(textLetterSpacing.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textLetterSpacing {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getTextLetterSpacing() ?: self.layer->getDefaultTextLetterSpacing()];
}

- (void)setTextJustify:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textJustify {
    MGLSetEnumProperty(textJustify, TextJustify, TextJustifyType, MGLTextJustify);
}

- (id <MGLStyleAttributeValue>)textJustify {
    MGLGetEnumProperty(TextJustify, TextJustifyType, MGLTextJustify);
}

- (void)setTextAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textAnchor {
    MGLSetEnumProperty(textAnchor, TextAnchor, TextAnchorType, MGLTextAnchor);
}

- (id <MGLStyleAttributeValue>)textAnchor {
    MGLGetEnumProperty(TextAnchor, TextAnchorType, MGLTextAnchor);
}

- (void)setTextMaxAngle:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textMaxAngle {
    self.layer->setTextMaxAngle(textMaxAngle.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textMaxAngle {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getTextMaxAngle() ?: self.layer->getDefaultTextMaxAngle()];
}

- (void)setTextRotate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textRotate {
    self.layer->setTextRotate(textRotate.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textRotate {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getTextRotate() ?: self.layer->getDefaultTextRotate()];
}

- (void)setTextPadding:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textPadding {
    self.layer->setTextPadding(textPadding.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textPadding {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getTextPadding() ?: self.layer->getDefaultTextPadding()];
}

- (void)setTextKeepUpright:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textKeepUpright {
    self.layer->setTextKeepUpright(textKeepUpright.mgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)textKeepUpright {
    return [MGLStyleAttribute mgl_boolWithPropertyValueBool:self.layer->getTextKeepUpright() ?: self.layer->getDefaultTextKeepUpright()];
}

- (void)setTextTransform:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textTransform {
    MGLSetEnumProperty(textTransform, TextTransform, TextTransformType, MGLTextTransform);
}

- (id <MGLStyleAttributeValue>)textTransform {
    MGLGetEnumProperty(TextTransform, TextTransformType, MGLTextTransform);
}

- (void)setTextOffset:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textOffset {
    self.layer->setTextOffset(textOffset.mgl_offsetPropertyValue);
}

- (id <MGLStyleAttributeValue>)textOffset {
    return [MGLStyleAttribute mgl_offsetWithPropertyValueOffset:self.layer->getTextOffset() ?: self.layer->getDefaultTextOffset()];
}

- (void)setTextAllowOverlap:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textAllowOverlap {
    self.layer->setTextAllowOverlap(textAllowOverlap.mgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)textAllowOverlap {
    return [MGLStyleAttribute mgl_boolWithPropertyValueBool:self.layer->getTextAllowOverlap() ?: self.layer->getDefaultTextAllowOverlap()];
}

- (void)setTextIgnorePlacement:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textIgnorePlacement {
    self.layer->setTextIgnorePlacement(textIgnorePlacement.mgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)textIgnorePlacement {
    return [MGLStyleAttribute mgl_boolWithPropertyValueBool:self.layer->getTextIgnorePlacement() ?: self.layer->getDefaultTextIgnorePlacement()];
}

- (void)setTextOptional:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textOptional {
    self.layer->setTextOptional(textOptional.mgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)textOptional {
    return [MGLStyleAttribute mgl_boolWithPropertyValueBool:self.layer->getTextOptional() ?: self.layer->getDefaultTextOptional()];
}

#pragma mark - Accessing the Paint Attributes

- (void)setIconOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconOpacity {
    self.layer->setIconOpacity(iconOpacity.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconOpacity {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getIconOpacity() ?: self.layer->getDefaultIconOpacity()];
}

- (void)setIconColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconColor {
    self.layer->setIconColor(iconColor.mgl_colorPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconColor {
    return [MGLStyleAttribute mgl_colorWithPropertyValueColor:self.layer->getIconColor() ?: self.layer->getDefaultIconColor()];
}

- (void)setIconHaloColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconHaloColor {
    self.layer->setIconHaloColor(iconHaloColor.mgl_colorPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconHaloColor {
    return [MGLStyleAttribute mgl_colorWithPropertyValueColor:self.layer->getIconHaloColor() ?: self.layer->getDefaultIconHaloColor()];
}

- (void)setIconHaloWidth:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconHaloWidth {
    self.layer->setIconHaloWidth(iconHaloWidth.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconHaloWidth {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getIconHaloWidth() ?: self.layer->getDefaultIconHaloWidth()];
}

- (void)setIconHaloBlur:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconHaloBlur {
    self.layer->setIconHaloBlur(iconHaloBlur.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconHaloBlur {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getIconHaloBlur() ?: self.layer->getDefaultIconHaloBlur()];
}

- (void)setIconTranslate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconTranslate {
    self.layer->setIconTranslate(iconTranslate.mgl_offsetPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconTranslate {
    return [MGLStyleAttribute mgl_offsetWithPropertyValueOffset:self.layer->getIconTranslate() ?: self.layer->getDefaultIconTranslate()];
}

- (void)setIconTranslateAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconTranslateAnchor {
    MGLSetEnumProperty(iconTranslateAnchor, IconTranslateAnchor, TranslateAnchorType, MGLIconTranslateAnchor);
}

- (id <MGLStyleAttributeValue>)iconTranslateAnchor {
    MGLGetEnumProperty(IconTranslateAnchor, TranslateAnchorType, MGLIconTranslateAnchor);
}

- (void)setTextOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textOpacity {
    self.layer->setTextOpacity(textOpacity.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textOpacity {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getTextOpacity() ?: self.layer->getDefaultTextOpacity()];
}

- (void)setTextColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textColor {
    self.layer->setTextColor(textColor.mgl_colorPropertyValue);
}

- (id <MGLStyleAttributeValue>)textColor {
    return [MGLStyleAttribute mgl_colorWithPropertyValueColor:self.layer->getTextColor() ?: self.layer->getDefaultTextColor()];
}

- (void)setTextHaloColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textHaloColor {
    self.layer->setTextHaloColor(textHaloColor.mgl_colorPropertyValue);
}

- (id <MGLStyleAttributeValue>)textHaloColor {
    return [MGLStyleAttribute mgl_colorWithPropertyValueColor:self.layer->getTextHaloColor() ?: self.layer->getDefaultTextHaloColor()];
}

- (void)setTextHaloWidth:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textHaloWidth {
    self.layer->setTextHaloWidth(textHaloWidth.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textHaloWidth {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getTextHaloWidth() ?: self.layer->getDefaultTextHaloWidth()];
}

- (void)setTextHaloBlur:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textHaloBlur {
    self.layer->setTextHaloBlur(textHaloBlur.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textHaloBlur {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getTextHaloBlur() ?: self.layer->getDefaultTextHaloBlur()];
}

- (void)setTextTranslate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textTranslate {
    self.layer->setTextTranslate(textTranslate.mgl_offsetPropertyValue);
}

- (id <MGLStyleAttributeValue>)textTranslate {
    return [MGLStyleAttribute mgl_offsetWithPropertyValueOffset:self.layer->getTextTranslate() ?: self.layer->getDefaultTextTranslate()];
}

- (void)setTextTranslateAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textTranslateAnchor {
    MGLSetEnumProperty(textTranslateAnchor, TextTranslateAnchor, TranslateAnchorType, MGLTextTranslateAnchor);
}

- (id <MGLStyleAttributeValue>)textTranslateAnchor {
    MGLGetEnumProperty(TextTranslateAnchor, TranslateAnchorType, MGLTextTranslateAnchor);
}

@end
