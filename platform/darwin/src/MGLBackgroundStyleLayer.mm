// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
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

- (void)setBackgroundColor:(MGLStyleValue<MGLColor *> *)backgroundColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(backgroundColor);
    self.layer->setBackgroundColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)backgroundColor {
    auto propertyValue = self.layer->getBackgroundColor() ?: self.layer->getDefaultBackgroundColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setBackgroundPattern:(MGLStyleValue<NSString *> *)backgroundPattern {
    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(backgroundPattern);
    self.layer->setBackgroundPattern(mbglValue);
}

- (MGLStyleValue<NSString *> *)backgroundPattern {
    auto propertyValue = self.layer->getBackgroundPattern() ?: self.layer->getDefaultBackgroundPattern();
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

- (void)setBackgroundOpacity:(MGLStyleValue<NSNumber *> *)backgroundOpacity {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(backgroundOpacity);
    self.layer->setBackgroundOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)backgroundOpacity {
    auto propertyValue = self.layer->getBackgroundOpacity() ?: self.layer->getDefaultBackgroundOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

@end
