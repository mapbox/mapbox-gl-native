// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLStyleAttribute.hpp"
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
    // fillLayer->setFillAntialias(fillAntialias);
}
- (id <MGLStyleAttributeValue>)fillAntialias
{
    return [[MGLStyleAttribute alloc] init]; //return fillLayer->getFillAntialias().asConstant();
}
- (void)setFillOpacity:(id <MGLStyleAttributeValue>)fillOpacity
{
    // fillLayer->setFillOpacity(fillOpacity.number.floatValue);
}
- (id <MGLStyleAttributeValue>)fillOpacity
{
    return [[MGLStyleAttribute alloc] init]; //return fillLayer->getFillOpacity().asConstant();
}
- (void)setFillColor:(id <MGLStyleAttributeValue>)fillColor
{
    fillLayer->setFillColor([MGLStyleAttribute colorPropertyValueWith:fillColor]);
}
- (id <MGLStyleAttributeValue>)fillColor
{
    return [[MGLStyleAttribute alloc] init]; //return [MGLColor mbgl_color:fillLayer->getFillColor().asConstant()];
}
- (void)setFillOutlineColor:(id <MGLStyleAttributeValue>)fillOutlineColor
{
    fillLayer->setFillOutlineColor([MGLStyleAttribute colorPropertyValueWith:fillOutlineColor]);
}
- (id <MGLStyleAttributeValue>)fillOutlineColor
{
    return [[MGLStyleAttribute alloc] init]; //return [MGLColor mbgl_color:fillLayer->getFillOutlineColor().asConstant()];
}
- (void)setFillTranslate:(id <MGLStyleAttributeValue>)fillTranslate
{
    // TODO: setterArray
}
- (id <MGLStyleAttributeValue>)fillTranslate
{
    return [[MGLStyleAttribute alloc] init]; //return @[]; // TODO: getterArray
}
- (void)setFillTranslateAnchor:(id <MGLStyleAttributeValue>)fillTranslateAnchor
{
    // TODO: setterEnum
}
- (id <MGLStyleAttributeValue>)fillTranslateAnchor
{
    return [[MGLStyleAttribute alloc] init]; //return MGLFillStyleLayerFillTranslateAnchorMap;
}
- (void)setFillPattern:(id <MGLStyleAttributeValue>)fillPattern
{
    // fillLayer->setFillPattern(std::string(fillPattern.string.UTF8String));
}
- (id <MGLStyleAttributeValue>)fillPattern
{
    return [[MGLStyleAttribute alloc] init]; //return @(std::string(fillLayer->getFillPattern().asConstant()).c_str());
}

@end
