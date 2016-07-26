// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLStyleAttribute.hpp"
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

- (void)setBackgroundColor:(id <MGLStyleAttributeValue>)backgroundColor
{
    backgroundLayer->setBackgroundColor([MGLStyleAttribute colorPropertyValueWith:backgroundColor]);
}
- (id <MGLStyleAttributeValue>)backgroundColor
{
    return [[MGLStyleAttribute alloc] init]; //return [MGLColor mbgl_color:backgroundLayer->getBackgroundColor().asConstant()];
}
- (void)setBackgroundPattern:(id <MGLStyleAttributeValue>)backgroundPattern
{
    // backgroundLayer->setBackgroundPattern(std::string(backgroundPattern.string.UTF8String));
}
- (id <MGLStyleAttributeValue>)backgroundPattern
{
    return [[MGLStyleAttribute alloc] init]; //return @(std::string(backgroundLayer->getBackgroundPattern().asConstant()).c_str());
}
- (void)setBackgroundOpacity:(id <MGLStyleAttributeValue>)backgroundOpacity
{
    // backgroundLayer->setBackgroundOpacity(backgroundOpacity.number.floatValue);
}
- (id <MGLStyleAttributeValue>)backgroundOpacity
{
    return [[MGLStyleAttribute alloc] init]; //return backgroundLayer->getBackgroundOpacity().asConstant();
}

@end
