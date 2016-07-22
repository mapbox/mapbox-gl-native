// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
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


- (void)setSymbolPlacement:(MGLSymbolStyleLayerSymbolPlacement)symbolPlacement
{
    // TODO: setterEnum
}

- (MGLSymbolStyleLayerSymbolPlacement)symbolPlacement
{
    return MGLSymbolStyleLayerSymbolPlacementPoint;
}

- (void)setSymbolSpacing:(CGFloat)symbolSpacing
{
    symbolLayer->setSymbolSpacing(symbolSpacing);
}

- (CGFloat)symbolSpacing
{
    return symbolLayer->getSymbolSpacing().asConstant();
}

- (void)setSymbolAvoidEdges:(BOOL)symbolAvoidEdges
{
    symbolLayer->setSymbolAvoidEdges(symbolAvoidEdges);
}

- (BOOL)symbolAvoidEdges
{
    return symbolLayer->getSymbolAvoidEdges().asConstant();
}

- (void)setIconAllowOverlap:(BOOL)iconAllowOverlap
{
    symbolLayer->setIconAllowOverlap(iconAllowOverlap);
}

- (BOOL)iconAllowOverlap
{
    return symbolLayer->getIconAllowOverlap().asConstant();
}

- (void)setIconIgnorePlacement:(BOOL)iconIgnorePlacement
{
    symbolLayer->setIconIgnorePlacement(iconIgnorePlacement);
}

- (BOOL)iconIgnorePlacement
{
    return symbolLayer->getIconIgnorePlacement().asConstant();
}

- (void)setIconOptional:(BOOL)iconOptional
{
    symbolLayer->setIconOptional(iconOptional);
}

- (BOOL)iconOptional
{
    return symbolLayer->getIconOptional().asConstant();
}

- (void)setIconRotationAlignment:(MGLSymbolStyleLayerIconRotationAlignment)iconRotationAlignment
{
    // TODO: setterEnum
}

- (MGLSymbolStyleLayerIconRotationAlignment)iconRotationAlignment
{
    return MGLSymbolStyleLayerIconRotationAlignmentMap;
}

- (void)setIconSize:(CGFloat)iconSize
{
    symbolLayer->setIconSize(iconSize);
}

- (CGFloat)iconSize
{
    return symbolLayer->getIconSize().asConstant();
}

- (void)setIconTextFit:(MGLSymbolStyleLayerIconTextFit)iconTextFit
{
    // TODO: setterEnum
}

- (MGLSymbolStyleLayerIconTextFit)iconTextFit
{
    return MGLSymbolStyleLayerIconTextFitNone;
}

- (void)setIconTextFitPadding:(NSArray*)iconTextFitPadding
{
    // TODO: setterArray
}

- (NSArray *)iconTextFitPadding
{
    return @[]; // TODO: getterArray
}

- (void)setIconImage:(NSString*)iconImage
{
    symbolLayer->setIconImage(std::string(iconImage.UTF8String));
}

- (NSString *)iconImage
{
    return @(std::string(symbolLayer->getIconImage().asConstant()).c_str());
}

- (void)setIconRotate:(CGFloat)iconRotate
{
    symbolLayer->setIconRotate(iconRotate);
}

- (CGFloat)iconRotate
{
    return symbolLayer->getIconRotate().asConstant();
}

- (void)setIconPadding:(CGFloat)iconPadding
{
    symbolLayer->setIconPadding(iconPadding);
}

- (CGFloat)iconPadding
{
    return symbolLayer->getIconPadding().asConstant();
}

- (void)setIconKeepUpright:(BOOL)iconKeepUpright
{
    symbolLayer->setIconKeepUpright(iconKeepUpright);
}

- (BOOL)iconKeepUpright
{
    return symbolLayer->getIconKeepUpright().asConstant();
}

- (void)setIconOffset:(NSArray*)iconOffset
{
    // TODO: setterArray
}

- (NSArray *)iconOffset
{
    return @[]; // TODO: getterArray
}

- (void)setTextPitchAlignment:(MGLSymbolStyleLayerTextPitchAlignment)textPitchAlignment
{
    // TODO: setterEnum
}

- (MGLSymbolStyleLayerTextPitchAlignment)textPitchAlignment
{
    return MGLSymbolStyleLayerTextPitchAlignmentMap;
}

- (void)setTextRotationAlignment:(MGLSymbolStyleLayerTextRotationAlignment)textRotationAlignment
{
    // TODO: setterEnum
}

- (MGLSymbolStyleLayerTextRotationAlignment)textRotationAlignment
{
    return MGLSymbolStyleLayerTextRotationAlignmentMap;
}

- (void)setTextField:(NSString*)textField
{
    symbolLayer->setTextField(std::string(textField.UTF8String));
}

- (NSString *)textField
{
    return @(std::string(symbolLayer->getTextField().asConstant()).c_str());
}

- (void)setTextFont:(NSArray*)textFont
{
    // TODO: setterArray
}

- (NSArray *)textFont
{
    return @[]; // TODO: getterArray
}

- (void)setTextSize:(CGFloat)textSize
{
    symbolLayer->setTextSize(textSize);
}

- (CGFloat)textSize
{
    return symbolLayer->getTextSize().asConstant();
}

- (void)setTextMaxWidth:(CGFloat)textMaxWidth
{
    symbolLayer->setTextMaxWidth(textMaxWidth);
}

- (CGFloat)textMaxWidth
{
    return symbolLayer->getTextMaxWidth().asConstant();
}

- (void)setTextLineHeight:(CGFloat)textLineHeight
{
    symbolLayer->setTextLineHeight(textLineHeight);
}

- (CGFloat)textLineHeight
{
    return symbolLayer->getTextLineHeight().asConstant();
}

- (void)setTextLetterSpacing:(CGFloat)textLetterSpacing
{
    symbolLayer->setTextLetterSpacing(textLetterSpacing);
}

- (CGFloat)textLetterSpacing
{
    return symbolLayer->getTextLetterSpacing().asConstant();
}

- (void)setTextJustify:(MGLSymbolStyleLayerTextJustify)textJustify
{
    // TODO: setterEnum
}

- (MGLSymbolStyleLayerTextJustify)textJustify
{
    return MGLSymbolStyleLayerTextJustifyLeft;
}

- (void)setTextAnchor:(MGLSymbolStyleLayerTextAnchor)textAnchor
{
    // TODO: setterEnum
}

- (MGLSymbolStyleLayerTextAnchor)textAnchor
{
    return MGLSymbolStyleLayerTextAnchorCenter;
}

- (void)setTextMaxAngle:(CGFloat)textMaxAngle
{
    symbolLayer->setTextMaxAngle(textMaxAngle);
}

- (CGFloat)textMaxAngle
{
    return symbolLayer->getTextMaxAngle().asConstant();
}

- (void)setTextRotate:(CGFloat)textRotate
{
    symbolLayer->setTextRotate(textRotate);
}

- (CGFloat)textRotate
{
    return symbolLayer->getTextRotate().asConstant();
}

- (void)setTextPadding:(CGFloat)textPadding
{
    symbolLayer->setTextPadding(textPadding);
}

- (CGFloat)textPadding
{
    return symbolLayer->getTextPadding().asConstant();
}

- (void)setTextKeepUpright:(BOOL)textKeepUpright
{
    symbolLayer->setTextKeepUpright(textKeepUpright);
}

- (BOOL)textKeepUpright
{
    return symbolLayer->getTextKeepUpright().asConstant();
}

- (void)setTextTransform:(MGLSymbolStyleLayerTextTransform)textTransform
{
    // TODO: setterEnum
}

- (MGLSymbolStyleLayerTextTransform)textTransform
{
    return MGLSymbolStyleLayerTextTransformNone;
}

- (void)setTextOffset:(NSArray*)textOffset
{
    // TODO: setterArray
}

- (NSArray *)textOffset
{
    return @[]; // TODO: getterArray
}

- (void)setTextAllowOverlap:(BOOL)textAllowOverlap
{
    symbolLayer->setTextAllowOverlap(textAllowOverlap);
}

- (BOOL)textAllowOverlap
{
    return symbolLayer->getTextAllowOverlap().asConstant();
}

- (void)setTextIgnorePlacement:(BOOL)textIgnorePlacement
{
    symbolLayer->setTextIgnorePlacement(textIgnorePlacement);
}

- (BOOL)textIgnorePlacement
{
    return symbolLayer->getTextIgnorePlacement().asConstant();
}

- (void)setTextOptional:(BOOL)textOptional
{
    symbolLayer->setTextOptional(textOptional);
}

- (BOOL)textOptional
{
    return symbolLayer->getTextOptional().asConstant();
}
#pragma mark - Accessing the Paint Attributes

- (void)setIconOpacity:(CGFloat)iconOpacity
{
    symbolLayer->setIconOpacity(iconOpacity);
}
- (CGFloat)iconOpacity
{
    return symbolLayer->getIconOpacity().asConstant();
}
- (void)setIconColor:(MGLColor*)iconColor
{
    symbolLayer->setIconColor(iconColor.mbgl_color);
}
- (MGLColor *)iconColor
{
    return [MGLColor mbgl_color:symbolLayer->getIconColor().asConstant()];
}
- (void)setIconHaloColor:(MGLColor*)iconHaloColor
{
    symbolLayer->setIconHaloColor(iconHaloColor.mbgl_color);
}
- (MGLColor *)iconHaloColor
{
    return [MGLColor mbgl_color:symbolLayer->getIconHaloColor().asConstant()];
}
- (void)setIconHaloWidth:(CGFloat)iconHaloWidth
{
    symbolLayer->setIconHaloWidth(iconHaloWidth);
}
- (CGFloat)iconHaloWidth
{
    return symbolLayer->getIconHaloWidth().asConstant();
}
- (void)setIconHaloBlur:(CGFloat)iconHaloBlur
{
    symbolLayer->setIconHaloBlur(iconHaloBlur);
}
- (CGFloat)iconHaloBlur
{
    return symbolLayer->getIconHaloBlur().asConstant();
}
- (void)setIconTranslate:(NSArray*)iconTranslate
{
    // TODO: setterArray
}
- (NSArray *)iconTranslate
{
    return @[]; // TODO: getterArray
}
- (void)setIconTranslateAnchor:(MGLSymbolStyleLayerIconTranslateAnchor)iconTranslateAnchor
{
    // TODO: setterEnum
}
- (MGLSymbolStyleLayerIconTranslateAnchor)iconTranslateAnchor
{
    return MGLSymbolStyleLayerIconTranslateAnchorMap;
}
- (void)setTextOpacity:(CGFloat)textOpacity
{
    symbolLayer->setTextOpacity(textOpacity);
}
- (CGFloat)textOpacity
{
    return symbolLayer->getTextOpacity().asConstant();
}
- (void)setTextColor:(MGLColor*)textColor
{
    symbolLayer->setTextColor(textColor.mbgl_color);
}
- (MGLColor *)textColor
{
    return [MGLColor mbgl_color:symbolLayer->getTextColor().asConstant()];
}
- (void)setTextHaloColor:(MGLColor*)textHaloColor
{
    symbolLayer->setTextHaloColor(textHaloColor.mbgl_color);
}
- (MGLColor *)textHaloColor
{
    return [MGLColor mbgl_color:symbolLayer->getTextHaloColor().asConstant()];
}
- (void)setTextHaloWidth:(CGFloat)textHaloWidth
{
    symbolLayer->setTextHaloWidth(textHaloWidth);
}
- (CGFloat)textHaloWidth
{
    return symbolLayer->getTextHaloWidth().asConstant();
}
- (void)setTextHaloBlur:(CGFloat)textHaloBlur
{
    symbolLayer->setTextHaloBlur(textHaloBlur);
}
- (CGFloat)textHaloBlur
{
    return symbolLayer->getTextHaloBlur().asConstant();
}
- (void)setTextTranslate:(NSArray*)textTranslate
{
    // TODO: setterArray
}
- (NSArray *)textTranslate
{
    return @[]; // TODO: getterArray
}
- (void)setTextTranslateAnchor:(MGLSymbolStyleLayerTextTranslateAnchor)textTranslateAnchor
{
    // TODO: setterEnum
}
- (MGLSymbolStyleLayerTextTranslateAnchor)textTranslateAnchor
{
    return MGLSymbolStyleLayerTextTranslateAnchorMap;
}

@end
