// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLLineStyleLayer.h"
#import <mbgl/style/layers/line_layer.hpp>

@interface MGLLineStyleLayer()
@property (nonatomic) mbgl::style::LineLayer *layer;
@end

@implementation MGLLineStyleLayer

@synthesize mapView;
#pragma mark - Accessing the Layout Attributes


- (void)setLineCap:(id <MGLStyleAttributeValue>)lineCap
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)lineCap
{
    auto rawValue = self.layer->getLineCap();
    const char *type = @encode(MGLLineStyleLayerLineCap);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setLineJoin:(id <MGLStyleAttributeValue>)lineJoin
{
    // TODO: setterEnum
}

- (id <MGLStyleAttributeValue>)lineJoin
{
    auto rawValue = self.layer->getLineJoin();
    const char *type = @encode(MGLLineStyleLayerLineJoin);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setLineMiterLimit:(id <MGLStyleAttributeValue>)lineMiterLimit
{
    self.layer->setLineMiterLimit(lineMiterLimit.numberValue.mbgl_numberPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineMiterLimit
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getLineMiterLimit()];
}

- (void)setLineRoundLimit:(id <MGLStyleAttributeValue>)lineRoundLimit
{
    self.layer->setLineRoundLimit(lineRoundLimit.numberValue.mbgl_numberPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineRoundLimit
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getLineRoundLimit()];
}
#pragma mark - Accessing the Paint Attributes

- (void)setLineOpacity:(id <MGLStyleAttributeValue>)lineOpacity
{
    self.layer->setLineOpacity(lineOpacity.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getLineOpacity()];
}
- (void)setLineColor:(id <MGLStyleAttributeValue>)lineColor
{
    self.layer->setLineColor(lineColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)lineColor
{
    return [MGLColor mbgl_propertyValue:self.layer->getLineColor()];
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
    auto rawValue = self.layer->getLineTranslateAnchor();
    const char *type = @encode(MGLLineStyleLayerLineTranslateAnchor);
    return [NSValue value:&rawValue withObjCType:type];
}
- (void)setLineWidth:(id <MGLStyleAttributeValue>)lineWidth
{
    self.layer->setLineWidth(lineWidth.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineWidth
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getLineWidth()];
}
- (void)setLineGapWidth:(id <MGLStyleAttributeValue>)lineGapWidth
{
    self.layer->setLineGapWidth(lineGapWidth.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineGapWidth
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getLineGapWidth()];
}
- (void)setLineOffset:(id <MGLStyleAttributeValue>)lineOffset
{
    self.layer->setLineOffset(lineOffset.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineOffset
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getLineOffset()];
}
- (void)setLineBlur:(id <MGLStyleAttributeValue>)lineBlur
{
    self.layer->setLineBlur(lineBlur.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineBlur
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getLineBlur()];
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
    self.layer->setLinePattern(linePattern.stringValue.mbgl_stringPropertyValue);
}
- (id <MGLStyleAttributeValue>)linePattern
{
    return [NSString mbgl_stringWithPropertyValue:self.layer->getLinePattern()];
}

@end
