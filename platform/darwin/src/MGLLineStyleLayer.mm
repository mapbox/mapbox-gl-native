// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLLineStyleLayer.h"
#import <mbgl/style/layers/line_layer.hpp>

@interface MGLLineStyleLayer()
@property (nonatomic) mbgl::style::LineLayer *layer;
@property (nonatomic, readwrite) NSString *layerID;
@property (nonatomic, readwrite) NSString *sourceID;
@end

@implementation MGLLineStyleLayer

@synthesize mapView;

- (instancetype)initWithLayerID:(NSString *)layerID sourceID:(NSString *)sourceID
{
    self = [super init];
    if (self == nil) return nil;
    _layerID = layerID;
    _sourceID = sourceID;
    _layer = new mbgl::style::LineLayer(layerID.UTF8String, sourceID.UTF8String);
    return self;
}

#pragma mark - Accessing the Layout Attributes


- (void)setLineCap:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineCap
{
    MGLSetEnumProperty(lineCap, LineCap, LineCapType);
}

- (id <MGLStyleAttributeValue>)lineCap
{
    auto rawValue = self.layer->getLineCap();
    const char *type = @encode(MGLLineStyleLayerLineCap);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setLineJoin:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineJoin
{
    MGLSetEnumProperty(lineJoin, LineJoin, LineJoinType);
}

- (id <MGLStyleAttributeValue>)lineJoin
{
    auto rawValue = self.layer->getLineJoin();
    const char *type = @encode(MGLLineStyleLayerLineJoin);
    return [NSValue value:&rawValue withObjCType:type];
}

- (void)setLineMiterLimit:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineMiterLimit
{
    self.layer->setLineMiterLimit(lineMiterLimit.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineMiterLimit
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getLineMiterLimit()];
}

- (void)setLineRoundLimit:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineRoundLimit
{
    self.layer->setLineRoundLimit(lineRoundLimit.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)lineRoundLimit
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getLineRoundLimit()];
}
#pragma mark - Accessing the Paint Attributes

- (void)setLineOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineOpacity
{
    self.layer->setLineOpacity(lineOpacity.mbgl_floatPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getLineOpacity()];
}
- (void)setLineColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineColor
{
    self.layer->setLineColor(lineColor.mbgl_colorPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineColor
{
    return [MGLColor mbgl_propertyValue:self.layer->getLineColor()];
}
- (void)setLineTranslate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineTranslate
{
    self.layer->setLineTranslate(lineTranslate.mbgl_offsetPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineTranslate
{
    return @[];
}
- (void)setLineTranslateAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineTranslateAnchor
{
    MGLSetEnumProperty(lineTranslateAnchor, LineTranslateAnchor, TranslateAnchorType);
}
- (id <MGLStyleAttributeValue>)lineTranslateAnchor
{
    auto rawValue = self.layer->getLineTranslateAnchor();
    const char *type = @encode(MGLLineStyleLayerLineTranslateAnchor);
    return [NSValue value:&rawValue withObjCType:type];
}
- (void)setLineWidth:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineWidth
{
    self.layer->setLineWidth(lineWidth.mbgl_floatPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineWidth
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getLineWidth()];
}
- (void)setLineGapWidth:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineGapWidth
{
    self.layer->setLineGapWidth(lineGapWidth.mbgl_floatPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineGapWidth
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getLineGapWidth()];
}
- (void)setLineOffset:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineOffset
{
    self.layer->setLineOffset(lineOffset.mbgl_floatPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineOffset
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getLineOffset()];
}
- (void)setLineBlur:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineBlur
{
    self.layer->setLineBlur(lineBlur.mbgl_floatPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineBlur
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getLineBlur()];
}
- (void)setLineDasharray:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)lineDasharray
{
    self.layer->setLineDasharray(lineDasharray.mbgl_dashArrayPropertyValue);
}
- (id <MGLStyleAttributeValue>)lineDasharray
{
    return @[];
}
- (void)setLinePattern:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)linePattern
{
    self.layer->setLinePattern(linePattern.mbgl_stringPropertyValue);
}
- (id <MGLStyleAttributeValue>)linePattern
{
    return [NSString mbgl_stringWithPropertyValue:self.layer->getLinePattern()];
}

@end
