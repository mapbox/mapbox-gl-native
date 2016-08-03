// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
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
}

- (id <MGLStyleAttributeValue>)symbolPlacement {
    auto rawValue = self.layer->getSymbolPlacement();
    const char *type = @encode(MGLSymbolStyleLayerSymbolPlacement);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setSymbolSpacing:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)symbolSpacing {
    self.layer->setSymbolSpacing(symbolSpacing.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)symbolSpacing {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getSymbolSpacing()];
}

- (void)setSymbolAvoidEdges:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)symbolAvoidEdges {
    self.layer->setSymbolAvoidEdges(symbolAvoidEdges.mbgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)symbolAvoidEdges {
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getSymbolAvoidEdges()];
}

- (void)setIconAllowOverlap:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconAllowOverlap {
    self.layer->setIconAllowOverlap(iconAllowOverlap.mbgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconAllowOverlap {
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getIconAllowOverlap()];
}

- (void)setIconIgnorePlacement:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconIgnorePlacement {
    self.layer->setIconIgnorePlacement(iconIgnorePlacement.mbgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconIgnorePlacement {
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getIconIgnorePlacement()];
}

- (void)setIconOptional:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconOptional {
    self.layer->setIconOptional(iconOptional.mbgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconOptional {
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getIconOptional()];
}

- (void)setIconRotationAlignment:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconRotationAlignment {
    MGLSetEnumProperty(iconRotationAlignment, IconRotationAlignment, AlignmentType, MGLSymbolStyleLayerIconRotationAlignment);
}

- (id <MGLStyleAttributeValue>)iconRotationAlignment {
    auto rawValue = self.layer->getIconRotationAlignment();
    const char *type = @encode(MGLSymbolStyleLayerIconRotationAlignment);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setIconSize:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconSize {
    self.layer->setIconSize(iconSize.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconSize {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getIconSize()];
}

- (void)setIconTextFit:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconTextFit {
    MGLSetEnumProperty(iconTextFit, IconTextFit, IconTextFitType, MGLSymbolStyleLayerIconTextFit);
}

- (id <MGLStyleAttributeValue>)iconTextFit {
    auto rawValue = self.layer->getIconTextFit();
    const char *type = @encode(MGLSymbolStyleLayerIconTextFit);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setIconTextFitPadding:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconTextFitPadding {
    self.layer->setIconTextFitPadding(iconTextFitPadding.mbgl_paddingPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconTextFitPadding {
    return [NSArray mbgl_paddingPropertyValue:self.layer->getIconTextFitPadding()];
}

- (void)setIconImage:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconImage {
    self.layer->setIconImage(iconImage.mbgl_stringPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconImage {
    return [NSString mbgl_stringWithPropertyValue:self.layer->getIconImage()];
}

- (void)setIconRotate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconRotate {
    self.layer->setIconRotate(iconRotate.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconRotate {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getIconRotate()];
}

- (void)setIconPadding:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconPadding {
    self.layer->setIconPadding(iconPadding.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconPadding {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getIconPadding()];
}

- (void)setIconKeepUpright:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconKeepUpright {
    self.layer->setIconKeepUpright(iconKeepUpright.mbgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconKeepUpright {
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getIconKeepUpright()];
}

- (void)setIconOffset:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconOffset {
    self.layer->setIconOffset(iconOffset.mbgl_offsetPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconOffset {
    return [NSArray mbgl_offsetPropertyValue:self.layer->getIconOffset()];
}

- (void)setTextPitchAlignment:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textPitchAlignment {
    MGLSetEnumProperty(textPitchAlignment, TextPitchAlignment, AlignmentType, MGLSymbolStyleLayerTextPitchAlignment);
}

- (id <MGLStyleAttributeValue>)textPitchAlignment {
    auto rawValue = self.layer->getTextPitchAlignment();
    const char *type = @encode(MGLSymbolStyleLayerTextPitchAlignment);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setTextRotationAlignment:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textRotationAlignment {
    MGLSetEnumProperty(textRotationAlignment, TextRotationAlignment, AlignmentType, MGLSymbolStyleLayerTextRotationAlignment);
}

- (id <MGLStyleAttributeValue>)textRotationAlignment {
    auto rawValue = self.layer->getTextRotationAlignment();
    const char *type = @encode(MGLSymbolStyleLayerTextRotationAlignment);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setTextField:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textField {
    self.layer->setTextField(textField.mbgl_stringPropertyValue);
}

- (id <MGLStyleAttributeValue>)textField {
    return [NSString mbgl_stringWithPropertyValue:self.layer->getTextField()];
}

- (void)setTextFont:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textFont {
    self.layer->setTextFont(textFont.mbgl_stringArrayPropertyValue);
}

- (id <MGLStyleAttributeValue>)textFont {
    return [NSArray mbgl_stringArrayPropertyValue:self.layer->getTextFont()];
}

- (void)setTextSize:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textSize {
    self.layer->setTextSize(textSize.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textSize {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextSize()];
}

- (void)setTextMaxWidth:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textMaxWidth {
    self.layer->setTextMaxWidth(textMaxWidth.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textMaxWidth {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextMaxWidth()];
}

- (void)setTextLineHeight:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textLineHeight {
    self.layer->setTextLineHeight(textLineHeight.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textLineHeight {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextLineHeight()];
}

- (void)setTextLetterSpacing:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textLetterSpacing {
    self.layer->setTextLetterSpacing(textLetterSpacing.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textLetterSpacing {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextLetterSpacing()];
}

- (void)setTextJustify:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textJustify {
    MGLSetEnumProperty(textJustify, TextJustify, TextJustifyType, MGLSymbolStyleLayerTextJustify);
}

- (id <MGLStyleAttributeValue>)textJustify {
    auto rawValue = self.layer->getTextJustify();
    const char *type = @encode(MGLSymbolStyleLayerTextJustify);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setTextAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textAnchor {
    MGLSetEnumProperty(textAnchor, TextAnchor, TextAnchorType, MGLSymbolStyleLayerTextAnchor);
}

- (id <MGLStyleAttributeValue>)textAnchor {
    auto rawValue = self.layer->getTextAnchor();
    const char *type = @encode(MGLSymbolStyleLayerTextAnchor);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setTextMaxAngle:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textMaxAngle {
    self.layer->setTextMaxAngle(textMaxAngle.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textMaxAngle {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextMaxAngle()];
}

- (void)setTextRotate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textRotate {
    self.layer->setTextRotate(textRotate.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textRotate {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextRotate()];
}

- (void)setTextPadding:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textPadding {
    self.layer->setTextPadding(textPadding.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textPadding {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextPadding()];
}

- (void)setTextKeepUpright:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textKeepUpright {
    self.layer->setTextKeepUpright(textKeepUpright.mbgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)textKeepUpright {
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getTextKeepUpright()];
}

- (void)setTextTransform:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textTransform {
    MGLSetEnumProperty(textTransform, TextTransform, TextTransformType, MGLSymbolStyleLayerTextTransform);
}

- (id <MGLStyleAttributeValue>)textTransform {
    auto rawValue = self.layer->getTextTransform();
    const char *type = @encode(MGLSymbolStyleLayerTextTransform);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setTextOffset:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textOffset {
    self.layer->setTextOffset(textOffset.mbgl_offsetPropertyValue);
}

- (id <MGLStyleAttributeValue>)textOffset {
    return [NSArray mbgl_offsetPropertyValue:self.layer->getTextOffset()];
}

- (void)setTextAllowOverlap:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textAllowOverlap {
    self.layer->setTextAllowOverlap(textAllowOverlap.mbgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)textAllowOverlap {
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getTextAllowOverlap()];
}

- (void)setTextIgnorePlacement:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textIgnorePlacement {
    self.layer->setTextIgnorePlacement(textIgnorePlacement.mbgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)textIgnorePlacement {
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getTextIgnorePlacement()];
}

- (void)setTextOptional:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textOptional {
    self.layer->setTextOptional(textOptional.mbgl_boolPropertyValue);
}

- (id <MGLStyleAttributeValue>)textOptional {
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getTextOptional()];
}

#pragma mark - Accessing the Paint Attributes

- (void)setIconOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconOpacity {
    self.layer->setIconOpacity(iconOpacity.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconOpacity {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getIconOpacity()];
}

- (void)setIconColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconColor {
    self.layer->setIconColor(iconColor.mbgl_colorPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconColor {
    return [MGLColor mbgl_propertyValue:self.layer->getIconColor()];
}

- (void)setIconHaloColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconHaloColor {
    self.layer->setIconHaloColor(iconHaloColor.mbgl_colorPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconHaloColor {
    return [MGLColor mbgl_propertyValue:self.layer->getIconHaloColor()];
}

- (void)setIconHaloWidth:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconHaloWidth {
    self.layer->setIconHaloWidth(iconHaloWidth.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconHaloWidth {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getIconHaloWidth()];
}

- (void)setIconHaloBlur:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconHaloBlur {
    self.layer->setIconHaloBlur(iconHaloBlur.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconHaloBlur {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getIconHaloBlur()];
}

- (void)setIconTranslate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconTranslate {
    self.layer->setIconTranslate(iconTranslate.mbgl_offsetPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconTranslate {
    return [NSArray mbgl_offsetPropertyValue:self.layer->getIconTranslate()];
}

- (void)setIconTranslateAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)iconTranslateAnchor {
    MGLSetEnumProperty(iconTranslateAnchor, IconTranslateAnchor, TranslateAnchorType, MGLSymbolStyleLayerIconTranslateAnchor);
}

- (id <MGLStyleAttributeValue>)iconTranslateAnchor {
    auto rawValue = self.layer->getIconTranslateAnchor();
    const char *type = @encode(MGLSymbolStyleLayerIconTranslateAnchor);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setTextOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textOpacity {
    self.layer->setTextOpacity(textOpacity.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textOpacity {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextOpacity()];
}

- (void)setTextColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textColor {
    self.layer->setTextColor(textColor.mbgl_colorPropertyValue);
}

- (id <MGLStyleAttributeValue>)textColor {
    return [MGLColor mbgl_propertyValue:self.layer->getTextColor()];
}

- (void)setTextHaloColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textHaloColor {
    self.layer->setTextHaloColor(textHaloColor.mbgl_colorPropertyValue);
}

- (id <MGLStyleAttributeValue>)textHaloColor {
    return [MGLColor mbgl_propertyValue:self.layer->getTextHaloColor()];
}

- (void)setTextHaloWidth:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textHaloWidth {
    self.layer->setTextHaloWidth(textHaloWidth.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textHaloWidth {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextHaloWidth()];
}

- (void)setTextHaloBlur:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textHaloBlur {
    self.layer->setTextHaloBlur(textHaloBlur.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)textHaloBlur {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextHaloBlur()];
}

- (void)setTextTranslate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textTranslate {
    self.layer->setTextTranslate(textTranslate.mbgl_offsetPropertyValue);
}

- (id <MGLStyleAttributeValue>)textTranslate {
    return [NSArray mbgl_offsetPropertyValue:self.layer->getTextTranslate()];
}

- (void)setTextTranslateAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)textTranslateAnchor {
    MGLSetEnumProperty(textTranslateAnchor, TextTranslateAnchor, TranslateAnchorType, MGLSymbolStyleLayerTextTranslateAnchor);
}

- (id <MGLStyleAttributeValue>)textTranslateAnchor {
    auto rawValue = self.layer->getTextTranslateAnchor();
    const char *type = @encode(MGLSymbolStyleLayerTextTranslateAnchor);
    return [NSValue value:&rawValue withObjCType:type];
}

@end
