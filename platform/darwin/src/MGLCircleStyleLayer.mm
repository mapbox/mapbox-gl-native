// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLCircleStyleLayer.h"

#include <mbgl/style/layers/circle_layer.hpp>

@interface MGLCircleStyleLayer ()

@property (nonatomic) mbgl::style::CircleLayer *layer;

@end

@implementation MGLCircleStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    if (self = [super initWithIdentifier:identifier source:source]) {
        _layer = new mbgl::style::CircleLayer(identifier.UTF8String, source.identifier.UTF8String);
    }
    return self;
}

- (NSString *)sourceLayerIdentifier
{
    auto layerID = self.layer->getSourceLayer();
    return layerID.empty() ? nil : @(layerID.c_str());
}

- (void)setSourceLayerIdentifier:(NSString *)sourceLayerIdentifier
{
    self.layer->setSourceLayer(sourceLayerIdentifier.UTF8String ?: "");
}

- (void)setPredicate:(NSPredicate *)predicate
{
    self.layer->setFilter(predicate.mgl_filter);
}

- (NSPredicate *)predicate
{
    return [NSPredicate mgl_predicateWithFilter:self.layer->getFilter()];
}

#pragma mark - Accessing the Paint Attributes

- (void)setCircleRadius:(MGLStyleValue<NSNumber *> *)circleRadius {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(circleRadius);
    self.layer->setCircleRadius(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)circleRadius {
    auto propertyValue = self.layer->getCircleRadius() ?: self.layer->getDefaultCircleRadius();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setCircleColor:(MGLStyleValue<MGLColor *> *)circleColor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue(circleColor);
    self.layer->setCircleColor(mbglValue);
}

- (MGLStyleValue<MGLColor *> *)circleColor {
    auto propertyValue = self.layer->getCircleColor() ?: self.layer->getDefaultCircleColor();
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toStyleValue(propertyValue);
}

- (void)setCircleBlur:(MGLStyleValue<NSNumber *> *)circleBlur {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(circleBlur);
    self.layer->setCircleBlur(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)circleBlur {
    auto propertyValue = self.layer->getCircleBlur() ?: self.layer->getDefaultCircleBlur();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setCircleOpacity:(MGLStyleValue<NSNumber *> *)circleOpacity {
    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue(circleOpacity);
    self.layer->setCircleOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)circleOpacity {
    auto propertyValue = self.layer->getCircleOpacity() ?: self.layer->getDefaultCircleOpacity();
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setCircleTranslate:(MGLStyleValue<NSValue *> *)circleTranslate {
    auto mbglValue = MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toPropertyValue(circleTranslate);
    self.layer->setCircleTranslate(mbglValue);
}

- (MGLStyleValue<NSValue *> *)circleTranslate {
    auto propertyValue = self.layer->getCircleTranslate() ?: self.layer->getDefaultCircleTranslate();
    return MGLStyleValueTransformer<std::array<float, 2>, NSValue *>().toStyleValue(propertyValue);
}

- (void)setCircleTranslateAnchor:(MGLStyleValue<NSValue *> *)circleTranslateAnchor {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toPropertyValue(circleTranslateAnchor);
    self.layer->setCircleTranslateAnchor(mbglValue);
}

- (MGLStyleValue<NSValue *> *)circleTranslateAnchor {
    auto propertyValue = self.layer->getCircleTranslateAnchor() ?: self.layer->getDefaultCircleTranslateAnchor();
    return MGLStyleValueTransformer<mbgl::style::TranslateAnchorType, NSValue *>().toStyleValue(propertyValue);
}

- (void)setCirclePitchScale:(MGLStyleValue<NSValue *> *)circlePitchScale {
    auto mbglValue = MGLStyleValueTransformer<mbgl::style::CirclePitchScaleType, NSValue *>().toPropertyValue(circlePitchScale);
    self.layer->setCirclePitchScale(mbglValue);
}

- (MGLStyleValue<NSValue *> *)circlePitchScale {
    auto propertyValue = self.layer->getCirclePitchScale() ?: self.layer->getDefaultCirclePitchScale();
    return MGLStyleValueTransformer<mbgl::style::CirclePitchScaleType, NSValue *>().toStyleValue(propertyValue);
}

@end
