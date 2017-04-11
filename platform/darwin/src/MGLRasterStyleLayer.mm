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

- (void)setMaximumRasterBrightness:(MGLStyleValue<NSNumber *> *)maximumRasterBrightness {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(maximumRasterBrightness);
    self.rawLayer->setRasterBrightnessMax(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)maximumRasterBrightness {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterBrightnessMax();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultRasterBrightnessMax());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
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

- (void)setRasterBrightnessMax:(MGLStyleValue<NSNumber *> *)rasterBrightnessMax {
}

- (MGLStyleValue<NSNumber *> *)rasterBrightnessMax {
    return self.maximumRasterBrightness;
}

- (void)setMinimumRasterBrightness:(MGLStyleValue<NSNumber *> *)minimumRasterBrightness {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(minimumRasterBrightness);
    self.rawLayer->setRasterBrightnessMin(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)minimumRasterBrightness {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterBrightnessMin();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultRasterBrightnessMin());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
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

- (void)setRasterBrightnessMin:(MGLStyleValue<NSNumber *> *)rasterBrightnessMin {
}

- (MGLStyleValue<NSNumber *> *)rasterBrightnessMin {
    return self.minimumRasterBrightness;
}

- (void)setRasterContrast:(MGLStyleValue<NSNumber *> *)rasterContrast {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(rasterContrast);
    self.rawLayer->setRasterContrast(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterContrast {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterContrast();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultRasterContrast());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
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

- (void)setRasterFadeDuration:(MGLStyleValue<NSNumber *> *)rasterFadeDuration {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(rasterFadeDuration);
    self.rawLayer->setRasterFadeDuration(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterFadeDuration {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterFadeDuration();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultRasterFadeDuration());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
}

- (void)setRasterFadeDurationTransition:(MGLTransition )transition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions options { { MGLDurationFromTimeInterval(transition.duration) }, { MGLDurationFromTimeInterval(transition.delay) } };
    self.rawLayer->setRasterFadeDurationTransition(options);
}

- (MGLTransition)rasterFadeDurationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getRasterFadeDurationTransition();
    MGLTransition transition;
    transition.duration = MGLTimeIntervalFromDuration(transitionOptions.duration.value_or(mbgl::Duration::zero()));
    transition.delay = MGLTimeIntervalFromDuration(transitionOptions.delay.value_or(mbgl::Duration::zero()));

    return transition;
}

- (void)setRasterHueRotation:(MGLStyleValue<NSNumber *> *)rasterHueRotation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(rasterHueRotation);
    self.rawLayer->setRasterHueRotate(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterHueRotation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterHueRotate();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultRasterHueRotate());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
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

- (void)setRasterHueRotate:(MGLStyleValue<NSNumber *> *)rasterHueRotate {
}

- (MGLStyleValue<NSNumber *> *)rasterHueRotate {
    return self.rasterHueRotation;
}

- (void)setRasterOpacity:(MGLStyleValue<NSNumber *> *)rasterOpacity {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(rasterOpacity);
    self.rawLayer->setRasterOpacity(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterOpacity {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterOpacity();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultRasterOpacity());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
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

- (void)setRasterSaturation:(MGLStyleValue<NSNumber *> *)rasterSaturation {
    MGLAssertStyleLayerIsValid();

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toInterpolatablePropertyValue(rasterSaturation);
    self.rawLayer->setRasterSaturation(mbglValue);
}

- (MGLStyleValue<NSNumber *> *)rasterSaturation {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterSaturation();
    if (propertyValue.isUndefined()) {
        return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(self.rawLayer->getDefaultRasterSaturation());
    }
    return MGLStyleValueTransformer<float, NSNumber *>().toStyleValue(propertyValue);
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
