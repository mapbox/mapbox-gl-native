// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "MGLMapView_Private.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLFillStyleLayer.h"

#include <mbgl/style/layers/fill_layer.hpp>
namespace mbgl {

    MBGL_DEFINE_ENUM(MGLFillTranslateAnchor, {
        { MGLFillTranslateAnchorMap, "map" },
        { MGLFillTranslateAnchorViewport, "viewport" },
    });

}

@interface MGLFillStyleLayer ()

@property (nonatomic) mbgl::style::FillLayer *rawLayer;

@end

@implementation MGLFillStyleLayer
{
    std::unique_ptr<mbgl::style::FillLayer> _pendingLayer;
}

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    if (self = [super initWithIdentifier:identifier source:source]) {
        auto layer = std::make_unique<mbgl::style::FillLayer>(identifier.UTF8String, source.identifier.UTF8String);
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

    mbgl::style::FillLayer *layer = dynamic_cast<mbgl::style::FillLayer *>(removedLayer.get());
    if (!layer) {
        return;
    }

    removedLayer.release();

    _pendingLayer = std::unique_ptr<mbgl::style::FillLayer>(layer);
    _rawLayer = _pendingLayer.get();
}

#pragma mark - Accessing the Paint Attributes

- (void)setFillAntialias:(MGLStyleValue<NSNumber *> *)fillAntialias {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(fillAntialias);
    _rawLayer->setFillAntialias(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)fillAntialias {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getFillAntialias() ?: _rawLayer->getDefaultFillAntialias();
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setFillColor:(MGLStyleValue<MGLColor *> *)fillColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(fillColor);
    _rawLayer->setFillColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)fillColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getFillColor() ?: _rawLayer->getDefaultFillColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setFillOpacity:(MGLStyleValue<NSNumber *> *)fillOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(fillOpacity);
    _rawLayer->setFillOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)fillOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getFillOpacity() ?: _rawLayer->getDefaultFillOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setFillOutlineColor:(MGLStyleValue<MGLColor *> *)fillOutlineColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(fillOutlineColor);
    _rawLayer->setFillOutlineColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)fillOutlineColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getFillOutlineColor() ?: _rawLayer->getDefaultFillOutlineColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setFillPattern:(MGLStyleValue<NSString *> *)fillPattern {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(fillPattern);
    _rawLayer->setFillPattern(mbglValue);
}

- (MGLStyleValue<NSString *> *)fillPattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getFillPattern() ?: _rawLayer->getDefaultFillPattern();
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

- (void)setFillTranslate:(MGLStyleValue<NSValue *> *)fillTranslate {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(fillTranslate);
    _rawLayer->setFillTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)fillTranslate {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getFillTranslate() ?: _rawLayer->getDefaultFillTranslate();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setFillTranslateAnchor:(MGLStyleValue<NSValue *> *)fillTranslateAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillTranslateAnchor>().toEnumPropertyValue(fillTranslateAnchor);
    _rawLayer->setFillTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)fillTranslateAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = _rawLayer->getFillTranslateAnchor() ?: _rawLayer->getDefaultFillTranslateAnchor();
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillTranslateAnchor>().toEnumStyleValue(propertyValue);
}


@end
