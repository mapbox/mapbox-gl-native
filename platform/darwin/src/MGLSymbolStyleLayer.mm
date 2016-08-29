// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLBaseStyleLayer_Private.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleAttributeValue.h"
#import "MGLSymbolStyleLayer.h"

#include <mbgl/style/layers/symbol_layer.hpp>

@interface MGLSymbolStyleLayer ()

@property (nonatomic) mbgl::style::SymbolLayer *layer;
@property (nonatomic, readwrite) NSString *layerIdentifier;
@property (nonatomic, readwrite) NSString *sourceIdentifier;

@end

@implementation MGLSymbolStyleLayer

@synthesize mapView;

- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier sourceIdentifier:(NSString *)sourceIdentifier {
    if (self = [super init]) {
        _layerIdentifier = layerIdentifier;
        _sourceIdentifier = sourceIdentifier;
        _layer = new mbgl::style::SymbolLayer(layerIdentifier.UTF8String, sourceIdentifier.UTF8String);
    }
    return self;
}

#pragma mark - Accessing the Layout Attributes

- (void)setSymbolPlacement:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)symbolPlacement {
    MGLSetEnumProperty(symbolPlacement, SymbolPlacement, SymbolPlacementType, MGLSymbolStyleLayerSymbolPlacement);
    [self update];
}

- (id <MGLStyleAttributeValue>)symbolPlacement {
    MGLGetEnumProperty(SymbolPlacement, SymbolPlacementType, MGLSymbolStyleLayerSymbolPlacement);
}

- (void)setSymbolSpacing:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)symbolSpacing {
    self.layer->setSymbolSpacing(symbolSpacing.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)symbolSpacing {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getSymbolSpacing() ?: self.layer->getDefaultSymbolSpacing()];
}

- (void)setSymbolAvoidEdges:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)symbolAvoidEdges {
    self.layer->setSymbolAvoidEdges(symbolAvoidEdges.mbgl_boolPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)symbolAvoidEdges {
    return [MGLStyleAttribute mbgl_boolWithPropertyValueBool:self.layer->getSymbolAvoidEdges() ?: self.layer->getDefaultSymbolAvoidEdges()];
}

- (void)setIconAllowOverlap:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconAllowOverlap {
    self.layer->setIconAllowOverlap(iconAllowOverlap.mbgl_boolPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconAllowOverlap {
    return [MGLStyleAttribute mbgl_boolWithPropertyValueBool:self.layer->getIconAllowOverlap() ?: self.layer->getDefaultIconAllowOverlap()];
}

- (void)setIconIgnorePlacement:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconIgnorePlacement {
    self.layer->setIconIgnorePlacement(iconIgnorePlacement.mbgl_boolPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconIgnorePlacement {
    return [MGLStyleAttribute mbgl_boolWithPropertyValueBool:self.layer->getIconIgnorePlacement() ?: self.layer->getDefaultIconIgnorePlacement()];
}

- (void)setIconOptional:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconOptional {
    self.layer->setIconOptional(iconOptional.mbgl_boolPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconOptional {
    return [MGLStyleAttribute mbgl_boolWithPropertyValueBool:self.layer->getIconOptional() ?: self.layer->getDefaultIconOptional()];
}

- (void)setIconRotationAlignment:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconRotationAlignment {
    MGLSetEnumProperty(iconRotationAlignment, IconRotationAlignment, AlignmentType, MGLSymbolStyleLayerIconRotationAlignment);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconRotationAlignment {
    MGLGetEnumProperty(IconRotationAlignment, AlignmentType, MGLSymbolStyleLayerIconRotationAlignment);
}

- (void)setIconSize:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconSize {
    self.layer->setIconSize(iconSize.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconSize {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getIconSize() ?: self.layer->getDefaultIconSize()];
}

- (void)setIconTextFit:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconTextFit {
    MGLSetEnumProperty(iconTextFit, IconTextFit, IconTextFitType, MGLSymbolStyleLayerIconTextFit);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconTextFit {
    MGLGetEnumProperty(IconTextFit, IconTextFitType, MGLSymbolStyleLayerIconTextFit);
}

- (void)setIconTextFitPadding:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconTextFitPadding {
    self.layer->setIconTextFitPadding(iconTextFitPadding.mbgl_paddingPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconTextFitPadding {
    return [MGLStyleAttribute mbgl_paddingWithPropertyValuePadding:self.layer->getIconTextFitPadding() ?: self.layer->getDefaultIconTextFitPadding()];
}

- (void)setIconImage:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconImage {
    self.layer->setIconImage(iconImage.mbgl_stringPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconImage {
    return [MGLStyleAttribute mbgl_stringWithPropertyValueString:self.layer->getIconImage() ?: self.layer->getDefaultIconImage()];
}

- (void)setIconRotate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconRotate {
    self.layer->setIconRotate(iconRotate.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconRotate {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getIconRotate() ?: self.layer->getDefaultIconRotate()];
}

- (void)setIconPadding:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconPadding {
    self.layer->setIconPadding(iconPadding.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconPadding {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getIconPadding() ?: self.layer->getDefaultIconPadding()];
}

- (void)setIconKeepUpright:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconKeepUpright {
    self.layer->setIconKeepUpright(iconKeepUpright.mbgl_boolPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconKeepUpright {
    return [MGLStyleAttribute mbgl_boolWithPropertyValueBool:self.layer->getIconKeepUpright() ?: self.layer->getDefaultIconKeepUpright()];
}

- (void)setIconOffset:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconOffset {
    self.layer->setIconOffset(iconOffset.mbgl_offsetPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconOffset {
    return [MGLStyleAttribute mbgl_offsetWithPropertyValueOffset:self.layer->getIconOffset() ?: self.layer->getDefaultIconOffset()];
}

- (void)setTextPitchAlignment:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textPitchAlignment {
    MGLSetEnumProperty(textPitchAlignment, TextPitchAlignment, AlignmentType, MGLSymbolStyleLayerTextPitchAlignment);
    [self update];
}

- (id <MGLStyleAttributeValue>)textPitchAlignment {
    MGLGetEnumProperty(TextPitchAlignment, AlignmentType, MGLSymbolStyleLayerTextPitchAlignment);
}

- (void)setTextRotationAlignment:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textRotationAlignment {
    MGLSetEnumProperty(textRotationAlignment, TextRotationAlignment, AlignmentType, MGLSymbolStyleLayerTextRotationAlignment);
    [self update];
}

- (id <MGLStyleAttributeValue>)textRotationAlignment {
    MGLGetEnumProperty(TextRotationAlignment, AlignmentType, MGLSymbolStyleLayerTextRotationAlignment);
}

- (void)setTextField:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textField {
    self.layer->setTextField(textField.mbgl_stringPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textField {
    return [MGLStyleAttribute mbgl_stringWithPropertyValueString:self.layer->getTextField() ?: self.layer->getDefaultTextField()];
}

- (void)setTextFont:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textFont {
    self.layer->setTextFont(textFont.mbgl_stringArrayPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textFont {
    return [MGLStyleAttribute mbgl_stringArrayWithPropertyValueStringArray:self.layer->getTextFont() ?: self.layer->getDefaultTextFont()];
}

- (void)setTextSize:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textSize {
    self.layer->setTextSize(textSize.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textSize {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getTextSize() ?: self.layer->getDefaultTextSize()];
}

- (void)setTextMaxWidth:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textMaxWidth {
    self.layer->setTextMaxWidth(textMaxWidth.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textMaxWidth {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getTextMaxWidth() ?: self.layer->getDefaultTextMaxWidth()];
}

- (void)setTextLineHeight:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textLineHeight {
    self.layer->setTextLineHeight(textLineHeight.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textLineHeight {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getTextLineHeight() ?: self.layer->getDefaultTextLineHeight()];
}

- (void)setTextLetterSpacing:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textLetterSpacing {
    self.layer->setTextLetterSpacing(textLetterSpacing.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textLetterSpacing {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getTextLetterSpacing() ?: self.layer->getDefaultTextLetterSpacing()];
}

- (void)setTextJustify:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textJustify {
    MGLSetEnumProperty(textJustify, TextJustify, TextJustifyType, MGLSymbolStyleLayerTextJustify);
    [self update];
}

- (id <MGLStyleAttributeValue>)textJustify {
    MGLGetEnumProperty(TextJustify, TextJustifyType, MGLSymbolStyleLayerTextJustify);
}

- (void)setTextAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textAnchor {
    MGLSetEnumProperty(textAnchor, TextAnchor, TextAnchorType, MGLSymbolStyleLayerTextAnchor);
    [self update];
}

- (id <MGLStyleAttributeValue>)textAnchor {
    MGLGetEnumProperty(TextAnchor, TextAnchorType, MGLSymbolStyleLayerTextAnchor);
}

- (void)setTextMaxAngle:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textMaxAngle {
    self.layer->setTextMaxAngle(textMaxAngle.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textMaxAngle {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getTextMaxAngle() ?: self.layer->getDefaultTextMaxAngle()];
}

- (void)setTextRotate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textRotate {
    self.layer->setTextRotate(textRotate.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textRotate {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getTextRotate() ?: self.layer->getDefaultTextRotate()];
}

- (void)setTextPadding:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textPadding {
    self.layer->setTextPadding(textPadding.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textPadding {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getTextPadding() ?: self.layer->getDefaultTextPadding()];
}

- (void)setTextKeepUpright:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textKeepUpright {
    self.layer->setTextKeepUpright(textKeepUpright.mbgl_boolPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textKeepUpright {
    return [MGLStyleAttribute mbgl_boolWithPropertyValueBool:self.layer->getTextKeepUpright() ?: self.layer->getDefaultTextKeepUpright()];
}

- (void)setTextTransform:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textTransform {
    MGLSetEnumProperty(textTransform, TextTransform, TextTransformType, MGLSymbolStyleLayerTextTransform);
    [self update];
}

- (id <MGLStyleAttributeValue>)textTransform {
    MGLGetEnumProperty(TextTransform, TextTransformType, MGLSymbolStyleLayerTextTransform);
}

- (void)setTextOffset:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textOffset {
    self.layer->setTextOffset(textOffset.mbgl_offsetPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textOffset {
    return [MGLStyleAttribute mbgl_offsetWithPropertyValueOffset:self.layer->getTextOffset() ?: self.layer->getDefaultTextOffset()];
}

- (void)setTextAllowOverlap:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textAllowOverlap {
    self.layer->setTextAllowOverlap(textAllowOverlap.mbgl_boolPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textAllowOverlap {
    return [MGLStyleAttribute mbgl_boolWithPropertyValueBool:self.layer->getTextAllowOverlap() ?: self.layer->getDefaultTextAllowOverlap()];
}

- (void)setTextIgnorePlacement:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textIgnorePlacement {
    self.layer->setTextIgnorePlacement(textIgnorePlacement.mbgl_boolPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textIgnorePlacement {
    return [MGLStyleAttribute mbgl_boolWithPropertyValueBool:self.layer->getTextIgnorePlacement() ?: self.layer->getDefaultTextIgnorePlacement()];
}

- (void)setTextOptional:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textOptional {
    self.layer->setTextOptional(textOptional.mbgl_boolPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textOptional {
    return [MGLStyleAttribute mbgl_boolWithPropertyValueBool:self.layer->getTextOptional() ?: self.layer->getDefaultTextOptional()];
}

#pragma mark - Accessing the Paint Attributes

- (void)setIconOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconOpacity {
    self.layer->setIconOpacity(iconOpacity.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconOpacity {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getIconOpacity() ?: self.layer->getDefaultIconOpacity()];
}

- (void)setIconColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconColor {
    self.layer->setIconColor(iconColor.mbgl_colorPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconColor {
    return [MGLStyleAttribute mbgl_colorWithPropertyValueColor:self.layer->getIconColor() ?: self.layer->getDefaultIconColor()];
}

- (void)setIconHaloColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconHaloColor {
    self.layer->setIconHaloColor(iconHaloColor.mbgl_colorPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconHaloColor {
    return [MGLStyleAttribute mbgl_colorWithPropertyValueColor:self.layer->getIconHaloColor() ?: self.layer->getDefaultIconHaloColor()];
}

- (void)setIconHaloWidth:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconHaloWidth {
    self.layer->setIconHaloWidth(iconHaloWidth.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconHaloWidth {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getIconHaloWidth() ?: self.layer->getDefaultIconHaloWidth()];
}

- (void)setIconHaloBlur:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconHaloBlur {
    self.layer->setIconHaloBlur(iconHaloBlur.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconHaloBlur {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getIconHaloBlur() ?: self.layer->getDefaultIconHaloBlur()];
}

- (void)setIconTranslate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconTranslate {
    self.layer->setIconTranslate(iconTranslate.mbgl_offsetPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconTranslate {
    return [MGLStyleAttribute mbgl_offsetWithPropertyValueOffset:self.layer->getIconTranslate() ?: self.layer->getDefaultIconTranslate()];
}

- (void)setIconTranslateAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconTranslateAnchor {
    MGLSetEnumProperty(iconTranslateAnchor, IconTranslateAnchor, TranslateAnchorType, MGLSymbolStyleLayerIconTranslateAnchor);
    [self update];
}

- (id <MGLStyleAttributeValue>)iconTranslateAnchor {
    MGLGetEnumProperty(IconTranslateAnchor, TranslateAnchorType, MGLSymbolStyleLayerIconTranslateAnchor);
}

- (void)setTextOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textOpacity {
    self.layer->setTextOpacity(textOpacity.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textOpacity {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getTextOpacity() ?: self.layer->getDefaultTextOpacity()];
}

- (void)setTextColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textColor {
    self.layer->setTextColor(textColor.mbgl_colorPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textColor {
    return [MGLStyleAttribute mbgl_colorWithPropertyValueColor:self.layer->getTextColor() ?: self.layer->getDefaultTextColor()];
}

- (void)setTextHaloColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textHaloColor {
    self.layer->setTextHaloColor(textHaloColor.mbgl_colorPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textHaloColor {
    return [MGLStyleAttribute mbgl_colorWithPropertyValueColor:self.layer->getTextHaloColor() ?: self.layer->getDefaultTextHaloColor()];
}

- (void)setTextHaloWidth:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textHaloWidth {
    self.layer->setTextHaloWidth(textHaloWidth.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textHaloWidth {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getTextHaloWidth() ?: self.layer->getDefaultTextHaloWidth()];
}

- (void)setTextHaloBlur:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textHaloBlur {
    self.layer->setTextHaloBlur(textHaloBlur.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textHaloBlur {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getTextHaloBlur() ?: self.layer->getDefaultTextHaloBlur()];
}

- (void)setTextTranslate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textTranslate {
    self.layer->setTextTranslate(textTranslate.mbgl_offsetPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)textTranslate {
    return [MGLStyleAttribute mbgl_offsetWithPropertyValueOffset:self.layer->getTextTranslate() ?: self.layer->getDefaultTextTranslate()];
}

- (void)setTextTranslateAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textTranslateAnchor {
    MGLSetEnumProperty(textTranslateAnchor, TextTranslateAnchor, TranslateAnchorType, MGLSymbolStyleLayerTextTranslateAnchor);
    [self update];
}

- (id <MGLStyleAttributeValue>)textTranslateAnchor {
    MGLGetEnumProperty(TextTranslateAnchor, TranslateAnchorType, MGLSymbolStyleLayerTextTranslateAnchor);
}

@end
