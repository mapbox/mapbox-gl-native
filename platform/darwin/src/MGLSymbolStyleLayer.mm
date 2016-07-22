// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-ios`.

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
}

- (MGLSymbolStyleLayerSymbolPlacement)symbolPlacement
{
    // TODO:
    return MGLSymbolStyleLayerSymbolPlacementPoint;
}

- (void)setSymbolSpacing:(CGFloat)symbolSpacing
{
    symbolLayer->setSymbolSpacing(symbolSpacing);
}

- (CGFloat)symbolSpacing
{
    // TODO:
    return 0;
}

- (void)setSymbolAvoidEdges:(BOOL)symbolAvoidEdges
{
 // TODO: Boolean
}

- (BOOL)symbolAvoidEdges
{
    // TODO:
    return YES;
}

- (void)setIconAllowOverlap:(BOOL)iconAllowOverlap
{
 // TODO: Boolean
}

- (BOOL)iconAllowOverlap
{
    // TODO:
    return YES;
}

- (void)setIconIgnorePlacement:(BOOL)iconIgnorePlacement
{
 // TODO: Boolean
}

- (BOOL)iconIgnorePlacement
{
    // TODO:
    return YES;
}

- (void)setIconOptional:(BOOL)iconOptional
{
 // TODO: Boolean
}

- (BOOL)iconOptional
{
    // TODO:
    return YES;
}

- (void)setIconRotationAlignment:(MGLSymbolStyleLayerIconRotationAlignment)iconRotationAlignment
{
}

- (MGLSymbolStyleLayerIconRotationAlignment)iconRotationAlignment
{
    // TODO:
    return MGLSymbolStyleLayerIconRotationAlignmentMap;
}

- (void)setIconSize:(CGFloat)iconSize
{
    symbolLayer->setIconSize(iconSize);
}

- (CGFloat)iconSize
{
    // TODO:
    return 0;
}

- (void)setIconTextFit:(MGLSymbolStyleLayerIconTextFit)iconTextFit
{
}

- (MGLSymbolStyleLayerIconTextFit)iconTextFit
{
    // TODO:
    return MGLSymbolStyleLayerIconTextFitNone;
}

- (void)setIconTextFitPadding:(NSArray*)iconTextFitPadding
{
}

- (NSArray *)iconTextFitPadding
{
    // TODO:
    return @[];
}

- (void)setIconImage:(NSString*)iconImage
{
    // TODO:
    return @"string";
}

- (NSString *)iconImage
{
    // TODO:
    return @"string";
}

- (void)setIconRotate:(CGFloat)iconRotate
{
    symbolLayer->setIconRotate(iconRotate);
}

- (CGFloat)iconRotate
{
    // TODO:
    return 0;
}

- (void)setIconPadding:(CGFloat)iconPadding
{
    symbolLayer->setIconPadding(iconPadding);
}

- (CGFloat)iconPadding
{
    // TODO:
    return 0;
}

- (void)setIconKeepUpright:(BOOL)iconKeepUpright
{
 // TODO: Boolean
}

- (BOOL)iconKeepUpright
{
    // TODO:
    return YES;
}

- (void)setIconOffset:(NSArray*)iconOffset
{
}

- (NSArray *)iconOffset
{
    // TODO:
    return @[];
}

- (void)setTextPitchAlignment:(MGLSymbolStyleLayerTextPitchAlignment)textPitchAlignment
{
}

- (MGLSymbolStyleLayerTextPitchAlignment)textPitchAlignment
{
    // TODO:
    return MGLSymbolStyleLayerTextPitchAlignmentMap;
}

- (void)setTextRotationAlignment:(MGLSymbolStyleLayerTextRotationAlignment)textRotationAlignment
{
}

- (MGLSymbolStyleLayerTextRotationAlignment)textRotationAlignment
{
    // TODO:
    return MGLSymbolStyleLayerTextRotationAlignmentMap;
}

- (void)setTextField:(NSString*)textField
{
    // TODO:
    return @"string";
}

- (NSString *)textField
{
    // TODO:
    return @"string";
}

- (void)setTextFont:(NSArray*)textFont
{
}

- (NSArray *)textFont
{
    // TODO:
    return @[];
}

- (void)setTextSize:(CGFloat)textSize
{
    symbolLayer->setTextSize(textSize);
}

- (CGFloat)textSize
{
    // TODO:
    return 0;
}

- (void)setTextMaxWidth:(CGFloat)textMaxWidth
{
    symbolLayer->setTextMaxWidth(textMaxWidth);
}

- (CGFloat)textMaxWidth
{
    // TODO:
    return 0;
}

- (void)setTextLineHeight:(CGFloat)textLineHeight
{
    symbolLayer->setTextLineHeight(textLineHeight);
}

- (CGFloat)textLineHeight
{
    // TODO:
    return 0;
}

- (void)setTextLetterSpacing:(CGFloat)textLetterSpacing
{
    symbolLayer->setTextLetterSpacing(textLetterSpacing);
}

- (CGFloat)textLetterSpacing
{
    // TODO:
    return 0;
}

- (void)setTextJustify:(MGLSymbolStyleLayerTextJustify)textJustify
{
}

- (MGLSymbolStyleLayerTextJustify)textJustify
{
    // TODO:
    return MGLSymbolStyleLayerTextJustifyLeft;
}

- (void)setTextAnchor:(MGLSymbolStyleLayerTextAnchor)textAnchor
{
}

- (MGLSymbolStyleLayerTextAnchor)textAnchor
{
    // TODO:
    return MGLSymbolStyleLayerTextAnchorCenter;
}

- (void)setTextMaxAngle:(CGFloat)textMaxAngle
{
    symbolLayer->setTextMaxAngle(textMaxAngle);
}

- (CGFloat)textMaxAngle
{
    // TODO:
    return 0;
}

- (void)setTextRotate:(CGFloat)textRotate
{
    symbolLayer->setTextRotate(textRotate);
}

- (CGFloat)textRotate
{
    // TODO:
    return 0;
}

- (void)setTextPadding:(CGFloat)textPadding
{
    symbolLayer->setTextPadding(textPadding);
}

- (CGFloat)textPadding
{
    // TODO:
    return 0;
}

- (void)setTextKeepUpright:(BOOL)textKeepUpright
{
 // TODO: Boolean
}

- (BOOL)textKeepUpright
{
    // TODO:
    return YES;
}

- (void)setTextTransform:(MGLSymbolStyleLayerTextTransform)textTransform
{
}

- (MGLSymbolStyleLayerTextTransform)textTransform
{
    // TODO:
    return MGLSymbolStyleLayerTextTransformNone;
}

- (void)setTextOffset:(NSArray*)textOffset
{
}

- (NSArray *)textOffset
{
    // TODO:
    return @[];
}

- (void)setTextAllowOverlap:(BOOL)textAllowOverlap
{
 // TODO: Boolean
}

- (BOOL)textAllowOverlap
{
    // TODO:
    return YES;
}

- (void)setTextIgnorePlacement:(BOOL)textIgnorePlacement
{
 // TODO: Boolean
}

- (BOOL)textIgnorePlacement
{
    // TODO:
    return YES;
}

- (void)setTextOptional:(BOOL)textOptional
{
 // TODO: Boolean
}

- (BOOL)textOptional
{
    // TODO:
    return YES;
}
#pragma mark - Accessing the Paint Attributes

- (void)setIconOpacity:(CGFloat)iconOpacity
{
    symbolLayer->setIconOpacity(iconOpacity);
}
- (CGFloat)iconOpacity
{
    // TODO:
    return 0;
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
    // TODO:
    return 0;
}
- (void)setIconHaloBlur:(CGFloat)iconHaloBlur
{
    symbolLayer->setIconHaloBlur(iconHaloBlur);
}
- (CGFloat)iconHaloBlur
{
    // TODO:
    return 0;
}
- (void)setIconTranslate:(NSArray*)iconTranslate
{
}
- (NSArray *)iconTranslate
{
    // TODO:
    return @[];
}
- (void)setIconTranslateAnchor:(MGLSymbolStyleLayerIconTranslateAnchor)iconTranslateAnchor
{
}
- (MGLSymbolStyleLayerIconTranslateAnchor)iconTranslateAnchor
{
    // TODO:
    return MGLSymbolStyleLayerIconTranslateAnchorMap;
}
- (void)setTextOpacity:(CGFloat)textOpacity
{
    symbolLayer->setTextOpacity(textOpacity);
}
- (CGFloat)textOpacity
{
    // TODO:
    return 0;
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
    // TODO:
    return 0;
}
- (void)setTextHaloBlur:(CGFloat)textHaloBlur
{
    symbolLayer->setTextHaloBlur(textHaloBlur);
}
- (CGFloat)textHaloBlur
{
    // TODO:
    return 0;
}
- (void)setTextTranslate:(NSArray*)textTranslate
{
}
- (NSArray *)textTranslate
{
    // TODO:
    return @[];
}
- (void)setTextTranslateAnchor:(MGLSymbolStyleLayerTextTranslateAnchor)textTranslateAnchor
{
}
- (MGLSymbolStyleLayerTextTranslateAnchor)textTranslateAnchor
{
    // TODO:
    return MGLSymbolStyleLayerTextTranslateAnchorMap;
}

@end