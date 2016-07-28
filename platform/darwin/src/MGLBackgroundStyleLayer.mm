// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
#import "MGLStyleAttributeValue.h"
#import "MGLBackgroundStyleLayer.h"
#import <mbgl/style/layers/background_layer.hpp>

@interface MGLBackgroundStyleLayer()
@property (nonatomic) mbgl::style::BackgroundLayer *layer;
@end

@implementation MGLBackgroundStyleLayer

@synthesize mapView;
#pragma mark - Accessing the Paint Attributes

- (void)setBackgroundColor:(id <MGLStyleAttributeValue>)backgroundColor
{
    self.layer->setBackgroundColor(backgroundColor.colorValue.mbgl_propertyValue);
}
- (id <MGLStyleAttributeValue>)backgroundColor
{
    return [MGLColor mbgl_propertyValue:self.layer->getBackgroundColor()];
}
- (void)setBackgroundPattern:(id <MGLStyleAttributeValue>)backgroundPattern
{
    self.layer->setBackgroundPattern(backgroundPattern.stringValue.mbgl_stringPropertyValue);
}
- (id <MGLStyleAttributeValue>)backgroundPattern
{
    return [NSString mbgl_stringWithPropertyValue:self.layer->getBackgroundPattern()];
}
- (void)setBackgroundOpacity:(id <MGLStyleAttributeValue>)backgroundOpacity
{
    self.layer->setBackgroundOpacity(backgroundOpacity.numberValue.mbgl_numberPropertyValue);
}
- (id <MGLStyleAttributeValue>)backgroundOpacity
{
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getBackgroundOpacity()];
}

@end
