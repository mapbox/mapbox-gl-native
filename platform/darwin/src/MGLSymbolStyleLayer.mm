// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLSymbolStyleLayer.h"
#import <mbgl/style/layers/symbol_layer.hpp>

@interface MGLSymbolStyleLayer()
@property (nonatomic) mbgl::style::SymbolLayer *layer;
@end

@implementation MGLSymbolStyleLayer

#pragma mark - Accessing the Layout Attributes


- (void)setSymbolPlacement:(id <MGLStyleAttributeValue>)symbolPlacement
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)symbolPlacement
{
    auto rawValue = self.layer->getSymbolPlacement();
    const char *type = @encode(MGLSymbolStyleLayerSymbolPlacement);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setSymbolSpacing:(id <MGLStyleAttributeValue>)symbolSpacing
{
    self.layer->setSymbolSpacing(symbolSpacing.numberValue.mbgl_numberPropertyValue);
}

- (id <MGLStyleAttributeValue>)symbolSpacing
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getSymbolSpacing()];
}

- (void)setSymbolAvoidEdges:(id <MGLStyleAttributeValue>)symbolAvoidEdges
{
    self.layer->setSymbolAvoidEdges(symbolAvoidEdges.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)symbolAvoidEdges
{
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getSymbolAvoidEdges()];
}

- (void)setIconAllowOverlap:(id <MGLStyleAttributeValue>)iconAllowOverlap
{
    self.layer->setIconAllowOverlap(iconAllowOverlap.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconAllowOverlap
{
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getIconAllowOverlap()];
}

- (void)setIconIgnorePlacement:(id <MGLStyleAttributeValue>)iconIgnorePlacement
{
    self.layer->setIconIgnorePlacement(iconIgnorePlacement.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconIgnorePlacement
{
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getIconIgnorePlacement()];
}

- (void)setIconOptional:(id <MGLStyleAttributeValue>)iconOptional
{
    self.layer->setIconOptional(iconOptional.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconOptional
{
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getIconOptional()];
}

- (void)setIconRotationAlignment:(id <MGLStyleAttributeValue>)iconRotationAlignment
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)iconRotationAlignment
{
    auto rawValue = self.layer->getIconRotationAlignment();
    const char *type = @encode(MGLSymbolStyleLayerIconRotationAlignment);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setIconSize:(id <MGLStyleAttributeValue>)iconSize
{
    self.layer->setIconSize(iconSize.numberValue.mbgl_numberPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconSize
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getIconSize()];
}

- (void)setIconTextFit:(id <MGLStyleAttributeValue>)iconTextFit
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)iconTextFit
{
    auto rawValue = self.layer->getIconTextFit();
    const char *type = @encode(MGLSymbolStyleLayerIconTextFit);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setIconTextFitPadding:(id <MGLStyleAttributeValue>)iconTextFitPadding
{
    // TODO: setterArray
}

- (id <MGLStyleAttributeValue>)iconTextFitPadding
{
    return @[];
}

- (void)setIconImage:(id <MGLStyleAttributeValue>)iconImage
{
    self.layer->setIconImage(iconImage.stringValue.mbgl_stringPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconImage
{
    return [NSString mbgl_stringWithPropertyValue:self.layer->getIconImage()];
}

- (void)setIconRotate:(id <MGLStyleAttributeValue>)iconRotate
{
    self.layer->setIconRotate(iconRotate.numberValue.mbgl_numberPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconRotate
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getIconRotate()];
}

- (void)setIconPadding:(id <MGLStyleAttributeValue>)iconPadding
{
    self.layer->setIconPadding(iconPadding.numberValue.mbgl_numberPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconPadding
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getIconPadding()];
}

- (void)setIconKeepUpright:(id <MGLStyleAttributeValue>)iconKeepUpright
{
    self.layer->setIconKeepUpright(iconKeepUpright.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconKeepUpright
{
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getIconKeepUpright()];
}

- (void)setIconOffset:(id <MGLStyleAttributeValue>)iconOffset
{
    // TODO: setterArray
}

- (id <MGLStyleAttributeValue>)iconOffset
{
    return @[];
}

- (void)setTextPitchAlignment:(id <MGLStyleAttributeValue>)textPitchAlignment
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)textPitchAlignment
{
    auto rawValue = self.layer->getTextPitchAlignment();
    const char *type = @encode(MGLSymbolStyleLayerTextPitchAlignment);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setTextRotationAlignment:(id <MGLStyleAttributeValue>)textRotationAlignment
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)textRotationAlignment
{
    auto rawValue = self.layer->getTextRotationAlignment();
    const char *type = @encode(MGLSymbolStyleLayerTextRotationAlignment);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setTextField:(id <MGLStyleAttributeValue>)textField
{
    self.layer->setTextField(textField.stringValue.mbgl_stringPropertyValue);
}

- (id <MGLStyleAttributeValue>)textField
{
    return [NSString mbgl_stringWithPropertyValue:self.layer->getTextField()];
}

- (void)setTextFont:(id <MGLStyleAttributeValue>)textFont
{
    // TODO: setterArray
}

- (id <MGLStyleAttributeValue>)textFont
{
    return @[];
}

- (void)setTextSize:(id <MGLStyleAttributeValue>)textSize
{
    self.layer->setTextSize(textSize.numberValue.mbgl_numberPropertyValue);
}

- (id <MGLStyleAttributeValue>)textSize
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextSize()];
}

- (void)setTextMaxWidth:(id <MGLStyleAttributeValue>)textMaxWidth
{
    self.layer->setTextMaxWidth(textMaxWidth.numberValue.mbgl_numberPropertyValue);
}

- (id <MGLStyleAttributeValue>)textMaxWidth
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextMaxWidth()];
}

- (void)setTextLineHeight:(id <MGLStyleAttributeValue>)textLineHeight
{
    self.layer->setTextLineHeight(textLineHeight.numberValue.mbgl_numberPropertyValue);
}

- (id <MGLStyleAttributeValue>)textLineHeight
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextLineHeight()];
}

- (void)setTextLetterSpacing:(id <MGLStyleAttributeValue>)textLetterSpacing
{
    self.layer->setTextLetterSpacing(textLetterSpacing.numberValue.mbgl_numberPropertyValue);
}

- (id <MGLStyleAttributeValue>)textLetterSpacing
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextLetterSpacing()];
}

- (void)setTextJustify:(id <MGLStyleAttributeValue>)textJustify
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)textJustify
{
    auto rawValue = self.layer->getTextJustify();
    const char *type = @encode(MGLSymbolStyleLayerTextJustify);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setTextAnchor:(id <MGLStyleAttributeValue>)textAnchor
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)textAnchor
{
    auto rawValue = self.layer->getTextAnchor();
    const char *type = @encode(MGLSymbolStyleLayerTextAnchor);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setTextMaxAngle:(id <MGLStyleAttributeValue>)textMaxAngle
{
    self.layer->setTextMaxAngle(textMaxAngle.numberValue.mbgl_numberPropertyValue);
}

- (id <MGLStyleAttributeValue>)textMaxAngle
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextMaxAngle()];
}

- (void)setTextRotate:(id <MGLStyleAttributeValue>)textRotate
{
    self.layer->setTextRotate(textRotate.numberValue.mbgl_numberPropertyValue);
}

- (id <MGLStyleAttributeValue>)textRotate
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextRotate()];
}

- (void)setTextPadding:(id <MGLStyleAttributeValue>)textPadding
{
    self.layer->setTextPadding(textPadding.numberValue.mbgl_numberPropertyValue);
}

- (id <MGLStyleAttributeValue>)textPadding
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextPadding()];
}

- (void)setTextKeepUpright:(id <MGLStyleAttributeValue>)textKeepUpright
{
    self.layer->setTextKeepUpright(textKeepUpright.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)textKeepUpright
{
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getTextKeepUpright()];
}

- (void)setTextTransform:(id <MGLStyleAttributeValue>)textTransform
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)textTransform
{
    auto rawValue = self.layer->getTextTransform();
    const char *type = @encode(MGLSymbolStyleLayerTextTransform);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setTextOffset:(id <MGLStyleAttributeValue>)textOffset
{
    // TODO: setterArray
}

- (id <MGLStyleAttributeValue>)textOffset
{
    return @[];
}

- (void)setTextAllowOverlap:(id <MGLStyleAttributeValue>)textAllowOverlap
{
    self.layer->setTextAllowOverlap(textAllowOverlap.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)textAllowOverlap
{
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getTextAllowOverlap()];
}

- (void)setTextIgnorePlacement:(id <MGLStyleAttributeValue>)textIgnorePlacement
{
    self.layer->setTextIgnorePlacement(textIgnorePlacement.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)textIgnorePlacement
{
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getTextIgnorePlacement()];
}

- (void)setTextOptional:(id <MGLStyleAttributeValue>)textOptional
{
    self.layer->setTextOptional(textOptional.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)textOptional
{
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getTextOptional()];
}
#pragma mark - Accessing the Paint Attributes

- (void)setIconOpacity:(id <MGLStyleAttributeValue>)iconOpacity
{
    self.layer->setIconOpacity(iconOpacity.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)iconOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getIconOpacity()];
}
- (void)setIconColor:(id <MGLStyleAttributeValue>)iconColor
{
    self.layer->setIconColor(iconColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)iconColor
{
    return [MGLColor mbgl_propertyValue:self.layer->getIconColor()];
}
- (void)setIconHaloColor:(id <MGLStyleAttributeValue>)iconHaloColor
{
    self.layer->setIconHaloColor(iconHaloColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)iconHaloColor
{
    return [MGLColor mbgl_propertyValue:self.layer->getIconHaloColor()];
}
- (void)setIconHaloWidth:(id <MGLStyleAttributeValue>)iconHaloWidth
{
    self.layer->setIconHaloWidth(iconHaloWidth.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)iconHaloWidth
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getIconHaloWidth()];
}
- (void)setIconHaloBlur:(id <MGLStyleAttributeValue>)iconHaloBlur
{
    self.layer->setIconHaloBlur(iconHaloBlur.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)iconHaloBlur
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getIconHaloBlur()];
}
- (void)setIconTranslate:(id <MGLStyleAttributeValue>)iconTranslate
{
    // TODO: setterArray
}
- (id <MGLStyleAttributeValue>)iconTranslate
{
    return @[];
}
- (void)setIconTranslateAnchor:(id <MGLStyleAttributeValue>)iconTranslateAnchor
{
    // TODO: setterEnum
}
- (id <MGLStyleAttributeValue>)iconTranslateAnchor
{
    auto rawValue = self.layer->getIconTranslateAnchor();
    const char *type = @encode(MGLSymbolStyleLayerIconTranslateAnchor);
    return [NSValue value:&rawValue withObjCType:type];
}
- (void)setTextOpacity:(id <MGLStyleAttributeValue>)textOpacity
{
    self.layer->setTextOpacity(textOpacity.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)textOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextOpacity()];
}
- (void)setTextColor:(id <MGLStyleAttributeValue>)textColor
{
    self.layer->setTextColor(textColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)textColor
{
    return [MGLColor mbgl_propertyValue:self.layer->getTextColor()];
}
- (void)setTextHaloColor:(id <MGLStyleAttributeValue>)textHaloColor
{
    self.layer->setTextHaloColor(textHaloColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)textHaloColor
{
    return [MGLColor mbgl_propertyValue:self.layer->getTextHaloColor()];
}
- (void)setTextHaloWidth:(id <MGLStyleAttributeValue>)textHaloWidth
{
    self.layer->setTextHaloWidth(textHaloWidth.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)textHaloWidth
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextHaloWidth()];
}
- (void)setTextHaloBlur:(id <MGLStyleAttributeValue>)textHaloBlur
{
    self.layer->setTextHaloBlur(textHaloBlur.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)textHaloBlur
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getTextHaloBlur()];
}
- (void)setTextTranslate:(id <MGLStyleAttributeValue>)textTranslate
{
    // TODO: setterArray
}
- (id <MGLStyleAttributeValue>)textTranslate
{
    return @[];
}
- (void)setTextTranslateAnchor:(id <MGLStyleAttributeValue>)textTranslateAnchor
{
    // TODO: setterEnum
}
- (id <MGLStyleAttributeValue>)textTranslateAnchor
{
    auto rawValue = self.layer->getTextTranslateAnchor();
    const char *type = @encode(MGLSymbolStyleLayerTextTranslateAnchor);
    return [NSValue value:&rawValue withObjCType:type];
}

@end
