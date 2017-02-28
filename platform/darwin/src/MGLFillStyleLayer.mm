// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "MGLMapView_Private.h"
#import "NSPredicate+MGLAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLFillStyleLayer.h"

#include <mbgl/map/map.hpp>
#include <mbgl/style/layers/fill_layer.hpp>

namespace mbgl {

    MBGL_DEFINE_ENUM(MGLFillTranslationAnchor, {
        { MGLFillTranslationAnchorMap, "map" },
        { MGLFillTranslationAnchorViewport, "viewport" },
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
        self.rawLayer = _pendingLayer.get();
    }
    return self;
}

- (mbgl::style::FillLayer *)rawLayer
{
    return (mbgl::style::FillLayer *)super.rawLayer;
}

- (void)setRawLayer:(mbgl::style::FillLayer *)rawLayer
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

    mbgl::style::FillLayer *layer = dynamic_cast<mbgl::style::FillLayer *>(removedLayer.get());
    if (!layer) {
        return;
    }

    removedLayer.release();

    _pendingLayer = std::unique_ptr<mbgl::style::FillLayer>(layer);
    self.rawLayer = _pendingLayer.get();
}

#pragma mark - Accessing the Paint Attributes

- (NSArray *)transitionKeys
{
    NSMutableArray *transitionKeys = [NSMutableArray array];
    [transitionKeys addObject:@"fillAntialiased"];
    [transitionKeys addObject:@"fillAntialias"];
    [transitionKeys addObject:@"fillColor"];
    [transitionKeys addObject:@"fillOpacity"];
    [transitionKeys addObject:@"fillOutlineColor"];
    [transitionKeys addObject:@"fillPattern"];
    [transitionKeys addObject:@"fillTranslation"];
    [transitionKeys addObject:@"fillTranslate"];
    [transitionKeys addObject:@"fillTranslationAnchor"];
    [transitionKeys addObject:@"fillTranslateAnchor"];

    return transitionKeys;
}

- (void)setFillAntialiased:(MGLStyleValue<NSNumber *> *)fillAntialiased {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<bool, NSNumber *>().toPropertyValue(fillAntialiased);
    self.rawLayer->setFillAntialias(mbglValue);
}

- (void)mbx_setFillAntialiasedTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillAntialiasTransition(options);
}

- (MGLStyleValue<NSNumber *> *)isFillAntialiased {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillAntialias();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(self.rawLayer->getDefaultFillAntialias());
    }
    return MGLStyleValueTransformer<bool, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setFillAntialias:(MGLStyleValue<NSNumber *> *)fillAntialias {
}

- (MGLStyleValue<NSNumber *> *)fillAntialias {
    return self.isFillAntialiased;
}

- (void)setFillColor:(MGLStyleValue<MGLColor *> *)fillColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenPropertyValue(fillColor);
    self.rawLayer->setFillColor(mbglValue);
}

- (void)mbx_setFillColorTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillColorTransition(options);
}

- (MGLStyleValue<MGLColor *> *)fillColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(self.rawLayer->getDefaultFillColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setFillOpacity:(MGLStyleValue<NSNumber *> *)fillOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(fillOpacity);
    self.rawLayer->setFillOpacity(mbglValue);
}

- (void)mbx_setFillOpacityTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillOpacityTransition(options);
}

- (MGLStyleValue<NSNumber *> *)fillOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillOpacity();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultFillOpacity());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setFillOutlineColor:(MGLStyleValue<MGLColor *> *)fillOutlineColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenPropertyValue(fillOutlineColor);
    self.rawLayer->setFillOutlineColor(mbglValue);
}

- (void)mbx_setFillOutlineColorTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillOutlineColorTransition(options);
}

- (MGLStyleValue<MGLColor *> *)fillOutlineColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillOutlineColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(self.rawLayer->getDefaultFillOutlineColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setFillPattern:(MGLStyleValue<NSString *> *)fillPattern {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::string, NSString *>().toPropertyValue(fillPattern);
    self.rawLayer->setFillPattern(mbglValue);
}

- (void)mbx_setFillPatternTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillPatternTransition(options);
}

- (MGLStyleValue<NSString *> *)fillPattern {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillPattern();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(self.rawLayer->getDefaultFillPattern());
    }
    return MGLStyleValueTransformer<std::string, NSString *>().toStyleValue(propertyValue);
}

- (void)setFillTranslation:(MGLStyleValue<NSValue *> *)fillTranslation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toInterpolatablePropertyValue(fillTranslation);
    self.rawLayer->setFillTranslate(mbglValue);
}

- (void)mbx_setFillTranslationTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillTranslateTransition(options);
}

- (MGLStyleValue<NSValue *> *)fillTranslation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillTranslate();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(self.rawLayer->getDefaultFillTranslate());
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setFillTranslate:(MGLStyleValue<NSValue *> *)fillTranslate {
}

- (MGLStyleValue<NSValue *> *)fillTranslate {
    return self.fillTranslation;
}

- (void)setFillTranslationAnchor:(MGLStyleValue<NSValue *> *)fillTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillTranslationAnchor>().toEnumPropertyValue(fillTranslationAnchor);
    self.rawLayer->setFillTranslateAnchor(mbglValue);
}

- (void)mbx_setFillTranslationAnchorTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setFillTranslateAnchorTransition(options);
}

- (MGLStyleValue<NSValue *> *)fillTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getFillTranslateAnchor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillTranslationAnchor>().toEnumStyleValue(self.rawLayer->getDefaultFillTranslateAnchor());
    }
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLFillTranslationAnchor>().toEnumStyleValue(propertyValue);
}

- (void)setFillTranslateAnchor:(MGLStyleValue<NSValue *> *)fillTranslateAnchor {
}

- (MGLStyleValue<NSValue *> *)fillTranslateAnchor {
    return self.fillTranslationAnchor;
}


@end

@implementation NSValue (MGLFillStyleLayerAdditions)

+ (NSValue *)valueWithMGLFillTranslationAnchor:(MGLFillTranslationAnchor)fillTranslationAnchor {
    return [NSValue value:&fillTranslationAnchor withObjCType:@encode(MGLFillTranslationAnchor)];
}

- (MGLFillTranslationAnchor)MGLFillTranslationAnchorValue {
    MGLFillTranslationAnchor fillTranslationAnchor;
    [self getValue:&fillTranslationAnchor];
    return fillTranslationAnchor;
}

@end
