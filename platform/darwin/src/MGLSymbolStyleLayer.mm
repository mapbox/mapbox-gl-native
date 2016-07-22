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
    return 0; // TODO: getterNumber
}

- (void)setSymbolAvoidEdges:(BOOL)symbolAvoidEdges
{
    // TODO: setterBool
}

- (BOOL)symbolAvoidEdges
{
    return YES; // TODO: getterBool
}

- (void)setIconAllowOverlap:(BOOL)iconAllowOverlap
{
    // TODO: setterBool
}

- (BOOL)iconAllowOverlap
{
    return YES; // TODO: getterBool
}

- (void)setIconIgnorePlacement:(BOOL)iconIgnorePlacement
{
    // TODO: setterBool
}

- (BOOL)iconIgnorePlacement
{
    return YES; // TODO: getterBool
}

- (void)setIconOptional:(BOOL)iconOptional
{
    // TODO: setterBool
}

- (BOOL)iconOptional
{
    return YES; // TODO: getterBool
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
    return 0; // TODO: getterNumber
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
    // TODO: setterString
}

- (NSString *)iconImage
{
    return @""; // TODO: getterString
}

- (void)setIconRotate:(CGFloat)iconRotate
{
    symbolLayer->setIconRotate(iconRotate);
}

- (CGFloat)iconRotate
{
    return 0; // TODO: getterNumber
}

- (void)setIconPadding:(CGFloat)iconPadding
{
    symbolLayer->setIconPadding(iconPadding);
}

- (CGFloat)iconPadding
{
    return 0; // TODO: getterNumber
}

- (void)setIconKeepUpright:(BOOL)iconKeepUpright
{
    // TODO: setterBool
}

- (BOOL)iconKeepUpright
{
    return YES; // TODO: getterBool
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
    // TODO: setterString
}

- (NSString *)textField
{
    return @""; // TODO: getterString
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
    return 0; // TODO: getterNumber
}

- (void)setTextMaxWidth:(CGFloat)textMaxWidth
{
    symbolLayer->setTextMaxWidth(textMaxWidth);
}

- (CGFloat)textMaxWidth
{
    return 0; // TODO: getterNumber
}

- (void)setTextLineHeight:(CGFloat)textLineHeight
{
    symbolLayer->setTextLineHeight(textLineHeight);
}

- (CGFloat)textLineHeight
{
    return 0; // TODO: getterNumber
}

- (void)setTextLetterSpacing:(CGFloat)textLetterSpacing
{
    symbolLayer->setTextLetterSpacing(textLetterSpacing);
}

- (CGFloat)textLetterSpacing
{
    return 0; // TODO: getterNumber
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
    return 0; // TODO: getterNumber
}

- (void)setTextRotate:(CGFloat)textRotate
{
    symbolLayer->setTextRotate(textRotate);
}

- (CGFloat)textRotate
{
    return 0; // TODO: getterNumber
}

- (void)setTextPadding:(CGFloat)textPadding
{
    symbolLayer->setTextPadding(textPadding);
}

- (CGFloat)textPadding
{
    return 0; // TODO: getterNumber
}

- (void)setTextKeepUpright:(BOOL)textKeepUpright
{
    // TODO: setterBool
}

- (BOOL)textKeepUpright
{
    return YES; // TODO: getterBool
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
    // TODO: setterBool
}

- (BOOL)textAllowOverlap
{
    return YES; // TODO: getterBool
}

- (void)setTextIgnorePlacement:(BOOL)textIgnorePlacement
{
    // TODO: setterBool
}

- (BOOL)textIgnorePlacement
{
    return YES; // TODO: getterBool
}

- (void)setTextOptional:(BOOL)textOptional
{
    // TODO: setterBool
}

- (BOOL)textOptional
{
    return YES; // TODO: getterBool
}
#pragma mark - Accessing the Paint Attributes

- (void)setIconOpacity:(CGFloat)iconOpacity
{
    symbolLayer->setIconOpacity(iconOpacity);
}
- (CGFloat)iconOpacity
{
    return 0; // TODO: getterNumber
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
    return 0; // TODO: getterNumber
}
- (void)setIconHaloBlur:(CGFloat)iconHaloBlur
{
    symbolLayer->setIconHaloBlur(iconHaloBlur);
}
- (CGFloat)iconHaloBlur
{
    return 0; // TODO: getterNumber
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
    return 0; // TODO: getterNumber
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
    return 0; // TODO: getterNumber
}
- (void)setTextHaloBlur:(CGFloat)textHaloBlur
{
    symbolLayer->setTextHaloBlur(textHaloBlur);
}
- (CGFloat)textHaloBlur
{
    return 0; // TODO: getterNumber
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
