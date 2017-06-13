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
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.lineCap;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLLineCap:MGLLineCapSquare]];
        layer.lineCap = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::LineCapType> propertyValue = { mbgl::style::LineCapType::Square };
        XCTAssertEqual(rawLayer->getLineCap(), propertyValue,
                       @"Setting lineCap to a constant value should update line-cap.");
        XCTAssertEqualObjects(layer.lineCap, constantStyleValue,
                              @"lineCap should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.lineCap = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::LineCapType> intervalStops = { {{18, mbgl::style::LineCapType::Square}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::LineCapType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getLineCap(), propertyValue,
                       @"Setting lineCap to a camera function should update line-cap.");
        XCTAssertEqualObjects(layer.lineCap, functionStyleValue,
                              @"lineCap should round-trip camera functions.");

                              

        layer.lineCap = nil;
        XCTAssertTrue(rawLayer->getLineCap().isUndefined(),
                      @"Unsetting lineCap should return line-cap to the default value.");
        XCTAssertEqualObjects(layer.lineCap, defaultStyleValue,
                              @"lineCap should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.lineCap = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.lineCap = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // line-join
    {
        XCTAssertTrue(rawLayer->getLineJoin().isUndefined(),
                      @"line-join should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.lineJoin;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLLineJoin:MGLLineJoinMiter]];
        layer.lineJoin = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::LineJoinType> propertyValue = { mbgl::style::LineJoinType::Miter };
        XCTAssertEqual(rawLayer->getLineJoin(), propertyValue,
                       @"Setting lineJoin to a constant value should update line-join.");
        XCTAssertEqualObjects(layer.lineJoin, constantStyleValue,
                              @"lineJoin should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.lineJoin = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::LineJoinType> intervalStops = { {{18, mbgl::style::LineJoinType::Miter}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::LineJoinType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getLineJoin(), propertyValue,
                       @"Setting lineJoin to a camera function should update line-join.");
        XCTAssertEqualObjects(layer.lineJoin, functionStyleValue,
                              @"lineJoin should round-trip camera functions.");

                              

        layer.lineJoin = nil;
        XCTAssertTrue(rawLayer->getLineJoin().isUndefined(),
                      @"Unsetting lineJoin should return line-join to the default value.");
        XCTAssertEqualObjects(layer.lineJoin, defaultStyleValue,
                              @"lineJoin should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.lineJoin = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.lineJoin = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // line-miter-limit
    {
        XCTAssertTrue(rawLayer->getLineMiterLimit().isUndefined(),
                      @"line-miter-limit should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.lineMiterLimit;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.lineMiterLimit = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineMiterLimit(), propertyValue,
                       @"Setting lineMiterLimit to a constant value should update line-miter-limit.");
        XCTAssertEqualObjects(layer.lineMiterLimit, constantStyleValue,
                              @"lineMiterLimit should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.lineMiterLimit = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getLineMiterLimit(), propertyValue,
                       @"Setting lineMiterLimit to a camera function should update line-miter-limit.");
        XCTAssertEqualObjects(layer.lineMiterLimit, functionStyleValue,
                              @"lineMiterLimit should round-trip camera functions.");

                              

        layer.lineMiterLimit = nil;
        XCTAssertTrue(rawLayer->getLineMiterLimit().isUndefined(),
                      @"Unsetting lineMiterLimit should return line-miter-limit to the default value.");
        XCTAssertEqualObjects(layer.lineMiterLimit, defaultStyleValue,
                              @"lineMiterLimit should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.lineMiterLimit = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.lineMiterLimit = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // line-round-limit
    {
        XCTAssertTrue(rawLayer->getLineRoundLimit().isUndefined(),
                      @"line-round-limit should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.lineRoundLimit;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.lineRoundLimit = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineRoundLimit(), propertyValue,
                       @"Setting lineRoundLimit to a constant value should update line-round-limit.");
        XCTAssertEqualObjects(layer.lineRoundLimit, constantStyleValue,
                              @"lineRoundLimit should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.lineRoundLimit = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getLineRoundLimit(), propertyValue,
                       @"Setting lineRoundLimit to a camera function should update line-round-limit.");
        XCTAssertEqualObjects(layer.lineRoundLimit, functionStyleValue,
                              @"lineRoundLimit should round-trip camera functions.");

                              

        layer.lineRoundLimit = nil;
        XCTAssertTrue(rawLayer->getLineRoundLimit().isUndefined(),
                      @"Unsetting lineRoundLimit should return line-round-limit to the default value.");
        XCTAssertEqualObjects(layer.lineRoundLimit, defaultStyleValue,
                              @"lineRoundLimit should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.lineRoundLimit = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.lineRoundLimit = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // line-blur
    {
        XCTAssertTrue(rawLayer->getLineBlur().isUndefined(),
                      @"line-blur should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.lineBlur;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.lineBlur = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineBlur(), propertyValue,
                       @"Setting lineBlur to a constant value should update line-blur.");
        XCTAssertEqualObjects(layer.lineBlur, constantStyleValue,
                              @"lineBlur should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.lineBlur = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getLineBlur(), propertyValue,
                       @"Setting lineBlur to a camera function should update line-blur.");
        XCTAssertEqualObjects(layer.lineBlur, functionStyleValue,
                              @"lineBlur should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.lineBlur = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getLineBlur(), propertyValue,
                       @"Setting lineBlur to a source function should update line-blur.");
        XCTAssertEqualObjects(layer.lineBlur, functionStyleValue,
                              @"lineBlur should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.lineBlur = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getLineBlur(), propertyValue,
                       @"Setting lineBlur to a composite function should update line-blur.");
        XCTAssertEqualObjects(layer.lineBlur, functionStyleValue,
                              @"lineBlur should round-trip composite functions.");                                                                                                          
                              

        layer.lineBlur = nil;
        XCTAssertTrue(rawLayer->getLineBlur().isUndefined(),
                      @"Unsetting lineBlur should return line-blur to the default value.");
        XCTAssertEqualObjects(layer.lineBlur, defaultStyleValue,
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
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.lineColor;

        MGLStyleValue<MGLColor *> *constantStyleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.lineColor = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getLineColor(), propertyValue,
                       @"Setting lineColor to a constant value should update line-color.");
        XCTAssertEqualObjects(layer.lineColor, constantStyleValue,
                              @"lineColor should round-trip constant values.");

        MGLStyleValue<MGLColor *> * functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.lineColor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = { {{18, { 1, 0, 0, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };
        
        XCTAssertEqual(rawLayer->getLineColor(), propertyValue,
                       @"Setting lineColor to a camera function should update line-color.");
        XCTAssertEqualObjects(layer.lineColor, functionStyleValue,
                              @"lineColor should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.lineColor = functionStyleValue;

        mbgl::style::ExponentialStops<mbgl::Color> exponentialStops = { {{18, { 1, 0, 0, 1 }}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<mbgl::Color> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getLineColor(), propertyValue,
                       @"Setting lineColor to a source function should update line-color.");
        XCTAssertEqualObjects(layer.lineColor, functionStyleValue,
                              @"lineColor should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.lineColor = functionStyleValue;

        std::map<float, mbgl::Color> innerStops { {18, { 1, 0, 0, 1 }} };
        mbgl::style::CompositeExponentialStops<mbgl::Color> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<mbgl::Color> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getLineColor(), propertyValue,
                       @"Setting lineColor to a composite function should update line-color.");
        XCTAssertEqualObjects(layer.lineColor, functionStyleValue,
                              @"lineColor should round-trip composite functions.");                                                                                                          
                              

        layer.lineColor = nil;
        XCTAssertTrue(rawLayer->getLineColor().isUndefined(),
                      @"Unsetting lineColor should return line-color to the default value.");
        XCTAssertEqualObjects(layer.lineColor, defaultStyleValue,
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
        MGLStyleValue<NSArray<NSNumber *> *> *defaultStyleValue = layer.lineDashPattern;

        MGLStyleValue<NSArray<NSNumber *> *> *constantStyleValue = [MGLStyleValue<NSArray<NSNumber *> *> valueWithRawValue:@[@1, @2]];
        layer.lineDashPattern = constantStyleValue;
        mbgl::style::PropertyValue<std::vector<float>> propertyValue = { {1, 2} };
        XCTAssertEqual(rawLayer->getLineDasharray(), propertyValue,
                       @"Setting lineDashPattern to a constant value should update line-dasharray.");
        XCTAssertEqualObjects(layer.lineDashPattern, constantStyleValue,
                              @"lineDashPattern should round-trip constant values.");

        MGLStyleValue<NSArray<NSNumber *> *> * functionStyleValue = [MGLStyleValue<NSArray<NSNumber *> *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.lineDashPattern = functionStyleValue;

        mbgl::style::IntervalStops<std::vector<float>> intervalStops = { {{18, {1, 2}}} };
        propertyValue = mbgl::style::CameraFunction<std::vector<float>> { intervalStops };
        
        XCTAssertEqual(rawLayer->getLineDasharray(), propertyValue,
                       @"Setting lineDashPattern to a camera function should update line-dasharray.");
        XCTAssertEqualObjects(layer.lineDashPattern, functionStyleValue,
                              @"lineDashPattern should round-trip camera functions.");

                              

        layer.lineDashPattern = nil;
        XCTAssertTrue(rawLayer->getLineDasharray().isUndefined(),
                      @"Unsetting lineDashPattern should return line-dasharray to the default value.");
        XCTAssertEqualObjects(layer.lineDashPattern, defaultStyleValue,
                              @"lineDashPattern should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSArray<NSNumber *> *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.lineDashPattern = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSArray<NSNumber *> *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.lineDashPattern = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // line-gap-width
    {
        XCTAssertTrue(rawLayer->getLineGapWidth().isUndefined(),
                      @"line-gap-width should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.lineGapWidth;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.lineGapWidth = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineGapWidth(), propertyValue,
                       @"Setting lineGapWidth to a constant value should update line-gap-width.");
        XCTAssertEqualObjects(layer.lineGapWidth, constantStyleValue,
                              @"lineGapWidth should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.lineGapWidth = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getLineGapWidth(), propertyValue,
                       @"Setting lineGapWidth to a camera function should update line-gap-width.");
        XCTAssertEqualObjects(layer.lineGapWidth, functionStyleValue,
                              @"lineGapWidth should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.lineGapWidth = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getLineGapWidth(), propertyValue,
                       @"Setting lineGapWidth to a source function should update line-gap-width.");
        XCTAssertEqualObjects(layer.lineGapWidth, functionStyleValue,
                              @"lineGapWidth should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.lineGapWidth = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getLineGapWidth(), propertyValue,
                       @"Setting lineGapWidth to a composite function should update line-gap-width.");
        XCTAssertEqualObjects(layer.lineGapWidth, functionStyleValue,
                              @"lineGapWidth should round-trip composite functions.");                                                                                                          
                              

        layer.lineGapWidth = nil;
        XCTAssertTrue(rawLayer->getLineGapWidth().isUndefined(),
                      @"Unsetting lineGapWidth should return line-gap-width to the default value.");
        XCTAssertEqualObjects(layer.lineGapWidth, defaultStyleValue,
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
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.lineOffset;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.lineOffset = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineOffset(), propertyValue,
                       @"Setting lineOffset to a constant value should update line-offset.");
        XCTAssertEqualObjects(layer.lineOffset, constantStyleValue,
                              @"lineOffset should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.lineOffset = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getLineOffset(), propertyValue,
                       @"Setting lineOffset to a camera function should update line-offset.");
        XCTAssertEqualObjects(layer.lineOffset, functionStyleValue,
                              @"lineOffset should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.lineOffset = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getLineOffset(), propertyValue,
                       @"Setting lineOffset to a source function should update line-offset.");
        XCTAssertEqualObjects(layer.lineOffset, functionStyleValue,
                              @"lineOffset should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.lineOffset = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getLineOffset(), propertyValue,
                       @"Setting lineOffset to a composite function should update line-offset.");
        XCTAssertEqualObjects(layer.lineOffset, functionStyleValue,
                              @"lineOffset should round-trip composite functions.");                                                                                                          
                              

        layer.lineOffset = nil;
        XCTAssertTrue(rawLayer->getLineOffset().isUndefined(),
                      @"Unsetting lineOffset should return line-offset to the default value.");
        XCTAssertEqualObjects(layer.lineOffset, defaultStyleValue,
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
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.lineOpacity;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.lineOpacity = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineOpacity(), propertyValue,
                       @"Setting lineOpacity to a constant value should update line-opacity.");
        XCTAssertEqualObjects(layer.lineOpacity, constantStyleValue,
                              @"lineOpacity should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.lineOpacity = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getLineOpacity(), propertyValue,
                       @"Setting lineOpacity to a camera function should update line-opacity.");
        XCTAssertEqualObjects(layer.lineOpacity, functionStyleValue,
                              @"lineOpacity should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.lineOpacity = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getLineOpacity(), propertyValue,
                       @"Setting lineOpacity to a source function should update line-opacity.");
        XCTAssertEqualObjects(layer.lineOpacity, functionStyleValue,
                              @"lineOpacity should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.lineOpacity = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getLineOpacity(), propertyValue,
                       @"Setting lineOpacity to a composite function should update line-opacity.");
        XCTAssertEqualObjects(layer.lineOpacity, functionStyleValue,
                              @"lineOpacity should round-trip composite functions.");                                                                                                          
                              

        layer.lineOpacity = nil;
        XCTAssertTrue(rawLayer->getLineOpacity().isUndefined(),
                      @"Unsetting lineOpacity should return line-opacity to the default value.");
        XCTAssertEqualObjects(layer.lineOpacity, defaultStyleValue,
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
        MGLStyleValue<NSString *> *defaultStyleValue = layer.linePattern;

        MGLStyleValue<NSString *> *constantStyleValue = [MGLStyleValue<NSString *> valueWithRawValue:@"Line Pattern"];
        layer.linePattern = constantStyleValue;
        mbgl::style::PropertyValue<std::string> propertyValue = { "Line Pattern" };
        XCTAssertEqual(rawLayer->getLinePattern(), propertyValue,
                       @"Setting linePattern to a constant value should update line-pattern.");
        XCTAssertEqualObjects(layer.linePattern, constantStyleValue,
                              @"linePattern should round-trip constant values.");

        MGLStyleValue<NSString *> * functionStyleValue = [MGLStyleValue<NSString *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.linePattern = functionStyleValue;

        mbgl::style::IntervalStops<std::string> intervalStops = { {{18, "Line Pattern"}} };
        propertyValue = mbgl::style::CameraFunction<std::string> { intervalStops };
        
        XCTAssertEqual(rawLayer->getLinePattern(), propertyValue,
                       @"Setting linePattern to a camera function should update line-pattern.");
        XCTAssertEqualObjects(layer.linePattern, functionStyleValue,
                              @"linePattern should round-trip camera functions.");

                              

        layer.linePattern = nil;
        XCTAssertTrue(rawLayer->getLinePattern().isUndefined(),
                      @"Unsetting linePattern should return line-pattern to the default value.");
        XCTAssertEqualObjects(layer.linePattern, defaultStyleValue,
                              @"linePattern should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSString *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.linePattern = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSString *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.linePattern = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
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
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.lineTranslation;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.lineTranslation = constantStyleValue;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getLineTranslate(), propertyValue,
                       @"Setting lineTranslation to a constant value should update line-translate.");
        XCTAssertEqualObjects(layer.lineTranslation, constantStyleValue,
                              @"lineTranslation should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.lineTranslation = functionStyleValue;

        mbgl::style::IntervalStops<std::array<float, 2>> intervalStops = { {{18, { 1, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<std::array<float, 2>> { intervalStops };
        
        XCTAssertEqual(rawLayer->getLineTranslate(), propertyValue,
                       @"Setting lineTranslation to a camera function should update line-translate.");
        XCTAssertEqualObjects(layer.lineTranslation, functionStyleValue,
                              @"lineTranslation should round-trip camera functions.");

                              

        layer.lineTranslation = nil;
        XCTAssertTrue(rawLayer->getLineTranslate().isUndefined(),
                      @"Unsetting lineTranslation should return line-translate to the default value.");
        XCTAssertEqualObjects(layer.lineTranslation, defaultStyleValue,
                              @"lineTranslation should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.lineTranslation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.lineTranslation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // line-translate-anchor
    {
        XCTAssertTrue(rawLayer->getLineTranslateAnchor().isUndefined(),
                      @"line-translate-anchor should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.lineTranslationAnchor;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLLineTranslationAnchor:MGLLineTranslationAnchorViewport]];
        layer.lineTranslationAnchor = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType> propertyValue = { mbgl::style::TranslateAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getLineTranslateAnchor(), propertyValue,
                       @"Setting lineTranslationAnchor to a constant value should update line-translate-anchor.");
        XCTAssertEqualObjects(layer.lineTranslationAnchor, constantStyleValue,
                              @"lineTranslationAnchor should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.lineTranslationAnchor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::TranslateAnchorType> intervalStops = { {{18, mbgl::style::TranslateAnchorType::Viewport}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::TranslateAnchorType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getLineTranslateAnchor(), propertyValue,
                       @"Setting lineTranslationAnchor to a camera function should update line-translate-anchor.");
        XCTAssertEqualObjects(layer.lineTranslationAnchor, functionStyleValue,
                              @"lineTranslationAnchor should round-trip camera functions.");

                              

        layer.lineTranslationAnchor = nil;
        XCTAssertTrue(rawLayer->getLineTranslateAnchor().isUndefined(),
                      @"Unsetting lineTranslationAnchor should return line-translate-anchor to the default value.");
        XCTAssertEqualObjects(layer.lineTranslationAnchor, defaultStyleValue,
                              @"lineTranslationAnchor should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.lineTranslationAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.lineTranslationAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // line-width
    {
        XCTAssertTrue(rawLayer->getLineWidth().isUndefined(),
                      @"line-width should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.lineWidth;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.lineWidth = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineWidth(), propertyValue,
                       @"Setting lineWidth to a constant value should update line-width.");
        XCTAssertEqualObjects(layer.lineWidth, constantStyleValue,
                              @"lineWidth should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.lineWidth = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getLineWidth(), propertyValue,
                       @"Setting lineWidth to a camera function should update line-width.");
        XCTAssertEqualObjects(layer.lineWidth, functionStyleValue,
                              @"lineWidth should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.lineWidth = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getLineWidth(), propertyValue,
                       @"Setting lineWidth to a source function should update line-width.");
        XCTAssertEqualObjects(layer.lineWidth, functionStyleValue,
                              @"lineWidth should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.lineWidth = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getLineWidth(), propertyValue,
                       @"Setting lineWidth to a composite function should update line-width.");
        XCTAssertEqualObjects(layer.lineWidth, functionStyleValue,
                              @"lineWidth should round-trip composite functions.");                                                                                                          
                              

        layer.lineWidth = nil;
        XCTAssertTrue(rawLayer->getLineWidth().isUndefined(),
                      @"Unsetting lineWidth should return line-width to the default value.");
        XCTAssertEqualObjects(layer.lineWidth, defaultStyleValue,
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
