// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
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


- (void)setLineCap:(id <MGLStyleAttributeValue>)lineCap
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)lineCap
{
    return @0;
}

- (void)setLineJoin:(id <MGLStyleAttributeValue>)lineJoin
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)lineJoin
{
    return @0;
}

- (void)setLineMiterLimit:(id <MGLStyleAttributeValue>)lineMiterLimit
{
    lineLayer->setLineMiterLimit(lineMiterLimit.numberValue.mbgl_numberPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineMiterLimit
{
    return [NSNumber mbgl_numberWithPropertyValue:lineLayer->getLineMiterLimit()];
}

- (void)setLineRoundLimit:(id <MGLStyleAttributeValue>)lineRoundLimit
{
    lineLayer->setLineRoundLimit(lineRoundLimit.numberValue.mbgl_numberPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineRoundLimit
{
    return [NSNumber mbgl_numberWithPropertyValue:lineLayer->getLineRoundLimit()];
}
#pragma mark - Accessing the Paint Attributes

- (void)setLineOpacity:(id <MGLStyleAttributeValue>)lineOpacity
{
    lineLayer->setLineOpacity(lineOpacity.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:lineLayer->getLineOpacity()];
}
- (void)setLineColor:(id <MGLStyleAttributeValue>)lineColor
{
    lineLayer->setLineColor(lineColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)lineColor
{
    return [MGLColor mbgl_propertyValue:lineLayer->getLineColor()];
}
- (void)setLineTranslate:(id <MGLStyleAttributeValue>)lineTranslate
{
    // TODO: setterArray
}
- (id <MGLStyleAttributeValue>)lineTranslate
{
    return @[];
}
- (void)setLineTranslateAnchor:(id <MGLStyleAttributeValue>)lineTranslateAnchor
{
    // TODO: setterEnum
}
- (id <MGLStyleAttributeValue>)lineTranslateAnchor
{
    return @0;
}
- (void)setLineWidth:(id <MGLStyleAttributeValue>)lineWidth
{
    lineLayer->setLineWidth(lineWidth.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineWidth
{
    return [NSNumber mbgl_numberWithPropertyValue:lineLayer->getLineWidth()];
}
- (void)setLineGapWidth:(id <MGLStyleAttributeValue>)lineGapWidth
{
    lineLayer->setLineGapWidth(lineGapWidth.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineGapWidth
{
    return [NSNumber mbgl_numberWithPropertyValue:lineLayer->getLineGapWidth()];
}
- (void)setLineOffset:(id <MGLStyleAttributeValue>)lineOffset
{
    lineLayer->setLineOffset(lineOffset.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineOffset
{
    return [NSNumber mbgl_numberWithPropertyValue:lineLayer->getLineOffset()];
}
- (void)setLineBlur:(id <MGLStyleAttributeValue>)lineBlur
{
    lineLayer->setLineBlur(lineBlur.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineBlur
{
    return [NSNumber mbgl_numberWithPropertyValue:lineLayer->getLineBlur()];
}
- (void)setLineDasharray:(id <MGLStyleAttributeValue>)lineDasharray
{
    // TODO: setterArray
}
- (id <MGLStyleAttributeValue>)lineDasharray
{
    return @[];
}
- (void)setLinePattern:(id <MGLStyleAttributeValue>)linePattern
{
    lineLayer->setLinePattern(linePattern.stringValue.mbgl_stringPropertyValue);
}
- (id <MGLStyleAttributeValue>)linePattern
{
    return [NSString mbgl_stringWithPropertyValue:lineLayer->getLinePattern()];
}

@end
