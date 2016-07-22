// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-ios`.

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
}

- (MGLLineStyleLayerLineCap)lineCap
{
    // TODO:
    return MGLLineStyleLayerLineCapButt;
}

- (void)setLineJoin:(MGLLineStyleLayerLineJoin)lineJoin
{
}

- (MGLLineStyleLayerLineJoin)lineJoin
{
    // TODO:
    return MGLLineStyleLayerLineJoinBevel;
}

- (void)setLineMiterLimit:(CGFloat)lineMiterLimit
{
    lineLayer->setLineMiterLimit(lineMiterLimit);
}

- (CGFloat)lineMiterLimit
{
    // TODO:
    return 0;
}

- (void)setLineRoundLimit:(CGFloat)lineRoundLimit
{
    lineLayer->setLineRoundLimit(lineRoundLimit);
}

- (CGFloat)lineRoundLimit
{
    // TODO:
    return 0;
}
#pragma mark - Accessing the Paint Attributes

- (void)setLineOpacity:(CGFloat)lineOpacity
{
    lineLayer->setLineOpacity(lineOpacity);
}
- (CGFloat)lineOpacity
{
    // TODO:
    return 0;
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
}
- (NSArray *)lineTranslate
{
    // TODO:
    return @[];
}
- (void)setLineTranslateAnchor:(MGLLineStyleLayerLineTranslateAnchor)lineTranslateAnchor
{
}
- (MGLLineStyleLayerLineTranslateAnchor)lineTranslateAnchor
{
    // TODO:
    return MGLLineStyleLayerLineTranslateAnchorMap;
}
- (void)setLineWidth:(CGFloat)lineWidth
{
    lineLayer->setLineWidth(lineWidth);
}
- (CGFloat)lineWidth
{
    // TODO:
    return 0;
}
- (void)setLineGapWidth:(CGFloat)lineGapWidth
{
    lineLayer->setLineGapWidth(lineGapWidth);
}
- (CGFloat)lineGapWidth
{
    // TODO:
    return 0;
}
- (void)setLineOffset:(CGFloat)lineOffset
{
    lineLayer->setLineOffset(lineOffset);
}
- (CGFloat)lineOffset
{
    // TODO:
    return 0;
}
- (void)setLineBlur:(CGFloat)lineBlur
{
    lineLayer->setLineBlur(lineBlur);
}
- (CGFloat)lineBlur
{
    // TODO:
    return 0;
}
- (void)setLineDasharray:(NSArray*)lineDasharray
{
}
- (NSArray *)lineDasharray
{
    // TODO:
    return @[];
}
- (void)setLinePattern:(NSString*)linePattern
{
}
- (NSString *)linePattern
{
    // TODO:
    return @"string";
}

@end