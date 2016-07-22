// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
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

- (void)setFillAntialias:(BOOL)fillAntialias
{
    // TODO: setterBool
}
- (BOOL)fillAntialias
{
    return YES; // TODO: getterBool
}
- (void)setFillOpacity:(CGFloat)fillOpacity
{
    fillLayer->setFillOpacity(fillOpacity);
}
- (CGFloat)fillOpacity
{
    return 0; // TODO: getterNumber
}
- (void)setFillColor:(MGLColor*)fillColor
{
    fillLayer->setFillColor(fillColor.mbgl_color);
}
- (MGLColor *)fillColor
{
    return [MGLColor mbgl_color:fillLayer->getFillColor().asConstant()];
}
- (void)setFillOutlineColor:(MGLColor*)fillOutlineColor
{
    fillLayer->setFillOutlineColor(fillOutlineColor.mbgl_color);
}
- (MGLColor *)fillOutlineColor
{
    return [MGLColor mbgl_color:fillLayer->getFillOutlineColor().asConstant()];
}
- (void)setFillTranslate:(NSArray*)fillTranslate
{
    // TODO: setterArray
}
- (NSArray *)fillTranslate
{
    return @[]; // TODO: getterArray
}
- (void)setFillTranslateAnchor:(MGLFillStyleLayerFillTranslateAnchor)fillTranslateAnchor
{
    // TODO: setterEnum
}
- (MGLFillStyleLayerFillTranslateAnchor)fillTranslateAnchor
{
    return MGLFillStyleLayerFillTranslateAnchorMap;
}
- (void)setFillPattern:(NSString*)fillPattern
{
    // TODO: setterString
}
- (NSString *)fillPattern
{
    return @""; // TODO: getterString
}

@end
