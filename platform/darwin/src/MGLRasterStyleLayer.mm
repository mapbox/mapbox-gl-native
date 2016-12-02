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
    if (_pendingLayer == nullptr) {
        [NSException raise:@"MGLRedundantLayerException"
            format:@"This instance %@ was already added to %@. Adding the same layer instance " \
                    "to the style more than once is invalid.", self, mapView.style];
    }

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
    _pendingLayer = nullptr;
    _rawLayer = nullptr;

    auto removedLayer = mapView.mbglMap->removeLayer(self.identifier.UTF8String);
    if (!removedLayer) {
        return;
    }

    mbgl::style::RasterLayer *layer = dynamic_cast<mbgl::style::RasterLayer *>(removedLayer.get());
    if (!layer) {
        return;
    }

    removedLayer.release();

    _pendingLayer = std::unique_ptr<mbgl::style::RasterLayer>(layer);
    _rawLayer = _pendingLayer.get();
}

#pragma mark - Accessing the Paint Attributes

- (void)setMaximumRasterBrightness:(MGLStyleValue<NSNumber *> *)maximumRasterBrightness {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(maximumRasterBrightness);
    _rawLayer->setRasterBrightnessMax(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)maximumRasterBrightness {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getRasterBrightnessMax() ?: _rawLayer->getDefaultRasterBrightnessMax();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setMinimumRasterBrightness:(MGLStyleValue<NSNumber *> *)minimumRasterBrightness {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(minimumRasterBrightness);
    _rawLayer->setRasterBrightnessMin(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)minimumRasterBrightness {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getRasterBrightnessMin() ?: _rawLayer->getDefaultRasterBrightnessMin();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterContrast:(MGLStyleValue<NSNumber *> *)rasterContrast {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterContrast);
    _rawLayer->setRasterContrast(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterContrast {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getRasterContrast() ?: _rawLayer->getDefaultRasterContrast();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterFadeDuration:(MGLStyleValue<NSNumber *> *)rasterFadeDuration {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterFadeDuration);
    _rawLayer->setRasterFadeDuration(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterFadeDuration {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getRasterFadeDuration() ?: _rawLayer->getDefaultRasterFadeDuration();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterHueRotate:(MGLStyleValue<NSNumber *> *)rasterHueRotate {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterHueRotate);
    _rawLayer->setRasterHueRotate(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterHueRotate {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getRasterHueRotate() ?: _rawLayer->getDefaultRasterHueRotate();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterOpacity:(MGLStyleValue<NSNumber *> *)rasterOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterOpacity);
    _rawLayer->setRasterOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getRasterOpacity() ?: _rawLayer->getDefaultRasterOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterSaturation:(MGLStyleValue<NSNumber *> *)rasterSaturation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(rasterSaturation);
    _rawLayer->setRasterSaturation(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterSaturation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getRasterSaturation() ?: _rawLayer->getDefaultRasterSaturation();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}


@end
