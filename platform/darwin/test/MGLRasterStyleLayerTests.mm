// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLStyleLayerTests.h"
#import "../../darwin/src/NSDate+MGLAdditions.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/transition_options.hpp>

@interface MGLRasterLayerTests : MGLStyleLayerTests
@end

@implementation MGLRasterLayerTests

+ (NSString *)layerType {
    return @"raster";
}

- (void)testProperties {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];

    MGLRasterStyleLayer *layer = [[MGLRasterStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    XCTAssertNotEqual(layer.rawLayer, nullptr);
    XCTAssertTrue(layer.rawLayer->is<mbgl::style::RasterLayer>());
    auto rawLayer = layer.rawLayer->as<mbgl::style::RasterLayer>();

    MGLTransition transitionTest = MGLTransitionMake(5, 4);


    // raster-brightness-max
    {
        XCTAssertTrue(rawLayer->getRasterBrightnessMax().isUndefined(),
                      @"raster-brightness-max should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.maximumRasterBrightness;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.maximumRasterBrightness = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getRasterBrightnessMax(), propertyValue,
                       @"Setting maximumRasterBrightness to a constant value should update raster-brightness-max.");
        XCTAssertEqualObjects(layer.maximumRasterBrightness, constantStyleValue,
                              @"maximumRasterBrightness should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.maximumRasterBrightness = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getRasterBrightnessMax(), propertyValue,
                       @"Setting maximumRasterBrightness to a camera function should update raster-brightness-max.");
        XCTAssertEqualObjects(layer.maximumRasterBrightness, functionStyleValue,
                              @"maximumRasterBrightness should round-trip camera functions.");

                              

        layer.maximumRasterBrightness = nil;
        XCTAssertTrue(rawLayer->getRasterBrightnessMax().isUndefined(),
                      @"Unsetting maximumRasterBrightness should return raster-brightness-max to the default value.");
        XCTAssertEqualObjects(layer.maximumRasterBrightness, defaultStyleValue,
                              @"maximumRasterBrightness should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.maximumRasterBrightness = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.maximumRasterBrightness = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // raster-brightness-min
    {
        XCTAssertTrue(rawLayer->getRasterBrightnessMin().isUndefined(),
                      @"raster-brightness-min should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.minimumRasterBrightness;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.minimumRasterBrightness = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getRasterBrightnessMin(), propertyValue,
                       @"Setting minimumRasterBrightness to a constant value should update raster-brightness-min.");
        XCTAssertEqualObjects(layer.minimumRasterBrightness, constantStyleValue,
                              @"minimumRasterBrightness should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.minimumRasterBrightness = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getRasterBrightnessMin(), propertyValue,
                       @"Setting minimumRasterBrightness to a camera function should update raster-brightness-min.");
        XCTAssertEqualObjects(layer.minimumRasterBrightness, functionStyleValue,
                              @"minimumRasterBrightness should round-trip camera functions.");

                              

        layer.minimumRasterBrightness = nil;
        XCTAssertTrue(rawLayer->getRasterBrightnessMin().isUndefined(),
                      @"Unsetting minimumRasterBrightness should return raster-brightness-min to the default value.");
        XCTAssertEqualObjects(layer.minimumRasterBrightness, defaultStyleValue,
                              @"minimumRasterBrightness should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.minimumRasterBrightness = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.minimumRasterBrightness = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // raster-contrast
    {
        XCTAssertTrue(rawLayer->getRasterContrast().isUndefined(),
                      @"raster-contrast should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.rasterContrast;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.rasterContrast = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getRasterContrast(), propertyValue,
                       @"Setting rasterContrast to a constant value should update raster-contrast.");
        XCTAssertEqualObjects(layer.rasterContrast, constantStyleValue,
                              @"rasterContrast should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.rasterContrast = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getRasterContrast(), propertyValue,
                       @"Setting rasterContrast to a camera function should update raster-contrast.");
        XCTAssertEqualObjects(layer.rasterContrast, functionStyleValue,
                              @"rasterContrast should round-trip camera functions.");

                              

        layer.rasterContrast = nil;
        XCTAssertTrue(rawLayer->getRasterContrast().isUndefined(),
                      @"Unsetting rasterContrast should return raster-contrast to the default value.");
        XCTAssertEqualObjects(layer.rasterContrast, defaultStyleValue,
                              @"rasterContrast should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.rasterContrast = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.rasterContrast = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        // Transition property test
        layer.rasterContrastTransition = transitionTest;
        auto toptions = rawLayer->getRasterContrastTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition rasterContrastTransition = layer.rasterContrastTransition;
        XCTAssertEqual(rasterContrastTransition.delay, transitionTest.delay);
        XCTAssertEqual(rasterContrastTransition.duration, transitionTest.duration);
    }

    // raster-fade-duration
    {
        XCTAssertTrue(rawLayer->getRasterFadeDuration().isUndefined(),
                      @"raster-fade-duration should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.rasterFadeDuration;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.rasterFadeDuration = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getRasterFadeDuration(), propertyValue,
                       @"Setting rasterFadeDuration to a constant value should update raster-fade-duration.");
        XCTAssertEqualObjects(layer.rasterFadeDuration, constantStyleValue,
                              @"rasterFadeDuration should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.rasterFadeDuration = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getRasterFadeDuration(), propertyValue,
                       @"Setting rasterFadeDuration to a camera function should update raster-fade-duration.");
        XCTAssertEqualObjects(layer.rasterFadeDuration, functionStyleValue,
                              @"rasterFadeDuration should round-trip camera functions.");

                              

        layer.rasterFadeDuration = nil;
        XCTAssertTrue(rawLayer->getRasterFadeDuration().isUndefined(),
                      @"Unsetting rasterFadeDuration should return raster-fade-duration to the default value.");
        XCTAssertEqualObjects(layer.rasterFadeDuration, defaultStyleValue,
                              @"rasterFadeDuration should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.rasterFadeDuration = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.rasterFadeDuration = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        // Transition property test
        layer.rasterFadeDurationTransition = transitionTest;
        auto toptions = rawLayer->getRasterFadeDurationTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition rasterFadeDurationTransition = layer.rasterFadeDurationTransition;
        XCTAssertEqual(rasterFadeDurationTransition.delay, transitionTest.delay);
        XCTAssertEqual(rasterFadeDurationTransition.duration, transitionTest.duration);
    }

    // raster-hue-rotate
    {
        XCTAssertTrue(rawLayer->getRasterHueRotate().isUndefined(),
                      @"raster-hue-rotate should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.rasterHueRotation;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.rasterHueRotation = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getRasterHueRotate(), propertyValue,
                       @"Setting rasterHueRotation to a constant value should update raster-hue-rotate.");
        XCTAssertEqualObjects(layer.rasterHueRotation, constantStyleValue,
                              @"rasterHueRotation should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.rasterHueRotation = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getRasterHueRotate(), propertyValue,
                       @"Setting rasterHueRotation to a camera function should update raster-hue-rotate.");
        XCTAssertEqualObjects(layer.rasterHueRotation, functionStyleValue,
                              @"rasterHueRotation should round-trip camera functions.");

                              

        layer.rasterHueRotation = nil;
        XCTAssertTrue(rawLayer->getRasterHueRotate().isUndefined(),
                      @"Unsetting rasterHueRotation should return raster-hue-rotate to the default value.");
        XCTAssertEqualObjects(layer.rasterHueRotation, defaultStyleValue,
                              @"rasterHueRotation should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.rasterHueRotation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.rasterHueRotation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // raster-opacity
    {
        XCTAssertTrue(rawLayer->getRasterOpacity().isUndefined(),
                      @"raster-opacity should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.rasterOpacity;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.rasterOpacity = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getRasterOpacity(), propertyValue,
                       @"Setting rasterOpacity to a constant value should update raster-opacity.");
        XCTAssertEqualObjects(layer.rasterOpacity, constantStyleValue,
                              @"rasterOpacity should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.rasterOpacity = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getRasterOpacity(), propertyValue,
                       @"Setting rasterOpacity to a camera function should update raster-opacity.");
        XCTAssertEqualObjects(layer.rasterOpacity, functionStyleValue,
                              @"rasterOpacity should round-trip camera functions.");

                              

        layer.rasterOpacity = nil;
        XCTAssertTrue(rawLayer->getRasterOpacity().isUndefined(),
                      @"Unsetting rasterOpacity should return raster-opacity to the default value.");
        XCTAssertEqualObjects(layer.rasterOpacity, defaultStyleValue,
                              @"rasterOpacity should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.rasterOpacity = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.rasterOpacity = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        // Transition property test
        layer.rasterOpacityTransition = transitionTest;
        auto toptions = rawLayer->getRasterOpacityTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition rasterOpacityTransition = layer.rasterOpacityTransition;
        XCTAssertEqual(rasterOpacityTransition.delay, transitionTest.delay);
        XCTAssertEqual(rasterOpacityTransition.duration, transitionTest.duration);
    }

    // raster-saturation
    {
        XCTAssertTrue(rawLayer->getRasterSaturation().isUndefined(),
                      @"raster-saturation should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.rasterSaturation;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.rasterSaturation = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getRasterSaturation(), propertyValue,
                       @"Setting rasterSaturation to a constant value should update raster-saturation.");
        XCTAssertEqualObjects(layer.rasterSaturation, constantStyleValue,
                              @"rasterSaturation should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.rasterSaturation = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getRasterSaturation(), propertyValue,
                       @"Setting rasterSaturation to a camera function should update raster-saturation.");
        XCTAssertEqualObjects(layer.rasterSaturation, functionStyleValue,
                              @"rasterSaturation should round-trip camera functions.");

                              

        layer.rasterSaturation = nil;
        XCTAssertTrue(rawLayer->getRasterSaturation().isUndefined(),
                      @"Unsetting rasterSaturation should return raster-saturation to the default value.");
        XCTAssertEqualObjects(layer.rasterSaturation, defaultStyleValue,
                              @"rasterSaturation should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.rasterSaturation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.rasterSaturation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        // Transition property test
        layer.rasterSaturationTransition = transitionTest;
        auto toptions = rawLayer->getRasterSaturationTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition rasterSaturationTransition = layer.rasterSaturationTransition;
        XCTAssertEqual(rasterSaturationTransition.delay, transitionTest.delay);
        XCTAssertEqual(rasterSaturationTransition.duration, transitionTest.duration);
    }
}

- (void)testPropertyNames {
    [self testPropertyName:@"maximum-raster-brightness" isBoolean:NO];
    [self testPropertyName:@"minimum-raster-brightness" isBoolean:NO];
    [self testPropertyName:@"raster-contrast" isBoolean:NO];
    [self testPropertyName:@"raster-fade-duration" isBoolean:NO];
    [self testPropertyName:@"raster-hue-rotation" isBoolean:NO];
    [self testPropertyName:@"raster-opacity" isBoolean:NO];
    [self testPropertyName:@"raster-saturation" isBoolean:NO];
}

@end
