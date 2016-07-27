// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "NSNumber+MGLStyleAttributeAdditions_Private.hpp"
#import "NSArray+MGLStyleAttributeAdditions_Private.hpp"
#import "NSString+MGLStyleAttributeAdditions_Private.hpp"
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
    backgroundLayer->setBackgroundColor(backgroundColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)backgroundColor
{
    return [MGLColor mbgl_propertyValue:backgroundLayer->getBackgroundColor()];
}
- (void)setBackgroundPattern:(id <MGLStyleAttributeValue>)backgroundPattern
{
    // backgroundLayer->setBackgroundPattern(std::string(backgroundPattern.string.UTF8String));
}
- (id <MGLStyleAttributeValue>)backgroundPattern
{
    return @"";
}
- (void)setBackgroundOpacity:(id <MGLStyleAttributeValue>)backgroundOpacity
{
    // backgroundLayer->setBackgroundOpacity(backgroundOpacity.number.floatValue);
}
- (id <MGLStyleAttributeValue>)backgroundOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:backgroundLayer->getBackgroundOpacity()];
}

@end
