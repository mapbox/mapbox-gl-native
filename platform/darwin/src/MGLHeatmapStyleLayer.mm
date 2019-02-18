// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLPrivateAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLHeatmapStyleLayer.h"
#import "MGLLoggingConfiguration_Private.h"
#import "MGLHeatmapStyleLayer_Private.h"

#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/transition_options.hpp>


@interface MGLHeatmapStyleLayer ()

@property (nonatomic, readonly) mbgl::style::HeatmapLayer *rawLayer;

@end

@implementation MGLHeatmapStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    MGLLogDebug(@"Initializing %@ with identifier: %@ source: %@", NSStringFromClass([self class]), identifier, source);
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
    MGLLogDebug(@"Setting sourceLayerIdentifier: %@", sourceLayerIdentifier);

    self.rawLayer->setSourceLayer(sourceLayerIdentifier.UTF8String ?: "");
}

- (void)setPredicate:(NSPredicate *)predicate
{
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting predicate: %@", predicate);

    self.rawLayer->setFilter(predicate ? predicate.mgl_filter : mbgl::style::Filter());
}

- (NSPredicate *)predicate
{
    MGLAssertStyleLayerIsValid();

    return [NSPredicate mgl_predicateWithFilter:self.rawLayer->getFilter()];
}

#pragma mark - Accessing the Paint Attributes

- (void)setHeatmapColor:(NSExpression *)heatmapColor {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting heatmapColor: %@", heatmapColor);

    auto mbglValue = MGLStyleValueTransformer<mbgl::Color, MGLColor *>().toPropertyValue<mbgl::style::ColorRampPropertyValue>(heatmapColor);
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
    MGLLogDebug(@"Setting heatmapIntensity: %@", heatmapIntensity);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(heatmapIntensity, false);
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
    MGLLogDebug(@"Setting heatmapIntensityTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setHeatmapIntensityTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)heatmapIntensityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getHeatmapIntensityTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setHeatmapOpacity:(NSExpression *)heatmapOpacity {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting heatmapOpacity: %@", heatmapOpacity);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(heatmapOpacity, false);
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
    MGLLogDebug(@"Setting heatmapOpacityTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setHeatmapOpacityTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)heatmapOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getHeatmapOpacityTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setHeatmapRadius:(NSExpression *)heatmapRadius {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting heatmapRadius: %@", heatmapRadius);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(heatmapRadius, true);
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
    MGLLogDebug(@"Setting heatmapRadiusTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setHeatmapRadiusTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)heatmapRadiusTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getHeatmapRadiusTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setHeatmapWeight:(NSExpression *)heatmapWeight {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting heatmapWeight: %@", heatmapWeight);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(heatmapWeight, true);
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

namespace mbgl {

MGLStyleLayer* HeatmapStyleLayerPeerFactory::createPeer(style::Layer* rawLayer) {
    return [[MGLHeatmapStyleLayer alloc] initWithRawLayer:rawLayer];
}

}  // namespace mbgl
