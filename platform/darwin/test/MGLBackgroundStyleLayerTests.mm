// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLStyleLayerTests.h"
#import "../../darwin/src/NSDate+MGLAdditions.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/transition_options.hpp>

@interface MGLBackgroundLayerTests : MGLStyleLayerTests
@end

@implementation MGLBackgroundLayerTests

+ (NSString *)layerType {
    return @"background";
}

- (void)testProperties {
    MGLBackgroundStyleLayer *layer = [[MGLBackgroundStyleLayer alloc] initWithIdentifier:@"layerID"];
    XCTAssertNotEqual(layer.rawLayer, nullptr);
    XCTAssertTrue(layer.rawLayer->is<mbgl::style::BackgroundLayer>());
    auto rawLayer = layer.rawLayer->as<mbgl::style::BackgroundLayer>();

    MGLTransition transitionTest = MGLTransitionMake(5, 4);


    // background-color
    {
        XCTAssertTrue(rawLayer->getBackgroundColor().isUndefined(),
                      @"background-color should be unset initially.");
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.backgroundColor;

        MGLStyleValue<MGLColor *> *constantStyleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.backgroundColor = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getBackgroundColor(), propertyValue,
                       @"Setting backgroundColor to a constant value should update background-color.");
        XCTAssertEqualObjects(layer.backgroundColor, constantStyleValue,
                              @"backgroundColor should round-trip constant values.");

        MGLStyleValue<MGLColor *> * functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.backgroundColor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = { {{18, { 1, 0, 0, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };
        
        XCTAssertEqual(rawLayer->getBackgroundColor(), propertyValue,
                       @"Setting backgroundColor to a camera function should update background-color.");
        XCTAssertEqualObjects(layer.backgroundColor, functionStyleValue,
                              @"backgroundColor should round-trip camera functions.");

                              

        layer.backgroundColor = nil;
        XCTAssertTrue(rawLayer->getBackgroundColor().isUndefined(),
                      @"Unsetting backgroundColor should return background-color to the default value.");
        XCTAssertEqualObjects(layer.backgroundColor, defaultStyleValue,
                              @"backgroundColor should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.backgroundColor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.backgroundColor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        // Transition property test
        layer.backgroundColorTransition = transitionTest;
        auto toptions = rawLayer->getBackgroundColorTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition backgroundColorTransition = layer.backgroundColorTransition;
        XCTAssertEqual(backgroundColorTransition.delay, transitionTest.delay);
        XCTAssertEqual(backgroundColorTransition.duration, transitionTest.duration);
    }

    // background-opacity
    {
        XCTAssertTrue(rawLayer->getBackgroundOpacity().isUndefined(),
                      @"background-opacity should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.backgroundOpacity;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.backgroundOpacity = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getBackgroundOpacity(), propertyValue,
                       @"Setting backgroundOpacity to a constant value should update background-opacity.");
        XCTAssertEqualObjects(layer.backgroundOpacity, constantStyleValue,
                              @"backgroundOpacity should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.backgroundOpacity = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getBackgroundOpacity(), propertyValue,
                       @"Setting backgroundOpacity to a camera function should update background-opacity.");
        XCTAssertEqualObjects(layer.backgroundOpacity, functionStyleValue,
                              @"backgroundOpacity should round-trip camera functions.");

                              

        layer.backgroundOpacity = nil;
        XCTAssertTrue(rawLayer->getBackgroundOpacity().isUndefined(),
                      @"Unsetting backgroundOpacity should return background-opacity to the default value.");
        XCTAssertEqualObjects(layer.backgroundOpacity, defaultStyleValue,
                              @"backgroundOpacity should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.backgroundOpacity = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.backgroundOpacity = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        // Transition property test
        layer.backgroundOpacityTransition = transitionTest;
        auto toptions = rawLayer->getBackgroundOpacityTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition backgroundOpacityTransition = layer.backgroundOpacityTransition;
        XCTAssertEqual(backgroundOpacityTransition.delay, transitionTest.delay);
        XCTAssertEqual(backgroundOpacityTransition.duration, transitionTest.duration);
    }

    // background-pattern
    {
        XCTAssertTrue(rawLayer->getBackgroundPattern().isUndefined(),
                      @"background-pattern should be unset initially.");
        MGLStyleValue<NSString *> *defaultStyleValue = layer.backgroundPattern;

        MGLStyleValue<NSString *> *constantStyleValue = [MGLStyleValue<NSString *> valueWithRawValue:@"Background Pattern"];
        layer.backgroundPattern = constantStyleValue;
        mbgl::style::PropertyValue<std::string> propertyValue = { "Background Pattern" };
        XCTAssertEqual(rawLayer->getBackgroundPattern(), propertyValue,
                       @"Setting backgroundPattern to a constant value should update background-pattern.");
        XCTAssertEqualObjects(layer.backgroundPattern, constantStyleValue,
                              @"backgroundPattern should round-trip constant values.");

        MGLStyleValue<NSString *> * functionStyleValue = [MGLStyleValue<NSString *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.backgroundPattern = functionStyleValue;

        mbgl::style::IntervalStops<std::string> intervalStops = { {{18, "Background Pattern"}} };
        propertyValue = mbgl::style::CameraFunction<std::string> { intervalStops };
        
        XCTAssertEqual(rawLayer->getBackgroundPattern(), propertyValue,
                       @"Setting backgroundPattern to a camera function should update background-pattern.");
        XCTAssertEqualObjects(layer.backgroundPattern, functionStyleValue,
                              @"backgroundPattern should round-trip camera functions.");

                              

        layer.backgroundPattern = nil;
        XCTAssertTrue(rawLayer->getBackgroundPattern().isUndefined(),
                      @"Unsetting backgroundPattern should return background-pattern to the default value.");
        XCTAssertEqualObjects(layer.backgroundPattern, defaultStyleValue,
                              @"backgroundPattern should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSString *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.backgroundPattern = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSString *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.backgroundPattern = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        // Transition property test
        layer.backgroundPatternTransition = transitionTest;
        auto toptions = rawLayer->getBackgroundPatternTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition backgroundPatternTransition = layer.backgroundPatternTransition;
        XCTAssertEqual(backgroundPatternTransition.delay, transitionTest.delay);
        XCTAssertEqual(backgroundPatternTransition.duration, transitionTest.duration);
    }
}

- (void)testPropertyNames {
    [self testPropertyName:@"background-color" isBoolean:NO];
    [self testPropertyName:@"background-opacity" isBoolean:NO];
    [self testPropertyName:@"background-pattern" isBoolean:NO];
}

@end
