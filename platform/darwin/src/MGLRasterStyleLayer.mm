// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLSource.h"
#import "NSPredicate+MGLPrivateAdditions.h"
#import "NSDate+MGLAdditions.h"
#import "MGLStyleLayer_Private.h"
#import "MGLStyleValue_Private.h"
#import "MGLRasterStyleLayer.h"
#import "MGLLoggingConfiguration_Private.h"
#import "MGLRasterStyleLayer_Private.h"

#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/transition_options.hpp>


namespace mbgl {

    MBGL_DEFINE_ENUM(MGLRasterResamplingMode, {
        { MGLRasterResamplingModeLinear, "linear" },
        { MGLRasterResamplingModeNearest, "nearest" },
    });

}

@interface MGLRasterStyleLayer ()

@property (nonatomic, readonly) mbgl::style::RasterLayer *rawLayer;

@end

@implementation MGLRasterStyleLayer

- (instancetype)initWithIdentifier:(NSString *)identifier source:(MGLSource *)source
{
    MGLLogDebug(@"Initializing %@ with identifier: %@ source: %@", NSStringFromClass([self class]), identifier, source);
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
    MGLLogDebug(@"Setting maximumRasterBrightness: %@", maximumRasterBrightness);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(maximumRasterBrightness, false);
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
    MGLLogDebug(@"Setting maximumRasterBrightnessTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setRasterBrightnessMaxTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)maximumRasterBrightnessTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getRasterBrightnessMaxTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setRasterBrightnessMax:(NSExpression *)rasterBrightnessMax {
}

- (NSExpression *)rasterBrightnessMax {
    return self.maximumRasterBrightness;
}

- (void)setMinimumRasterBrightness:(NSExpression *)minimumRasterBrightness {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting minimumRasterBrightness: %@", minimumRasterBrightness);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(minimumRasterBrightness, false);
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
    MGLLogDebug(@"Setting minimumRasterBrightnessTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setRasterBrightnessMinTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)minimumRasterBrightnessTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getRasterBrightnessMinTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setRasterBrightnessMin:(NSExpression *)rasterBrightnessMin {
}

- (NSExpression *)rasterBrightnessMin {
    return self.minimumRasterBrightness;
}

- (void)setRasterContrast:(NSExpression *)rasterContrast {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting rasterContrast: %@", rasterContrast);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(rasterContrast, false);
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
    MGLLogDebug(@"Setting rasterContrastTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setRasterContrastTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)rasterContrastTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getRasterContrastTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setRasterFadeDuration:(NSExpression *)rasterFadeDuration {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting rasterFadeDuration: %@", rasterFadeDuration);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(rasterFadeDuration, false);
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
    MGLLogDebug(@"Setting rasterHueRotation: %@", rasterHueRotation);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(rasterHueRotation, false);
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
    MGLLogDebug(@"Setting rasterHueRotationTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setRasterHueRotateTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)rasterHueRotationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getRasterHueRotateTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setRasterHueRotate:(NSExpression *)rasterHueRotate {
}

- (NSExpression *)rasterHueRotate {
    return self.rasterHueRotation;
}

- (void)setRasterOpacity:(NSExpression *)rasterOpacity {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting rasterOpacity: %@", rasterOpacity);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(rasterOpacity, false);
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
    MGLLogDebug(@"Setting rasterOpacityTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setRasterOpacityTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)rasterOpacityTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getRasterOpacityTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

- (void)setRasterResamplingMode:(NSExpression *)rasterResamplingMode {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting rasterResamplingMode: %@", rasterResamplingMode);

    auto mbglValue = MGLStyleValueTransformer<mbgl::style::RasterResamplingType, NSValue *, mbgl::style::RasterResamplingType, MGLRasterResamplingMode>().toPropertyValue<mbgl::style::PropertyValue<mbgl::style::RasterResamplingType>>(rasterResamplingMode, false);
    self.rawLayer->setRasterResampling(mbglValue);
}

- (NSExpression *)rasterResamplingMode {
    MGLAssertStyleLayerIsValid();

    auto propertyValue = self.rawLayer->getRasterResampling();
    if (propertyValue.isUndefined()) {
        propertyValue = self.rawLayer->getDefaultRasterResampling();
    }
    return MGLStyleValueTransformer<mbgl::style::RasterResamplingType, NSValue *, mbgl::style::RasterResamplingType, MGLRasterResamplingMode>().toExpression(propertyValue);
}

- (void)setRasterResampling:(NSExpression *)rasterResampling {
}

- (NSExpression *)rasterResampling {
    return self.rasterResamplingMode;
}

- (void)setRasterSaturation:(NSExpression *)rasterSaturation {
    MGLAssertStyleLayerIsValid();
    MGLLogDebug(@"Setting rasterSaturation: %@", rasterSaturation);

    auto mbglValue = MGLStyleValueTransformer<float, NSNumber *>().toPropertyValue<mbgl::style::PropertyValue<float>>(rasterSaturation, false);
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
    MGLLogDebug(@"Setting rasterSaturationTransition: %@", MGLStringFromMGLTransition(transition));

    self.rawLayer->setRasterSaturationTransition(MGLOptionsFromTransition(transition));
}

- (MGLTransition)rasterSaturationTransition {
    MGLAssertStyleLayerIsValid();

    mbgl::style::TransitionOptions transitionOptions = self.rawLayer->getRasterSaturationTransition();

    return MGLTransitionFromOptions(transitionOptions);
}

@end

@implementation NSValue (MGLRasterStyleLayerAdditions)

+ (NSValue *)valueWithMGLRasterResamplingMode:(MGLRasterResamplingMode)rasterResamplingMode {
    return [NSValue value:&rasterResamplingMode withObjCType:@encode(MGLRasterResamplingMode)];
}

- (MGLRasterResamplingMode)MGLRasterResamplingModeValue {
    MGLRasterResamplingMode rasterResamplingMode;
    [self getValue:&rasterResamplingMode];
    return rasterResamplingMode;
}

@end

namespace mbgl {

MGLStyleLayer* RasterStyleLayerPeerFactory::createPeer(style::Layer* rawLayer) {
    return [[MGLRasterStyleLayer alloc] initWithRawLayer:rawLayer];
}

}  // namespace mbgl
