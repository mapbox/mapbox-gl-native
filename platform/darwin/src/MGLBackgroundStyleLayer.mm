// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "MGLMapView_Private.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLBackgroundStyleLayer.h"

#include <mbgl/style/layers/background_layer.hpp>

@interface MGLBackgroundStyleLayer ()

@property (nonatomic) mbgl::style::BackgroundLayer *rawLayer;

@end

@implementation MGLBackgroundStyleLayer
{
    std::unique_ptr<mbgl::style::BackgroundLayer> _pendingLayer;
}

- (instancetype)initWithIdentifier:(NSString *)identifier
{
    if (self = [super initWithIdentifier:identifier]) {
        auto layer = std::make_unique<mbgl::style::BackgroundLayer>(identifier.UTF8String);
        _pendingLayer = std::move(layer);
        self.rawLayer = _pendingLayer.get();
    }
    return self;
}



#pragma mark -  Adding to and removing from a map view

- (void)addToMapView:(MGLMapView *)mapView
{
    [self addToMapView:mapView belowLayer:nil];
}

- (void)addToMapView:(MGLMapView *)mapView belowLayer:(MGLStyleLayer *)otherLayer
{
    if (otherLayer) {
        const mbgl::optional<std::string> belowLayerId{otherLayer.identifier.UTF8String};
        mapView.mbglMap->addLayer(std::move(_pendingLayer), belowLayerId);
    } else {
        mapView.mbglMap->addLayer(std::move(_pendingLayer));
    }
}

- (void)removeFromMapView:(MGLMapView *)mapView
{
    auto removedLayer = mapView.mbglMap->removeLayer(self.identifier.UTF8String);
    _pendingLayer = std::move(reinterpret_cast<std::unique_ptr<mbgl::style::BackgroundLayer> &>(removedLayer));
    self.rawLayer = _pendingLayer.get();
}

#pragma mark - Accessing the Paint Attributes

- (void)setBackgroundColor:(MGLStyleValue<MGLColor *> *)backgroundColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(backgroundColor);
    self.rawLayer->setBackgroundColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)backgroundColor {
    auto propertyValue = self.rawLayer->getBackgroundColor() ?: self.rawLayer->getDefaultBackgroundColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setBackgroundPattern:(MGLStyleValue<NSString *> *)backgroundPattern {
    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(backgroundPattern);
    self.rawLayer->setBackgroundPattern(mbglValue);
}

- (MGLStyleValue<NSString *> *)backgroundPattern {
    auto propertyValue = self.rawLayer->getBackgroundPattern() ?: self.rawLayer->getDefaultBackgroundPattern();
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

- (void)setBackgroundOpacity:(MGLStyleValue<NSNumber *> *)backgroundOpacity {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(backgroundOpacity);
    self.rawLayer->setBackgroundOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)backgroundOpacity {
    auto propertyValue = self.rawLayer->getBackgroundOpacity() ?: self.rawLayer->getDefaultBackgroundOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}


@end
