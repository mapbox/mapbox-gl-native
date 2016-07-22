// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLBackgroundStyleLayer.h"
#import <mbgl/style/layers/background_layer.hpp>

@interface MGLBackgroundStyleLayer() {
    mbgl::style::BackgroundLayer *backgroundLayer;
}
@end

@implementation MGLBackgroundStyleLayer

- (void)setLayer:(mbgl::style::Layer *)layer
{
    backgroundLayer = reinterpret_cast<mbgl::style::BackgroundLayer *>(layer);
}

#pragma mark - Accessing the Paint Attributes

- (void)setBackgroundColor:(MGLColor*)backgroundColor
{
    backgroundLayer->setBackgroundColor(backgroundColor.mbgl_color);
}
- (MGLColor *)backgroundColor
{
    return [MGLColor mbgl_color:backgroundLayer->getBackgroundColor().asConstant()];
}
- (void)setBackgroundPattern:(NSString*)backgroundPattern
{
}
- (NSString *)backgroundPattern
{
    // TODO:
    return @"string";
}
- (void)setBackgroundOpacity:(CGFloat)backgroundOpacity
{
    backgroundLayer->setBackgroundOpacity(backgroundOpacity);
}
- (CGFloat)backgroundOpacity
{
    // TODO:
    return 0;
}

@end