// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLRasterStyleLayer.h"

#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/layers/raster_layer.hpp>

@interface MGLRasterStyleLayer ()

@property (nonatomic, readonly) mbgl::style::RasterLayer *rawLayer;

@end

@implementation MGLRasterStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    auto layer = std::make_unique<mbgl::style::RasterLayer>(identifier.UTF8String, source.identifier.UTF8String);
    return self = [super initWithPendingLayer:std::move(layer)];
}

- (mbgl::style::RasterLayer *)rawLayer
{
    return (mbgl::style::RasterLayer *)super.rawLayer;
}

- (NSString *)sourceIdentifier
{
    MGLAssertStyleLayerIsValid();

    return @(self.rawLayer->getSourceID().c_str());
}

#pragma mark - Accessing the Paint Attributes

- (void)setMaximumRasterBrightness:(NSExpression *)maximumRasterBrightness {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(maximumRasterBrightness);
    self.rawLayer->setRasterBrightnessMax(mbglValue);
}

- (NSExpression *)maximumRasterBrightness {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterBrightnessMax();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultRasterBrightnessMax();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setMaximumRasterBrightnessTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setRasterBrightnessMaxTransition(options);
}

- (MGLTransition)maximumRasterBrightnessTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getRasterBrightnessMaxTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setRasterBrightnessMax:(NSExpression *)rasterBrightnessMax {
}

- (NSExpression *)rasterBrightnessMax {
    return self.maximumRasterBrightness;
}

- (void)setMinimumRasterBrightness:(NSExpression *)minimumRasterBrightness {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(minimumRasterBrightness);
    self.rawLayer->setRasterBrightnessMin(mbglValue);
}

- (NSExpression *)minimumRasterBrightness {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterBrightnessMin();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultRasterBrightnessMin();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setMinimumRasterBrightnessTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setRasterBrightnessMinTransition(options);
}

- (MGLTransition)minimumRasterBrightnessTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getRasterBrightnessMinTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setRasterBrightnessMin:(NSExpression *)rasterBrightnessMin {
}

- (NSExpression *)rasterBrightnessMin {
    return self.minimumRasterBrightness;
}

- (void)setRasterContrast:(NSExpression *)rasterContrast {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(rasterContrast);
    self.rawLayer->setRasterContrast(mbglValue);
}

- (NSExpression *)rasterContrast {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterContrast();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultRasterContrast();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setRasterContrastTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setRasterContrastTransition(options);
}

- (MGLTransition)rasterContrastTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getRasterContrastTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setRasterFadeDuration:(NSExpression *)rasterFadeDuration {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(rasterFadeDuration);
    self.rawLayer->setRasterFadeDuration(mbglValue);
}

- (NSExpression *)rasterFadeDuration {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterFadeDuration();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultRasterFadeDuration();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setRasterHueRotation:(NSExpression *)rasterHueRotation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(rasterHueRotation);
    self.rawLayer->setRasterHueRotate(mbglValue);
}

- (NSExpression *)rasterHueRotation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterHueRotate();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultRasterHueRotate();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setRasterHueRotationTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setRasterHueRotateTransition(options);
}

- (MGLTransition)rasterHueRotationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getRasterHueRotateTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setRasterHueRotate:(NSExpression *)rasterHueRotate {
}

- (NSExpression *)rasterHueRotate {
    return self.rasterHueRotation;
}

- (void)setRasterOpacity:(NSExpression *)rasterOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(rasterOpacity);
    self.rawLayer->setRasterOpacity(mbglValue);
}

- (NSExpression *)rasterOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterOpacity();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultRasterOpacity();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setRasterOpacityTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setRasterOpacityTransition(options);
}

- (MGLTransition)rasterOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getRasterOpacityTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setRasterSaturation:(NSExpression *)rasterSaturation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(rasterSaturation);
    self.rawLayer->setRasterSaturation(mbglValue);
}

- (NSExpression *)rasterSaturation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterSaturation();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultRasterSaturation();
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toExpression(propertyValue);
}

- (void)setRasterSaturationTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setRasterSaturationTransition(options);
}

- (MGLTransition)rasterSaturationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getRasterSaturationTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

@end
