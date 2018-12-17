// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLStyleLayerTests.h"
#import "../../darwin/src/NSDate+MGLAdditions.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/expression/dsl.hpp>

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
    XCTAssertEqualObjects(@(layer.rawLayer->getTypeInfo()->type), @"raster");
    auto rawLayer = static_cast<mbgl::style::RasterLayer*>(layer.rawLayer);

    MGLTransition transitionTest = MGLTransitionMake(5, 4);


    // raster-brightness-max
    {
        XCTAssertTrue(rawLayer->getRasterBrightnessMax().isUndefined(),
                      @"raster-brightness-max should be unset initially.");
        NSExpression *defaultExpression = layer.maximumRasterBrightness;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.maximumRasterBrightness = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getRasterBrightnessMax(), propertyValue,
                       @"Setting maximumRasterBrightness to a constant value expression should update raster-brightness-max.");
        XCTAssertEqualObjects(layer.maximumRasterBrightness, constantExpression,
                              @"maximumRasterBrightness should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.maximumRasterBrightness = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getRasterBrightnessMax(), propertyValue,
                       @"Setting maximumRasterBrightness to a camera expression should update raster-brightness-max.");
        XCTAssertEqualObjects(layer.maximumRasterBrightness, functionExpression,
                              @"maximumRasterBrightness should round-trip camera expressions.");


        layer.maximumRasterBrightness = nil;
        XCTAssertTrue(rawLayer->getRasterBrightnessMax().isUndefined(),
                      @"Unsetting maximumRasterBrightness should return raster-brightness-max to the default value.");
        XCTAssertEqualObjects(layer.maximumRasterBrightness, defaultExpression,
                              @"maximumRasterBrightness should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.maximumRasterBrightness = functionExpression, NSException, NSInvalidArgumentException, @"MGLRasterLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.maximumRasterBrightness = functionExpression, NSException, NSInvalidArgumentException, @"MGLRasterLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // raster-brightness-min
    {
        XCTAssertTrue(rawLayer->getRasterBrightnessMin().isUndefined(),
                      @"raster-brightness-min should be unset initially.");
        NSExpression *defaultExpression = layer.minimumRasterBrightness;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.minimumRasterBrightness = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getRasterBrightnessMin(), propertyValue,
                       @"Setting minimumRasterBrightness to a constant value expression should update raster-brightness-min.");
        XCTAssertEqualObjects(layer.minimumRasterBrightness, constantExpression,
                              @"minimumRasterBrightness should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.minimumRasterBrightness = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getRasterBrightnessMin(), propertyValue,
                       @"Setting minimumRasterBrightness to a camera expression should update raster-brightness-min.");
        XCTAssertEqualObjects(layer.minimumRasterBrightness, functionExpression,
                              @"minimumRasterBrightness should round-trip camera expressions.");


        layer.minimumRasterBrightness = nil;
        XCTAssertTrue(rawLayer->getRasterBrightnessMin().isUndefined(),
                      @"Unsetting minimumRasterBrightness should return raster-brightness-min to the default value.");
        XCTAssertEqualObjects(layer.minimumRasterBrightness, defaultExpression,
                              @"minimumRasterBrightness should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.minimumRasterBrightness = functionExpression, NSException, NSInvalidArgumentException, @"MGLRasterLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.minimumRasterBrightness = functionExpression, NSException, NSInvalidArgumentException, @"MGLRasterLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // raster-contrast
    {
        XCTAssertTrue(rawLayer->getRasterContrast().isUndefined(),
                      @"raster-contrast should be unset initially.");
        NSExpression *defaultExpression = layer.rasterContrast;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.rasterContrast = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getRasterContrast(), propertyValue,
                       @"Setting rasterContrast to a constant value expression should update raster-contrast.");
        XCTAssertEqualObjects(layer.rasterContrast, constantExpression,
                              @"rasterContrast should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.rasterContrast = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getRasterContrast(), propertyValue,
                       @"Setting rasterContrast to a camera expression should update raster-contrast.");
        XCTAssertEqualObjects(layer.rasterContrast, functionExpression,
                              @"rasterContrast should round-trip camera expressions.");


        layer.rasterContrast = nil;
        XCTAssertTrue(rawLayer->getRasterContrast().isUndefined(),
                      @"Unsetting rasterContrast should return raster-contrast to the default value.");
        XCTAssertEqualObjects(layer.rasterContrast, defaultExpression,
                              @"rasterContrast should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.rasterContrast = functionExpression, NSException, NSInvalidArgumentException, @"MGLRasterLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.rasterContrast = functionExpression, NSException, NSInvalidArgumentException, @"MGLRasterLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
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
        NSExpression *defaultExpression = layer.rasterFadeDuration;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.rasterFadeDuration = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getRasterFadeDuration(), propertyValue,
                       @"Setting rasterFadeDuration to a constant value expression should update raster-fade-duration.");
        XCTAssertEqualObjects(layer.rasterFadeDuration, constantExpression,
                              @"rasterFadeDuration should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.rasterFadeDuration = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getRasterFadeDuration(), propertyValue,
                       @"Setting rasterFadeDuration to a camera expression should update raster-fade-duration.");
        XCTAssertEqualObjects(layer.rasterFadeDuration, functionExpression,
                              @"rasterFadeDuration should round-trip camera expressions.");


        layer.rasterFadeDuration = nil;
        XCTAssertTrue(rawLayer->getRasterFadeDuration().isUndefined(),
                      @"Unsetting rasterFadeDuration should return raster-fade-duration to the default value.");
        XCTAssertEqualObjects(layer.rasterFadeDuration, defaultExpression,
                              @"rasterFadeDuration should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.rasterFadeDuration = functionExpression, NSException, NSInvalidArgumentException, @"MGLRasterLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.rasterFadeDuration = functionExpression, NSException, NSInvalidArgumentException, @"MGLRasterLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // raster-hue-rotate
    {
        XCTAssertTrue(rawLayer->getRasterHueRotate().isUndefined(),
                      @"raster-hue-rotate should be unset initially.");
        NSExpression *defaultExpression = layer.rasterHueRotation;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.rasterHueRotation = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getRasterHueRotate(), propertyValue,
                       @"Setting rasterHueRotation to a constant value expression should update raster-hue-rotate.");
        XCTAssertEqualObjects(layer.rasterHueRotation, constantExpression,
                              @"rasterHueRotation should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.rasterHueRotation = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getRasterHueRotate(), propertyValue,
                       @"Setting rasterHueRotation to a camera expression should update raster-hue-rotate.");
        XCTAssertEqualObjects(layer.rasterHueRotation, functionExpression,
                              @"rasterHueRotation should round-trip camera expressions.");


        layer.rasterHueRotation = nil;
        XCTAssertTrue(rawLayer->getRasterHueRotate().isUndefined(),
                      @"Unsetting rasterHueRotation should return raster-hue-rotate to the default value.");
        XCTAssertEqualObjects(layer.rasterHueRotation, defaultExpression,
                              @"rasterHueRotation should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.rasterHueRotation = functionExpression, NSException, NSInvalidArgumentException, @"MGLRasterLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.rasterHueRotation = functionExpression, NSException, NSInvalidArgumentException, @"MGLRasterLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // raster-opacity
    {
        XCTAssertTrue(rawLayer->getRasterOpacity().isUndefined(),
                      @"raster-opacity should be unset initially.");
        NSExpression *defaultExpression = layer.rasterOpacity;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.rasterOpacity = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getRasterOpacity(), propertyValue,
                       @"Setting rasterOpacity to a constant value expression should update raster-opacity.");
        XCTAssertEqualObjects(layer.rasterOpacity, constantExpression,
                              @"rasterOpacity should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.rasterOpacity = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getRasterOpacity(), propertyValue,
                       @"Setting rasterOpacity to a camera expression should update raster-opacity.");
        XCTAssertEqualObjects(layer.rasterOpacity, functionExpression,
                              @"rasterOpacity should round-trip camera expressions.");


        layer.rasterOpacity = nil;
        XCTAssertTrue(rawLayer->getRasterOpacity().isUndefined(),
                      @"Unsetting rasterOpacity should return raster-opacity to the default value.");
        XCTAssertEqualObjects(layer.rasterOpacity, defaultExpression,
                              @"rasterOpacity should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.rasterOpacity = functionExpression, NSException, NSInvalidArgumentException, @"MGLRasterLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.rasterOpacity = functionExpression, NSException, NSInvalidArgumentException, @"MGLRasterLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        // Transition property test
        layer.rasterOpacityTransition = transitionTest;
        auto toptions = rawLayer->getRasterOpacityTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition rasterOpacityTransition = layer.rasterOpacityTransition;
        XCTAssertEqual(rasterOpacityTransition.delay, transitionTest.delay);
        XCTAssertEqual(rasterOpacityTransition.duration, transitionTest.duration);
    }

    // raster-resampling
    {
        XCTAssertTrue(rawLayer->getRasterResampling().isUndefined(),
                      @"raster-resampling should be unset initially.");
        NSExpression *defaultExpression = layer.rasterResamplingMode;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'nearest'"];
        layer.rasterResamplingMode = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::RasterResamplingType> propertyValue = { mbgl::style::RasterResamplingType::Nearest };
        XCTAssertEqual(rawLayer->getRasterResampling(), propertyValue,
                       @"Setting rasterResamplingMode to a constant value expression should update raster-resampling.");
        XCTAssertEqualObjects(layer.rasterResamplingMode, constantExpression,
                              @"rasterResamplingMode should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'nearest'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.rasterResamplingMode = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::RasterResamplingType>(
                step(zoom(), literal("nearest"), 18.0, literal("nearest"))
            );
        }

        XCTAssertEqual(rawLayer->getRasterResampling(), propertyValue,
                       @"Setting rasterResamplingMode to a camera expression should update raster-resampling.");
        XCTAssertEqualObjects(layer.rasterResamplingMode, functionExpression,
                              @"rasterResamplingMode should round-trip camera expressions.");


        layer.rasterResamplingMode = nil;
        XCTAssertTrue(rawLayer->getRasterResampling().isUndefined(),
                      @"Unsetting rasterResamplingMode should return raster-resampling to the default value.");
        XCTAssertEqualObjects(layer.rasterResamplingMode, defaultExpression,
                              @"rasterResamplingMode should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.rasterResamplingMode = functionExpression, NSException, NSInvalidArgumentException, @"MGLRasterLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.rasterResamplingMode = functionExpression, NSException, NSInvalidArgumentException, @"MGLRasterLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // raster-saturation
    {
        XCTAssertTrue(rawLayer->getRasterSaturation().isUndefined(),
                      @"raster-saturation should be unset initially.");
        NSExpression *defaultExpression = layer.rasterSaturation;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.rasterSaturation = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getRasterSaturation(), propertyValue,
                       @"Setting rasterSaturation to a constant value expression should update raster-saturation.");
        XCTAssertEqualObjects(layer.rasterSaturation, constantExpression,
                              @"rasterSaturation should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.rasterSaturation = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getRasterSaturation(), propertyValue,
                       @"Setting rasterSaturation to a camera expression should update raster-saturation.");
        XCTAssertEqualObjects(layer.rasterSaturation, functionExpression,
                              @"rasterSaturation should round-trip camera expressions.");


        layer.rasterSaturation = nil;
        XCTAssertTrue(rawLayer->getRasterSaturation().isUndefined(),
                      @"Unsetting rasterSaturation should return raster-saturation to the default value.");
        XCTAssertEqualObjects(layer.rasterSaturation, defaultExpression,
                              @"rasterSaturation should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.rasterSaturation = functionExpression, NSException, NSInvalidArgumentException, @"MGLRasterLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.rasterSaturation = functionExpression, NSException, NSInvalidArgumentException, @"MGLRasterLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
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
    [self testPropertyName:@"raster-resampling-mode" isBoolean:NO];
    [self testPropertyName:@"raster-saturation" isBoolean:NO];
}

- (void)testValueAdditions {
    XCTAssertEqual([NSValue valueWithMGLRasterResamplingMode:MGLRasterResamplingModeLinear].MGLRasterResamplingModeValue, MGLRasterResamplingModeLinear);
    XCTAssertEqual([NSValue valueWithMGLRasterResamplingMode:MGLRasterResamplingModeNearest].MGLRasterResamplingModeValue, MGLRasterResamplingModeNearest);
}

@end
