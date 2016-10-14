// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleAttributeValue.h"
#import "MGLBackgroundStyleLayer.h"

#include <mbgl/style/layers/background_layer.hpp>

@interface MGLBackgroundStyleLayer ()

@property (nonatomic) mbgl::style::BackgroundLayer *layer;

@end

@implementation MGLBackgroundStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier
{
    if (self = [super initWithIdentifier:identifier]) {
        _layer = new mbgl::style::BackgroundLayer(identifier.UTF8String);
    }
    return self;
}

#pragma mark - Accessing the Paint Attributes

- (void)setBackgroundColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)backgroundColor {
    self.layer->setBackgroundColor(backgroundColor.mgl_colorPropertyValue);
}

- (id <MGLStyleAttributeValue>)backgroundColor {
    return [MGLStyleAttribute mgl_colorWithPropertyValueColor:self.layer->getBackgroundColor() ?: self.layer->getDefaultBackgroundColor()];
}

- (void)setBackgroundPattern:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)backgroundPattern {
    self.layer->setBackgroundPattern(backgroundPattern.mgl_stringPropertyValue);
}

- (id <MGLStyleAttributeValue>)backgroundPattern {
    return [MGLStyleAttribute mgl_stringWithPropertyValueString:self.layer->getBackgroundPattern() ?: self.layer->getDefaultBackgroundPattern()];
}

- (void)setBackgroundOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)backgroundOpacity {
    self.layer->setBackgroundOpacity(backgroundOpacity.mgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)backgroundOpacity {
    return [MGLStyleAttribute mgl_numberWithPropertyValueNumber:self.layer->getBackgroundOpacity() ?: self.layer->getDefaultBackgroundOpacity()];
}

@end
