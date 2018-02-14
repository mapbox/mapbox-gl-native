// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLStyleLayerTests.h"
#import "../../darwin/src/NSDate+MGLAdditions.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/hillshade_layer.hpp>
#include <mbgl/style/transition_options.hpp>

@interface MGLHillshadeLayerTests : MGLStyleLayerTests
@end

@implementation MGLHillshadeLayerTests

+ (NSString *)layerType {
    return @"hillshade";
}

- (void)testProperties {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];

    MGLHillshadeStyleLayer *layer = [[MGLHillshadeStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    XCTAssertNotEqual(layer.rawLayer, nullptr);
    XCTAssertTrue(layer.rawLayer->is<mbgl::style::HillshadeLayer>());
    auto rawLayer = layer.rawLayer->as<mbgl::style::HillshadeLayer>();

    MGLTransition transitionTest = MGLTransitionMake(5, 4);


    // hillshade-accent-color
    {
        XCTAssertTrue(rawLayer->getHillshadeAccentColor().isUndefined(),
                      @"hillshade-accent-color should be unset initially.");
        NSExpression *defaultExpression = layer.hillshadeAccentColor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        layer.hillshadeAccentColor = constantExpression;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getHillshadeAccentColor(), propertyValue,
                       @"Setting hillshadeAccentColor to a constant value expression should update hillshade-accent-color.");
        XCTAssertEqualObjects(layer.hillshadeAccentColor, constantExpression,
                              @"hillshadeAccentColor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.hillshadeAccentColor = functionExpression;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = {{
            { -INFINITY, { 1, 0, 0, 1 } },
            { 18, { 1, 0, 0, 1 } },
        }};
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };

        XCTAssertEqual(rawLayer->getHillshadeAccentColor(), propertyValue,
                       @"Setting hillshadeAccentColor to a camera expression should update hillshade-accent-color.");
        XCTAssertEqualObjects(layer.hillshadeAccentColor, functionExpression,
                              @"hillshadeAccentColor should round-trip camera expressions.");


        layer.hillshadeAccentColor = nil;
        XCTAssertTrue(rawLayer->getHillshadeAccentColor().isUndefined(),
                      @"Unsetting hillshadeAccentColor should return hillshade-accent-color to the default value.");
        XCTAssertEqualObjects(layer.hillshadeAccentColor, defaultExpression,
                              @"hillshadeAccentColor should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.hillshadeAccentColor = functionExpression, NSException, NSInvalidArgumentException, @"MGLHillshadeLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(bogus, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.hillshadeAccentColor = functionExpression, NSException, NSInvalidArgumentException, @"MGLHillshadeLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        // Transition property test
        layer.hillshadeAccentColorTransition = transitionTest;
        auto toptions = rawLayer->getHillshadeAccentColorTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition hillshadeAccentColorTransition = layer.hillshadeAccentColorTransition;
        XCTAssertEqual(hillshadeAccentColorTransition.delay, transitionTest.delay);
        XCTAssertEqual(hillshadeAccentColorTransition.duration, transitionTest.duration);
    }

    // hillshade-exaggeration
    {
        XCTAssertTrue(rawLayer->getHillshadeExaggeration().isUndefined(),
                      @"hillshade-exaggeration should be unset initially.");
        NSExpression *defaultExpression = layer.hillshadeExaggeration;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        layer.hillshadeExaggeration = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getHillshadeExaggeration(), propertyValue,
                       @"Setting hillshadeExaggeration to a constant value expression should update hillshade-exaggeration.");
        XCTAssertEqualObjects(layer.hillshadeExaggeration, constantExpression,
                              @"hillshadeExaggeration should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.hillshadeExaggeration = functionExpression;

        mbgl::style::IntervalStops<float> intervalStops = {{
            { -INFINITY, 0xff },
            { 18, 0xff },
        }};
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };

        XCTAssertEqual(rawLayer->getHillshadeExaggeration(), propertyValue,
                       @"Setting hillshadeExaggeration to a camera expression should update hillshade-exaggeration.");
        XCTAssertEqualObjects(layer.hillshadeExaggeration, functionExpression,
                              @"hillshadeExaggeration should round-trip camera expressions.");


        layer.hillshadeExaggeration = nil;
        XCTAssertTrue(rawLayer->getHillshadeExaggeration().isUndefined(),
                      @"Unsetting hillshadeExaggeration should return hillshade-exaggeration to the default value.");
        XCTAssertEqualObjects(layer.hillshadeExaggeration, defaultExpression,
                              @"hillshadeExaggeration should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.hillshadeExaggeration = functionExpression, NSException, NSInvalidArgumentException, @"MGLHillshadeLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(bogus, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.hillshadeExaggeration = functionExpression, NSException, NSInvalidArgumentException, @"MGLHillshadeLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        // Transition property test
        layer.hillshadeExaggerationTransition = transitionTest;
        auto toptions = rawLayer->getHillshadeExaggerationTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition hillshadeExaggerationTransition = layer.hillshadeExaggerationTransition;
        XCTAssertEqual(hillshadeExaggerationTransition.delay, transitionTest.delay);
        XCTAssertEqual(hillshadeExaggerationTransition.duration, transitionTest.duration);
    }

    // hillshade-highlight-color
    {
        XCTAssertTrue(rawLayer->getHillshadeHighlightColor().isUndefined(),
                      @"hillshade-highlight-color should be unset initially.");
        NSExpression *defaultExpression = layer.hillshadeHighlightColor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        layer.hillshadeHighlightColor = constantExpression;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getHillshadeHighlightColor(), propertyValue,
                       @"Setting hillshadeHighlightColor to a constant value expression should update hillshade-highlight-color.");
        XCTAssertEqualObjects(layer.hillshadeHighlightColor, constantExpression,
                              @"hillshadeHighlightColor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.hillshadeHighlightColor = functionExpression;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = {{
            { -INFINITY, { 1, 0, 0, 1 } },
            { 18, { 1, 0, 0, 1 } },
        }};
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };

        XCTAssertEqual(rawLayer->getHillshadeHighlightColor(), propertyValue,
                       @"Setting hillshadeHighlightColor to a camera expression should update hillshade-highlight-color.");
        XCTAssertEqualObjects(layer.hillshadeHighlightColor, functionExpression,
                              @"hillshadeHighlightColor should round-trip camera expressions.");


        layer.hillshadeHighlightColor = nil;
        XCTAssertTrue(rawLayer->getHillshadeHighlightColor().isUndefined(),
                      @"Unsetting hillshadeHighlightColor should return hillshade-highlight-color to the default value.");
        XCTAssertEqualObjects(layer.hillshadeHighlightColor, defaultExpression,
                              @"hillshadeHighlightColor should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.hillshadeHighlightColor = functionExpression, NSException, NSInvalidArgumentException, @"MGLHillshadeLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(bogus, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.hillshadeHighlightColor = functionExpression, NSException, NSInvalidArgumentException, @"MGLHillshadeLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        // Transition property test
        layer.hillshadeHighlightColorTransition = transitionTest;
        auto toptions = rawLayer->getHillshadeHighlightColorTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition hillshadeHighlightColorTransition = layer.hillshadeHighlightColorTransition;
        XCTAssertEqual(hillshadeHighlightColorTransition.delay, transitionTest.delay);
        XCTAssertEqual(hillshadeHighlightColorTransition.duration, transitionTest.duration);
    }

    // hillshade-illumination-anchor
    {
        XCTAssertTrue(rawLayer->getHillshadeIlluminationAnchor().isUndefined(),
                      @"hillshade-illumination-anchor should be unset initially.");
        NSExpression *defaultExpression = layer.hillshadeIlluminationAnchor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'viewport'"];
        layer.hillshadeIlluminationAnchor = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::HillshadeIlluminationAnchorType> propertyValue = { mbgl::style::HillshadeIlluminationAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getHillshadeIlluminationAnchor(), propertyValue,
                       @"Setting hillshadeIlluminationAnchor to a constant value expression should update hillshade-illumination-anchor.");
        XCTAssertEqualObjects(layer.hillshadeIlluminationAnchor, constantExpression,
                              @"hillshadeIlluminationAnchor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'viewport'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.hillshadeIlluminationAnchor = functionExpression;

        mbgl::style::IntervalStops<mbgl::style::HillshadeIlluminationAnchorType> intervalStops = {{
            { -INFINITY, mbgl::style::HillshadeIlluminationAnchorType::Viewport },
            { 18, mbgl::style::HillshadeIlluminationAnchorType::Viewport },
        }};
        propertyValue = mbgl::style::CameraFunction<mbgl::style::HillshadeIlluminationAnchorType> { intervalStops };

        XCTAssertEqual(rawLayer->getHillshadeIlluminationAnchor(), propertyValue,
                       @"Setting hillshadeIlluminationAnchor to a camera expression should update hillshade-illumination-anchor.");
        XCTAssertEqualObjects(layer.hillshadeIlluminationAnchor, functionExpression,
                              @"hillshadeIlluminationAnchor should round-trip camera expressions.");


        layer.hillshadeIlluminationAnchor = nil;
        XCTAssertTrue(rawLayer->getHillshadeIlluminationAnchor().isUndefined(),
                      @"Unsetting hillshadeIlluminationAnchor should return hillshade-illumination-anchor to the default value.");
        XCTAssertEqualObjects(layer.hillshadeIlluminationAnchor, defaultExpression,
                              @"hillshadeIlluminationAnchor should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.hillshadeIlluminationAnchor = functionExpression, NSException, NSInvalidArgumentException, @"MGLHillshadeLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(bogus, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.hillshadeIlluminationAnchor = functionExpression, NSException, NSInvalidArgumentException, @"MGLHillshadeLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // hillshade-illumination-direction
    {
        XCTAssertTrue(rawLayer->getHillshadeIlluminationDirection().isUndefined(),
                      @"hillshade-illumination-direction should be unset initially.");
        NSExpression *defaultExpression = layer.hillshadeIlluminationDirection;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        layer.hillshadeIlluminationDirection = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getHillshadeIlluminationDirection(), propertyValue,
                       @"Setting hillshadeIlluminationDirection to a constant value expression should update hillshade-illumination-direction.");
        XCTAssertEqualObjects(layer.hillshadeIlluminationDirection, constantExpression,
                              @"hillshadeIlluminationDirection should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.hillshadeIlluminationDirection = functionExpression;

        mbgl::style::IntervalStops<float> intervalStops = {{
            { -INFINITY, 0xff },
            { 18, 0xff },
        }};
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };

        XCTAssertEqual(rawLayer->getHillshadeIlluminationDirection(), propertyValue,
                       @"Setting hillshadeIlluminationDirection to a camera expression should update hillshade-illumination-direction.");
        XCTAssertEqualObjects(layer.hillshadeIlluminationDirection, functionExpression,
                              @"hillshadeIlluminationDirection should round-trip camera expressions.");


        layer.hillshadeIlluminationDirection = nil;
        XCTAssertTrue(rawLayer->getHillshadeIlluminationDirection().isUndefined(),
                      @"Unsetting hillshadeIlluminationDirection should return hillshade-illumination-direction to the default value.");
        XCTAssertEqualObjects(layer.hillshadeIlluminationDirection, defaultExpression,
                              @"hillshadeIlluminationDirection should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.hillshadeIlluminationDirection = functionExpression, NSException, NSInvalidArgumentException, @"MGLHillshadeLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(bogus, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.hillshadeIlluminationDirection = functionExpression, NSException, NSInvalidArgumentException, @"MGLHillshadeLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // hillshade-shadow-color
    {
        XCTAssertTrue(rawLayer->getHillshadeShadowColor().isUndefined(),
                      @"hillshade-shadow-color should be unset initially.");
        NSExpression *defaultExpression = layer.hillshadeShadowColor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        layer.hillshadeShadowColor = constantExpression;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getHillshadeShadowColor(), propertyValue,
                       @"Setting hillshadeShadowColor to a constant value expression should update hillshade-shadow-color.");
        XCTAssertEqualObjects(layer.hillshadeShadowColor, constantExpression,
                              @"hillshadeShadowColor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.hillshadeShadowColor = functionExpression;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = {{
            { -INFINITY, { 1, 0, 0, 1 } },
            { 18, { 1, 0, 0, 1 } },
        }};
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };

        XCTAssertEqual(rawLayer->getHillshadeShadowColor(), propertyValue,
                       @"Setting hillshadeShadowColor to a camera expression should update hillshade-shadow-color.");
        XCTAssertEqualObjects(layer.hillshadeShadowColor, functionExpression,
                              @"hillshadeShadowColor should round-trip camera expressions.");


        layer.hillshadeShadowColor = nil;
        XCTAssertTrue(rawLayer->getHillshadeShadowColor().isUndefined(),
                      @"Unsetting hillshadeShadowColor should return hillshade-shadow-color to the default value.");
        XCTAssertEqualObjects(layer.hillshadeShadowColor, defaultExpression,
                              @"hillshadeShadowColor should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.hillshadeShadowColor = functionExpression, NSException, NSInvalidArgumentException, @"MGLHillshadeLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(bogus, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.hillshadeShadowColor = functionExpression, NSException, NSInvalidArgumentException, @"MGLHillshadeLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        // Transition property test
        layer.hillshadeShadowColorTransition = transitionTest;
        auto toptions = rawLayer->getHillshadeShadowColorTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition hillshadeShadowColorTransition = layer.hillshadeShadowColorTransition;
        XCTAssertEqual(hillshadeShadowColorTransition.delay, transitionTest.delay);
        XCTAssertEqual(hillshadeShadowColorTransition.duration, transitionTest.duration);
    }
}

- (void)testPropertyNames {
    [self testPropertyName:@"hillshade-accent-color" isBoolean:NO];
    [self testPropertyName:@"hillshade-exaggeration" isBoolean:NO];
    [self testPropertyName:@"hillshade-highlight-color" isBoolean:NO];
    [self testPropertyName:@"hillshade-illumination-anchor" isBoolean:NO];
    [self testPropertyName:@"hillshade-illumination-direction" isBoolean:NO];
    [self testPropertyName:@"hillshade-shadow-color" isBoolean:NO];
}

- (void)testValueAdditions {
    XCTAssertEqual([NSValue valueWithMGLHillshadeIlluminationAnchor:MGLHillshadeIlluminationAnchorMap].MGLHillshadeIlluminationAnchorValue, MGLHillshadeIlluminationAnchorMap);
    XCTAssertEqual([NSValue valueWithMGLHillshadeIlluminationAnchor:MGLHillshadeIlluminationAnchorViewport].MGLHillshadeIlluminationAnchorValue, MGLHillshadeIlluminationAnchorViewport);
}

@end
