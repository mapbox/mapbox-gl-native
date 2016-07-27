// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "NSNumber+MGLStyleAttributeAdditions_Private.hpp"
#import "NSArray+MGLStyleAttributeAdditions_Private.hpp"
#import "NSString+MGLStyleAttributeAdditions_Private.hpp"
#import "MGLFillStyleLayer.h"
#import <mbgl/style/layers/fill_layer.hpp>

@interface MGLFillStyleLayer() {
    mbgl::style::FillLayer *fillLayer;
}
@end

@implementation MGLFillStyleLayer

- (void)setLayer:(mbgl::style::Layer *)layer
{
    fillLayer = reinterpret_cast<mbgl::style::FillLayer *>(layer);
}

#pragma mark - Accessing the Paint Attributes

- (void)setFillAntialias:(id <MGLStyleAttributeValue>)fillAntialias
{
    fillLayer->setFillAntialias(fillAntialias.numberValue.mbgl_booleanPropertyValue);
}
- (id <MGLStyleAttributeValue>)fillAntialias
{
    return [NSNumber mbgl_booleanWithPropertyValue:fillLayer->getFillAntialias()];
}
- (void)setFillOpacity:(id <MGLStyleAttributeValue>)fillOpacity
{
    // fillLayer->setFillOpacity(fillOpacity.number.floatValue);
}
- (id <MGLStyleAttributeValue>)fillOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:fillLayer->getFillOpacity()];
}
- (void)setFillColor:(id <MGLStyleAttributeValue>)fillColor
{
    fillLayer->setFillColor(fillColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)fillColor
{
    return [MGLColor mbgl_propertyValue:fillLayer->getFillColor()];
}
- (void)setFillOutlineColor:(id <MGLStyleAttributeValue>)fillOutlineColor
{
    fillLayer->setFillOutlineColor(fillOutlineColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)fillOutlineColor
{
    return [MGLColor mbgl_propertyValue:fillLayer->getFillOutlineColor()];
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
    return @0;
}
- (void)setFillPattern:(id <MGLStyleAttributeValue>)fillPattern
{
    // fillLayer->setFillPattern(std::string(fillPattern.string.UTF8String));
}
- (id <MGLStyleAttributeValue>)fillPattern
{
    return @"";
}

@end
