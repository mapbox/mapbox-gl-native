// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

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

- (void)setFillAntialias:(id <MGLStyleAttributeValue>)fillAntialias
{
    self.layer->setFillAntialias(fillAntialias.numberValue.mbgl_booleanPropertyValue);
}
- (id <MGLStyleAttributeValue>)fillAntialias
{
    return [NSNumber mbgl_booleanWithPropertyValue:self.layer->getFillAntialias()];
}
- (void)setFillOpacity:(id <MGLStyleAttributeValue>)fillOpacity
{
    self.layer->setFillOpacity(fillOpacity.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)fillOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getFillOpacity()];
}
- (void)setFillColor:(id <MGLStyleAttributeValue>)fillColor
{
    self.layer->setFillColor(fillColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)fillColor
{
    return [MGLColor mbgl_propertyValue:self.layer->getFillColor()];
}
- (void)setFillOutlineColor:(id <MGLStyleAttributeValue>)fillOutlineColor
{
    self.layer->setFillOutlineColor(fillOutlineColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)fillOutlineColor
{
    return [MGLColor mbgl_propertyValue:self.layer->getFillOutlineColor()];
}
- (void)setFillTranslate:(id <MGLStyleAttributeValue>)fillTranslate
{
    // TODO: setterArray
}
- (id <MGLStyleAttributeValue>)fillTranslate
{
    return @[];
}
- (void)setFillTranslateAnchor:(id <MGLStyleAttributeValue>)fillTranslateAnchor
{
    // TODO: setterEnum
}
- (id <MGLStyleAttributeValue>)fillTranslateAnchor
{
    auto rawValue = self.layer->getFillTranslateAnchor();
    const char *type = @encode(MGLFillStyleLayerFillTranslateAnchor);
    return [NSValue value:&rawValue withObjCType:type];
}
- (void)setFillPattern:(id <MGLStyleAttributeValue>)fillPattern
{
    self.layer->setFillPattern(fillPattern.stringValue.mbgl_stringPropertyValue);
}
- (id <MGLStyleAttributeValue>)fillPattern
{
    return [NSString mbgl_stringWithPropertyValue:self.layer->getFillPattern()];
}

@end
