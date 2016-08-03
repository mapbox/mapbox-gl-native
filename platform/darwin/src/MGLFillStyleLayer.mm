// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLFillStyleLayer.h"
#import <mbgl/style/layers/fill_layer.hpp>

@interface MGLFillStyleLayer()
@property (nonatomic) mbgl::style::FillLayer *layer;
@property (nonatomic, readwrite) NSString *layerID;
@property (nonatomic, readwrite) NSString *sourceID;
@end

@implementation MGLFillStyleLayer

@synthesize mapView;

- (instancetype)initWithLayerID:(NSString *)layerID sourceID:(NSString *)sourceID
{
    self = [super init];
    if (self == nil) return nil;
    _layerID = layerID;
    _sourceID = sourceID;
    _layer = new mbgl::style::FillLayer(layerID.UTF8String, sourceID.UTF8String);
    return self;
}

#pragma mark - Accessing the Paint Attributes

- (void)setFillAntialias:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillAntialias
{
    self.layer->setFillAntialias(fillAntialias.mbgl_boolPropertyValue);
}
- (id <MGLStyleAttributeValue>)fillAntialias
{
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getFillAntialias()];
}
- (void)setFillOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillOpacity
{
    self.layer->setFillOpacity(fillOpacity.mbgl_floatPropertyValue);
}
- (id <MGLStyleAttributeValue>)fillOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getFillOpacity()];
}
- (void)setFillColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillColor
{
    self.layer->setFillColor(fillColor.mbgl_colorPropertyValue);
}
- (id <MGLStyleAttributeValue>)fillColor
{
    return [MGLColor mbgl_propertyValue:self.layer->getFillColor()];
}
- (void)setFillOutlineColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillOutlineColor
{
    self.layer->setFillOutlineColor(fillOutlineColor.mbgl_colorPropertyValue);
}
- (id <MGLStyleAttributeValue>)fillOutlineColor
{
    return [MGLColor mbgl_propertyValue:self.layer->getFillOutlineColor()];
}
- (void)setFillTranslate:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillTranslate
{
    self.layer->setFillTranslate(fillTranslate.mbgl_offsetPropertyValue);
}
- (id <MGLStyleAttributeValue>)fillTranslate
{
    return @[];
}
- (void)setFillTranslateAnchor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillTranslateAnchor
{
    MGLSetEnumProperty(fillTranslateAnchor, FillTranslateAnchor, TranslateAnchorType, MGLFillStyleLayerFillTranslateAnchor);
}
- (id <MGLStyleAttributeValue>)fillTranslateAnchor
{
    auto rawValue = self.layer->getFillTranslateAnchor();
    const char *type = @encode(MGLFillStyleLayerFillTranslateAnchor);
    return [NSValue value:&rawValue withObjCType:type];
}
- (void)setFillPattern:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)fillPattern
{
    self.layer->setFillPattern(fillPattern.mbgl_stringPropertyValue);
}
- (id <MGLStyleAttributeValue>)fillPattern
{
    return [NSString mbgl_stringWithPropertyValue:self.layer->getFillPattern()];
}

@end
