// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLStyleLayerTests.h"
#import "../../darwin/src/NSDate+MGLAdditions.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/expression/dsl.hpp>

@interface MGLFillExtrusionLayerTests : MGLStyleLayerTests
@end

@implementation MGLFillExtrusionLayerTests

+ (NSString *)layerType {
    return @"fill-extrusion";
}

- (void)testPredicates {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];
    MGLFillExtrusionStyleLayer *layer = [[MGLFillExtrusionStyleLayer alloc] initWithIdentifier:@"layerID" source:source];

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

    MGLFillExtrusionStyleLayer *layer = [[MGLFillExtrusionStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    XCTAssertNotEqual(layer.rawLayer, nullptr);
    XCTAssertTrue(layer.rawLayer->is<mbgl::style::FillExtrusionLayer>());
    auto rawLayer = layer.rawLayer->as<mbgl::style::FillExtrusionLayer>();

    MGLTransition transitionTest = MGLTransitionMake(5, 4);


    // fill-extrusion-base
    {
        XCTAssertTrue(rawLayer->getFillExtrusionBase().isUndefined(),
                      @"fill-extrusion-base should be unset initially.");
        NSExpression *defaultExpression = layer.fillExtrusionBase;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.fillExtrusionBase = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getFillExtrusionBase(), propertyValue,
                       @"Setting fillExtrusionBase to a constant value expression should update fill-extrusion-base.");
        XCTAssertEqualObjects(layer.fillExtrusionBase, constantExpression,
                              @"fillExtrusionBase should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.fillExtrusionBase = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getFillExtrusionBase(), propertyValue,
                       @"Setting fillExtrusionBase to a camera expression should update fill-extrusion-base.");
        XCTAssertEqualObjects(layer.fillExtrusionBase, functionExpression,
                              @"fillExtrusionBase should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.fillExtrusionBase = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getFillExtrusionBase(), propertyValue,
                       @"Setting fillExtrusionBase to a data expression should update fill-extrusion-base.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.fillExtrusionBase, pedanticFunctionExpression,
                              @"fillExtrusionBase should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.fillExtrusionBase = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getFillExtrusionBase(), propertyValue,
                       @"Setting fillExtrusionBase to a camera-data expression should update fill-extrusion-base.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.fillExtrusionBase, pedanticFunctionExpression,
                              @"fillExtrusionBase should round-trip camera-data expressions.");

        layer.fillExtrusionBase = nil;
        XCTAssertTrue(rawLayer->getFillExtrusionBase().isUndefined(),
                      @"Unsetting fillExtrusionBase should return fill-extrusion-base to the default value.");
        XCTAssertEqualObjects(layer.fillExtrusionBase, defaultExpression,
                              @"fillExtrusionBase should return the default value after being unset.");
        // Transition property test
        layer.fillExtrusionBaseTransition = transitionTest;
        auto toptions = rawLayer->getFillExtrusionBaseTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition fillExtrusionBaseTransition = layer.fillExtrusionBaseTransition;
        XCTAssertEqual(fillExtrusionBaseTransition.delay, transitionTest.delay);
        XCTAssertEqual(fillExtrusionBaseTransition.duration, transitionTest.duration);
    }

    // fill-extrusion-color
    {
        XCTAssertTrue(rawLayer->getFillExtrusionColor().isUndefined(),
                      @"fill-extrusion-color should be unset initially.");
        NSExpression *defaultExpression = layer.fillExtrusionColor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        layer.fillExtrusionColor = constantExpression;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getFillExtrusionColor(), propertyValue,
                       @"Setting fillExtrusionColor to a constant value expression should update fill-extrusion-color.");
        XCTAssertEqualObjects(layer.fillExtrusionColor, constantExpression,
                              @"fillExtrusionColor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"%@", [MGLColor redColor]];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.fillExtrusionColor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::Color>(
                step(zoom(), literal(mbgl::Color(1, 0, 0, 1)), 18.0, literal(mbgl::Color(1, 0, 0, 1)))
            );
        }

        XCTAssertEqual(rawLayer->getFillExtrusionColor(), propertyValue,
                       @"Setting fillExtrusionColor to a camera expression should update fill-extrusion-color.");
        XCTAssertEqualObjects(layer.fillExtrusionColor, functionExpression,
                              @"fillExtrusionColor should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.fillExtrusionColor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::Color>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(mbgl::Color(1, 0, 0, 1)))
            );
        }

        XCTAssertEqual(rawLayer->getFillExtrusionColor(), propertyValue,
                       @"Setting fillExtrusionColor to a data expression should update fill-extrusion-color.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.fillExtrusionColor, pedanticFunctionExpression,
                              @"fillExtrusionColor should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.fillExtrusionColor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::Color>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(mbgl::Color(1, 0, 0, 1))))
            );
        }

        XCTAssertEqual(rawLayer->getFillExtrusionColor(), propertyValue,
                       @"Setting fillExtrusionColor to a camera-data expression should update fill-extrusion-color.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.fillExtrusionColor, pedanticFunctionExpression,
                              @"fillExtrusionColor should round-trip camera-data expressions.");

        layer.fillExtrusionColor = nil;
        XCTAssertTrue(rawLayer->getFillExtrusionColor().isUndefined(),
                      @"Unsetting fillExtrusionColor should return fill-extrusion-color to the default value.");
        XCTAssertEqualObjects(layer.fillExtrusionColor, defaultExpression,
                              @"fillExtrusionColor should return the default value after being unset.");
        // Transition property test
        layer.fillExtrusionColorTransition = transitionTest;
        auto toptions = rawLayer->getFillExtrusionColorTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition fillExtrusionColorTransition = layer.fillExtrusionColorTransition;
        XCTAssertEqual(fillExtrusionColorTransition.delay, transitionTest.delay);
        XCTAssertEqual(fillExtrusionColorTransition.duration, transitionTest.duration);
    }

    // fill-extrusion-height
    {
        XCTAssertTrue(rawLayer->getFillExtrusionHeight().isUndefined(),
                      @"fill-extrusion-height should be unset initially.");
        NSExpression *defaultExpression = layer.fillExtrusionHeight;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.fillExtrusionHeight = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getFillExtrusionHeight(), propertyValue,
                       @"Setting fillExtrusionHeight to a constant value expression should update fill-extrusion-height.");
        XCTAssertEqualObjects(layer.fillExtrusionHeight, constantExpression,
                              @"fillExtrusionHeight should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.fillExtrusionHeight = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getFillExtrusionHeight(), propertyValue,
                       @"Setting fillExtrusionHeight to a camera expression should update fill-extrusion-height.");
        XCTAssertEqualObjects(layer.fillExtrusionHeight, functionExpression,
                              @"fillExtrusionHeight should round-trip camera expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(keyName, 'linear', nil, %@)", @{@18: constantExpression}];
        layer.fillExtrusionHeight = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), number(get("keyName")), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getFillExtrusionHeight(), propertyValue,
                       @"Setting fillExtrusionHeight to a data expression should update fill-extrusion-height.");
        NSExpression *pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:(CAST(keyName, 'NSNumber'), 'linear', nil, %@)", @{@18: constantExpression}];
        XCTAssertEqualObjects(layer.fillExtrusionHeight, pedanticFunctionExpression,
                              @"fillExtrusionHeight should round-trip data expressions.");

        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        layer.fillExtrusionHeight = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                interpolate(linear(), zoom(), 10.0, interpolate(linear(), number(get("keyName")), 18.0, literal(1.0)))
            );
        }

        XCTAssertEqual(rawLayer->getFillExtrusionHeight(), propertyValue,
                       @"Setting fillExtrusionHeight to a camera-data expression should update fill-extrusion-height.");
        pedanticFunctionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: pedanticFunctionExpression}];
        XCTAssertEqualObjects(layer.fillExtrusionHeight, pedanticFunctionExpression,
                              @"fillExtrusionHeight should round-trip camera-data expressions.");

        layer.fillExtrusionHeight = nil;
        XCTAssertTrue(rawLayer->getFillExtrusionHeight().isUndefined(),
                      @"Unsetting fillExtrusionHeight should return fill-extrusion-height to the default value.");
        XCTAssertEqualObjects(layer.fillExtrusionHeight, defaultExpression,
                              @"fillExtrusionHeight should return the default value after being unset.");
        // Transition property test
        layer.fillExtrusionHeightTransition = transitionTest;
        auto toptions = rawLayer->getFillExtrusionHeightTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition fillExtrusionHeightTransition = layer.fillExtrusionHeightTransition;
        XCTAssertEqual(fillExtrusionHeightTransition.delay, transitionTest.delay);
        XCTAssertEqual(fillExtrusionHeightTransition.duration, transitionTest.duration);
    }

    // fill-extrusion-opacity
    {
        XCTAssertTrue(rawLayer->getFillExtrusionOpacity().isUndefined(),
                      @"fill-extrusion-opacity should be unset initially.");
        NSExpression *defaultExpression = layer.fillExtrusionOpacity;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"1"];
        layer.fillExtrusionOpacity = constantExpression;
        mbgl::style::PropertyValue<float> propertyValue = { 1.0 };
        XCTAssertEqual(rawLayer->getFillExtrusionOpacity(), propertyValue,
                       @"Setting fillExtrusionOpacity to a constant value expression should update fill-extrusion-opacity.");
        XCTAssertEqualObjects(layer.fillExtrusionOpacity, constantExpression,
                              @"fillExtrusionOpacity should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"1"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.fillExtrusionOpacity = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<float>(
                step(zoom(), literal(1.0), 18.0, literal(1.0))
            );
        }

        XCTAssertEqual(rawLayer->getFillExtrusionOpacity(), propertyValue,
                       @"Setting fillExtrusionOpacity to a camera expression should update fill-extrusion-opacity.");
        XCTAssertEqualObjects(layer.fillExtrusionOpacity, functionExpression,
                              @"fillExtrusionOpacity should round-trip camera expressions.");


        layer.fillExtrusionOpacity = nil;
        XCTAssertTrue(rawLayer->getFillExtrusionOpacity().isUndefined(),
                      @"Unsetting fillExtrusionOpacity should return fill-extrusion-opacity to the default value.");
        XCTAssertEqualObjects(layer.fillExtrusionOpacity, defaultExpression,
                              @"fillExtrusionOpacity should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionOpacity = functionExpression, NSException, NSInvalidArgumentException, @"MGLFillExtrusionLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionOpacity = functionExpression, NSException, NSInvalidArgumentException, @"MGLFillExtrusionLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        // Transition property test
        layer.fillExtrusionOpacityTransition = transitionTest;
        auto toptions = rawLayer->getFillExtrusionOpacityTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition fillExtrusionOpacityTransition = layer.fillExtrusionOpacityTransition;
        XCTAssertEqual(fillExtrusionOpacityTransition.delay, transitionTest.delay);
        XCTAssertEqual(fillExtrusionOpacityTransition.duration, transitionTest.duration);
    }

    // fill-extrusion-pattern
    {
        XCTAssertTrue(rawLayer->getFillExtrusionPattern().isUndefined(),
                      @"fill-extrusion-pattern should be unset initially.");
        NSExpression *defaultExpression = layer.fillExtrusionPattern;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'Fill Extrusion Pattern'"];
        layer.fillExtrusionPattern = constantExpression;
        mbgl::style::PropertyValue<std::string> propertyValue = { "Fill Extrusion Pattern" };
        XCTAssertEqual(rawLayer->getFillExtrusionPattern(), propertyValue,
                       @"Setting fillExtrusionPattern to a constant value expression should update fill-extrusion-pattern.");
        XCTAssertEqualObjects(layer.fillExtrusionPattern, constantExpression,
                              @"fillExtrusionPattern should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'Fill Extrusion Pattern'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.fillExtrusionPattern = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<std::string>(
                step(zoom(), literal("Fill Extrusion Pattern"), 18.0, literal("Fill Extrusion Pattern"))
            );
        }

        XCTAssertEqual(rawLayer->getFillExtrusionPattern(), propertyValue,
                       @"Setting fillExtrusionPattern to a camera expression should update fill-extrusion-pattern.");
        XCTAssertEqualObjects(layer.fillExtrusionPattern, functionExpression,
                              @"fillExtrusionPattern should round-trip camera expressions.");


        layer.fillExtrusionPattern = nil;
        XCTAssertTrue(rawLayer->getFillExtrusionPattern().isUndefined(),
                      @"Unsetting fillExtrusionPattern should return fill-extrusion-pattern to the default value.");
        XCTAssertEqualObjects(layer.fillExtrusionPattern, defaultExpression,
                              @"fillExtrusionPattern should return the default value after being unset.");
        // Transition property test
        layer.fillExtrusionPatternTransition = transitionTest;
        auto toptions = rawLayer->getFillExtrusionPatternTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition fillExtrusionPatternTransition = layer.fillExtrusionPatternTransition;
        XCTAssertEqual(fillExtrusionPatternTransition.delay, transitionTest.delay);
        XCTAssertEqual(fillExtrusionPatternTransition.duration, transitionTest.duration);
    }

    // fill-extrusion-translate
    {
        XCTAssertTrue(rawLayer->getFillExtrusionTranslate().isUndefined(),
                      @"fill-extrusion-translate should be unset initially.");
        NSExpression *defaultExpression = layer.fillExtrusionTranslation;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"%@",
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.fillExtrusionTranslation = constantExpression;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getFillExtrusionTranslate(), propertyValue,
                       @"Setting fillExtrusionTranslation to a constant value expression should update fill-extrusion-translate.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslation, constantExpression,
                              @"fillExtrusionTranslation should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"{1, 1}"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.fillExtrusionTranslation = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<std::array<float, 2>>(
                step(zoom(), literal({ 1, 1 }), 18.0, literal({ 1, 1 }))
            );
        }

        XCTAssertEqual(rawLayer->getFillExtrusionTranslate(), propertyValue,
                       @"Setting fillExtrusionTranslation to a camera expression should update fill-extrusion-translate.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslation, functionExpression,
                              @"fillExtrusionTranslation should round-trip camera expressions.");


        layer.fillExtrusionTranslation = nil;
        XCTAssertTrue(rawLayer->getFillExtrusionTranslate().isUndefined(),
                      @"Unsetting fillExtrusionTranslation should return fill-extrusion-translate to the default value.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslation, defaultExpression,
                              @"fillExtrusionTranslation should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionTranslation = functionExpression, NSException, NSInvalidArgumentException, @"MGLFillExtrusionLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionTranslation = functionExpression, NSException, NSInvalidArgumentException, @"MGLFillExtrusionLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }

    // fill-extrusion-translate-anchor
    {
        XCTAssertTrue(rawLayer->getFillExtrusionTranslateAnchor().isUndefined(),
                      @"fill-extrusion-translate-anchor should be unset initially.");
        NSExpression *defaultExpression = layer.fillExtrusionTranslationAnchor;

        NSExpression *constantExpression = [NSExpression expressionWithFormat:@"'viewport'"];
        layer.fillExtrusionTranslationAnchor = constantExpression;
        mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType> propertyValue = { mbgl::style::TranslateAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getFillExtrusionTranslateAnchor(), propertyValue,
                       @"Setting fillExtrusionTranslationAnchor to a constant value expression should update fill-extrusion-translate-anchor.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslationAnchor, constantExpression,
                              @"fillExtrusionTranslationAnchor should round-trip constant value expressions.");

        constantExpression = [NSExpression expressionWithFormat:@"'viewport'"];
        NSExpression *functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:($zoomLevel, %@, %@)", constantExpression, @{@18: constantExpression}];
        layer.fillExtrusionTranslationAnchor = functionExpression;

        {
            using namespace mbgl::style::expression::dsl;
            propertyValue = mbgl::style::PropertyExpression<mbgl::style::TranslateAnchorType>(
                step(zoom(), literal("viewport"), 18.0, literal("viewport"))
            );
        }

        XCTAssertEqual(rawLayer->getFillExtrusionTranslateAnchor(), propertyValue,
                       @"Setting fillExtrusionTranslationAnchor to a camera expression should update fill-extrusion-translate-anchor.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslationAnchor, functionExpression,
                              @"fillExtrusionTranslationAnchor should round-trip camera expressions.");


        layer.fillExtrusionTranslationAnchor = nil;
        XCTAssertTrue(rawLayer->getFillExtrusionTranslateAnchor().isUndefined(),
                      @"Unsetting fillExtrusionTranslationAnchor should return fill-extrusion-translate-anchor to the default value.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslationAnchor, defaultExpression,
                              @"fillExtrusionTranslationAnchor should return the default value after being unset.");

        functionExpression = [NSExpression expressionForKeyPath:@"bogus"];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionTranslationAnchor = functionExpression, NSException, NSInvalidArgumentException, @"MGLFillExtrusionLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
        functionExpression = [NSExpression expressionWithFormat:@"mgl_step:from:stops:(bogus, %@, %@)", constantExpression, @{@18: constantExpression}];
        functionExpression = [NSExpression expressionWithFormat:@"mgl_interpolate:withCurveType:parameters:stops:($zoomLevel, 'linear', nil, %@)", @{@10: functionExpression}];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionTranslationAnchor = functionExpression, NSException, NSInvalidArgumentException, @"MGLFillExtrusionLayer should raise an exception if a camera-data expression is applied to a property that does not support key paths to feature attributes.");
    }
}

- (void)testPropertyNames {
    [self testPropertyName:@"fill-extrusion-base" isBoolean:NO];
    [self testPropertyName:@"fill-extrusion-color" isBoolean:NO];
    [self testPropertyName:@"fill-extrusion-height" isBoolean:NO];
    [self testPropertyName:@"fill-extrusion-opacity" isBoolean:NO];
    [self testPropertyName:@"fill-extrusion-pattern" isBoolean:NO];
    [self testPropertyName:@"fill-extrusion-translation" isBoolean:NO];
    [self testPropertyName:@"fill-extrusion-translation-anchor" isBoolean:NO];
}

- (void)testValueAdditions {
    XCTAssertEqual([NSValue valueWithMGLFillExtrusionTranslationAnchor:MGLFillExtrusionTranslationAnchorMap].MGLFillExtrusionTranslationAnchorValue, MGLFillExtrusionTranslationAnchorMap);
    XCTAssertEqual([NSValue valueWithMGLFillExtrusionTranslationAnchor:MGLFillExtrusionTranslationAnchorViewport].MGLFillExtrusionTranslationAnchorValue, MGLFillExtrusionTranslationAnchorViewport);
}

@end
