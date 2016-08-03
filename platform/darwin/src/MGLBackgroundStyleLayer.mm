// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayer_Private.hpp"
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
}

- (id <MGLStyleAttributeValue>)backgroundColor {
    return [MGLColor mbgl_propertyValue:self.layer->getBackgroundColor()];
}

- (void)setBackgroundPattern:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)backgroundPattern {
    self.layer->setBackgroundPattern(backgroundPattern.mbgl_stringPropertyValue);
}

- (id <MGLStyleAttributeValue>)backgroundPattern {
    return [NSString mbgl_stringWithPropertyValue:self.layer->getBackgroundPattern()];
}

- (void)setBackgroundOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)backgroundOpacity {
    self.layer->setBackgroundOpacity(backgroundOpacity.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)backgroundOpacity {
    return [NSNumber mbgl_numberWithPropertyValue:self.layer->getBackgroundOpacity()];
}

@end
