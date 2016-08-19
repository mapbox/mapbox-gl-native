// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "MGLMapView_Private.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLRasterStyleLayer.h"

#include <mbgl/style/layers/raster_layer.hpp>

@interface MGLRasterStyleLayer ()

@property (nonatomic) mbgl::style::RasterLayer *rawLayer;

@end

@implementation MGLRasterStyleLayer
{
    std::unique_ptr<mbgl::style::RasterLayer> _pendingLayer;
}

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    if (self = [super initWithIdentifier:identifier source:source]) {
        auto layer = std::make_unique<mbgl::style::RasterLayer>(identifier.UTF8String, source.identifier.UTF8String);
        _pendingLayer = std::move(layer);
        _rawLayer = _pendingLayer.get();
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
    if (!removedLayer) {
        return;
    }
    _pendingLayer = std::move(reinterpret_cast<std::unique_ptr<mbgl::style::RasterLayer> &>(removedLayer));
    _rawLayer = _pendingLayer.get();
}

#pragma mark - Accessing the Paint Attributes

- (void)setRasterOpacity:(MGLStyleValue<NSNumber *> *)rasterOpacity {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterOpacity);
    _rawLayer->setRasterOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterOpacity {
    auto propertyValue = _rawLayer->getRasterOpacity() ?: _rawLayer->getDefaultRasterOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterHueRotate:(MGLStyleValue<NSNumber *> *)rasterHueRotate {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterHueRotate);
    _rawLayer->setRasterHueRotate(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterHueRotate {
    auto propertyValue = _rawLayer->getRasterHueRotate() ?: _rawLayer->getDefaultRasterHueRotate();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterBrightnessMin:(MGLStyleValue<NSNumber *> *)rasterBrightnessMin {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterBrightnessMin);
    _rawLayer->setRasterBrightnessMin(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterBrightnessMin {
    auto propertyValue = _rawLayer->getRasterBrightnessMin() ?: _rawLayer->getDefaultRasterBrightnessMin();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterBrightnessMax:(MGLStyleValue<NSNumber *> *)rasterBrightnessMax {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterBrightnessMax);
    _rawLayer->setRasterBrightnessMax(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterBrightnessMax {
    auto propertyValue = _rawLayer->getRasterBrightnessMax() ?: _rawLayer->getDefaultRasterBrightnessMax();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterSaturation:(MGLStyleValue<NSNumber *> *)rasterSaturation {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterSaturation);
    _rawLayer->setRasterSaturation(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterSaturation {
    auto propertyValue = _rawLayer->getRasterSaturation() ?: _rawLayer->getDefaultRasterSaturation();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterContrast:(MGLStyleValue<NSNumber *> *)rasterContrast {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterContrast);
    _rawLayer->setRasterContrast(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterContrast {
    auto propertyValue = _rawLayer->getRasterContrast() ?: _rawLayer->getDefaultRasterContrast();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterFadeDuration:(MGLStyleValue<NSNumber *> *)rasterFadeDuration {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterFadeDuration);
    _rawLayer->setRasterFadeDuration(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterFadeDuration {
    auto propertyValue = _rawLayer->getRasterFadeDuration() ?: _rawLayer->getDefaultRasterFadeDuration();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}


@end
