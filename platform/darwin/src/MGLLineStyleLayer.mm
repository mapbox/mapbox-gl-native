// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLStyleAttribute.hpp"
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
    return [[MGLStyleAttribute alloc] init]; //return MGLLineStyleLayerLineCapButt;
}

- (void)setLineJoin:(id <MGLStyleAttributeValue>)lineJoin
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)lineJoin
{
    return [[MGLStyleAttribute alloc] init]; //return MGLLineStyleLayerLineJoinBevel;
}

- (void)setLineMiterLimit:(id <MGLStyleAttributeValue>)lineMiterLimit
{
    // lineLayer->setLineMiterLimit(lineMiterLimit.number.floatValue);
}

- (id <MGLStyleAttributeValue>)lineMiterLimit
{
    return [[MGLStyleAttribute alloc] init]; //return lineLayer->getLineMiterLimit().asConstant();
}

- (void)setLineRoundLimit:(id <MGLStyleAttributeValue>)lineRoundLimit
{
    // lineLayer->setLineRoundLimit(lineRoundLimit.number.floatValue);
}

- (id <MGLStyleAttributeValue>)lineRoundLimit
{
    return [[MGLStyleAttribute alloc] init]; //return lineLayer->getLineRoundLimit().asConstant();
}
#pragma mark - Accessing the Paint Attributes

- (void)setLineOpacity:(id <MGLStyleAttributeValue>)lineOpacity
{
    // lineLayer->setLineOpacity(lineOpacity.number.floatValue);
}
- (id <MGLStyleAttributeValue>)lineOpacity
{
    return [[MGLStyleAttribute alloc] init]; //return lineLayer->getLineOpacity().asConstant();
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
    return [[MGLStyleAttribute alloc] init]; //return @[]; // TODO: getterArray
}
- (void)setLineTranslateAnchor:(id <MGLStyleAttributeValue>)lineTranslateAnchor
{
    // TODO: setterEnum
}
- (id <MGLStyleAttributeValue>)lineTranslateAnchor
{
    return [[MGLStyleAttribute alloc] init]; //return MGLLineStyleLayerLineTranslateAnchorMap;
}
- (void)setLineWidth:(id <MGLStyleAttributeValue>)lineWidth
{
    // lineLayer->setLineWidth(lineWidth.number.floatValue);
}
- (id <MGLStyleAttributeValue>)lineWidth
{
    return [[MGLStyleAttribute alloc] init]; //return lineLayer->getLineWidth().asConstant();
}
- (void)setLineGapWidth:(id <MGLStyleAttributeValue>)lineGapWidth
{
    // lineLayer->setLineGapWidth(lineGapWidth.number.floatValue);
}
- (id <MGLStyleAttributeValue>)lineGapWidth
{
    return [[MGLStyleAttribute alloc] init]; //return lineLayer->getLineGapWidth().asConstant();
}
- (void)setLineOffset:(id <MGLStyleAttributeValue>)lineOffset
{
    // lineLayer->setLineOffset(lineOffset.number.floatValue);
}
- (id <MGLStyleAttributeValue>)lineOffset
{
    return [[MGLStyleAttribute alloc] init]; //return lineLayer->getLineOffset().asConstant();
}
- (void)setLineBlur:(id <MGLStyleAttributeValue>)lineBlur
{
    // lineLayer->setLineBlur(lineBlur.number.floatValue);
}
- (id <MGLStyleAttributeValue>)lineBlur
{
    return [[MGLStyleAttribute alloc] init]; //return lineLayer->getLineBlur().asConstant();
}
- (void)setLineDasharray:(id <MGLStyleAttributeValue>)lineDasharray
{
    // TODO: setterArray
}
- (id <MGLStyleAttributeValue>)lineDasharray
{
    return [[MGLStyleAttribute alloc] init]; //return @[]; // TODO: getterArray
}
- (void)setLinePattern:(id <MGLStyleAttributeValue>)linePattern
{
    // lineLayer->setLinePattern(std::string(linePattern.string.UTF8String));
}
- (id <MGLStyleAttributeValue>)linePattern
{
    return [[MGLStyleAttribute alloc] init]; //return @(std::string(lineLayer->getLinePattern().asConstant()).c_str());
}

@end
