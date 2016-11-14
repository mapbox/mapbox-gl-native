// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "MGLMapView_Private.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLCircleStyleLayer.h"

#include <mbgl/style/layers/circle_layer.hpp>

@interface MGLCircleStyleLayer ()

@property (nonatomic) mbgl::style::CircleLayer *rawLayer;

@end

@implementation MGLCircleStyleLayer
{
    std::unique_ptr<mbgl::style::CircleLayer> _pendingLayer;
}

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    if (self = [super initWithIdentifier:identifier source:source]) {
        auto layer = std::make_unique<mbgl::style::CircleLayer>(identifier.UTF8String, source.identifier.UTF8String);
        _pendingLayer = std::move(layer);
        self.rawLayer = _pendingLayer.get();
    }
    return self;
}


- (NSString *)sourceLayerIdentifier
{
    auto layerID = self.rawLayer->getSourceLayer();
    return layerID.empty() ? nil : @(layerID.c_str());
}

- (void)setSourceLayerIdentifier:(NSString *)sourceLayerIdentifier
{
    self.rawLayer->setSourceLayer(sourceLayerIdentifier.UTF8String ?: "");
}

- (void)setPredicate:(NSPredicate *)predicate
{
    self.rawLayer->setFilter(predicate.mgl_filter);
}

- (NSPredicate *)predicate
{
    return [NSPredicate mgl_predicateWithFilter:self.rawLayer->getFilter()];
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
    _pendingLayer = std::move(reinterpret_cast<std::unique_ptr<mbgl::style::CircleLayer> &>(removedLayer));
    self.rawLayer = _pendingLayer.get();
}

#pragma mark - Accessing the Paint Attributes

- (void)setCircleRadius:(MGLStyleValue<NSNumber *> *)circleRadius {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(circleRadius);
    self.rawLayer->setCircleRadius(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)circleRadius {
    auto propertyValue = self.rawLayer->getCircleRadius() ?: self.rawLayer->getDefaultCircleRadius();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setCircleColor:(MGLStyleValue<MGLColor *> *)circleColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(circleColor);
    self.rawLayer->setCircleColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)circleColor {
    auto propertyValue = self.rawLayer->getCircleColor() ?: self.rawLayer->getDefaultCircleColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setCircleBlur:(MGLStyleValue<NSNumber *> *)circleBlur {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(circleBlur);
    self.rawLayer->setCircleBlur(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)circleBlur {
    auto propertyValue = self.rawLayer->getCircleBlur() ?: self.rawLayer->getDefaultCircleBlur();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setCircleOpacity:(MGLStyleValue<NSNumber *> *)circleOpacity {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(circleOpacity);
    self.rawLayer->setCircleOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)circleOpacity {
    auto propertyValue = self.rawLayer->getCircleOpacity() ?: self.rawLayer->getDefaultCircleOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setCircleTranslate:(MGLStyleValue<NSValue *> *)circleTranslate {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(circleTranslate);
    self.rawLayer->setCircleTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)circleTranslate {
    auto propertyValue = self.rawLayer->getCircleTranslate() ?: self.rawLayer->getDefaultCircleTranslate();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setCircleTranslateAnchor:(MGLStyleValue<NSValue *> *)circleTranslateAnchor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toPropertyValue(circleTranslateAnchor);
    self.rawLayer->setCircleTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)circleTranslateAnchor {
    auto propertyValue = self.rawLayer->getCircleTranslateAnchor() ?: self.rawLayer->getDefaultCircleTranslateAnchor();
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setCirclePitchScale:(MGLStyleValue<NSValue *> *)circlePitchScale {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::CirclePitchScaleType, NSValue *>().toPropertyValue(circlePitchScale);
    self.rawLayer->setCirclePitchScale(mbglValue);
}

- (MGLStyleValue<NSValue *> *)circlePitchScale {
    auto propertyValue = self.rawLayer->getCirclePitchScale() ?: self.rawLayer->getDefaultCirclePitchScale();
    return MGLStyleValueTransformer<mbgl::style::CirclePitchScaleType, NSValue *>().toStyleValue(propertyValue);
}


@end
