// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLStyleLayerTests.h"
#import "../../darwin/src/NSDate+MGLAdditions.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/transition_options.hpp>

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
    layer.predicate = [NSPredicate predicateWithValue:NO];
    XCTAssertEqualObjects(layer.predicate, [NSPredicate predicateWithValue:NO]);
    layer.predicate = nil;
    XCTAssertNil(layer.predicate);
}

- (void)testProperties {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];

    MGLHeatmapStyleLayer *layer = [[MGLHeatmapStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    XCTAssertNotEqual(layer.rawLayer, nullptr);
    XCTAssertTrue(layer.rawLayer->is<mbgl::style::HeatmapLayer>());
    auto rawLayer = layer.rawLayer->as<mbgl::style::HeatmapLayer>();

    MGLTransition transitionTest = MGLTransitionMake(5, 4);


    // heatmap-color
    {
        XCTAssertTrue(rawLayer->getHeatmapColor().isUndefined(),
                      @"heatmap-color should be unset initially.");
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.heatmapColor;

        MGLStyleValue<MGLColor *> *constantStyleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.heatmapColor = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getHeatmapColor(), propertyValue,
                       @"Setting heatmapColor to a constant value should update heatmap-color.");
        XCTAssertEqualObjects(layer.heatmapColor, constantStyleValue,
                              @"heatmapColor should round-trip constant values.");

        MGLStyleValue<MGLColor *> * functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.heatmapColor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = { {{18, { 1, 0, 0, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };
        
        XCTAssertEqual(rawLayer->getHeatmapColor(), propertyValue,
                       @"Setting heatmapColor to a camera function should update heatmap-color.");
        XCTAssertEqualObjects(layer.heatmapColor, functionStyleValue,
                              @"heatmapColor should round-trip camera functions.");

                              

        layer.heatmapColor = nil;
        XCTAssertTrue(rawLayer->getHeatmapColor().isUndefined(),
                      @"Unsetting heatmapColor should return heatmap-color to the default value.");
        XCTAssertEqualObjects(layer.heatmapColor, defaultStyleValue,
                              @"heatmapColor should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.heatmapColor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.heatmapColor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        // Transition property test
        layer.heatmapColorTransition = transitionTest;
        auto toptions = rawLayer->getHeatmapColorTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition heatmapColorTransition = layer.heatmapColorTransition;
        XCTAssertEqual(heatmapColorTransition.delay, transitionTest.delay);
        XCTAssertEqual(heatmapColorTransition.duration, transitionTest.duration);
    }

    // heatmap-intensity
    {
        XCTAssertTrue(rawLayer->getHeatmapIntensity().isUndefined(),
                      @"heatmap-intensity should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.heatmapIntensity;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.heatmapIntensity = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getHeatmapIntensity(), propertyValue,
                       @"Setting heatmapIntensity to a constant value should update heatmap-intensity.");
        XCTAssertEqualObjects(layer.heatmapIntensity, constantStyleValue,
                              @"heatmapIntensity should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.heatmapIntensity = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getHeatmapIntensity(), propertyValue,
                       @"Setting heatmapIntensity to a camera function should update heatmap-intensity.");
        XCTAssertEqualObjects(layer.heatmapIntensity, functionStyleValue,
                              @"heatmapIntensity should round-trip camera functions.");

                              

        layer.heatmapIntensity = nil;
        XCTAssertTrue(rawLayer->getHeatmapIntensity().isUndefined(),
                      @"Unsetting heatmapIntensity should return heatmap-intensity to the default value.");
        XCTAssertEqualObjects(layer.heatmapIntensity, defaultStyleValue,
                              @"heatmapIntensity should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.heatmapIntensity = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.heatmapIntensity = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
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
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.heatmapOpacity;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.heatmapOpacity = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getHeatmapOpacity(), propertyValue,
                       @"Setting heatmapOpacity to a constant value should update heatmap-opacity.");
        XCTAssertEqualObjects(layer.heatmapOpacity, constantStyleValue,
                              @"heatmapOpacity should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.heatmapOpacity = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getHeatmapOpacity(), propertyValue,
                       @"Setting heatmapOpacity to a camera function should update heatmap-opacity.");
        XCTAssertEqualObjects(layer.heatmapOpacity, functionStyleValue,
                              @"heatmapOpacity should round-trip camera functions.");

                              

        layer.heatmapOpacity = nil;
        XCTAssertTrue(rawLayer->getHeatmapOpacity().isUndefined(),
                      @"Unsetting heatmapOpacity should return heatmap-opacity to the default value.");
        XCTAssertEqualObjects(layer.heatmapOpacity, defaultStyleValue,
                              @"heatmapOpacity should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.heatmapOpacity = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.heatmapOpacity = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
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
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.heatmapRadius;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.heatmapRadius = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getHeatmapRadius(), propertyValue,
                       @"Setting heatmapRadius to a constant value should update heatmap-radius.");
        XCTAssertEqualObjects(layer.heatmapRadius, constantStyleValue,
                              @"heatmapRadius should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.heatmapRadius = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getHeatmapRadius(), propertyValue,
                       @"Setting heatmapRadius to a camera function should update heatmap-radius.");
        XCTAssertEqualObjects(layer.heatmapRadius, functionStyleValue,
                              @"heatmapRadius should round-trip camera functions.");

                              

        layer.heatmapRadius = nil;
        XCTAssertTrue(rawLayer->getHeatmapRadius().isUndefined(),
                      @"Unsetting heatmapRadius should return heatmap-radius to the default value.");
        XCTAssertEqualObjects(layer.heatmapRadius, defaultStyleValue,
                              @"heatmapRadius should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.heatmapRadius = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.heatmapRadius = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
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
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.heatmapWeight;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.heatmapWeight = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getHeatmapWeight(), propertyValue,
                       @"Setting heatmapWeight to a constant value should update heatmap-weight.");
        XCTAssertEqualObjects(layer.heatmapWeight, constantStyleValue,
                              @"heatmapWeight should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.heatmapWeight = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getHeatmapWeight(), propertyValue,
                       @"Setting heatmapWeight to a camera function should update heatmap-weight.");
        XCTAssertEqualObjects(layer.heatmapWeight, functionStyleValue,
                              @"heatmapWeight should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.heatmapWeight = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getHeatmapWeight(), propertyValue,
                       @"Setting heatmapWeight to a source function should update heatmap-weight.");
        XCTAssertEqualObjects(layer.heatmapWeight, functionStyleValue,
                              @"heatmapWeight should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.heatmapWeight = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getHeatmapWeight(), propertyValue,
                       @"Setting heatmapWeight to a composite function should update heatmap-weight.");
        XCTAssertEqualObjects(layer.heatmapWeight, functionStyleValue,
                              @"heatmapWeight should round-trip composite functions.");                                                                                                          
                              

        layer.heatmapWeight = nil;
        XCTAssertTrue(rawLayer->getHeatmapWeight().isUndefined(),
                      @"Unsetting heatmapWeight should return heatmap-weight to the default value.");
        XCTAssertEqualObjects(layer.heatmapWeight, defaultStyleValue,
                              @"heatmapWeight should return the default value after being unset.");
    }
}

- (void)testPropertyNames {
    [self testPropertyName:@"heatmap-color" isBoolean:NO];
    [self testPropertyName:@"heatmap-intensity" isBoolean:NO];
    [self testPropertyName:@"heatmap-opacity" isBoolean:NO];
    [self testPropertyName:@"heatmap-radius" isBoolean:NO];
    [self testPropertyName:@"heatmap-weight" isBoolean:NO];
}

@end
