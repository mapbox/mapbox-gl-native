// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLStyleLayerTests.h"
#import "../../darwin/src/NSDate+MGLAdditions.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/transition_options.hpp>

@interface MGLLineLayerTests : MGLStyleLayerTests
@end

@implementation MGLLineLayerTests

+ (NSString *)layerType {
    return @"line";
}

- (void)testPredicates {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];
    MGLLineStyleLayer *layer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"layerID" source:source];

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

    MGLLineStyleLayer *layer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    XCTAssertNotEqual(layer.rawLayer, nullptr);
    XCTAssertTrue(layer.rawLayer->is<mbgl::style::LineLayer>());
    auto rawLayer = layer.rawLayer->as<mbgl::style::LineLayer>();

    MGLTransition transitionTest = MGLTransitionMake(5, 4);


    // line-cap
    {
        XCTAssertTrue(rawLayer->getLineCap().isUndefined(),
                      @"line-cap should be unset initially.");
        NSExpression *defaultExpression = layer.lineCap;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'square'"];
        layer.lineCap = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::LineCapType> propertyValue = { mbgl::style::LineCapType::Square };
        XCTAssertEqual(rawLayer->getLineCap(), propertyValue,
                       @"Setting lineCap to a constant value expression should update line-cap.");
        XCTAssertEqualObjects(layer.lineCap, constantExpression,
                              @"lineCap should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'square'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.lineCap = functionExpression;

        mbgl::style::IntervalStops<mbgl::style::LineCapType> intervalStops = {{
            { -INFINITY, mbgl::style::LineCapType::Square },
            { 18, mbgl::style::LineCapType::Square },
        }};
        propertyValue = mbgl::style::CameraFunction<mbgl::style::LineCapType> { intervalStops };

        XCTAssertEqual(rawLayer->getLineCap(), propertyValue,
                       @"Setting lineCap to a camera expression should update line-cap.");
        XCTAssertEqualObjects(layer.lineCap, functionExpression,
                              @"lineCap should round-trip camera expressions.");


        layer.lineCap = nil;
        XCTAssertTrue(rawLayer->getLineCap().isUndefined(),
                      @"Unsetting lineCap should return line-cap to the default value.");
        XCTAssertEqualObjects(layer.lineCap, defaultExpression,
                              @"lineCap should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.lineCap = functionExpression, NSException, NSInvalidArgumentException, @"MGLLineLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(bogus, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.lineCap = functionExpression, NSException, NSInvalidArgumentException, @"MGLLineLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // line-join
    {
        XCTAssertTrue(rawLayer->getLineJoin().isUndefined(),
                      @"line-join should be unset initially.");
        NSExpression *defaultExpression = layer.lineJoin;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'miter'"];
        layer.lineJoin = constantExpression;
        mbgl::style::DataDrivenPropertyValue<mbgl::style::LineJoinType> propertyValue = { mbgl::style::LineJoinType::Miter };
        XCTAssertEqual(rawLayer->getLineJoin(), propertyValue,
                       @"Setting lineJoin to a constant value expression should update line-join.");
        XCTAssertEqualObjects(layer.lineJoin, constantExpression,
                              @"lineJoin should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'miter'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.lineJoin = functionExpression;

        mbgl::style::IntervalStops<mbgl::style::LineJoinType> intervalStops = {{
            { -INFINITY, mbgl::style::LineJoinType::Miter },
            { 18, mbgl::style::LineJoinType::Miter },
        }};
        propertyValue = mbgl::style::CameraFunction<mbgl::style::LineJoinType> { intervalStops };

        XCTAssertEqual(rawLayer->getLineJoin(), propertyValue,
                       @"Setting lineJoin to a camera expression should update line-join.");
        XCTAssertEqualObjects(layer.lineJoin, functionExpression,
                              @"lineJoin should round-trip camera expressions.");


        layer.lineJoin = nil;
        XCTAssertTrue(rawLayer->getLineJoin().isUndefined(),
                      @"Unsetting lineJoin should return line-join to the default value.");
        XCTAssertEqualObjects(layer.lineJoin, defaultExpression,
                              @"lineJoin should return the default value after being unset.");
    }

    // line-miter-limit
    {
        XCTAssertTrue(rawLayer->getLineMiterLimit().isUndefined(),
                      @"line-miter-limit should be unset initially.");
        NSExpression *defaultExpression = layer.lineMiterLimit;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        layer.lineMiterLimit = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineMiterLimit(), propertyValue,
                       @"Setting lineMiterLimit to a constant value expression should update line-miter-limit.");
        XCTAssertEqualObjects(layer.lineMiterLimit, constantExpression,
                              @"lineMiterLimit should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.lineMiterLimit = functionExpression;

        mbgl::style::IntervalStops<float> intervalStops = {{
            { -INFINITY, 0xff },
            { 18, 0xff },
        }};
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };

        XCTAssertEqual(rawLayer->getLineMiterLimit(), propertyValue,
                       @"Setting lineMiterLimit to a camera expression should update line-miter-limit.");
        XCTAssertEqualObjects(layer.lineMiterLimit, functionExpression,
                              @"lineMiterLimit should round-trip camera expressions.");


        layer.lineMiterLimit = nil;
        XCTAssertTrue(rawLayer->getLineMiterLimit().isUndefined(),
                      @"Unsetting lineMiterLimit should return line-miter-limit to the default value.");
        XCTAssertEqualObjects(layer.lineMiterLimit, defaultExpression,
                              @"lineMiterLimit should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.lineMiterLimit = functionExpression, NSException, NSInvalidArgumentException, @"MGLLineLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(bogus, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.lineMiterLimit = functionExpression, NSException, NSInvalidArgumentException, @"MGLLineLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // line-round-limit
    {
        XCTAssertTrue(rawLayer->getLineRoundLimit().isUndefined(),
                      @"line-round-limit should be unset initially.");
        NSExpression *defaultExpression = layer.lineRoundLimit;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        layer.lineRoundLimit = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineRoundLimit(), propertyValue,
                       @"Setting lineRoundLimit to a constant value expression should update line-round-limit.");
        XCTAssertEqualObjects(layer.lineRoundLimit, constantExpression,
                              @"lineRoundLimit should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.lineRoundLimit = functionExpression;

        mbgl::style::IntervalStops<float> intervalStops = {{
            { -INFINITY, 0xff },
            { 18, 0xff },
        }};
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };

        XCTAssertEqual(rawLayer->getLineRoundLimit(), propertyValue,
                       @"Setting lineRoundLimit to a camera expression should update line-round-limit.");
        XCTAssertEqualObjects(layer.lineRoundLimit, functionExpression,
                              @"lineRoundLimit should round-trip camera expressions.");


        layer.lineRoundLimit = nil;
        XCTAssertTrue(rawLayer->getLineRoundLimit().isUndefined(),
                      @"Unsetting lineRoundLimit should return line-round-limit to the default value.");
        XCTAssertEqualObjects(layer.lineRoundLimit, defaultExpression,
                              @"lineRoundLimit should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.lineRoundLimit = functionExpression, NSException, NSInvalidArgumentException, @"MGLLineLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(bogus, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.lineRoundLimit = functionExpression, NSException, NSInvalidArgumentException, @"MGLLineLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // line-blur
    {
        XCTAssertTrue(rawLayer->getLineBlur().isUndefined(),
                      @"line-blur should be unset initially.");
        NSExpression *defaultExpression = layer.lineBlur;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        layer.lineBlur = constantExpression;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineBlur(), propertyValue,
                       @"Setting lineBlur to a constant value expression should update line-blur.");
        XCTAssertEqualObjects(layer.lineBlur, constantExpression,
                              @"lineBlur should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.lineBlur = functionExpression;

        mbgl::style::IntervalStops<float> intervalStops = {{
            { -INFINITY, 0xff },
            { 18, 0xff },
        }};
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };

        XCTAssertEqual(rawLayer->getLineBlur(), propertyValue,
                       @"Setting lineBlur to a camera expression should update line-blur.");
        XCTAssertEqualObjects(layer.lineBlur, functionExpression,
                              @"lineBlur should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(keyName, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@18: constantExpression}];
        layer.lineBlur = functionExpression;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getLineBlur(), propertyValue,
                       @"Setting lineBlur to a data expression should update line-blur.");
        XCTAssertEqualObjects(layer.lineBlur, functionExpression,
                              @"lineBlur should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        layer.lineBlur = functionExpression;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getLineBlur(), propertyValue,
                       @"Setting lineBlur to a camera-data expression should update line-blur.");
        XCTAssertEqualObjects(layer.lineBlur, functionExpression,
                              @"lineBlur should round-trip camera-data expressions.");

        layer.lineBlur = nil;
        XCTAssertTrue(rawLayer->getLineBlur().isUndefined(),
                      @"Unsetting lineBlur should return line-blur to the default value.");
        XCTAssertEqualObjects(layer.lineBlur, defaultExpression,
                              @"lineBlur should return the default value after being unset.");
        // Transition property test
        layer.lineBlurTransition = transitionTest;
        auto toptions = rawLayer->getLineBlurTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition lineBlurTransition = layer.lineBlurTransition;
        XCTAssertEqual(lineBlurTransition.delay, transitionTest.delay);
        XCTAssertEqual(lineBlurTransition.duration, transitionTest.duration);
    }

    // line-color
    {
        XCTAssertTrue(rawLayer->getLineColor().isUndefined(),
                      @"line-color should be unset initially.");
        NSExpression *defaultExpression = layer.lineColor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        layer.lineColor = constantExpression;
        mbgl::style::DataDrivenPropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getLineColor(), propertyValue,
                       @"Setting lineColor to a constant value expression should update line-color.");
        XCTAssertEqualObjects(layer.lineColor, constantExpression,
                              @"lineColor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.lineColor = functionExpression;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = {{
            { -INFINITY, { 1, 0, 0, 1 } },
            { 18, { 1, 0, 0, 1 } },
        }};
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };

        XCTAssertEqual(rawLayer->getLineColor(), propertyValue,
                       @"Setting lineColor to a camera expression should update line-color.");
        XCTAssertEqualObjects(layer.lineColor, functionExpression,
                              @"lineColor should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(keyName, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@18: constantExpression}];
        layer.lineColor = functionExpression;

        mbgl::style::ExponentialStops<mbgl::Color> exponentialStops = { {{18, { 1, 0, 0, 1 }}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<mbgl::Color> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getLineColor(), propertyValue,
                       @"Setting lineColor to a data expression should update line-color.");
        XCTAssertEqualObjects(layer.lineColor, functionExpression,
                              @"lineColor should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        layer.lineColor = functionExpression;

        std::map<float, mbgl::Color> innerStops { {18, { 1, 0, 0, 1 }} };
        mbgl::style::CompositeExponentialStops<mbgl::Color> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<mbgl::Color> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getLineColor(), propertyValue,
                       @"Setting lineColor to a camera-data expression should update line-color.");
        XCTAssertEqualObjects(layer.lineColor, functionExpression,
                              @"lineColor should round-trip camera-data expressions.");

        layer.lineColor = nil;
        XCTAssertTrue(rawLayer->getLineColor().isUndefined(),
                      @"Unsetting lineColor should return line-color to the default value.");
        XCTAssertEqualObjects(layer.lineColor, defaultExpression,
                              @"lineColor should return the default value after being unset.");
        // Transition property test
        layer.lineColorTransition = transitionTest;
        auto toptions = rawLayer->getLineColorTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition lineColorTransition = layer.lineColorTransition;
        XCTAssertEqual(lineColorTransition.delay, transitionTest.delay);
        XCTAssertEqual(lineColorTransition.duration, transitionTest.duration);
    }

    // line-dasharray
    {
        XCTAssertTrue(rawLayer->getLineDasharray().isUndefined(),
                      @"line-dasharray should be unset initially.");
        NSExpression *defaultExpression = layer.lineDashPattern;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"{1, 2}"];
        layer.lineDashPattern = constantExpression;
        mbgl::style::PropertyValue<std::vector<float>> propertyValue = { {1, 2} };
        XCTAssertEqual(rawLayer->getLineDasharray(), propertyValue,
                       @"Setting lineDashPattern to a constant value expression should update line-dasharray.");
        XCTAssertEqualObjects(layer.lineDashPattern, constantExpression,
                              @"lineDashPattern should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"{1, 2}"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.lineDashPattern = functionExpression;

        mbgl::style::IntervalStops<std::vector<float>> intervalStops = {{
            { -INFINITY, {1, 2} },
            { 18, {1, 2} },
        }};
        propertyValue = mbgl::style::CameraFunction<std::vector<float>> { intervalStops };

        XCTAssertEqual(rawLayer->getLineDasharray(), propertyValue,
                       @"Setting lineDashPattern to a camera expression should update line-dasharray.");
        XCTAssertEqualObjects(layer.lineDashPattern, functionExpression,
                              @"lineDashPattern should round-trip camera expressions.");


        layer.lineDashPattern = nil;
        XCTAssertTrue(rawLayer->getLineDasharray().isUndefined(),
                      @"Unsetting lineDashPattern should return line-dasharray to the default value.");
        XCTAssertEqualObjects(layer.lineDashPattern, defaultExpression,
                              @"lineDashPattern should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.lineDashPattern = functionExpression, NSException, NSInvalidArgumentException, @"MGLLineLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(bogus, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.lineDashPattern = functionExpression, NSException, NSInvalidArgumentException, @"MGLLineLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // line-gap-width
    {
        XCTAssertTrue(rawLayer->getLineGapWidth().isUndefined(),
                      @"line-gap-width should be unset initially.");
        NSExpression *defaultExpression = layer.lineGapWidth;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        layer.lineGapWidth = constantExpression;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineGapWidth(), propertyValue,
                       @"Setting lineGapWidth to a constant value expression should update line-gap-width.");
        XCTAssertEqualObjects(layer.lineGapWidth, constantExpression,
                              @"lineGapWidth should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.lineGapWidth = functionExpression;

        mbgl::style::IntervalStops<float> intervalStops = {{
            { -INFINITY, 0xff },
            { 18, 0xff },
        }};
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };

        XCTAssertEqual(rawLayer->getLineGapWidth(), propertyValue,
                       @"Setting lineGapWidth to a camera expression should update line-gap-width.");
        XCTAssertEqualObjects(layer.lineGapWidth, functionExpression,
                              @"lineGapWidth should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(keyName, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@18: constantExpression}];
        layer.lineGapWidth = functionExpression;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getLineGapWidth(), propertyValue,
                       @"Setting lineGapWidth to a data expression should update line-gap-width.");
        XCTAssertEqualObjects(layer.lineGapWidth, functionExpression,
                              @"lineGapWidth should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        layer.lineGapWidth = functionExpression;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getLineGapWidth(), propertyValue,
                       @"Setting lineGapWidth to a camera-data expression should update line-gap-width.");
        XCTAssertEqualObjects(layer.lineGapWidth, functionExpression,
                              @"lineGapWidth should round-trip camera-data expressions.");

        layer.lineGapWidth = nil;
        XCTAssertTrue(rawLayer->getLineGapWidth().isUndefined(),
                      @"Unsetting lineGapWidth should return line-gap-width to the default value.");
        XCTAssertEqualObjects(layer.lineGapWidth, defaultExpression,
                              @"lineGapWidth should return the default value after being unset.");
        // Transition property test
        layer.lineGapWidthTransition = transitionTest;
        auto toptions = rawLayer->getLineGapWidthTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition lineGapWidthTransition = layer.lineGapWidthTransition;
        XCTAssertEqual(lineGapWidthTransition.delay, transitionTest.delay);
        XCTAssertEqual(lineGapWidthTransition.duration, transitionTest.duration);
    }

    // line-offset
    {
        XCTAssertTrue(rawLayer->getLineOffset().isUndefined(),
                      @"line-offset should be unset initially.");
        NSExpression *defaultExpression = layer.lineOffset;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        layer.lineOffset = constantExpression;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineOffset(), propertyValue,
                       @"Setting lineOffset to a constant value expression should update line-offset.");
        XCTAssertEqualObjects(layer.lineOffset, constantExpression,
                              @"lineOffset should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.lineOffset = functionExpression;

        mbgl::style::IntervalStops<float> intervalStops = {{
            { -INFINITY, 0xff },
            { 18, 0xff },
        }};
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };

        XCTAssertEqual(rawLayer->getLineOffset(), propertyValue,
                       @"Setting lineOffset to a camera expression should update line-offset.");
        XCTAssertEqualObjects(layer.lineOffset, functionExpression,
                              @"lineOffset should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(keyName, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@18: constantExpression}];
        layer.lineOffset = functionExpression;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getLineOffset(), propertyValue,
                       @"Setting lineOffset to a data expression should update line-offset.");
        XCTAssertEqualObjects(layer.lineOffset, functionExpression,
                              @"lineOffset should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        layer.lineOffset = functionExpression;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getLineOffset(), propertyValue,
                       @"Setting lineOffset to a camera-data expression should update line-offset.");
        XCTAssertEqualObjects(layer.lineOffset, functionExpression,
                              @"lineOffset should round-trip camera-data expressions.");

        layer.lineOffset = nil;
        XCTAssertTrue(rawLayer->getLineOffset().isUndefined(),
                      @"Unsetting lineOffset should return line-offset to the default value.");
        XCTAssertEqualObjects(layer.lineOffset, defaultExpression,
                              @"lineOffset should return the default value after being unset.");
        // Transition property test
        layer.lineOffsetTransition = transitionTest;
        auto toptions = rawLayer->getLineOffsetTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition lineOffsetTransition = layer.lineOffsetTransition;
        XCTAssertEqual(lineOffsetTransition.delay, transitionTest.delay);
        XCTAssertEqual(lineOffsetTransition.duration, transitionTest.duration);
    }

    // line-opacity
    {
        XCTAssertTrue(rawLayer->getLineOpacity().isUndefined(),
                      @"line-opacity should be unset initially.");
        NSExpression *defaultExpression = layer.lineOpacity;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        layer.lineOpacity = constantExpression;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineOpacity(), propertyValue,
                       @"Setting lineOpacity to a constant value expression should update line-opacity.");
        XCTAssertEqualObjects(layer.lineOpacity, constantExpression,
                              @"lineOpacity should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.lineOpacity = functionExpression;

        mbgl::style::IntervalStops<float> intervalStops = {{
            { -INFINITY, 0xff },
            { 18, 0xff },
        }};
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };

        XCTAssertEqual(rawLayer->getLineOpacity(), propertyValue,
                       @"Setting lineOpacity to a camera expression should update line-opacity.");
        XCTAssertEqualObjects(layer.lineOpacity, functionExpression,
                              @"lineOpacity should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(keyName, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@18: constantExpression}];
        layer.lineOpacity = functionExpression;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getLineOpacity(), propertyValue,
                       @"Setting lineOpacity to a data expression should update line-opacity.");
        XCTAssertEqualObjects(layer.lineOpacity, functionExpression,
                              @"lineOpacity should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        layer.lineOpacity = functionExpression;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getLineOpacity(), propertyValue,
                       @"Setting lineOpacity to a camera-data expression should update line-opacity.");
        XCTAssertEqualObjects(layer.lineOpacity, functionExpression,
                              @"lineOpacity should round-trip camera-data expressions.");

        layer.lineOpacity = nil;
        XCTAssertTrue(rawLayer->getLineOpacity().isUndefined(),
                      @"Unsetting lineOpacity should return line-opacity to the default value.");
        XCTAssertEqualObjects(layer.lineOpacity, defaultExpression,
                              @"lineOpacity should return the default value after being unset.");
        // Transition property test
        layer.lineOpacityTransition = transitionTest;
        auto toptions = rawLayer->getLineOpacityTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition lineOpacityTransition = layer.lineOpacityTransition;
        XCTAssertEqual(lineOpacityTransition.delay, transitionTest.delay);
        XCTAssertEqual(lineOpacityTransition.duration, transitionTest.duration);
    }

    // line-pattern
    {
        XCTAssertTrue(rawLayer->getLinePattern().isUndefined(),
                      @"line-pattern should be unset initially.");
        NSExpression *defaultExpression = layer.linePattern;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'Line Pattern'"];
        layer.linePattern = constantExpression;
        mbgl::style::PropertyValue<std::string> propertyValue = { "Line Pattern" };
        XCTAssertEqual(rawLayer->getLinePattern(), propertyValue,
                       @"Setting linePattern to a constant value expression should update line-pattern.");
        XCTAssertEqualObjects(layer.linePattern, constantExpression,
                              @"linePattern should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'Line Pattern'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.linePattern = functionExpression;

        mbgl::style::IntervalStops<std::string> intervalStops = {{
            { -INFINITY, "Line Pattern" },
            { 18, "Line Pattern" },
        }};
        propertyValue = mbgl::style::CameraFunction<std::string> { intervalStops };

        XCTAssertEqual(rawLayer->getLinePattern(), propertyValue,
                       @"Setting linePattern to a camera expression should update line-pattern.");
        XCTAssertEqualObjects(layer.linePattern, functionExpression,
                              @"linePattern should round-trip camera expressions.");


        layer.linePattern = nil;
        XCTAssertTrue(rawLayer->getLinePattern().isUndefined(),
                      @"Unsetting linePattern should return line-pattern to the default value.");
        XCTAssertEqualObjects(layer.linePattern, defaultExpression,
                              @"linePattern should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.linePattern = functionExpression, NSException, NSInvalidArgumentException, @"MGLLineLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(bogus, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.linePattern = functionExpression, NSException, NSInvalidArgumentException, @"MGLLineLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        // Transition property test
        layer.linePatternTransition = transitionTest;
        auto toptions = rawLayer->getLinePatternTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition linePatternTransition = layer.linePatternTransition;
        XCTAssertEqual(linePatternTransition.delay, transitionTest.delay);
        XCTAssertEqual(linePatternTransition.duration, transitionTest.duration);
    }

    // line-translate
    {
        XCTAssertTrue(rawLayer->getLineTranslate().isUndefined(),
                      @"line-translate should be unset initially.");
        NSExpression *defaultExpression = layer.lineTranslation;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@",
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.lineTranslation = constantExpression;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getLineTranslate(), propertyValue,
                       @"Setting lineTranslation to a constant value expression should update line-translate.");
        XCTAssertEqualObjects(layer.lineTranslation, constantExpression,
                              @"lineTranslation should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"{1, 1}"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.lineTranslation = functionExpression;

        mbgl::style::IntervalStops<std::array<float, 2>> intervalStops = {{
            { -INFINITY, { 1, 1 } },
            { 18, { 1, 1 } },
        }};
        propertyValue = mbgl::style::CameraFunction<std::array<float, 2>> { intervalStops };

        XCTAssertEqual(rawLayer->getLineTranslate(), propertyValue,
                       @"Setting lineTranslation to a camera expression should update line-translate.");
        XCTAssertEqualObjects(layer.lineTranslation, functionExpression,
                              @"lineTranslation should round-trip camera expressions.");


        layer.lineTranslation = nil;
        XCTAssertTrue(rawLayer->getLineTranslate().isUndefined(),
                      @"Unsetting lineTranslation should return line-translate to the default value.");
        XCTAssertEqualObjects(layer.lineTranslation, defaultExpression,
                              @"lineTranslation should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.lineTranslation = functionExpression, NSException, NSInvalidArgumentException, @"MGLLineLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(bogus, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.lineTranslation = functionExpression, NSException, NSInvalidArgumentException, @"MGLLineLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // line-translate-anchor
    {
        XCTAssertTrue(rawLayer->getLineTranslateAnchor().isUndefined(),
                      @"line-translate-anchor should be unset initially.");
        NSExpression *defaultExpression = layer.lineTranslationAnchor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'viewport'"];
        layer.lineTranslationAnchor = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType> propertyValue = { mbgl::style::TranslateAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getLineTranslateAnchor(), propertyValue,
                       @"Setting lineTranslationAnchor to a constant value expression should update line-translate-anchor.");
        XCTAssertEqualObjects(layer.lineTranslationAnchor, constantExpression,
                              @"lineTranslationAnchor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'viewport'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.lineTranslationAnchor = functionExpression;

        mbgl::style::IntervalStops<mbgl::style::TranslateAnchorType> intervalStops = {{
            { -INFINITY, mbgl::style::TranslateAnchorType::Viewport },
            { 18, mbgl::style::TranslateAnchorType::Viewport },
        }};
        propertyValue = mbgl::style::CameraFunction<mbgl::style::TranslateAnchorType> { intervalStops };

        XCTAssertEqual(rawLayer->getLineTranslateAnchor(), propertyValue,
                       @"Setting lineTranslationAnchor to a camera expression should update line-translate-anchor.");
        XCTAssertEqualObjects(layer.lineTranslationAnchor, functionExpression,
                              @"lineTranslationAnchor should round-trip camera expressions.");


        layer.lineTranslationAnchor = nil;
        XCTAssertTrue(rawLayer->getLineTranslateAnchor().isUndefined(),
                      @"Unsetting lineTranslationAnchor should return line-translate-anchor to the default value.");
        XCTAssertEqualObjects(layer.lineTranslationAnchor, defaultExpression,
                              @"lineTranslationAnchor should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.lineTranslationAnchor = functionExpression, NSException, NSInvalidArgumentException, @"MGLLineLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(bogus, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.lineTranslationAnchor = functionExpression, NSException, NSInvalidArgumentException, @"MGLLineLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // line-width
    {
        XCTAssertTrue(rawLayer->getLineWidth().isUndefined(),
                      @"line-width should be unset initially.");
        NSExpression *defaultExpression = layer.lineWidth;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        layer.lineWidth = constantExpression;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineWidth(), propertyValue,
                       @"Setting lineWidth to a constant value expression should update line-width.");
        XCTAssertEqualObjects(layer.lineWidth, constantExpression,
                              @"lineWidth should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"0xff"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_stepWithMinimum:stops:', %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.lineWidth = functionExpression;

        mbgl::style::IntervalStops<float> intervalStops = {{
            { -INFINITY, 0xff },
            { 18, 0xff },
        }};
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };

        XCTAssertEqual(rawLayer->getLineWidth(), propertyValue,
                       @"Setting lineWidth to a camera expression should update line-width.");
        XCTAssertEqualObjects(layer.lineWidth, functionExpression,
                              @"lineWidth should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION(keyName, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@18: constantExpression}];
        layer.lineWidth = functionExpression;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getLineWidth(), propertyValue,
                       @"Setting lineWidth to a data expression should update line-width.");
        XCTAssertEqualObjects(layer.lineWidth, functionExpression,
                              @"lineWidth should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"FUNCTION($zoomLevel, 'mgl_interpolateWithCurveType:parameters:stops:', 'linear', nil, %@)", @{@10: functionExpression}];
        layer.lineWidth = functionExpression;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getLineWidth(), propertyValue,
                       @"Setting lineWidth to a camera-data expression should update line-width.");
        XCTAssertEqualObjects(layer.lineWidth, functionExpression,
                              @"lineWidth should round-trip camera-data expressions.");

        layer.lineWidth = nil;
        XCTAssertTrue(rawLayer->getLineWidth().isUndefined(),
                      @"Unsetting lineWidth should return line-width to the default value.");
        XCTAssertEqualObjects(layer.lineWidth, defaultExpression,
                              @"lineWidth should return the default value after being unset.");
        // Transition property test
        layer.lineWidthTransition = transitionTest;
        auto toptions = rawLayer->getLineWidthTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition lineWidthTransition = layer.lineWidthTransition;
        XCTAssertEqual(lineWidthTransition.delay, transitionTest.delay);
        XCTAssertEqual(lineWidthTransition.duration, transitionTest.duration);
    }
}

- (void)testPropertyNames {
    [self testPropertyName:@"line-cap" isBoolean:NO];
    [self testPropertyName:@"line-join" isBoolean:NO];
    [self testPropertyName:@"line-miter-limit" isBoolean:NO];
    [self testPropertyName:@"line-round-limit" isBoolean:NO];
    [self testPropertyName:@"line-blur" isBoolean:NO];
    [self testPropertyName:@"line-color" isBoolean:NO];
    [self testPropertyName:@"line-dash-pattern" isBoolean:NO];
    [self testPropertyName:@"line-gap-width" isBoolean:NO];
    [self testPropertyName:@"line-offset" isBoolean:NO];
    [self testPropertyName:@"line-opacity" isBoolean:NO];
    [self testPropertyName:@"line-pattern" isBoolean:NO];
    [self testPropertyName:@"line-translation" isBoolean:NO];
    [self testPropertyName:@"line-translation-anchor" isBoolean:NO];
    [self testPropertyName:@"line-width" isBoolean:NO];
}

- (void)testValueAdditions {
    XCTAssertEqual([NSValue valueWithMGLLineCap:MGLLineCapButt].MGLLineCapValue, MGLLineCapButt);
    XCTAssertEqual([NSValue valueWithMGLLineCap:MGLLineCapRound].MGLLineCapValue, MGLLineCapRound);
    XCTAssertEqual([NSValue valueWithMGLLineCap:MGLLineCapSquare].MGLLineCapValue, MGLLineCapSquare);
    XCTAssertEqual([NSValue valueWithMGLLineJoin:MGLLineJoinBevel].MGLLineJoinValue, MGLLineJoinBevel);
    XCTAssertEqual([NSValue valueWithMGLLineJoin:MGLLineJoinRound].MGLLineJoinValue, MGLLineJoinRound);
    XCTAssertEqual([NSValue valueWithMGLLineJoin:MGLLineJoinMiter].MGLLineJoinValue, MGLLineJoinMiter);
    XCTAssertEqual([NSValue valueWithMGLLineTranslationAnchor:MGLLineTranslationAnchorMap].MGLLineTranslationAnchorValue, MGLLineTranslationAnchorMap);
    XCTAssertEqual([NSValue valueWithMGLLineTranslationAnchor:MGLLineTranslationAnchorViewport].MGLLineTranslationAnchorValue, MGLLineTranslationAnchorViewport);
}

@end
