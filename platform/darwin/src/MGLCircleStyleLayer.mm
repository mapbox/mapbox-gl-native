// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "MGLMapView_Private.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLCircleStyleLayer.h"

#include <mbgl/style/layers/circle_layer.hpp>
namespace mbgl {

    MBGL_DEFINE_ENUM(MGLCirclePitchScale, {
        { MGLCirclePitchScaleMap, "map" },
        { MGLCirclePitchScaleViewport, "viewport" },
    });

    MBGL_DEFINE_ENUM(MGLCircleTranslateAnchor, {
        { MGLCircleTranslateAnchorMap, "map" },
        { MGLCircleTranslateAnchorViewport, "viewport" },
    });

}

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
        _rawLayer = _pendingLayer.get();
    }
    return self;
}
- (NSString *)sourceLayerIdentifier
{
    MGLAssertStyleLayerIsValid();

    auto layerID = _rawLayer->getSourceLayer();
    return layerID.empty() ? nil : @(layerID.c_str());
}

- (void)setSourceLayerIdentifier:(NSString *)sourceLayerIdentifier
{
    MGLAssertStyleLayerIsValid();

    _rawLayer->setSourceLayer(sourceLayerIdentifier.UTF8String ?: "");
}

- (void)setPredicate:(NSPredicate *)predicate
{
    MGLAssertStyleLayerIsValid();

    _rawLayer->setFilter(predicate.mgl_filter);
}

- (NSPredicate *)predicate
{
    MGLAssertStyleLayerIsValid();

    return [NSPredicate mgl_predicateWithFilter:_rawLayer->getFilter()];
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
    _pendingLayer = nullptr;
    _rawLayer = nullptr;

    auto removedLayer = mapView.mbglMap->removeLayer(self.identifier.UTF8String);
    if (!removedLayer) {
        return;
    }

    mbgl::style::CircleLayer *layer = dynamic_cast<mbgl::style::CircleLayer *>(removedLayer.get());
    if (!layer) {
        return;
    }

    removedLayer.release();

    _pendingLayer = std::unique_ptr<mbgl::style::CircleLayer>(layer);
    _rawLayer = _pendingLayer.get();
}

#pragma mark - Accessing the Paint Attributes

- (void)setCircleBlur:(MGLStyleValue<NSNumber *> *)circleBlur {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(circleBlur);
    _rawLayer->setCircleBlur(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)circleBlur {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getCircleBlur() ?: _rawLayer->getDefaultCircleBlur();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setCircleColor:(MGLStyleValue<MGLColor *> *)circleColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(circleColor);
    _rawLayer->setCircleColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)circleColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getCircleColor() ?: _rawLayer->getDefaultCircleColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setCircleOpacity:(MGLStyleValue<NSNumber *> *)circleOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(circleOpacity);
    _rawLayer->setCircleOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)circleOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getCircleOpacity() ?: _rawLayer->getDefaultCircleOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setCirclePitchScale:(MGLStyleValue<NSValue *> *)circlePitchScale {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::CirclePitchScaleType, NSValue *, mbgl::style::CirclePitchScaleType, MGLCirclePitchScale>().toEnumPropertyValue(circlePitchScale);
    _rawLayer->setCirclePitchScale(mbglValue);
}

- (MGLStyleValue<NSValue *> *)circlePitchScale {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getCirclePitchScale() ?: _rawLayer->getDefaultCirclePitchScale();
    return MGLStyleValueTransformer<mbgl::style::CirclePitchScaleType, NSValue *, mbgl::style::CirclePitchScaleType, MGLCirclePitchScale>().toEnumStyleValue(propertyValue);
}

- (void)setCircleRadius:(MGLStyleValue<NSNumber *> *)circleRadius {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(circleRadius);
    _rawLayer->setCircleRadius(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)circleRadius {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getCircleRadius() ?: _rawLayer->getDefaultCircleRadius();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setCircleTranslate:(MGLStyleValue<NSValue *> *)circleTranslate {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(circleTranslate);
    _rawLayer->setCircleTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)circleTranslate {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getCircleTranslate() ?: _rawLayer->getDefaultCircleTranslate();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setCircleTranslateAnchor:(MGLStyleValue<NSValue *> *)circleTranslateAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLCircleTranslateAnchor>().toEnumPropertyValue(circleTranslateAnchor);
    _rawLayer->setCircleTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)circleTranslateAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getCircleTranslateAnchor() ?: _rawLayer->getDefaultCircleTranslateAnchor();
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLCircleTranslateAnchor>().toEnumStyleValue(propertyValue);
}


@end
