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

- (mbgl::style::BackgroundLayer *)rawLayer
{
    return (mbgl::style::BackgroundLayer *)super.rawLayer;
}

- (void)setRawLayer:(mbgl::style::BackgroundLayer *)rawLayer
{
    super.rawLayer = rawLayer;
}

#pragma mark - Adding to and removing from a map view

- (void)addToMapView:(MGLMapView *)mapView belowLayer:(MGLStyleLayer *)otherLayer
{
    if (_pendingLayer == nullptr) {
        [NSException raise:@"MGLRedundantLayerException"
            format:@"This instance %@ was already added to %@. Adding the same layer instance " \
                    "to the style more than once is invalid.", self, mapView.style];
    }

    if (otherLayer) {
        const mbgl::optional<std::string> belowLayerId{otherLayer.identifier.UTF8String};
        mapView.mbglMap->addLayer(std::move(_pendingLayer), belowLayerId);
    } else {
        mapView.mbglMap->addLayer(std::move(_pendingLayer));
    }
}

- (void)removeFromMapView:(MGLMapView *)mapView
{
    _pendingLayer = nullptr;
    self.rawLayer = nullptr;

    auto removedLayer = mapView.mbglMap->removeLayer(self.identifier.UTF8String);
    if (!removedLayer) {
        return;
    }

    mbgl::style::BackgroundLayer *layer = dynamic_cast<mbgl::style::BackgroundLayer *>(removedLayer.get());
    if (!layer) {
        return;
    }

    removedLayer.release();

    _pendingLayer = std::unique_ptr<mbgl::style::BackgroundLayer>(layer);
    self.rawLayer = _pendingLayer.get();
}

#pragma mark - Accessing the Paint Attributes

- (void)setBackgroundColor:(MGLStyleValue<MGLColor *> *)backgroundColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(backgroundColor);
    self.rawLayer->setBackgroundColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)backgroundColor {
    MGLAssertStyleLayerIsValid();

    static const auto propertyValue = self.rawLayer->getBackgroundColor() ?: self.rawLayer->getDefaultBackgroundColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setBackgroundOpacity:(MGLStyleValue<NSNumber *> *)backgroundOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(backgroundOpacity);
    self.rawLayer->setBackgroundOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)backgroundOpacity {
    MGLAssertStyleLayerIsValid();

    static const auto propertyValue = self.rawLayer->getBackgroundOpacity() ?: self.rawLayer->getDefaultBackgroundOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setBackgroundPattern:(MGLStyleValue<NSString *> *)backgroundPattern {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(backgroundPattern);
    self.rawLayer->setBackgroundPattern(mbglValue);
}

- (MGLStyleValue<NSString *> *)backgroundPattern {
    MGLAssertStyleLayerIsValid();

    static const auto propertyValue = self.rawLayer->getBackgroundPattern() ?: self.rawLayer->getDefaultBackgroundPattern();
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}


@end
