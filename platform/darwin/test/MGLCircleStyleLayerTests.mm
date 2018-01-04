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
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.circleBlur;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.circleBlur = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getCircleBlur(), propertyValue,
                       @"Setting circleBlur to a constant value should update circle-blur.");
        XCTAssertEqualObjects(layer.circleBlur, constantStyleValue,
                              @"circleBlur should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.circleBlur = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getCircleBlur(), propertyValue,
                       @"Setting circleBlur to a camera function should update circle-blur.");
        XCTAssertEqualObjects(layer.circleBlur, functionStyleValue,
                              @"circleBlur should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.circleBlur = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getCircleBlur(), propertyValue,
                       @"Setting circleBlur to a source function should update circle-blur.");
        XCTAssertEqualObjects(layer.circleBlur, functionStyleValue,
                              @"circleBlur should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.circleBlur = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getCircleBlur(), propertyValue,
                       @"Setting circleBlur to a composite function should update circle-blur.");
        XCTAssertEqualObjects(layer.circleBlur, functionStyleValue,
                              @"circleBlur should round-trip composite functions.");                                                                                                          
                              

        layer.circleBlur = nil;
        XCTAssertTrue(rawLayer->getCircleBlur().isUndefined(),
                      @"Unsetting circleBlur should return circle-blur to the default value.");
        XCTAssertEqualObjects(layer.circleBlur, defaultStyleValue,
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
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.circleColor;

        MGLStyleValue<MGLColor *> *constantStyleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.circleColor = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getCircleColor(), propertyValue,
                       @"Setting circleColor to a constant value should update circle-color.");
        XCTAssertEqualObjects(layer.circleColor, constantStyleValue,
                              @"circleColor should round-trip constant values.");

        MGLStyleValue<MGLColor *> * functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.circleColor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = { {{18, { 1, 0, 0, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };
        
        XCTAssertEqual(rawLayer->getCircleColor(), propertyValue,
                       @"Setting circleColor to a camera function should update circle-color.");
        XCTAssertEqualObjects(layer.circleColor, functionStyleValue,
                              @"circleColor should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.circleColor = functionStyleValue;

        mbgl::style::ExponentialStops<mbgl::Color> exponentialStops = { {{18, { 1, 0, 0, 1 }}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<mbgl::Color> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getCircleColor(), propertyValue,
                       @"Setting circleColor to a source function should update circle-color.");
        XCTAssertEqualObjects(layer.circleColor, functionStyleValue,
                              @"circleColor should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.circleColor = functionStyleValue;

        std::map<float, mbgl::Color> innerStops { {18, { 1, 0, 0, 1 }} };
        mbgl::style::CompositeExponentialStops<mbgl::Color> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<mbgl::Color> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getCircleColor(), propertyValue,
                       @"Setting circleColor to a composite function should update circle-color.");
        XCTAssertEqualObjects(layer.circleColor, functionStyleValue,
                              @"circleColor should round-trip composite functions.");                                                                                                          
                              

        layer.circleColor = nil;
        XCTAssertTrue(rawLayer->getCircleColor().isUndefined(),
                      @"Unsetting circleColor should return circle-color to the default value.");
        XCTAssertEqualObjects(layer.circleColor, defaultStyleValue,
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
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.circleOpacity;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.circleOpacity = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getCircleOpacity(), propertyValue,
                       @"Setting circleOpacity to a constant value should update circle-opacity.");
        XCTAssertEqualObjects(layer.circleOpacity, constantStyleValue,
                              @"circleOpacity should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.circleOpacity = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getCircleOpacity(), propertyValue,
                       @"Setting circleOpacity to a camera function should update circle-opacity.");
        XCTAssertEqualObjects(layer.circleOpacity, functionStyleValue,
                              @"circleOpacity should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.circleOpacity = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getCircleOpacity(), propertyValue,
                       @"Setting circleOpacity to a source function should update circle-opacity.");
        XCTAssertEqualObjects(layer.circleOpacity, functionStyleValue,
                              @"circleOpacity should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.circleOpacity = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getCircleOpacity(), propertyValue,
                       @"Setting circleOpacity to a composite function should update circle-opacity.");
        XCTAssertEqualObjects(layer.circleOpacity, functionStyleValue,
                              @"circleOpacity should round-trip composite functions.");                                                                                                          
                              

        layer.circleOpacity = nil;
        XCTAssertTrue(rawLayer->getCircleOpacity().isUndefined(),
                      @"Unsetting circleOpacity should return circle-opacity to the default value.");
        XCTAssertEqualObjects(layer.circleOpacity, defaultStyleValue,
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
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.circlePitchAlignment;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLCirclePitchAlignment:MGLCirclePitchAlignmentViewport]];
        layer.circlePitchAlignment = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::AlignmentType> propertyValue = { mbgl::style::AlignmentType::Viewport };
        XCTAssertEqual(rawLayer->getCirclePitchAlignment(), propertyValue,
                       @"Setting circlePitchAlignment to a constant value should update circle-pitch-alignment.");
        XCTAssertEqualObjects(layer.circlePitchAlignment, constantStyleValue,
                              @"circlePitchAlignment should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.circlePitchAlignment = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::AlignmentType> intervalStops = { {{18, mbgl::style::AlignmentType::Viewport}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::AlignmentType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getCirclePitchAlignment(), propertyValue,
                       @"Setting circlePitchAlignment to a camera function should update circle-pitch-alignment.");
        XCTAssertEqualObjects(layer.circlePitchAlignment, functionStyleValue,
                              @"circlePitchAlignment should round-trip camera functions.");

                              

        layer.circlePitchAlignment = nil;
        XCTAssertTrue(rawLayer->getCirclePitchAlignment().isUndefined(),
                      @"Unsetting circlePitchAlignment should return circle-pitch-alignment to the default value.");
        XCTAssertEqualObjects(layer.circlePitchAlignment, defaultStyleValue,
                              @"circlePitchAlignment should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.circlePitchAlignment = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.circlePitchAlignment = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // circle-radius
    {
        XCTAssertTrue(rawLayer->getCircleRadius().isUndefined(),
                      @"circle-radius should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.circleRadius;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.circleRadius = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getCircleRadius(), propertyValue,
                       @"Setting circleRadius to a constant value should update circle-radius.");
        XCTAssertEqualObjects(layer.circleRadius, constantStyleValue,
                              @"circleRadius should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.circleRadius = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getCircleRadius(), propertyValue,
                       @"Setting circleRadius to a camera function should update circle-radius.");
        XCTAssertEqualObjects(layer.circleRadius, functionStyleValue,
                              @"circleRadius should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.circleRadius = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getCircleRadius(), propertyValue,
                       @"Setting circleRadius to a source function should update circle-radius.");
        XCTAssertEqualObjects(layer.circleRadius, functionStyleValue,
                              @"circleRadius should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.circleRadius = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getCircleRadius(), propertyValue,
                       @"Setting circleRadius to a composite function should update circle-radius.");
        XCTAssertEqualObjects(layer.circleRadius, functionStyleValue,
                              @"circleRadius should round-trip composite functions.");                                                                                                          
                              

        layer.circleRadius = nil;
        XCTAssertTrue(rawLayer->getCircleRadius().isUndefined(),
                      @"Unsetting circleRadius should return circle-radius to the default value.");
        XCTAssertEqualObjects(layer.circleRadius, defaultStyleValue,
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
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.circleScaleAlignment;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLCircleScaleAlignment:MGLCircleScaleAlignmentViewport]];
        layer.circleScaleAlignment = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::CirclePitchScaleType> propertyValue = { mbgl::style::CirclePitchScaleType::Viewport };
        XCTAssertEqual(rawLayer->getCirclePitchScale(), propertyValue,
                       @"Setting circleScaleAlignment to a constant value should update circle-pitch-scale.");
        XCTAssertEqualObjects(layer.circleScaleAlignment, constantStyleValue,
                              @"circleScaleAlignment should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.circleScaleAlignment = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::CirclePitchScaleType> intervalStops = { {{18, mbgl::style::CirclePitchScaleType::Viewport}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::CirclePitchScaleType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getCirclePitchScale(), propertyValue,
                       @"Setting circleScaleAlignment to a camera function should update circle-pitch-scale.");
        XCTAssertEqualObjects(layer.circleScaleAlignment, functionStyleValue,
                              @"circleScaleAlignment should round-trip camera functions.");

                              

        layer.circleScaleAlignment = nil;
        XCTAssertTrue(rawLayer->getCirclePitchScale().isUndefined(),
                      @"Unsetting circleScaleAlignment should return circle-pitch-scale to the default value.");
        XCTAssertEqualObjects(layer.circleScaleAlignment, defaultStyleValue,
                              @"circleScaleAlignment should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.circleScaleAlignment = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.circleScaleAlignment = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // circle-stroke-color
    {
        XCTAssertTrue(rawLayer->getCircleStrokeColor().isUndefined(),
                      @"circle-stroke-color should be unset initially.");
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.circleStrokeColor;

        MGLStyleValue<MGLColor *> *constantStyleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.circleStrokeColor = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getCircleStrokeColor(), propertyValue,
                       @"Setting circleStrokeColor to a constant value should update circle-stroke-color.");
        XCTAssertEqualObjects(layer.circleStrokeColor, constantStyleValue,
                              @"circleStrokeColor should round-trip constant values.");

        MGLStyleValue<MGLColor *> * functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.circleStrokeColor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = { {{18, { 1, 0, 0, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };
        
        XCTAssertEqual(rawLayer->getCircleStrokeColor(), propertyValue,
                       @"Setting circleStrokeColor to a camera function should update circle-stroke-color.");
        XCTAssertEqualObjects(layer.circleStrokeColor, functionStyleValue,
                              @"circleStrokeColor should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.circleStrokeColor = functionStyleValue;

        mbgl::style::ExponentialStops<mbgl::Color> exponentialStops = { {{18, { 1, 0, 0, 1 }}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<mbgl::Color> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getCircleStrokeColor(), propertyValue,
                       @"Setting circleStrokeColor to a source function should update circle-stroke-color.");
        XCTAssertEqualObjects(layer.circleStrokeColor, functionStyleValue,
                              @"circleStrokeColor should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.circleStrokeColor = functionStyleValue;

        std::map<float, mbgl::Color> innerStops { {18, { 1, 0, 0, 1 }} };
        mbgl::style::CompositeExponentialStops<mbgl::Color> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<mbgl::Color> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getCircleStrokeColor(), propertyValue,
                       @"Setting circleStrokeColor to a composite function should update circle-stroke-color.");
        XCTAssertEqualObjects(layer.circleStrokeColor, functionStyleValue,
                              @"circleStrokeColor should round-trip composite functions.");                                                                                                          
                              

        layer.circleStrokeColor = nil;
        XCTAssertTrue(rawLayer->getCircleStrokeColor().isUndefined(),
                      @"Unsetting circleStrokeColor should return circle-stroke-color to the default value.");
        XCTAssertEqualObjects(layer.circleStrokeColor, defaultStyleValue,
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
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.circleStrokeOpacity;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.circleStrokeOpacity = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getCircleStrokeOpacity(), propertyValue,
                       @"Setting circleStrokeOpacity to a constant value should update circle-stroke-opacity.");
        XCTAssertEqualObjects(layer.circleStrokeOpacity, constantStyleValue,
                              @"circleStrokeOpacity should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.circleStrokeOpacity = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getCircleStrokeOpacity(), propertyValue,
                       @"Setting circleStrokeOpacity to a camera function should update circle-stroke-opacity.");
        XCTAssertEqualObjects(layer.circleStrokeOpacity, functionStyleValue,
                              @"circleStrokeOpacity should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.circleStrokeOpacity = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getCircleStrokeOpacity(), propertyValue,
                       @"Setting circleStrokeOpacity to a source function should update circle-stroke-opacity.");
        XCTAssertEqualObjects(layer.circleStrokeOpacity, functionStyleValue,
                              @"circleStrokeOpacity should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.circleStrokeOpacity = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getCircleStrokeOpacity(), propertyValue,
                       @"Setting circleStrokeOpacity to a composite function should update circle-stroke-opacity.");
        XCTAssertEqualObjects(layer.circleStrokeOpacity, functionStyleValue,
                              @"circleStrokeOpacity should round-trip composite functions.");                                                                                                          
                              

        layer.circleStrokeOpacity = nil;
        XCTAssertTrue(rawLayer->getCircleStrokeOpacity().isUndefined(),
                      @"Unsetting circleStrokeOpacity should return circle-stroke-opacity to the default value.");
        XCTAssertEqualObjects(layer.circleStrokeOpacity, defaultStyleValue,
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
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.circleStrokeWidth;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.circleStrokeWidth = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getCircleStrokeWidth(), propertyValue,
                       @"Setting circleStrokeWidth to a constant value should update circle-stroke-width.");
        XCTAssertEqualObjects(layer.circleStrokeWidth, constantStyleValue,
                              @"circleStrokeWidth should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.circleStrokeWidth = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getCircleStrokeWidth(), propertyValue,
                       @"Setting circleStrokeWidth to a camera function should update circle-stroke-width.");
        XCTAssertEqualObjects(layer.circleStrokeWidth, functionStyleValue,
                              @"circleStrokeWidth should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.circleStrokeWidth = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getCircleStrokeWidth(), propertyValue,
                       @"Setting circleStrokeWidth to a source function should update circle-stroke-width.");
        XCTAssertEqualObjects(layer.circleStrokeWidth, functionStyleValue,
                              @"circleStrokeWidth should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.circleStrokeWidth = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getCircleStrokeWidth(), propertyValue,
                       @"Setting circleStrokeWidth to a composite function should update circle-stroke-width.");
        XCTAssertEqualObjects(layer.circleStrokeWidth, functionStyleValue,
                              @"circleStrokeWidth should round-trip composite functions.");                                                                                                          
                              

        layer.circleStrokeWidth = nil;
        XCTAssertTrue(rawLayer->getCircleStrokeWidth().isUndefined(),
                      @"Unsetting circleStrokeWidth should return circle-stroke-width to the default value.");
        XCTAssertEqualObjects(layer.circleStrokeWidth, defaultStyleValue,
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
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.circleTranslation;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.circleTranslation = constantStyleValue;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getCircleTranslate(), propertyValue,
                       @"Setting circleTranslation to a constant value should update circle-translate.");
        XCTAssertEqualObjects(layer.circleTranslation, constantStyleValue,
                              @"circleTranslation should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.circleTranslation = functionStyleValue;

        mbgl::style::IntervalStops<std::array<float, 2>> intervalStops = { {{18, { 1, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<std::array<float, 2>> { intervalStops };
        
        XCTAssertEqual(rawLayer->getCircleTranslate(), propertyValue,
                       @"Setting circleTranslation to a camera function should update circle-translate.");
        XCTAssertEqualObjects(layer.circleTranslation, functionStyleValue,
                              @"circleTranslation should round-trip camera functions.");

                              

        layer.circleTranslation = nil;
        XCTAssertTrue(rawLayer->getCircleTranslate().isUndefined(),
                      @"Unsetting circleTranslation should return circle-translate to the default value.");
        XCTAssertEqualObjects(layer.circleTranslation, defaultStyleValue,
                              @"circleTranslation should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.circleTranslation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.circleTranslation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // circle-translate-anchor
    {
        XCTAssertTrue(rawLayer->getCircleTranslateAnchor().isUndefined(),
                      @"circle-translate-anchor should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.circleTranslationAnchor;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLCircleTranslationAnchor:MGLCircleTranslationAnchorViewport]];
        layer.circleTranslationAnchor = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType> propertyValue = { mbgl::style::TranslateAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getCircleTranslateAnchor(), propertyValue,
                       @"Setting circleTranslationAnchor to a constant value should update circle-translate-anchor.");
        XCTAssertEqualObjects(layer.circleTranslationAnchor, constantStyleValue,
                              @"circleTranslationAnchor should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.circleTranslationAnchor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::TranslateAnchorType> intervalStops = { {{18, mbgl::style::TranslateAnchorType::Viewport}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::TranslateAnchorType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getCircleTranslateAnchor(), propertyValue,
                       @"Setting circleTranslationAnchor to a camera function should update circle-translate-anchor.");
        XCTAssertEqualObjects(layer.circleTranslationAnchor, functionStyleValue,
                              @"circleTranslationAnchor should round-trip camera functions.");

                              

        layer.circleTranslationAnchor = nil;
        XCTAssertTrue(rawLayer->getCircleTranslateAnchor().isUndefined(),
                      @"Unsetting circleTranslationAnchor should return circle-translate-anchor to the default value.");
        XCTAssertEqualObjects(layer.circleTranslationAnchor, defaultStyleValue,
                              @"circleTranslationAnchor should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.circleTranslationAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.circleTranslationAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
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
