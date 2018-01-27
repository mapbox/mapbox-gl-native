// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLHeatmapStyleLayer.h"

#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/layers/heatmap_layer.hpp>

@interface MGLHeatmapStyleLayer ()

@property (nonatomic, readonly) mbgl::style::HeatmapLayer *rawLayer;

@end

@implementation MGLHeatmapStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    auto layer = std::make_unique<mbgl::style::HeatmapLayer>(identifier.UTF8String, source.identifier.UTF8String);
    return self = [super initWithPendingLayer:std::move(layer)];
}

- (mbgl::style::HeatmapLayer *)rawLayer
{
    return (mbgl::style::HeatmapLayer *)super.rawLayer;
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

#pragma mark - Accessing the Paint Attributes

- (void)setHeatmapColor:(NSExpression *)heatmapColor {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::HeatmapColorPropertyValue>(heatmapColor);
    self.rawLayer->setHeatmapColor(mbglValue);
}

- (NSExpression *)heatmapColor {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getHeatmapColor();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultHeatmapColor();
    }
    return MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toExpression(propertyValue);
}

- (void)setHeatmapIntensity:(NSExpression *)heatmapIntensity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(heatmapIntensity);
    self.rawLayer->setHeatmapIntensity(mbglValue);
}

- (NSExpression *)heatmapIntensity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getHeatmapIntensity();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultHeatmapIntensity();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setHeatmapIntensityTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setHeatmapIntensityTransition(options);
}

- (MGLTransition)heatmapIntensityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getHeatmapIntensityTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setHeatmapOpacity:(NSExpression *)heatmapOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(heatmapOpacity);
    self.rawLayer->setHeatmapOpacity(mbglValue);
}

- (NSExpression *)heatmapOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getHeatmapOpacity();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultHeatmapOpacity();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setHeatmapOpacityTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setHeatmapOpacityTransition(options);
}

- (MGLTransition)heatmapOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getHeatmapOpacityTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setHeatmapRadius:(NSExpression *)heatmapRadius {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(heatmapRadius);
    self.rawLayer->setHeatmapRadius(mbglValue);
}

- (NSExpression *)heatmapRadius {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getHeatmapRadius();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultHeatmapRadius();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setHeatmapRadiusTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setHeatmapRadiusTransition(options);
}

- (MGLTransition)heatmapRadiusTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getHeatmapRadiusTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setHeatmapWeight:(NSExpression *)heatmapWeight {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::DataDrivenPropertyValue<float>>(heatmapWeight);
    self.rawLayer->setHeatmapWeight(mbglValue);
}

- (NSExpression *)heatmapWeight {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getHeatmapWeight();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultHeatmapWeight();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

@end
