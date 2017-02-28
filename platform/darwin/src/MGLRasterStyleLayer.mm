// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "MGLMapView_Private.h"
#import "NSPredicate+MGLAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLRasterStyleLayer.h"

#include <mbgl/map/map.hpp>
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
        self.rawLayer = _pendingLayer.get();
    }
    return self;
}

- (mbgl::style::RasterLayer *)rawLayer
{
    return (mbgl::style::RasterLayer *)super.rawLayer;
}

- (void)setRawLayer:(mbgl::style::RasterLayer *)rawLayer
{
    super.rawLayer = rawLayer;
}

- (NSString *)sourceIdentifier
{
    MGLAssertStyleLayerIsValid();

    return @(self.rawLayer->getSourceID().c_str());
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
    if (self.rawLayer != mapView.mbglMap->getLayer(self.identifier.UTF8String)) {
        return;
    }

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
    self.rawLayer = _pendingLayer.get();
}

#pragma mark - Accessing the Paint Attributes

- (NSArray *)transitionKeys
{
    NSMutableArray *transitionKeys = [NSMutableArray array];
    [transitionKeys addObject:@"maximumRasterBrightness"];
    [transitionKeys addObject:@"rasterBrightnessMax"];
    [transitionKeys addObject:@"minimumRasterBrightness"];
    [transitionKeys addObject:@"rasterBrightnessMin"];
    [transitionKeys addObject:@"rasterContrast"];
    [transitionKeys addObject:@"rasterFadeDuration"];
    [transitionKeys addObject:@"rasterHueRotation"];
    [transitionKeys addObject:@"rasterHueRotate"];
    [transitionKeys addObject:@"rasterOpacity"];
    [transitionKeys addObject:@"rasterSaturation"];

    return transitionKeys;
}

- (void)setMaximumRasterBrightness:(MGLStyleValue<NSNumber *> *)maximumRasterBrightness {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(maximumRasterBrightness);
    self.rawLayer->setRasterBrightnessMax(mbglValue);
}

- (void)mbx_setMaximumRasterBrightnessTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setRasterBrightnessMaxTransition(options);
}

- (MGLStyleValue<NSNumber *> *)maximumRasterBrightness {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterBrightnessMax();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultRasterBrightnessMax());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterBrightnessMax:(MGLStyleValue<NSNumber *> *)rasterBrightnessMax {
}

- (MGLStyleValue<NSNumber *> *)rasterBrightnessMax {
    return self.maximumRasterBrightness;
}

- (void)setMinimumRasterBrightness:(MGLStyleValue<NSNumber *> *)minimumRasterBrightness {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(minimumRasterBrightness);
    self.rawLayer->setRasterBrightnessMin(mbglValue);
}

- (void)mbx_setMinimumRasterBrightnessTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setRasterBrightnessMinTransition(options);
}

- (MGLStyleValue<NSNumber *> *)minimumRasterBrightness {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterBrightnessMin();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultRasterBrightnessMin());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterBrightnessMin:(MGLStyleValue<NSNumber *> *)rasterBrightnessMin {
}

- (MGLStyleValue<NSNumber *> *)rasterBrightnessMin {
    return self.minimumRasterBrightness;
}

- (void)setRasterContrast:(MGLStyleValue<NSNumber *> *)rasterContrast {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(rasterContrast);
    self.rawLayer->setRasterContrast(mbglValue);
}

- (void)mbx_setRasterContrastTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setRasterContrastTransition(options);
}

- (MGLStyleValue<NSNumber *> *)rasterContrast {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterContrast();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultRasterContrast());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterFadeDuration:(MGLStyleValue<NSNumber *> *)rasterFadeDuration {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(rasterFadeDuration);
    self.rawLayer->setRasterFadeDuration(mbglValue);
}

- (void)mbx_setRasterFadeDurationTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setRasterFadeDurationTransition(options);
}

- (MGLStyleValue<NSNumber *> *)rasterFadeDuration {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterFadeDuration();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultRasterFadeDuration());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterHueRotation:(MGLStyleValue<NSNumber *> *)rasterHueRotation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(rasterHueRotation);
    self.rawLayer->setRasterHueRotate(mbglValue);
}

- (void)mbx_setRasterHueRotationTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setRasterHueRotateTransition(options);
}

- (MGLStyleValue<NSNumber *> *)rasterHueRotation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterHueRotate();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultRasterHueRotate());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterHueRotate:(MGLStyleValue<NSNumber *> *)rasterHueRotate {
}

- (MGLStyleValue<NSNumber *> *)rasterHueRotate {
    return self.rasterHueRotation;
}

- (void)setRasterOpacity:(MGLStyleValue<NSNumber *> *)rasterOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(rasterOpacity);
    self.rawLayer->setRasterOpacity(mbglValue);
}

- (void)mbx_setRasterOpacityTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setRasterOpacityTransition(options);
}

- (MGLStyleValue<NSNumber *> *)rasterOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterOpacity();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultRasterOpacity());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterSaturation:(MGLStyleValue<NSNumber *> *)rasterSaturation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(rasterSaturation);
    self.rawLayer->setRasterSaturation(mbglValue);
}

- (void)mbx_setRasterSaturationTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setRasterSaturationTransition(options);
}

- (MGLStyleValue<NSNumber *> *)rasterSaturation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterSaturation();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultRasterSaturation());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}


@end
