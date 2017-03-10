// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "MGLMapView_Private.h"
#import "NSPredicate+MGLAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLCircleStyleLayer.h"

#include <mbgl/map/map.hpp>
#include <mbgl/style/layers/circle_layer.hpp>

namespace mbgl {

    MBGL_DEFINE_ENUM(MGLCircleScaleAlignment, {
        { MGLCircleScaleAlignmentMap, "map" },
        { MGLCircleScaleAlignmentViewport, "viewport" },
    });

    MBGL_DEFINE_ENUM(MGLCircleTranslationAnchor, {
        { MGLCircleTranslationAnchorMap, "map" },
        { MGLCircleTranslationAnchorViewport, "viewport" },
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
        self.rawLayer = _pendingLayer.get();
    }
    return self;
}

- (mbgl::style::CircleLayer *)rawLayer
{
    return (mbgl::style::CircleLayer *)super.rawLayer;
}

- (void)setRawLayer:(mbgl::style::CircleLayer *)rawLayer
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

    mbgl::style::CircleLayer *layer = dynamic_cast<mbgl::style::CircleLayer *>(removedLayer.get());
    if (!layer) {
        return;
    }

    removedLayer.release();

    _pendingLayer = std::unique_ptr<mbgl::style::CircleLayer>(layer);
    self.rawLayer = _pendingLayer.get();
}

#pragma mark - Accessing the Paint Attributes

- (NSArray *)transitionKeys
{
    NSMutableArray *transitionKeys = [NSMutableArray array];
    [transitionKeys addObject:@"circleBlur"];
    [transitionKeys addObject:@"circleColor"];
    [transitionKeys addObject:@"circleOpacity"];
    [transitionKeys addObject:@"circleRadius"];
    [transitionKeys addObject:@"circleScaleAlignment"];
    [transitionKeys addObject:@"circlePitchScale"];
    [transitionKeys addObject:@"circleStrokeColor"];
    [transitionKeys addObject:@"circleStrokeOpacity"];
    [transitionKeys addObject:@"circleStrokeWidth"];
    [transitionKeys addObject:@"circleTranslation"];
    [transitionKeys addObject:@"circleTranslate"];
    [transitionKeys addObject:@"circleTranslationAnchor"];
    [transitionKeys addObject:@"circleTranslateAnchor"];

    return transitionKeys;
}

- (void)setCircleBlur:(MGLStyleValue<NSNumber *> *)circleBlur {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(circleBlur);
    self.rawLayer->setCircleBlur(mbglValue);
}

- (void)mbx_setCircleBlurTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setCircleBlurTransition(options);
}

- (MGLStyleValue<NSNumber *> *)circleBlur {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleBlur();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultCircleBlur());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setCircleColor:(MGLStyleValue<MGLColor *> *)circleColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenPropertyValue(circleColor);
    self.rawLayer->setCircleColor(mbglValue);
}

- (void)mbx_setCircleColorTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setCircleColorTransition(options);
}

- (MGLStyleValue<MGLColor *> *)circleColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(self.rawLayer->getDefaultCircleColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setCircleOpacity:(MGLStyleValue<NSNumber *> *)circleOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(circleOpacity);
    self.rawLayer->setCircleOpacity(mbglValue);
}

- (void)mbx_setCircleOpacityTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setCircleOpacityTransition(options);
}

- (MGLStyleValue<NSNumber *> *)circleOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleOpacity();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultCircleOpacity());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setCircleRadius:(MGLStyleValue<NSNumber *> *)circleRadius {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(circleRadius);
    self.rawLayer->setCircleRadius(mbglValue);
}

- (void)mbx_setCircleRadiusTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setCircleRadiusTransition(options);
}

- (MGLStyleValue<NSNumber *> *)circleRadius {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleRadius();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultCircleRadius());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setCircleScaleAlignment:(MGLStyleValue<NSValue *> *)circleScaleAlignment {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::CirclePitchScaleType, NSValue *, mbgl::style::CirclePitchScaleType, MGLCircleScaleAlignment>().toEnumPropertyValue(circleScaleAlignment);
    self.rawLayer->setCirclePitchScale(mbglValue);
}

- (void)mbx_setCircleScaleAlignmentTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setCirclePitchScaleTransition(options);
}

- (MGLStyleValue<NSValue *> *)circleScaleAlignment {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCirclePitchScale();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::CirclePitchScaleType, NSValue *, mbgl::style::CirclePitchScaleType, MGLCircleScaleAlignment>().toEnumStyleValue(self.rawLayer->getDefaultCirclePitchScale());
    }
    return MGLStyleValueTransformer<mbgl::style::CirclePitchScaleType, NSValue *, mbgl::style::CirclePitchScaleType, MGLCircleScaleAlignment>().toEnumStyleValue(propertyValue);
}

- (void)setCirclePitchScale:(MGLStyleValue<NSValue *> *)circlePitchScale {
}

- (MGLStyleValue<NSValue *> *)circlePitchScale {
    return self.circleScaleAlignment;
}

- (void)setCircleStrokeColor:(MGLStyleValue<MGLColor *> *)circleStrokeColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenPropertyValue(circleStrokeColor);
    self.rawLayer->setCircleStrokeColor(mbglValue);
}

- (void)mbx_setCircleStrokeColorTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setCircleStrokeColorTransition(options);
}

- (MGLStyleValue<MGLColor *> *)circleStrokeColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleStrokeColor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(self.rawLayer->getDefaultCircleStrokeColor());
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setCircleStrokeOpacity:(MGLStyleValue<NSNumber *> *)circleStrokeOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(circleStrokeOpacity);
    self.rawLayer->setCircleStrokeOpacity(mbglValue);
}

- (void)mbx_setCircleStrokeOpacityTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setCircleStrokeOpacityTransition(options);
}

- (MGLStyleValue<NSNumber *> *)circleStrokeOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleStrokeOpacity();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultCircleStrokeOpacity());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setCircleStrokeWidth:(MGLStyleValue<NSNumber *> *)circleStrokeWidth {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenPropertyValue(circleStrokeWidth);
    self.rawLayer->setCircleStrokeWidth(mbglValue);
}

- (void)mbx_setCircleStrokeWidthTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setCircleStrokeWidthTransition(options);
}

- (MGLStyleValue<NSNumber *> *)circleStrokeWidth {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleStrokeWidth();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(self.rawLayer->getDefaultCircleStrokeWidth());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toDataDrivenStyleValue(propertyValue);
}

- (void)setCircleTranslation:(MGLStyleValue<NSValue *> *)circleTranslation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toInterpolatablePropertyValue(circleTranslation);
    self.rawLayer->setCircleTranslate(mbglValue);
}

- (void)mbx_setCircleTranslationTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setCircleTranslateTransition(options);
}

- (MGLStyleValue<NSValue *> *)circleTranslation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleTranslate();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(self.rawLayer->getDefaultCircleTranslate());
    }
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setCircleTranslate:(MGLStyleValue<NSValue *> *)circleTranslate {
}

- (MGLStyleValue<NSValue *> *)circleTranslate {
    return self.circleTranslation;
}

- (void)setCircleTranslationAnchor:(MGLStyleValue<NSValue *> *)circleTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLCircleTranslationAnchor>().toEnumPropertyValue(circleTranslationAnchor);
    self.rawLayer->setCircleTranslateAnchor(mbglValue);
}

- (void)mbx_setCircleTranslationAnchorTransition:(NSValue *)transitionValue {
    MGLAssertStyleLayerIsValid();

    MGLTransition transition;
    [transitionValue getValue:&transition];

    mbgl::style::TransitionOptions options { { MGLDurationInSecondsFromTimeInterval(transition.duration) }, { MGLDurationInSecondsFromTimeInterval(transition.delay) } };
    self.rawLayer->setCircleTranslateAnchorTransition(options);
}

- (MGLStyleValue<NSValue *> *)circleTranslationAnchor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getCircleTranslateAnchor();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLCircleTranslationAnchor>().toEnumStyleValue(self.rawLayer->getDefaultCircleTranslateAnchor());
    }
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *, mbgl::style::TranslateAnchorType, MGLCircleTranslationAnchor>().toEnumStyleValue(propertyValue);
}

- (void)setCircleTranslateAnchor:(MGLStyleValue<NSValue *> *)circleTranslateAnchor {
}

- (MGLStyleValue<NSValue *> *)circleTranslateAnchor {
    return self.circleTranslationAnchor;
}


@end

@implementation NSValue (MGLCircleStyleLayerAdditions)

+ (NSValue *)valueWithMGLCircleScaleAlignment:(MGLCircleScaleAlignment)circleScaleAlignment {
    return [NSValue value:&circleScaleAlignment withObjCType:@encode(MGLCircleScaleAlignment)];
}

- (MGLCircleScaleAlignment)MGLCircleScaleAlignmentValue {
    MGLCircleScaleAlignment circleScaleAlignment;
    [self getValue:&circleScaleAlignment];
    return circleScaleAlignment;
}

+ (NSValue *)valueWithMGLCircleTranslationAnchor:(MGLCircleTranslationAnchor)circleTranslationAnchor {
    return [NSValue value:&circleTranslationAnchor withObjCType:@encode(MGLCircleTranslationAnchor)];
}

- (MGLCircleTranslationAnchor)MGLCircleTranslationAnchorValue {
    MGLCircleTranslationAnchor circleTranslationAnchor;
    [self getValue:&circleTranslationAnchor];
    return circleTranslationAnchor;
}

@end
