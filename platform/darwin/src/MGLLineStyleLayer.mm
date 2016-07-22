// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLLineStyleLayer.h"
#import <mbgl/style/layers/line_layer.hpp>

@interface MGLLineStyleLayer() {
    mbgl::style::LineLayer *lineLayer;
}
@end

@implementation MGLLineStyleLayer

- (void)setLayer:(mbgl::style::Layer *)layer
{
    lineLayer = reinterpret_cast<mbgl::style::LineLayer *>(layer);
}

#pragma mark - Accessing the Layout Attributes


- (void)setLineCap:(MGLLineStyleLayerLineCap)lineCap
{
    // TODO: setterEnum
}

- (MGLLineStyleLayerLineCap)lineCap
{
    return MGLLineStyleLayerLineCapButt;
}

- (void)setLineJoin:(MGLLineStyleLayerLineJoin)lineJoin
{
    // TODO: setterEnum
}

- (MGLLineStyleLayerLineJoin)lineJoin
{
    return MGLLineStyleLayerLineJoinBevel;
}

- (void)setLineMiterLimit:(CGFloat)lineMiterLimit
{
    lineLayer->setLineMiterLimit(lineMiterLimit);
}

- (CGFloat)lineMiterLimit
{
    return 0; // TODO: getterNumber
}

- (void)setLineRoundLimit:(CGFloat)lineRoundLimit
{
    lineLayer->setLineRoundLimit(lineRoundLimit);
}

- (CGFloat)lineRoundLimit
{
    return 0; // TODO: getterNumber
}
#pragma mark - Accessing the Paint Attributes

- (void)setLineOpacity:(CGFloat)lineOpacity
{
    lineLayer->setLineOpacity(lineOpacity);
}
- (CGFloat)lineOpacity
{
    return 0; // TODO: getterNumber
}
- (void)setLineColor:(MGLColor*)lineColor
{
    lineLayer->setLineColor(lineColor.mbgl_color);
}
- (MGLColor *)lineColor
{
    return [MGLColor mbgl_color:lineLayer->getLineColor().asConstant()];
}
- (void)setLineTranslate:(NSArray*)lineTranslate
{
    // TODO: setterArray
}
- (NSArray *)lineTranslate
{
    return @[]; // TODO: getterArray
}
- (void)setLineTranslateAnchor:(MGLLineStyleLayerLineTranslateAnchor)lineTranslateAnchor
{
    // TODO: setterEnum
}
- (MGLLineStyleLayerLineTranslateAnchor)lineTranslateAnchor
{
    return MGLLineStyleLayerLineTranslateAnchorMap;
}
- (void)setLineWidth:(CGFloat)lineWidth
{
    lineLayer->setLineWidth(lineWidth);
}
- (CGFloat)lineWidth
{
    return 0; // TODO: getterNumber
}
- (void)setLineGapWidth:(CGFloat)lineGapWidth
{
    lineLayer->setLineGapWidth(lineGapWidth);
}
- (CGFloat)lineGapWidth
{
    return 0; // TODO: getterNumber
}
- (void)setLineOffset:(CGFloat)lineOffset
{
    lineLayer->setLineOffset(lineOffset);
}
- (CGFloat)lineOffset
{
    return 0; // TODO: getterNumber
}
- (void)setLineBlur:(CGFloat)lineBlur
{
    lineLayer->setLineBlur(lineBlur);
}
- (CGFloat)lineBlur
{
    return 0; // TODO: getterNumber
}
- (void)setLineDasharray:(NSArray*)lineDasharray
{
    // TODO: setterArray
}
- (NSArray *)lineDasharray
{
    return @[]; // TODO: getterArray
}
- (void)setLinePattern:(NSString*)linePattern
{
    // TODO: setterString
}
- (NSString *)linePattern
{
    return @""; // TODO: getterString
}

@end
