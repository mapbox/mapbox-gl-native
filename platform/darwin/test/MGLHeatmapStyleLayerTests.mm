// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLStyleLayerTests.h"
#import "../../darwin/src/NSDate+MGLAdditions.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/expression/dsl.hpp>

@interface MGLHeatmapLayerTests : MGLStyleLayerTests
@end

@implementation MGLHeatmapLayerTests

+ (NSString *)layerType {
    return @"heatmap";
}

- (void)testPredicates {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];
    MGLHeatmapStyleLayer *layer = [[MGLHeatmapStyleLayer alloc] initWithIdentifier:@"layerID" source:source];

    XCTAssertNil(layer.sourceLayerIdentifier);
    layer.sourceLayerIdentifier = @"layerID";
    XCTAssertEqualObjects(layer.sourceLayerIdentifier, @"layerID");
    layer.sourceLayerIdentifier = nil;
    XCTAssertNil(layer.sourceLayerIdentifier);

    XCTAssertNil(layer.predicate);
    layer.predicate = [NSPredicate predicateWithFormat:@"$featureIdentifier = 1"];
    XCTAssertEqualObjects(layer.predicate,  [NSPredicate predicateWithFormat:@"$featureIdentifier = 1"]);
    layer.predicate = nil;
    XCTAssertNil(layer.predicate);
}

- (void)testProperties {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];

    MGLHeatmapStyleLayer *layer = [[MGLHeatmapStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    XCTAssertNotEqual(layer.rawLayer, nullptr);
    XCTAssertEqualObjects(@(layer.rawLayer->getTypeInfo()->type), @"heatmap");
    auto rawLayer = static_cast<mbgl::style::HeatmapLayer*>(layer.rawLayer);

    MGLTransition transitionTest = MGLTransitionMake(5, 4);


    // heatmap-intensity
    {
        XCTAssertTrue(rawLayer->getHeatmapIntensity().isUndefined(),
                      @"heatmap-intensity should be unset initially.");
        NSExpression *defaultExpression = layer.heatmapIntensity;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.heatmapIntensity = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getHeatmapIntensity(), propertyValue,
                       @"Setting heatmapIntensity to a constant value expression should update heatmap-intensity.");
        XCTAssertEqualObjects(layer.heatmapIntensity, constantExpression,
                              @"heatmapIntensity should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.heatmapIntensity = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getHeatmapIntensity(), propertyValue,
                       @"Setting heatmapIntensity to a camera expression should update heatmap-intensity.");
        XCTAssertEqualObjects(layer.heatmapIntensity, functionExpression,
                              @"heatmapIntensity should round-trip camera expressions.");


        layer.heatmapIntensity = nil;
        XCTAssertTrue(rawLayer->getHeatmapIntensity().isUndefined(),
                      @"Unsetting heatmapIntensity should return heatmap-intensity to the default value.");
        XCTAssertEqualObjects(layer.heatmapIntensity, defaultExpression,
                              @"heatmapIntensity should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.heatmapIntensity = functionExpression, NSException, NSInvalidArgumentException, @"MGLHeatmapLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.heatmapIntensity = functionExpression, NSException, NSInvalidArgumentException, @"MGLHeatmapLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        // Transition property test
        layer.heatmapIntensityTransition = transitionTest;
        auto toptions = rawLayer->getHeatmapIntensityTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition heatmapIntensityTransition = layer.heatmapIntensityTransition;
        XCTAssertEqual(heatmapIntensityTransition.delay, transitionTest.delay);
        XCTAssertEqual(heatmapIntensityTransition.duration, transitionTest.duration);
    }

    // heatmap-opacity
    {
        XCTAssertTrue(rawLayer->getHeatmapOpacity().isUndefined(),
                      @"heatmap-opacity should be unset initially.");
        NSExpression *defaultExpression = layer.heatmapOpacity;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.heatmapOpacity = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getHeatmapOpacity(), propertyValue,
                       @"Setting heatmapOpacity to a constant value expression should update heatmap-opacity.");
        XCTAssertEqualObjects(layer.heatmapOpacity, constantExpression,
                              @"heatmapOpacity should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.heatmapOpacity = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getHeatmapOpacity(), propertyValue,
                       @"Setting heatmapOpacity to a camera expression should update heatmap-opacity.");
        XCTAssertEqualObjects(layer.heatmapOpacity, functionExpression,
                              @"heatmapOpacity should round-trip camera expressions.");


        layer.heatmapOpacity = nil;
        XCTAssertTrue(rawLayer->getHeatmapOpacity().isUndefined(),
                      @"Unsetting heatmapOpacity should return heatmap-opacity to the default value.");
        XCTAssertEqualObjects(layer.heatmapOpacity, defaultExpression,
                              @"heatmapOpacity should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.heatmapOpacity = functionExpression, NSException, NSInvalidArgumentException, @"MGLHeatmapLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.heatmapOpacity = functionExpression, NSException, NSInvalidArgumentException, @"MGLHeatmapLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        // Transition property test
        layer.heatmapOpacityTransition = transitionTest;
        auto toptions = rawLayer->getHeatmapOpacityTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition heatmapOpacityTransition = layer.heatmapOpacityTransition;
        XCTAssertEqual(heatmapOpacityTransition.delay, transitionTest.delay);
        XCTAssertEqual(heatmapOpacityTransition.duration, transitionTest.duration);
    }

    // heatmap-radius
    {
        XCTAssertTrue(rawLayer->getHeatmapRadius().isUndefined(),
                      @"heatmap-radius should be unset initially.");
        NSExpression *defaultExpression = layer.heatmapRadius;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.heatmapRadius = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getHeatmapRadius(), propertyValue,
                       @"Setting heatmapRadius to a constant value expression should update heatmap-radius.");
        XCTAssertEqualObjects(layer.heatmapRadius, constantExpression,
                              @"heatmapRadius should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.heatmapRadius = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getHeatmapRadius(), propertyValue,
                       @"Setting heatmapRadius to a camera expression should update heatmap-radius.");
        XCTAssertEqualObjects(layer.heatmapRadius, functionExpression,
                              @"heatmapRadius should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.heatmapRadius = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getHeatmapRadius(), propertyValue,
                       @"Setting heatmapRadius to a data expression should update heatmap-radius.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.heatmapRadius, pedanticFunctionExpression,
                              @"heatmapRadius should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.heatmapRadius = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getHeatmapRadius(), propertyValue,
                       @"Setting heatmapRadius to a camera-data expression should update heatmap-radius.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.heatmapRadius, pedanticFunctionExpression,
                              @"heatmapRadius should round-trip camera-data expressions.");

        layer.heatmapRadius = nil;
        XCTAssertTrue(rawLayer->getHeatmapRadius().isUndefined(),
                      @"Unsetting heatmapRadius should return heatmap-radius to the default value.");
        XCTAssertEqualObjects(layer.heatmapRadius, defaultExpression,
                              @"heatmapRadius should return the default value after being unset.");
        // Transition property test
        layer.heatmapRadiusTransition = transitionTest;
        auto toptions = rawLayer->getHeatmapRadiusTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition heatmapRadiusTransition = layer.heatmapRadiusTransition;
        XCTAssertEqual(heatmapRadiusTransition.delay, transitionTest.delay);
        XCTAssertEqual(heatmapRadiusTransition.duration, transitionTest.duration);
    }

    // heatmap-weight
    {
        XCTAssertTrue(rawLayer->getHeatmapWeight().isUndefined(),
                      @"heatmap-weight should be unset initially.");
        NSExpression *defaultExpression = layer.heatmapWeight;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.heatmapWeight = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getHeatmapWeight(), propertyValue,
                       @"Setting heatmapWeight to a constant value expression should update heatmap-weight.");
        XCTAssertEqualObjects(layer.heatmapWeight, constantExpression,
                              @"heatmapWeight should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.heatmapWeight = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getHeatmapWeight(), propertyValue,
                       @"Setting heatmapWeight to a camera expression should update heatmap-weight.");
        XCTAssertEqualObjects(layer.heatmapWeight, functionExpression,
                              @"heatmapWeight should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.heatmapWeight = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getHeatmapWeight(), propertyValue,
                       @"Setting heatmapWeight to a data expression should update heatmap-weight.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.heatmapWeight, pedanticFunctionExpression,
                              @"heatmapWeight should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.heatmapWeight = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getHeatmapWeight(), propertyValue,
                       @"Setting heatmapWeight to a camera-data expression should update heatmap-weight.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.heatmapWeight, pedanticFunctionExpression,
                              @"heatmapWeight should round-trip camera-data expressions.");

        layer.heatmapWeight = nil;
        XCTAssertTrue(rawLayer->getHeatmapWeight().isUndefined(),
                      @"Unsetting heatmapWeight should return heatmap-weight to the default value.");
        XCTAssertEqualObjects(layer.heatmapWeight, defaultExpression,
                              @"heatmapWeight should return the default value after being unset.");
    }
}

- (void)testPropertyNames {
    [self testPropertyName:@"heatmap-intensity" isBoolean:NO];
    [self testPropertyName:@"heatmap-opacity" isBoolean:NO];
    [self testPropertyName:@"heatmap-radius" isBoolean:NO];
    [self testPropertyName:@"heatmap-weight" isBoolean:NO];
}

@end
