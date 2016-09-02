// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLBaseStyleLayer_Private.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleAttributeValue.h"
#import "MGLBackgroundStyleLayer.h"

#include <mbgl/style/layers/background_layer.hpp>

@interface MGLBackgroundStyleLayer ()

@property (nonatomic) mbgl::style::BackgroundLayer *layer;
@property (nonatomic, readwrite) NSString *layerIdentifier;
@property (nonatomic, readwrite) NSString *sourceIdentifier;

@end

@implementation MGLBackgroundStyleLayer

@synthesize mapView;

- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier sourceIdentifier:(NSString *)sourceIdentifier {
    if (self = [super init]) {
        _layerIdentifier = layerIdentifier;
        _sourceIdentifier = sourceIdentifier;
        _layer = new mbgl::style::BackgroundLayer(layerIdentifier.UTF8String);
    }
    return self;
}

#pragma mark - Accessing the Paint Attributes

- (void)setBackgroundColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)backgroundColor {
    self.layer->setBackgroundColor(backgroundColor.mbgl_colorPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)backgroundColor {
    return [MGLStyleAttribute mbgl_colorWithPropertyValueColor:self.layer->getBackgroundColor() ?: self.layer->getDefaultBackgroundColor()];
}

- (void)setBackgroundPattern:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)backgroundPattern {
    self.layer->setBackgroundPattern(backgroundPattern.mbgl_stringPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)backgroundPattern {
    return [MGLStyleAttribute mbgl_stringWithPropertyValueString:self.layer->getBackgroundPattern() ?: self.layer->getDefaultBackgroundPattern()];
}

- (void)setBackgroundOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)backgroundOpacity {
    self.layer->setBackgroundOpacity(backgroundOpacity.mbgl_floatPropertyValue);
    [self update];
}

- (id <MGLStyleAttributeValue>)backgroundOpacity {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getBackgroundOpacity() ?: self.layer->getDefaultBackgroundOpacity()];
}

@end
