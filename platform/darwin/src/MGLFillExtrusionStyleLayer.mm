// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "MGLMapView_Private.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLFillExtrusionStyleLayer.h"

#include <mbgl/map/map.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>

namespace mbgl {

    MBGL_DEFINE_ENUM(MGLFillExtrusionTranslateAnchor, {
        { MGLFillExtrusionTranslateAnchorMap, "map" },
        { MGLFillExtrusionTranslateAnchorViewport, "viewport" },
    });

}

@interface MGLFillExtrusionStyleLayer ()

@property (nonatomic) mbgl::style::FillExtrusionLayer *rawLayer;

@end

@implementation MGLFillExtrusionStyleLayer
{
    std::unique_ptr<mbgl::style::FillExtrusionLayer> _pendingLayer;
}

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    if (self = [super initWithIdentifier:identifier source:source]) {
        auto layer = std::make_unique<mbgl::style::FillExtrusionLayer>(identifier.UTF8String, source.identifier.UTF8String);
        _pendingLayer = std::move(layer);
        self.rawLayer = _pendingLayer.get();
    }
    return self;
}

- (mbgl::style::FillExtrusionLayer *)rawLayer
{
    return (mbgl::style::FillExtrusionLayer *)super.rawLayer;
}

- (void)setRawLayer:(mbgl::style::FillExtrusionLayer *)rawLayer
{
    super.rawLayer = rawLayer;
}

- (NSString *)sourceIdentifier
{
    MGLAssertStyleLayerIsValid();

    return @(self.rawLayer->getSourceID().c_str());
}

- (NSString *)sourceLayerIdentifier
{
    MGLAssertStyleLayerIsValid();

    auto layerID = self.rawLayer->getSourceLayer();
    return layerID.empty() ? nil : @(layerID.c_str());
}

- (void)setSourceLayerIdentifier:(NSString *)sourceLayerIdentifier
{
    MGLAssertStyleLayerIsValid();

    self.rawLayer->setSourceLayer(sourceLayerIdentifier.UTF8String ?: "");
}

- (void)setPredicate:(NSPredicate *)predicate
{
    MGLAssertStyleLayerIsValid();

    self.rawLayer->setFilter(predicate ? predicate.mgl_filter : mbgl::style::NullFilter());
}

- (NSPredicate *)predicate
{
    MGLAssertStyleLayerIsValid();

    return [NSPredicate mgl_predicateWithFilter:self.rawLayer->getFilter()];
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

    mbgl::style::FillExtrusionLayer *layer = dynamic_cast<mbgl::style::FillExtrusionLayer *>(removedLayer.get());
    if (!layer) {
        return;
    }

    removedLayer.release();

    _pendingLayer = std::unique_ptr<mbgl::style::FillExtrusionLayer>(layer);
    self.rawLayer = _pendingLayer.get();
}

#pragma mark - Accessing the Paint Attributes

- (void)setFillExtrusionBase:(MGLStyleValue<NSNumber *> *)fillExtrusionBase {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(fillExtrusionBase);
    self.rawLayer->setFillExtrusionBase(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)fillExtrusionBase {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionBase();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultFillExtrusionBase());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setFillExtrusionColor:(MGLStyleValue<MGLColor *> *)fillExtrusionColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenPropertyValue(fillExtrusionColor);
    self.rawLayer->setFillExtrusionColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)fillExtrusionColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(self.rawLayer->getDefaultFillExtrusionColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setFillExtrusionHeight:(MGLStyleValue<NSNumber *> *)fillExtrusionHeight {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(fillExtrusionHeight);
    self.rawLayer->setFillExtrusionHeight(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)fillExtrusionHeight {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionHeight();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultFillExtrusionHeight());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setFillExtrusionOpacity:(MGLStyleValue<NSNumber *> *)fillExtrusionOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(fillExtrusionOpacity);
    self.rawLayer->setFillExtrusionOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)fillExtrusionOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionOpacity();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultFillExtrusionOpacity());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setFillExtrusionPattern:(MGLStyleValue<NSString *> *)fillExtrusionPattern {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(fillExtrusionPattern);
    self.rawLayer->setFillExtrusionPattern(mbglValue);
}

- (MGLStyleValue<NSString *> *)fillExtrusionPattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionPattern();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(self.rawLayer->getDefaultFillExtrusionPattern());
    }
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

- (void)setFillExtrusionTranslate:(MGLStyleValue<NSValue *> *)fillExtrusionTranslate {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toInterpolatablePropertyValue(fillExtrusionTranslate);
    self.rawLayer->setFillExtrusionTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)fillExtrusionTranslate {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionTranslate();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(self.rawLayer->getDefaultFillExtrusionTranslate());
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setFillExtrusionTranslateAnchor:(MGLStyleValue<NSValue *> *)fillExtrusionTranslateAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillExtrusionTranslateAnchor>().toEnumPropertyValue(fillExtrusionTranslateAnchor);
    self.rawLayer->setFillExtrusionTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)fillExtrusionTranslateAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillExtrusionTranslateAnchor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillExtrusionTranslateAnchor>().toEnumStyleValue(self.rawLayer->getDefaultFillExtrusionTranslateAnchor());
    }
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillExtrusionTranslateAnchor>().toEnumStyleValue(propertyValue);
}


@end

@implementation NSValue (MGLFillExtrusionStyleLayerAdditions)

+ (NSValue *)valueWithMGLFillExtrusionTranslateAnchor:(MGLFillExtrusionTranslateAnchor)fillExtrusionTranslateAnchor {
    return [NSValue value:&fillExtrusionTranslateAnchor withObjCType:@encode(MGLFillExtrusionTranslateAnchor)];
}

- (MGLFillExtrusionTranslateAnchor)MGLFillExtrusionTranslateAnchorValue {
    MGLFillExtrusionTranslateAnchor fillExtrusionTranslateAnchor;
    [self getValue:&fillExtrusionTranslateAnchor];
    return fillExtrusionTranslateAnchor;
}

@end
