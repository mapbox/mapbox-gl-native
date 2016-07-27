// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "NSNumber+MGLStyleAttributeAdditions_Private.hpp"
#import "NSArray+MGLStyleAttributeAdditions_Private.hpp"
#import "NSString+MGLStyleAttributeAdditions_Private.hpp"
#import "MGLSymbolStyleLayer.h"
#import <mbgl/style/layers/symbol_layer.hpp>

@interface MGLSymbolStyleLayer() {
    mbgl::style::SymbolLayer *symbolLayer;
}
@end

@implementation MGLSymbolStyleLayer

- (void)setLayer:(mbgl::style::Layer *)layer
{
    symbolLayer = reinterpret_cast<mbgl::style::SymbolLayer *>(layer);
}

#pragma mark - Accessing the Layout Attributes


- (void)setSymbolPlacement:(id <MGLStyleAttributeValue>)symbolPlacement
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)symbolPlacement
{
    return @0;
}

- (void)setSymbolSpacing:(id <MGLStyleAttributeValue>)symbolSpacing
{
    // symbolLayer->setSymbolSpacing(symbolSpacing.number.floatValue);
}

- (id <MGLStyleAttributeValue>)symbolSpacing
{
    return [NSNumber mbgl_numberWithPropertyValue:symbolLayer->getSymbolSpacing()];
}

- (void)setSymbolAvoidEdges:(id <MGLStyleAttributeValue>)symbolAvoidEdges
{
    symbolLayer->setSymbolAvoidEdges(symbolAvoidEdges.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)symbolAvoidEdges
{
    return [NSNumber mbgl_booleanWithPropertyValue:symbolLayer->getSymbolAvoidEdges()];
}

- (void)setIconAllowOverlap:(id <MGLStyleAttributeValue>)iconAllowOverlap
{
    symbolLayer->setIconAllowOverlap(iconAllowOverlap.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconAllowOverlap
{
    return [NSNumber mbgl_booleanWithPropertyValue:symbolLayer->getIconAllowOverlap()];
}

- (void)setIconIgnorePlacement:(id <MGLStyleAttributeValue>)iconIgnorePlacement
{
    symbolLayer->setIconIgnorePlacement(iconIgnorePlacement.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconIgnorePlacement
{
    return [NSNumber mbgl_booleanWithPropertyValue:symbolLayer->getIconIgnorePlacement()];
}

- (void)setIconOptional:(id <MGLStyleAttributeValue>)iconOptional
{
    symbolLayer->setIconOptional(iconOptional.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconOptional
{
    return [NSNumber mbgl_booleanWithPropertyValue:symbolLayer->getIconOptional()];
}

- (void)setIconRotationAlignment:(id <MGLStyleAttributeValue>)iconRotationAlignment
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)iconRotationAlignment
{
    return @0;
}

- (void)setIconSize:(id <MGLStyleAttributeValue>)iconSize
{
    // symbolLayer->setIconSize(iconSize.number.floatValue);
}

- (id <MGLStyleAttributeValue>)iconSize
{
    return [NSNumber mbgl_numberWithPropertyValue:symbolLayer->getIconSize()];
}

- (void)setIconTextFit:(id <MGLStyleAttributeValue>)iconTextFit
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)iconTextFit
{
    return @0;
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
    // symbolLayer->setIconImage(std::string(iconImage.string.UTF8String));
}

- (id <MGLStyleAttributeValue>)iconImage
{
    return @"";
}

- (void)setIconRotate:(id <MGLStyleAttributeValue>)iconRotate
{
    // symbolLayer->setIconRotate(iconRotate.number.floatValue);
}

- (id <MGLStyleAttributeValue>)iconRotate
{
    return [NSNumber mbgl_numberWithPropertyValue:symbolLayer->getIconRotate()];
}

- (void)setIconPadding:(id <MGLStyleAttributeValue>)iconPadding
{
    // symbolLayer->setIconPadding(iconPadding.number.floatValue);
}

- (id <MGLStyleAttributeValue>)iconPadding
{
    return [NSNumber mbgl_numberWithPropertyValue:symbolLayer->getIconPadding()];
}

- (void)setIconKeepUpright:(id <MGLStyleAttributeValue>)iconKeepUpright
{
    symbolLayer->setIconKeepUpright(iconKeepUpright.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)iconKeepUpright
{
    return [NSNumber mbgl_booleanWithPropertyValue:symbolLayer->getIconKeepUpright()];
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
    return @0;
}

- (void)setTextRotationAlignment:(id <MGLStyleAttributeValue>)textRotationAlignment
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)textRotationAlignment
{
    return @0;
}

- (void)setTextField:(id <MGLStyleAttributeValue>)textField
{
    // symbolLayer->setTextField(std::string(textField.string.UTF8String));
}

- (id <MGLStyleAttributeValue>)textField
{
    return @"";
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
    // symbolLayer->setTextSize(textSize.number.floatValue);
}

- (id <MGLStyleAttributeValue>)textSize
{
    return [NSNumber mbgl_numberWithPropertyValue:symbolLayer->getTextSize()];
}

- (void)setTextMaxWidth:(id <MGLStyleAttributeValue>)textMaxWidth
{
    // symbolLayer->setTextMaxWidth(textMaxWidth.number.floatValue);
}

- (id <MGLStyleAttributeValue>)textMaxWidth
{
    return [NSNumber mbgl_numberWithPropertyValue:symbolLayer->getTextMaxWidth()];
}

- (void)setTextLineHeight:(id <MGLStyleAttributeValue>)textLineHeight
{
    // symbolLayer->setTextLineHeight(textLineHeight.number.floatValue);
}

- (id <MGLStyleAttributeValue>)textLineHeight
{
    return [NSNumber mbgl_numberWithPropertyValue:symbolLayer->getTextLineHeight()];
}

- (void)setTextLetterSpacing:(id <MGLStyleAttributeValue>)textLetterSpacing
{
    // symbolLayer->setTextLetterSpacing(textLetterSpacing.number.floatValue);
}

- (id <MGLStyleAttributeValue>)textLetterSpacing
{
    return [NSNumber mbgl_numberWithPropertyValue:symbolLayer->getTextLetterSpacing()];
}

- (void)setTextJustify:(id <MGLStyleAttributeValue>)textJustify
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)textJustify
{
    return @0;
}

- (void)setTextAnchor:(id <MGLStyleAttributeValue>)textAnchor
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)textAnchor
{
    return @0;
}

- (void)setTextMaxAngle:(id <MGLStyleAttributeValue>)textMaxAngle
{
    // symbolLayer->setTextMaxAngle(textMaxAngle.number.floatValue);
}

- (id <MGLStyleAttributeValue>)textMaxAngle
{
    return [NSNumber mbgl_numberWithPropertyValue:symbolLayer->getTextMaxAngle()];
}

- (void)setTextRotate:(id <MGLStyleAttributeValue>)textRotate
{
    // symbolLayer->setTextRotate(textRotate.number.floatValue);
}

- (id <MGLStyleAttributeValue>)textRotate
{
    return [NSNumber mbgl_numberWithPropertyValue:symbolLayer->getTextRotate()];
}

- (void)setTextPadding:(id <MGLStyleAttributeValue>)textPadding
{
    // symbolLayer->setTextPadding(textPadding.number.floatValue);
}

- (id <MGLStyleAttributeValue>)textPadding
{
    return [NSNumber mbgl_numberWithPropertyValue:symbolLayer->getTextPadding()];
}

- (void)setTextKeepUpright:(id <MGLStyleAttributeValue>)textKeepUpright
{
    symbolLayer->setTextKeepUpright(textKeepUpright.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)textKeepUpright
{
    return [NSNumber mbgl_booleanWithPropertyValue:symbolLayer->getTextKeepUpright()];
}

- (void)setTextTransform:(id <MGLStyleAttributeValue>)textTransform
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)textTransform
{
    return @0;
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
    symbolLayer->setTextAllowOverlap(textAllowOverlap.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)textAllowOverlap
{
    return [NSNumber mbgl_booleanWithPropertyValue:symbolLayer->getTextAllowOverlap()];
}

- (void)setTextIgnorePlacement:(id <MGLStyleAttributeValue>)textIgnorePlacement
{
    symbolLayer->setTextIgnorePlacement(textIgnorePlacement.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)textIgnorePlacement
{
    return [NSNumber mbgl_booleanWithPropertyValue:symbolLayer->getTextIgnorePlacement()];
}

- (void)setTextOptional:(id <MGLStyleAttributeValue>)textOptional
{
    symbolLayer->setTextOptional(textOptional.numberValue.mbgl_booleanPropertyValue);
}

- (id <MGLStyleAttributeValue>)textOptional
{
    return [NSNumber mbgl_booleanWithPropertyValue:symbolLayer->getTextOptional()];
}
#pragma mark - Accessing the Paint Attributes

- (void)setIconOpacity:(id <MGLStyleAttributeValue>)iconOpacity
{
    // symbolLayer->setIconOpacity(iconOpacity.number.floatValue);
}
- (id <MGLStyleAttributeValue>)iconOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:symbolLayer->getIconOpacity()];
}
- (void)setIconColor:(id <MGLStyleAttributeValue>)iconColor
{
    symbolLayer->setIconColor(iconColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)iconColor
{
    return [MGLColor mbgl_propertyValue:symbolLayer->getIconColor()];
}
- (void)setIconHaloColor:(id <MGLStyleAttributeValue>)iconHaloColor
{
    symbolLayer->setIconHaloColor(iconHaloColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)iconHaloColor
{
    return [MGLColor mbgl_propertyValue:symbolLayer->getIconHaloColor()];
}
- (void)setIconHaloWidth:(id <MGLStyleAttributeValue>)iconHaloWidth
{
    // symbolLayer->setIconHaloWidth(iconHaloWidth.number.floatValue);
}
- (id <MGLStyleAttributeValue>)iconHaloWidth
{
    return [NSNumber mbgl_numberWithPropertyValue:symbolLayer->getIconHaloWidth()];
}
- (void)setIconHaloBlur:(id <MGLStyleAttributeValue>)iconHaloBlur
{
    // symbolLayer->setIconHaloBlur(iconHaloBlur.number.floatValue);
}
- (id <MGLStyleAttributeValue>)iconHaloBlur
{
    return [NSNumber mbgl_numberWithPropertyValue:symbolLayer->getIconHaloBlur()];
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
    return @0;
}
- (void)setTextOpacity:(id <MGLStyleAttributeValue>)textOpacity
{
    // symbolLayer->setTextOpacity(textOpacity.number.floatValue);
}
- (id <MGLStyleAttributeValue>)textOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:symbolLayer->getTextOpacity()];
}
- (void)setTextColor:(id <MGLStyleAttributeValue>)textColor
{
    symbolLayer->setTextColor(textColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)textColor
{
    return [MGLColor mbgl_propertyValue:symbolLayer->getTextColor()];
}
- (void)setTextHaloColor:(id <MGLStyleAttributeValue>)textHaloColor
{
    symbolLayer->setTextHaloColor(textHaloColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)textHaloColor
{
    return [MGLColor mbgl_propertyValue:symbolLayer->getTextHaloColor()];
}
- (void)setTextHaloWidth:(id <MGLStyleAttributeValue>)textHaloWidth
{
    // symbolLayer->setTextHaloWidth(textHaloWidth.number.floatValue);
}
- (id <MGLStyleAttributeValue>)textHaloWidth
{
    return [NSNumber mbgl_numberWithPropertyValue:symbolLayer->getTextHaloWidth()];
}
- (void)setTextHaloBlur:(id <MGLStyleAttributeValue>)textHaloBlur
{
    // symbolLayer->setTextHaloBlur(textHaloBlur.number.floatValue);
}
- (id <MGLStyleAttributeValue>)textHaloBlur
{
    return [NSNumber mbgl_numberWithPropertyValue:symbolLayer->getTextHaloBlur()];
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
    return @0;
}

@end
