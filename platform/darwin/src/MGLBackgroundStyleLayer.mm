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
@property (nonatomic, readwrite) NSString *layerIdentifier;
@property (nonatomic, readwrite) NSString *sourceIdentifier;
@property (nonatomic, readwrite) NSString *sourceLayerIdentifier;

@end

@implementation MGLBackgroundStyleLayer

@synthesize mapView;

- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier
{
    if (self = [super init]) {
        _layerIdentifier = layerIdentifier;
        _layer = new mbgl::style::BackgroundLayer(layerIdentifier.UTF8String);
    }
    return self;
}

- (instancetype)initWithLayerIdentifier:(NSString *)layerIdentifier source:(MGLSource *)source
{
    if (self = [super init]) {
        _layerIdentifier = layerIdentifier;
        _sourceIdentifier = source.sourceIdentifier;
        _layer = new mbgl::style::BackgroundLayer(layerIdentifier.UTF8String);
    }
    return self;
}


#pragma mark - Accessing the Paint Attributes

- (void)setBackgroundColor:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)backgroundColor {
    self.layer->setBackgroundColor(backgroundColor.mbgl_colorPropertyValue);
}

- (id <MGLStyleAttributeValue>)backgroundColor {
    return [MGLStyleAttribute mbgl_colorWithPropertyValueColor:self.layer->getBackgroundColor() ?: self.layer->getDefaultBackgroundColor()];
}

- (void)setBackgroundPattern:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)backgroundPattern {
    self.layer->setBackgroundPattern(backgroundPattern.mbgl_stringPropertyValue);
}

- (id <MGLStyleAttributeValue>)backgroundPattern {
    return [MGLStyleAttribute mbgl_stringWithPropertyValueString:self.layer->getBackgroundPattern() ?: self.layer->getDefaultBackgroundPattern()];
}

- (void)setBackgroundOpacity:(id <MGLStyleAttributeValue, MGLStyleAttributeValue_Private>)backgroundOpacity {
    self.layer->setBackgroundOpacity(backgroundOpacity.mbgl_floatPropertyValue);
}

- (id <MGLStyleAttributeValue>)backgroundOpacity {
    return [MGLStyleAttribute mbgl_numberWithPropertyValueNumber:self.layer->getBackgroundOpacity() ?: self.layer->getDefaultBackgroundOpacity()];
}

@end
