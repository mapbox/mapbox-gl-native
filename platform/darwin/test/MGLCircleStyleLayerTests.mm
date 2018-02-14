// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLStyleLayerTests.h"
#import "../../darwin/src/NSDate+MGLAdditions.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/transition_options.hpp>

@interface MGLCircleLayerTests : MGLStyleLayerTests
@end

@implementation MGLCircleLayerTests

+ (NSString *)layerType {
    return @"circle";
}

- (void)testPredicates {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];
    MGLCircleStyleLayer *layer = [[MGLCircleStyleLayer alloc] initWithIdentifier:@"layerID" source:source];

    XCTAssertNil(layer.sourceLayerIdentifier);
    layer.sourceLayerIdentifier = @"layerID";
    XCTAssertEqualObjects(layer.sourceLayerIdentifier, @"layerID");
    layer.sourceLayerIdentifier = nil;
    XCTAssertNil(layer.sourceLayerIdentifier);

    XCTAssertNil(layer.predicate);
    layer.predicate = [NSPredicate predicateWithValue:NO];
    XCTAssertEqualObjects(layer.predicate, [NSPredicate predicateWithValue:NO]);
    layer.predicate = nil;
    XCTAssertNil(layer.predicate);
}

- (void)testProperties {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];

    MGLCircleStyleLayer *layer = [[MGLCircleStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    XCTAssertNotEqual(layer.rawLayer, nullptr);
    XCTAssertTrue(layer.rawLayer->is<mbgl::style::CircleLayer>());
    auto rawLayer = layer.rawLayer->as<mbgl::style::CircleLayer>();

    MGLTransition transitionTest = MGLTransitionMake(5, 4);


    // circle-blur
    {
        XCTAssertTrue(rawLayer->getCircleBlur().isUndefined(),
                      @"circle-blur should be unset initially.");
        NSExpression *defaultExpression = layer.circleBlur;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        layer.circleBlur = constantExpression;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getCircleBlur(), propertyValue,
                       @"Setting circleBlur to a constant value expression should update circle-blur.");
        XCTAssertEqualObjects(layer.circleBlur, constantExpression,
                              @"circleBlur should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.circleBlur = functionExpression;

        mbgl::style::IntervalStops<float> intervalStops = {{
            { -INFINITY, 0xff },
            { 18, 0xff },
        }};
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };

        XCTAssertEqual(rawLayer->getCircleBlur(), propertyValue,
                       @"Setting circleBlur to a camera expression should update circle-blur.");
        XCTAssertEqualObjects(layer.circleBlur, functionExpression,
                              @"circleBlur should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(keyName, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@18: constantExpression}];
        layer.circleBlur = functionExpression;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getCircleBlur(), propertyValue,
                       @"Setting circleBlur to a data expression should update circle-blur.");
        XCTAssertEqualObjects(layer.circleBlur, functionExpression,
                              @"circleBlur should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        layer.circleBlur = functionExpression;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getCircleBlur(), propertyValue,
                       @"Setting circleBlur to a camera-data expression should update circle-blur.");
        XCTAssertEqualObjects(layer.circleBlur, functionExpression,
                              @"circleBlur should round-trip camera-data expressions.");

        layer.circleBlur = nil;
        XCTAssertTrue(rawLayer->getCircleBlur().isUndefined(),
                      @"Unsetting circleBlur should return circle-blur to the default value.");
        XCTAssertEqualObjects(layer.circleBlur, defaultExpression,
                              @"circleBlur should return the default value after being unset.");
        // Transition property test
        layer.circleBlurTransition = transitionTest;
        auto toptions = rawLayer->getCircleBlurTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition circleBlurTransition = layer.circleBlurTransition;
        XCTAssertEqual(circleBlurTransition.delay, transitionTest.delay);
        XCTAssertEqual(circleBlurTransition.duration, transitionTest.duration);
    }

    // circle-color
    {
        XCTAssertTrue(rawLayer->getCircleColor().isUndefined(),
                      @"circle-color should be unset initially.");
        NSExpression *defaultExpression = layer.circleColor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        layer.circleColor = constantExpression;
        mbgl::style::DataDrivenPropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getCircleColor(), propertyValue,
                       @"Setting circleColor to a constant value expression should update circle-color.");
        XCTAssertEqualObjects(layer.circleColor, constantExpression,
                              @"circleColor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.circleColor = functionExpression;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = {{
            { -INFINITY, { 1, 0, 0, 1 } },
            { 18, { 1, 0, 0, 1 } },
        }};
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };

        XCTAssertEqual(rawLayer->getCircleColor(), propertyValue,
                       @"Setting circleColor to a camera expression should update circle-color.");
        XCTAssertEqualObjects(layer.circleColor, functionExpression,
                              @"circleColor should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(keyName, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@18: constantExpression}];
        layer.circleColor = functionExpression;

        mbgl::style::ExponentialStops<mbgl::Color> exponentialStops = { {{18, { 1, 0, 0, 1 }}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<mbgl::Color> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getCircleColor(), propertyValue,
                       @"Setting circleColor to a data expression should update circle-color.");
        XCTAssertEqualObjects(layer.circleColor, functionExpression,
                              @"circleColor should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        layer.circleColor = functionExpression;

        std::map<float, mbgl::Color> innerStops { {18, { 1, 0, 0, 1 }} };
        mbgl::style::CompositeExponentialStops<mbgl::Color> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<mbgl::Color> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getCircleColor(), propertyValue,
                       @"Setting circleColor to a camera-data expression should update circle-color.");
        XCTAssertEqualObjects(layer.circleColor, functionExpression,
                              @"circleColor should round-trip camera-data expressions.");

        layer.circleColor = nil;
        XCTAssertTrue(rawLayer->getCircleColor().isUndefined(),
                      @"Unsetting circleColor should return circle-color to the default value.");
        XCTAssertEqualObjects(layer.circleColor, defaultExpression,
                              @"circleColor should return the default value after being unset.");
        // Transition property test
        layer.circleColorTransition = transitionTest;
        auto toptions = rawLayer->getCircleColorTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition circleColorTransition = layer.circleColorTransition;
        XCTAssertEqual(circleColorTransition.delay, transitionTest.delay);
        XCTAssertEqual(circleColorTransition.duration, transitionTest.duration);
    }

    // circle-opacity
    {
        XCTAssertTrue(rawLayer->getCircleOpacity().isUndefined(),
                      @"circle-opacity should be unset initially.");
        NSExpression *defaultExpression = layer.circleOpacity;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        layer.circleOpacity = constantExpression;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getCircleOpacity(), propertyValue,
                       @"Setting circleOpacity to a constant value expression should update circle-opacity.");
        XCTAssertEqualObjects(layer.circleOpacity, constantExpression,
                              @"circleOpacity should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.circleOpacity = functionExpression;

        mbgl::style::IntervalStops<float> intervalStops = {{
            { -INFINITY, 0xff },
            { 18, 0xff },
        }};
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };

        XCTAssertEqual(rawLayer->getCircleOpacity(), propertyValue,
                       @"Setting circleOpacity to a camera expression should update circle-opacity.");
        XCTAssertEqualObjects(layer.circleOpacity, functionExpression,
                              @"circleOpacity should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(keyName, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@18: constantExpression}];
        layer.circleOpacity = functionExpression;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getCircleOpacity(), propertyValue,
                       @"Setting circleOpacity to a data expression should update circle-opacity.");
        XCTAssertEqualObjects(layer.circleOpacity, functionExpression,
                              @"circleOpacity should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        layer.circleOpacity = functionExpression;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getCircleOpacity(), propertyValue,
                       @"Setting circleOpacity to a camera-data expression should update circle-opacity.");
        XCTAssertEqualObjects(layer.circleOpacity, functionExpression,
                              @"circleOpacity should round-trip camera-data expressions.");

        layer.circleOpacity = nil;
        XCTAssertTrue(rawLayer->getCircleOpacity().isUndefined(),
                      @"Unsetting circleOpacity should return circle-opacity to the default value.");
        XCTAssertEqualObjects(layer.circleOpacity, defaultExpression,
                              @"circleOpacity should return the default value after being unset.");
        // Transition property test
        layer.circleOpacityTransition = transitionTest;
        auto toptions = rawLayer->getCircleOpacityTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition circleOpacityTransition = layer.circleOpacityTransition;
        XCTAssertEqual(circleOpacityTransition.delay, transitionTest.delay);
        XCTAssertEqual(circleOpacityTransition.duration, transitionTest.duration);
    }

    // circle-pitch-alignment
    {
        XCTAssertTrue(rawLayer->getCirclePitchAlignment().isUndefined(),
                      @"circle-pitch-alignment should be unset initially.");
        NSExpression *defaultExpression = layer.circlePitchAlignment;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'viewport'"];
        layer.circlePitchAlignment = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::AlignmentType> propertyValue = { mbgl::style::AlignmentType::Viewport };
        XCTAssertEqual(rawLayer->getCirclePitchAlignment(), propertyValue,
                       @"Setting circlePitchAlignment to a constant value expression should update circle-pitch-alignment.");
        XCTAssertEqualObjects(layer.circlePitchAlignment, constantExpression,
                              @"circlePitchAlignment should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'viewport'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.circlePitchAlignment = functionExpression;

        mbgl::style::IntervalStops<mbgl::style::AlignmentType> intervalStops = {{
            { -INFINITY, mbgl::style::AlignmentType::Viewport },
            { 18, mbgl::style::AlignmentType::Viewport },
        }};
        propertyValue = mbgl::style::CameraFunction<mbgl::style::AlignmentType> { intervalStops };

        XCTAssertEqual(rawLayer->getCirclePitchAlignment(), propertyValue,
                       @"Setting circlePitchAlignment to a camera expression should update circle-pitch-alignment.");
        XCTAssertEqualObjects(layer.circlePitchAlignment, functionExpression,
                              @"circlePitchAlignment should round-trip camera expressions.");


        layer.circlePitchAlignment = nil;
        XCTAssertTrue(rawLayer->getCirclePitchAlignment().isUndefined(),
                      @"Unsetting circlePitchAlignment should return circle-pitch-alignment to the default value.");
        XCTAssertEqualObjects(layer.circlePitchAlignment, defaultExpression,
                              @"circlePitchAlignment should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.circlePitchAlignment = functionExpression, NSException, NSInvalidArgumentException, @"MGLCircleLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(bogus, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.circlePitchAlignment = functionExpression, NSException, NSInvalidArgumentException, @"MGLCircleLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // circle-radius
    {
        XCTAssertTrue(rawLayer->getCircleRadius().isUndefined(),
                      @"circle-radius should be unset initially.");
        NSExpression *defaultExpression = layer.circleRadius;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        layer.circleRadius = constantExpression;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getCircleRadius(), propertyValue,
                       @"Setting circleRadius to a constant value expression should update circle-radius.");
        XCTAssertEqualObjects(layer.circleRadius, constantExpression,
                              @"circleRadius should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.circleRadius = functionExpression;

        mbgl::style::IntervalStops<float> intervalStops = {{
            { -INFINITY, 0xff },
            { 18, 0xff },
        }};
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };

        XCTAssertEqual(rawLayer->getCircleRadius(), propertyValue,
                       @"Setting circleRadius to a camera expression should update circle-radius.");
        XCTAssertEqualObjects(layer.circleRadius, functionExpression,
                              @"circleRadius should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(keyName, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@18: constantExpression}];
        layer.circleRadius = functionExpression;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getCircleRadius(), propertyValue,
                       @"Setting circleRadius to a data expression should update circle-radius.");
        XCTAssertEqualObjects(layer.circleRadius, functionExpression,
                              @"circleRadius should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        layer.circleRadius = functionExpression;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getCircleRadius(), propertyValue,
                       @"Setting circleRadius to a camera-data expression should update circle-radius.");
        XCTAssertEqualObjects(layer.circleRadius, functionExpression,
                              @"circleRadius should round-trip camera-data expressions.");

        layer.circleRadius = nil;
        XCTAssertTrue(rawLayer->getCircleRadius().isUndefined(),
                      @"Unsetting circleRadius should return circle-radius to the default value.");
        XCTAssertEqualObjects(layer.circleRadius, defaultExpression,
                              @"circleRadius should return the default value after being unset.");
        // Transition property test
        layer.circleRadiusTransition = transitionTest;
        auto toptions = rawLayer->getCircleRadiusTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition circleRadiusTransition = layer.circleRadiusTransition;
        XCTAssertEqual(circleRadiusTransition.delay, transitionTest.delay);
        XCTAssertEqual(circleRadiusTransition.duration, transitionTest.duration);
    }

    // circle-pitch-scale
    {
        XCTAssertTrue(rawLayer->getCirclePitchScale().isUndefined(),
                      @"circle-pitch-scale should be unset initially.");
        NSExpression *defaultExpression = layer.circleScaleAlignment;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'viewport'"];
        layer.circleScaleAlignment = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::CirclePitchScaleType> propertyValue = { mbgl::style::CirclePitchScaleType::Viewport };
        XCTAssertEqual(rawLayer->getCirclePitchScale(), propertyValue,
                       @"Setting circleScaleAlignment to a constant value expression should update circle-pitch-scale.");
        XCTAssertEqualObjects(layer.circleScaleAlignment, constantExpression,
                              @"circleScaleAlignment should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'viewport'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.circleScaleAlignment = functionExpression;

        mbgl::style::IntervalStops<mbgl::style::CirclePitchScaleType> intervalStops = {{
            { -INFINITY, mbgl::style::CirclePitchScaleType::Viewport },
            { 18, mbgl::style::CirclePitchScaleType::Viewport },
        }};
        propertyValue = mbgl::style::CameraFunction<mbgl::style::CirclePitchScaleType> { intervalStops };

        XCTAssertEqual(rawLayer->getCirclePitchScale(), propertyValue,
                       @"Setting circleScaleAlignment to a camera expression should update circle-pitch-scale.");
        XCTAssertEqualObjects(layer.circleScaleAlignment, functionExpression,
                              @"circleScaleAlignment should round-trip camera expressions.");


        layer.circleScaleAlignment = nil;
        XCTAssertTrue(rawLayer->getCirclePitchScale().isUndefined(),
                      @"Unsetting circleScaleAlignment should return circle-pitch-scale to the default value.");
        XCTAssertEqualObjects(layer.circleScaleAlignment, defaultExpression,
                              @"circleScaleAlignment should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.circleScaleAlignment = functionExpression, NSException, NSInvalidArgumentException, @"MGLCircleLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(bogus, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.circleScaleAlignment = functionExpression, NSException, NSInvalidArgumentException, @"MGLCircleLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // circle-stroke-color
    {
        XCTAssertTrue(rawLayer->getCircleStrokeColor().isUndefined(),
                      @"circle-stroke-color should be unset initially.");
        NSExpression *defaultExpression = layer.circleStrokeColor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        layer.circleStrokeColor = constantExpression;
        mbgl::style::DataDrivenPropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getCircleStrokeColor(), propertyValue,
                       @"Setting circleStrokeColor to a constant value expression should update circle-stroke-color.");
        XCTAssertEqualObjects(layer.circleStrokeColor, constantExpression,
                              @"circleStrokeColor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.circleStrokeColor = functionExpression;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = {{
            { -INFINITY, { 1, 0, 0, 1 } },
            { 18, { 1, 0, 0, 1 } },
        }};
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };

        XCTAssertEqual(rawLayer->getCircleStrokeColor(), propertyValue,
                       @"Setting circleStrokeColor to a camera expression should update circle-stroke-color.");
        XCTAssertEqualObjects(layer.circleStrokeColor, functionExpression,
                              @"circleStrokeColor should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(keyName, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@18: constantExpression}];
        layer.circleStrokeColor = functionExpression;

        mbgl::style::ExponentialStops<mbgl::Color> exponentialStops = { {{18, { 1, 0, 0, 1 }}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<mbgl::Color> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getCircleStrokeColor(), propertyValue,
                       @"Setting circleStrokeColor to a data expression should update circle-stroke-color.");
        XCTAssertEqualObjects(layer.circleStrokeColor, functionExpression,
                              @"circleStrokeColor should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        layer.circleStrokeColor = functionExpression;

        std::map<float, mbgl::Color> innerStops { {18, { 1, 0, 0, 1 }} };
        mbgl::style::CompositeExponentialStops<mbgl::Color> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<mbgl::Color> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getCircleStrokeColor(), propertyValue,
                       @"Setting circleStrokeColor to a camera-data expression should update circle-stroke-color.");
        XCTAssertEqualObjects(layer.circleStrokeColor, functionExpression,
                              @"circleStrokeColor should round-trip camera-data expressions.");

        layer.circleStrokeColor = nil;
        XCTAssertTrue(rawLayer->getCircleStrokeColor().isUndefined(),
                      @"Unsetting circleStrokeColor should return circle-stroke-color to the default value.");
        XCTAssertEqualObjects(layer.circleStrokeColor, defaultExpression,
                              @"circleStrokeColor should return the default value after being unset.");
        // Transition property test
        layer.circleStrokeColorTransition = transitionTest;
        auto toptions = rawLayer->getCircleStrokeColorTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition circleStrokeColorTransition = layer.circleStrokeColorTransition;
        XCTAssertEqual(circleStrokeColorTransition.delay, transitionTest.delay);
        XCTAssertEqual(circleStrokeColorTransition.duration, transitionTest.duration);
    }

    // circle-stroke-opacity
    {
        XCTAssertTrue(rawLayer->getCircleStrokeOpacity().isUndefined(),
                      @"circle-stroke-opacity should be unset initially.");
        NSExpression *defaultExpression = layer.circleStrokeOpacity;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        layer.circleStrokeOpacity = constantExpression;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getCircleStrokeOpacity(), propertyValue,
                       @"Setting circleStrokeOpacity to a constant value expression should update circle-stroke-opacity.");
        XCTAssertEqualObjects(layer.circleStrokeOpacity, constantExpression,
                              @"circleStrokeOpacity should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.circleStrokeOpacity = functionExpression;

        mbgl::style::IntervalStops<float> intervalStops = {{
            { -INFINITY, 0xff },
            { 18, 0xff },
        }};
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };

        XCTAssertEqual(rawLayer->getCircleStrokeOpacity(), propertyValue,
                       @"Setting circleStrokeOpacity to a camera expression should update circle-stroke-opacity.");
        XCTAssertEqualObjects(layer.circleStrokeOpacity, functionExpression,
                              @"circleStrokeOpacity should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(keyName, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@18: constantExpression}];
        layer.circleStrokeOpacity = functionExpression;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getCircleStrokeOpacity(), propertyValue,
                       @"Setting circleStrokeOpacity to a data expression should update circle-stroke-opacity.");
        XCTAssertEqualObjects(layer.circleStrokeOpacity, functionExpression,
                              @"circleStrokeOpacity should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        layer.circleStrokeOpacity = functionExpression;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getCircleStrokeOpacity(), propertyValue,
                       @"Setting circleStrokeOpacity to a camera-data expression should update circle-stroke-opacity.");
        XCTAssertEqualObjects(layer.circleStrokeOpacity, functionExpression,
                              @"circleStrokeOpacity should round-trip camera-data expressions.");

        layer.circleStrokeOpacity = nil;
        XCTAssertTrue(rawLayer->getCircleStrokeOpacity().isUndefined(),
                      @"Unsetting circleStrokeOpacity should return circle-stroke-opacity to the default value.");
        XCTAssertEqualObjects(layer.circleStrokeOpacity, defaultExpression,
                              @"circleStrokeOpacity should return the default value after being unset.");
        // Transition property test
        layer.circleStrokeOpacityTransition = transitionTest;
        auto toptions = rawLayer->getCircleStrokeOpacityTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition circleStrokeOpacityTransition = layer.circleStrokeOpacityTransition;
        XCTAssertEqual(circleStrokeOpacityTransition.delay, transitionTest.delay);
        XCTAssertEqual(circleStrokeOpacityTransition.duration, transitionTest.duration);
    }

    // circle-stroke-width
    {
        XCTAssertTrue(rawLayer->getCircleStrokeWidth().isUndefined(),
                      @"circle-stroke-width should be unset initially.");
        NSExpression *defaultExpression = layer.circleStrokeWidth;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        layer.circleStrokeWidth = constantExpression;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getCircleStrokeWidth(), propertyValue,
                       @"Setting circleStrokeWidth to a constant value expression should update circle-stroke-width.");
        XCTAssertEqualObjects(layer.circleStrokeWidth, constantExpression,
                              @"circleStrokeWidth should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.circleStrokeWidth = functionExpression;

        mbgl::style::IntervalStops<float> intervalStops = {{
            { -INFINITY, 0xff },
            { 18, 0xff },
        }};
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };

        XCTAssertEqual(rawLayer->getCircleStrokeWidth(), propertyValue,
                       @"Setting circleStrokeWidth to a camera expression should update circle-stroke-width.");
        XCTAssertEqualObjects(layer.circleStrokeWidth, functionExpression,
                              @"circleStrokeWidth should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(keyName, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@18: constantExpression}];
        layer.circleStrokeWidth = functionExpression;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getCircleStrokeWidth(), propertyValue,
                       @"Setting circleStrokeWidth to a data expression should update circle-stroke-width.");
        XCTAssertEqualObjects(layer.circleStrokeWidth, functionExpression,
                              @"circleStrokeWidth should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        layer.circleStrokeWidth = functionExpression;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getCircleStrokeWidth(), propertyValue,
                       @"Setting circleStrokeWidth to a camera-data expression should update circle-stroke-width.");
        XCTAssertEqualObjects(layer.circleStrokeWidth, functionExpression,
                              @"circleStrokeWidth should round-trip camera-data expressions.");

        layer.circleStrokeWidth = nil;
        XCTAssertTrue(rawLayer->getCircleStrokeWidth().isUndefined(),
                      @"Unsetting circleStrokeWidth should return circle-stroke-width to the default value.");
        XCTAssertEqualObjects(layer.circleStrokeWidth, defaultExpression,
                              @"circleStrokeWidth should return the default value after being unset.");
        // Transition property test
        layer.circleStrokeWidthTransition = transitionTest;
        auto toptions = rawLayer->getCircleStrokeWidthTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition circleStrokeWidthTransition = layer.circleStrokeWidthTransition;
        XCTAssertEqual(circleStrokeWidthTransition.delay, transitionTest.delay);
        XCTAssertEqual(circleStrokeWidthTransition.duration, transitionTest.duration);
    }

    // circle-translate
    {
        XCTAssertTrue(rawLayer->getCircleTranslate().isUndefined(),
                      @"circle-translate should be unset initially.");
        NSExpression *defaultExpression = layer.circleTranslation;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@",
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.circleTranslation = constantExpression;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getCircleTranslate(), propertyValue,
                       @"Setting circleTranslation to a constant value expression should update circle-translate.");
        XCTAssertEqualObjects(layer.circleTranslation, constantExpression,
                              @"circleTranslation should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"{1, 1}"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.circleTranslation = functionExpression;

        mbgl::style::IntervalStops<std::array<float, 2>> intervalStops = {{
            { -INFINITY, { 1, 1 } },
            { 18, { 1, 1 } },
        }};
        propertyValue = mbgl::style::CameraFunction<std::array<float, 2>> { intervalStops };

        XCTAssertEqual(rawLayer->getCircleTranslate(), propertyValue,
                       @"Setting circleTranslation to a camera expression should update circle-translate.");
        XCTAssertEqualObjects(layer.circleTranslation, functionExpression,
                              @"circleTranslation should round-trip camera expressions.");


        layer.circleTranslation = nil;
        XCTAssertTrue(rawLayer->getCircleTranslate().isUndefined(),
                      @"Unsetting circleTranslation should return circle-translate to the default value.");
        XCTAssertEqualObjects(layer.circleTranslation, defaultExpression,
                              @"circleTranslation should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.circleTranslation = functionExpression, NSException, NSInvalidArgumentException, @"MGLCircleLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(bogus, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.circleTranslation = functionExpression, NSException, NSInvalidArgumentException, @"MGLCircleLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // circle-translate-anchor
    {
        XCTAssertTrue(rawLayer->getCircleTranslateAnchor().isUndefined(),
                      @"circle-translate-anchor should be unset initially.");
        NSExpression *defaultExpression = layer.circleTranslationAnchor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'viewport'"];
        layer.circleTranslationAnchor = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType> propertyValue = { mbgl::style::TranslateAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getCircleTranslateAnchor(), propertyValue,
                       @"Setting circleTranslationAnchor to a constant value expression should update circle-translate-anchor.");
        XCTAssertEqualObjects(layer.circleTranslationAnchor, constantExpression,
                              @"circleTranslationAnchor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'viewport'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.circleTranslationAnchor = functionExpression;

        mbgl::style::IntervalStops<mbgl::style::TranslateAnchorType> intervalStops = {{
            { -INFINITY, mbgl::style::TranslateAnchorType::Viewport },
            { 18, mbgl::style::TranslateAnchorType::Viewport },
        }};
        propertyValue = mbgl::style::CameraFunction<mbgl::style::TranslateAnchorType> { intervalStops };

        XCTAssertEqual(rawLayer->getCircleTranslateAnchor(), propertyValue,
                       @"Setting circleTranslationAnchor to a camera expression should update circle-translate-anchor.");
        XCTAssertEqualObjects(layer.circleTranslationAnchor, functionExpression,
                              @"circleTranslationAnchor should round-trip camera expressions.");


        layer.circleTranslationAnchor = nil;
        XCTAssertTrue(rawLayer->getCircleTranslateAnchor().isUndefined(),
                      @"Unsetting circleTranslationAnchor should return circle-translate-anchor to the default value.");
        XCTAssertEqualObjects(layer.circleTranslationAnchor, defaultExpression,
                              @"circleTranslationAnchor should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.circleTranslationAnchor = functionExpression, NSException, NSInvalidArgumentException, @"MGLCircleLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(bogus, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.circleTranslationAnchor = functionExpression, NSException, NSInvalidArgumentException, @"MGLCircleLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }
}

- (void)testPropertyNames {
    [self testPropertyName:@"circle-blur" isBoolean:NO];
    [self testPropertyName:@"circle-color" isBoolean:NO];
    [self testPropertyName:@"circle-opacity" isBoolean:NO];
    [self testPropertyName:@"circle-pitch-alignment" isBoolean:NO];
    [self testPropertyName:@"circle-radius" isBoolean:NO];
    [self testPropertyName:@"circle-scale-alignment" isBoolean:NO];
    [self testPropertyName:@"circle-stroke-color" isBoolean:NO];
    [self testPropertyName:@"circle-stroke-opacity" isBoolean:NO];
    [self testPropertyName:@"circle-stroke-width" isBoolean:NO];
    [self testPropertyName:@"circle-translation" isBoolean:NO];
    [self testPropertyName:@"circle-translation-anchor" isBoolean:NO];
}

- (void)testValueAdditions {
    XCTAssertEqual([NSValue valueWithMGLCirclePitchAlignment:MGLCirclePitchAlignmentMap].MGLCirclePitchAlignmentValue, MGLCirclePitchAlignmentMap);
    XCTAssertEqual([NSValue valueWithMGLCirclePitchAlignment:MGLCirclePitchAlignmentViewport].MGLCirclePitchAlignmentValue, MGLCirclePitchAlignmentViewport);
    XCTAssertEqual([NSValue valueWithMGLCircleScaleAlignment:MGLCircleScaleAlignmentMap].MGLCircleScaleAlignmentValue, MGLCircleScaleAlignmentMap);
    XCTAssertEqual([NSValue valueWithMGLCircleScaleAlignment:MGLCircleScaleAlignmentViewport].MGLCircleScaleAlignmentValue, MGLCircleScaleAlignmentViewport);
    XCTAssertEqual([NSValue valueWithMGLCircleTranslationAnchor:MGLCircleTranslationAnchorMap].MGLCircleTranslationAnchorValue, MGLCircleTranslationAnchorMap);
    XCTAssertEqual([NSValue valueWithMGLCircleTranslationAnchor:MGLCircleTranslationAnchorViewport].MGLCircleTranslationAnchorValue, MGLCircleTranslationAnchorViewport);
}

@end
