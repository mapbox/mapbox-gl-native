// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLStyleAttribute.hpp"
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
    return [[MGLStyleAttribute alloc] init]; //return MGLSymbolStyleLayerSymbolPlacementPoint;
}

- (void)setSymbolSpacing:(id <MGLStyleAttributeValue>)symbolSpacing
{
    // symbolLayer->setSymbolSpacing(symbolSpacing.number.floatValue);
}

- (id <MGLStyleAttributeValue>)symbolSpacing
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getSymbolSpacing().asConstant();
}

- (void)setSymbolAvoidEdges:(id <MGLStyleAttributeValue>)symbolAvoidEdges
{
    // symbolLayer->setSymbolAvoidEdges(symbolAvoidEdges);
}

- (id <MGLStyleAttributeValue>)symbolAvoidEdges
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getSymbolAvoidEdges().asConstant();
}

- (void)setIconAllowOverlap:(id <MGLStyleAttributeValue>)iconAllowOverlap
{
    // symbolLayer->setIconAllowOverlap(iconAllowOverlap);
}

- (id <MGLStyleAttributeValue>)iconAllowOverlap
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getIconAllowOverlap().asConstant();
}

- (void)setIconIgnorePlacement:(id <MGLStyleAttributeValue>)iconIgnorePlacement
{
    // symbolLayer->setIconIgnorePlacement(iconIgnorePlacement);
}

- (id <MGLStyleAttributeValue>)iconIgnorePlacement
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getIconIgnorePlacement().asConstant();
}

- (void)setIconOptional:(id <MGLStyleAttributeValue>)iconOptional
{
    // symbolLayer->setIconOptional(iconOptional);
}

- (id <MGLStyleAttributeValue>)iconOptional
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getIconOptional().asConstant();
}

- (void)setIconRotationAlignment:(id <MGLStyleAttributeValue>)iconRotationAlignment
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)iconRotationAlignment
{
    return [[MGLStyleAttribute alloc] init]; //return MGLSymbolStyleLayerIconRotationAlignmentMap;
}

- (void)setIconSize:(id <MGLStyleAttributeValue>)iconSize
{
    // symbolLayer->setIconSize(iconSize.number.floatValue);
}

- (id <MGLStyleAttributeValue>)iconSize
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getIconSize().asConstant();
}

- (void)setIconTextFit:(id <MGLStyleAttributeValue>)iconTextFit
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)iconTextFit
{
    return [[MGLStyleAttribute alloc] init]; //return MGLSymbolStyleLayerIconTextFitNone;
}

- (void)setIconTextFitPadding:(id <MGLStyleAttributeValue>)iconTextFitPadding
{
    // TODO: setterArray
}

- (id <MGLStyleAttributeValue>)iconTextFitPadding
{
    return [[MGLStyleAttribute alloc] init]; //return @[]; // TODO: getterArray
}

- (void)setIconImage:(id <MGLStyleAttributeValue>)iconImage
{
    // symbolLayer->setIconImage(std::string(iconImage.string.UTF8String));
}

- (id <MGLStyleAttributeValue>)iconImage
{
    return [[MGLStyleAttribute alloc] init]; //return @(std::string(symbolLayer->getIconImage().asConstant()).c_str());
}

- (void)setIconRotate:(id <MGLStyleAttributeValue>)iconRotate
{
    // symbolLayer->setIconRotate(iconRotate.number.floatValue);
}

- (id <MGLStyleAttributeValue>)iconRotate
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getIconRotate().asConstant();
}

- (void)setIconPadding:(id <MGLStyleAttributeValue>)iconPadding
{
    // symbolLayer->setIconPadding(iconPadding.number.floatValue);
}

- (id <MGLStyleAttributeValue>)iconPadding
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getIconPadding().asConstant();
}

- (void)setIconKeepUpright:(id <MGLStyleAttributeValue>)iconKeepUpright
{
    // symbolLayer->setIconKeepUpright(iconKeepUpright);
}

- (id <MGLStyleAttributeValue>)iconKeepUpright
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getIconKeepUpright().asConstant();
}

- (void)setIconOffset:(id <MGLStyleAttributeValue>)iconOffset
{
    // TODO: setterArray
}

- (id <MGLStyleAttributeValue>)iconOffset
{
    return [[MGLStyleAttribute alloc] init]; //return @[]; // TODO: getterArray
}

- (void)setTextPitchAlignment:(id <MGLStyleAttributeValue>)textPitchAlignment
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)textPitchAlignment
{
    return [[MGLStyleAttribute alloc] init]; //return MGLSymbolStyleLayerTextPitchAlignmentMap;
}

- (void)setTextRotationAlignment:(id <MGLStyleAttributeValue>)textRotationAlignment
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)textRotationAlignment
{
    return [[MGLStyleAttribute alloc] init]; //return MGLSymbolStyleLayerTextRotationAlignmentMap;
}

- (void)setTextField:(id <MGLStyleAttributeValue>)textField
{
    // symbolLayer->setTextField(std::string(textField.string.UTF8String));
}

- (id <MGLStyleAttributeValue>)textField
{
    return [[MGLStyleAttribute alloc] init]; //return @(std::string(symbolLayer->getTextField().asConstant()).c_str());
}

- (void)setTextFont:(id <MGLStyleAttributeValue>)textFont
{
    // TODO: setterArray
}

- (id <MGLStyleAttributeValue>)textFont
{
    return [[MGLStyleAttribute alloc] init]; //return @[]; // TODO: getterArray
}

- (void)setTextSize:(id <MGLStyleAttributeValue>)textSize
{
    // symbolLayer->setTextSize(textSize.number.floatValue);
}

- (id <MGLStyleAttributeValue>)textSize
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getTextSize().asConstant();
}

- (void)setTextMaxWidth:(id <MGLStyleAttributeValue>)textMaxWidth
{
    // symbolLayer->setTextMaxWidth(textMaxWidth.number.floatValue);
}

- (id <MGLStyleAttributeValue>)textMaxWidth
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getTextMaxWidth().asConstant();
}

- (void)setTextLineHeight:(id <MGLStyleAttributeValue>)textLineHeight
{
    // symbolLayer->setTextLineHeight(textLineHeight.number.floatValue);
}

- (id <MGLStyleAttributeValue>)textLineHeight
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getTextLineHeight().asConstant();
}

- (void)setTextLetterSpacing:(id <MGLStyleAttributeValue>)textLetterSpacing
{
    // symbolLayer->setTextLetterSpacing(textLetterSpacing.number.floatValue);
}

- (id <MGLStyleAttributeValue>)textLetterSpacing
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getTextLetterSpacing().asConstant();
}

- (void)setTextJustify:(id <MGLStyleAttributeValue>)textJustify
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)textJustify
{
    return [[MGLStyleAttribute alloc] init]; //return MGLSymbolStyleLayerTextJustifyLeft;
}

- (void)setTextAnchor:(id <MGLStyleAttributeValue>)textAnchor
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)textAnchor
{
    return [[MGLStyleAttribute alloc] init]; //return MGLSymbolStyleLayerTextAnchorCenter;
}

- (void)setTextMaxAngle:(id <MGLStyleAttributeValue>)textMaxAngle
{
    // symbolLayer->setTextMaxAngle(textMaxAngle.number.floatValue);
}

- (id <MGLStyleAttributeValue>)textMaxAngle
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getTextMaxAngle().asConstant();
}

- (void)setTextRotate:(id <MGLStyleAttributeValue>)textRotate
{
    // symbolLayer->setTextRotate(textRotate.number.floatValue);
}

- (id <MGLStyleAttributeValue>)textRotate
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getTextRotate().asConstant();
}

- (void)setTextPadding:(id <MGLStyleAttributeValue>)textPadding
{
    // symbolLayer->setTextPadding(textPadding.number.floatValue);
}

- (id <MGLStyleAttributeValue>)textPadding
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getTextPadding().asConstant();
}

- (void)setTextKeepUpright:(id <MGLStyleAttributeValue>)textKeepUpright
{
    // symbolLayer->setTextKeepUpright(textKeepUpright);
}

- (id <MGLStyleAttributeValue>)textKeepUpright
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getTextKeepUpright().asConstant();
}

- (void)setTextTransform:(id <MGLStyleAttributeValue>)textTransform
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)textTransform
{
    return [[MGLStyleAttribute alloc] init]; //return MGLSymbolStyleLayerTextTransformNone;
}

- (void)setTextOffset:(id <MGLStyleAttributeValue>)textOffset
{
    // TODO: setterArray
}

- (id <MGLStyleAttributeValue>)textOffset
{
    return [[MGLStyleAttribute alloc] init]; //return @[]; // TODO: getterArray
}

- (void)setTextAllowOverlap:(id <MGLStyleAttributeValue>)textAllowOverlap
{
    // symbolLayer->setTextAllowOverlap(textAllowOverlap);
}

- (id <MGLStyleAttributeValue>)textAllowOverlap
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getTextAllowOverlap().asConstant();
}

- (void)setTextIgnorePlacement:(id <MGLStyleAttributeValue>)textIgnorePlacement
{
    // symbolLayer->setTextIgnorePlacement(textIgnorePlacement);
}

- (id <MGLStyleAttributeValue>)textIgnorePlacement
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getTextIgnorePlacement().asConstant();
}

- (void)setTextOptional:(id <MGLStyleAttributeValue>)textOptional
{
    // symbolLayer->setTextOptional(textOptional);
}

- (id <MGLStyleAttributeValue>)textOptional
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getTextOptional().asConstant();
}
#pragma mark - Accessing the Paint Attributes

- (void)setIconOpacity:(id <MGLStyleAttributeValue>)iconOpacity
{
    // symbolLayer->setIconOpacity(iconOpacity.number.floatValue);
}
- (id <MGLStyleAttributeValue>)iconOpacity
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getIconOpacity().asConstant();
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
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getIconHaloWidth().asConstant();
}
- (void)setIconHaloBlur:(id <MGLStyleAttributeValue>)iconHaloBlur
{
    // symbolLayer->setIconHaloBlur(iconHaloBlur.number.floatValue);
}
- (id <MGLStyleAttributeValue>)iconHaloBlur
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getIconHaloBlur().asConstant();
}
- (void)setIconTranslate:(id <MGLStyleAttributeValue>)iconTranslate
{
    // TODO: setterArray
}
- (id <MGLStyleAttributeValue>)iconTranslate
{
    return [[MGLStyleAttribute alloc] init]; //return @[]; // TODO: getterArray
}
- (void)setIconTranslateAnchor:(id <MGLStyleAttributeValue>)iconTranslateAnchor
{
    // TODO: setterEnum
}
- (id <MGLStyleAttributeValue>)iconTranslateAnchor
{
    return [[MGLStyleAttribute alloc] init]; //return MGLSymbolStyleLayerIconTranslateAnchorMap;
}
- (void)setTextOpacity:(id <MGLStyleAttributeValue>)textOpacity
{
    // symbolLayer->setTextOpacity(textOpacity.number.floatValue);
}
- (id <MGLStyleAttributeValue>)textOpacity
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getTextOpacity().asConstant();
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
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getTextHaloWidth().asConstant();
}
- (void)setTextHaloBlur:(id <MGLStyleAttributeValue>)textHaloBlur
{
    // symbolLayer->setTextHaloBlur(textHaloBlur.number.floatValue);
}
- (id <MGLStyleAttributeValue>)textHaloBlur
{
    return [[MGLStyleAttribute alloc] init]; //return symbolLayer->getTextHaloBlur().asConstant();
}
- (void)setTextTranslate:(id <MGLStyleAttributeValue>)textTranslate
{
    // TODO: setterArray
}
- (id <MGLStyleAttributeValue>)textTranslate
{
    return [[MGLStyleAttribute alloc] init]; //return @[]; // TODO: getterArray
}
- (void)setTextTranslateAnchor:(id <MGLStyleAttributeValue>)textTranslateAnchor
{
    // TODO: setterEnum
}
- (id <MGLStyleAttributeValue>)textTranslateAnchor
{
    return [[MGLStyleAttribute alloc] init]; //return MGLSymbolStyleLayerTextTranslateAnchorMap;
}

@end
