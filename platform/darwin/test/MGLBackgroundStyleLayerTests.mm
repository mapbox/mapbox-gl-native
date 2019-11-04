// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLStyleLayerTests.h"
#import "../../darwin/src/NSDate+MGLAdditions.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/expression/dsl.hpp>

@interface MGLBackgroundLayerTests : MGLStyleLayerTests
@end

@implementation MGLBackgroundLayerTests

+ (NSString *)layerType {
    return @"background";
}

- (void)testProperties {
    MGLBackgroundStyleLayer *layer = [[MGLBackgroundStyleLayer alloc] initWithIdentifier:@"layerID"];
    XCTAssertNotEqual(layer.rawLayer, nullptr);
    XCTAssertEqualObjects(@(layer.rawLayer->getTypeInfo()->type), @"background");
    auto rawLayer = static_cast<mbgl::style::BackgroundLayer*>(layer.rawLayer);

    MGLTransition transitionTest = MGLTransitionMake(5, 4);


    // background-color
    {
        XCTAssertTrue(rawLayer->getBackgroundColor().isUndefined(),
                      @"background-color should be unset initially.");
        NSExpression *defaultExpression = layer.backgroundColor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        layer.backgroundColor = constantExpression;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getBackgroundColor(), propertyValue,
                       @"Setting backgroundColor to a constant value expression should update background-color.");
        XCTAssertEqualObjects(layer.backgroundColor, constantExpression,
                              @"backgroundColor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.backgroundColor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::Color>(
                step(zoom(), literal(mbgl::Color(1, 0, 0, 1)), 18.0, literal(mbgl::Color(1, 0, 0, 1)))
            );
        }

        XCTAssertEqual(rawLayer->getBackgroundColor(), propertyValue,
                       @"Setting backgroundColor to a camera expression should update background-color.");
        XCTAssertEqualObjects(layer.backgroundColor, functionExpression,
                              @"backgroundColor should round-trip camera expressions.");


        layer.backgroundColor = nil;
        XCTAssertTrue(rawLayer->getBackgroundColor().isUndefined(),
                      @"Unsetting backgroundColor should return background-color to the default value.");
        XCTAssertEqualObjects(layer.backgroundColor, defaultExpression,
                              @"backgroundColor should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.backgroundColor = functionExpression, NSException, NSInvalidArgumentException, @"MGLBackgroundLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.backgroundColor = functionExpression, NSException, NSInvalidArgumentException, @"MGLBackgroundLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
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
        NSExpression *defaultExpression = layer.backgroundOpacity;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.backgroundOpacity = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getBackgroundOpacity(), propertyValue,
                       @"Setting backgroundOpacity to a constant value expression should update background-opacity.");
        XCTAssertEqualObjects(layer.backgroundOpacity, constantExpression,
                              @"backgroundOpacity should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.backgroundOpacity = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getBackgroundOpacity(), propertyValue,
                       @"Setting backgroundOpacity to a camera expression should update background-opacity.");
        XCTAssertEqualObjects(layer.backgroundOpacity, functionExpression,
                              @"backgroundOpacity should round-trip camera expressions.");


        layer.backgroundOpacity = nil;
        XCTAssertTrue(rawLayer->getBackgroundOpacity().isUndefined(),
                      @"Unsetting backgroundOpacity should return background-opacity to the default value.");
        XCTAssertEqualObjects(layer.backgroundOpacity, defaultExpression,
                              @"backgroundOpacity should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.backgroundOpacity = functionExpression, NSException, NSInvalidArgumentException, @"MGLBackgroundLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.backgroundOpacity = functionExpression, NSException, NSInvalidArgumentException, @"MGLBackgroundLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
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
        NSExpression *defaultExpression = layer.backgroundPattern;

        NSExpression *constantExpression = [NSExpression expressionForConstantValue:@"Background Pattern"];
        layer.backgroundPattern = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::expression::Image> propertyValue = { "Background Pattern" };
        XCTAssertEqual(rawLayer->getBackgroundPattern(), propertyValue,
                       @"Setting backgroundPattern to a constant value expression should update background-pattern.");
        XCTAssertEqualObjects(layer.backgroundPattern, constantExpression,
                              @"backgroundPattern should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"MGL_FUNCTION('image', 'Background Pattern')"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.backgroundPattern = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::expression::Image>(
                step(zoom(), image(literal("Background Pattern")), 18.0, image(literal("Background Pattern")))
            );
        }

        XCTAssertEqual(rawLayer->getBackgroundPattern(), propertyValue,
                       @"Setting backgroundPattern to a camera expression should update background-pattern.");
        XCTAssertEqualObjects(layer.backgroundPattern, functionExpression,
                              @"backgroundPattern should round-trip camera expressions.");


        layer.backgroundPattern = nil;
        XCTAssertTrue(rawLayer->getBackgroundPattern().isUndefined(),
                      @"Unsetting backgroundPattern should return background-pattern to the default value.");
        XCTAssertEqualObjects(layer.backgroundPattern, defaultExpression,
                              @"backgroundPattern should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.backgroundPattern = functionExpression, NSException, NSInvalidArgumentException, @"MGLBackgroundLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.backgroundPattern = functionExpression, NSException, NSInvalidArgumentException, @"MGLBackgroundLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
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
