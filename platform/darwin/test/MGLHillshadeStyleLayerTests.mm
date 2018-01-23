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

- (void)testPredicates {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];
    MGLHillshadeStyleLayer *layer = [[MGLHillshadeStyleLayer alloc] initWithIdentifier:@"layerID" source:source];

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

    MGLHillshadeStyleLayer *layer = [[MGLHillshadeStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    XCTAssertNotEqual(layer.rawLayer, nullptr);
    XCTAssertTrue(layer.rawLayer->is<mbgl::style::HillshadeLayer>());
    auto rawLayer = layer.rawLayer->as<mbgl::style::HillshadeLayer>();

    MGLTransition transitionTest = MGLTransitionMake(5, 4);


    // hillshade-accent-color
    {
        XCTAssertTrue(rawLayer->getHillshadeAccentColor().isUndefined(),
                      @"hillshade-accent-color should be unset initially.");
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.hillshadeAccentColor;

        MGLStyleValue<MGLColor *> *constantStyleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.hillshadeAccentColor = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getHillshadeAccentColor(), propertyValue,
                       @"Setting hillshadeAccentColor to a constant value should update hillshade-accent-color.");
        XCTAssertEqualObjects(layer.hillshadeAccentColor, constantStyleValue,
                              @"hillshadeAccentColor should round-trip constant values.");

        MGLStyleValue<MGLColor *> * functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.hillshadeAccentColor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = { {{18, { 1, 0, 0, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };
        
        XCTAssertEqual(rawLayer->getHillshadeAccentColor(), propertyValue,
                       @"Setting hillshadeAccentColor to a camera function should update hillshade-accent-color.");
        XCTAssertEqualObjects(layer.hillshadeAccentColor, functionStyleValue,
                              @"hillshadeAccentColor should round-trip camera functions.");

                              

        layer.hillshadeAccentColor = nil;
        XCTAssertTrue(rawLayer->getHillshadeAccentColor().isUndefined(),
                      @"Unsetting hillshadeAccentColor should return hillshade-accent-color to the default value.");
        XCTAssertEqualObjects(layer.hillshadeAccentColor, defaultStyleValue,
                              @"hillshadeAccentColor should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.hillshadeAccentColor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.hillshadeAccentColor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
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
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.hillshadeExaggeration;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.hillshadeExaggeration = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getHillshadeExaggeration(), propertyValue,
                       @"Setting hillshadeExaggeration to a constant value should update hillshade-exaggeration.");
        XCTAssertEqualObjects(layer.hillshadeExaggeration, constantStyleValue,
                              @"hillshadeExaggeration should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.hillshadeExaggeration = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getHillshadeExaggeration(), propertyValue,
                       @"Setting hillshadeExaggeration to a camera function should update hillshade-exaggeration.");
        XCTAssertEqualObjects(layer.hillshadeExaggeration, functionStyleValue,
                              @"hillshadeExaggeration should round-trip camera functions.");

                              

        layer.hillshadeExaggeration = nil;
        XCTAssertTrue(rawLayer->getHillshadeExaggeration().isUndefined(),
                      @"Unsetting hillshadeExaggeration should return hillshade-exaggeration to the default value.");
        XCTAssertEqualObjects(layer.hillshadeExaggeration, defaultStyleValue,
                              @"hillshadeExaggeration should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.hillshadeExaggeration = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.hillshadeExaggeration = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
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
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.hillshadeHighlightColor;

        MGLStyleValue<MGLColor *> *constantStyleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.hillshadeHighlightColor = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getHillshadeHighlightColor(), propertyValue,
                       @"Setting hillshadeHighlightColor to a constant value should update hillshade-highlight-color.");
        XCTAssertEqualObjects(layer.hillshadeHighlightColor, constantStyleValue,
                              @"hillshadeHighlightColor should round-trip constant values.");

        MGLStyleValue<MGLColor *> * functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.hillshadeHighlightColor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = { {{18, { 1, 0, 0, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };
        
        XCTAssertEqual(rawLayer->getHillshadeHighlightColor(), propertyValue,
                       @"Setting hillshadeHighlightColor to a camera function should update hillshade-highlight-color.");
        XCTAssertEqualObjects(layer.hillshadeHighlightColor, functionStyleValue,
                              @"hillshadeHighlightColor should round-trip camera functions.");

                              

        layer.hillshadeHighlightColor = nil;
        XCTAssertTrue(rawLayer->getHillshadeHighlightColor().isUndefined(),
                      @"Unsetting hillshadeHighlightColor should return hillshade-highlight-color to the default value.");
        XCTAssertEqualObjects(layer.hillshadeHighlightColor, defaultStyleValue,
                              @"hillshadeHighlightColor should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.hillshadeHighlightColor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.hillshadeHighlightColor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
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
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.hillshadeIlluminationAnchor;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLHillshadeIlluminationAnchor:MGLHillshadeIlluminationAnchorViewport]];
        layer.hillshadeIlluminationAnchor = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::HillshadeIlluminationAnchorType> propertyValue = { mbgl::style::HillshadeIlluminationAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getHillshadeIlluminationAnchor(), propertyValue,
                       @"Setting hillshadeIlluminationAnchor to a constant value should update hillshade-illumination-anchor.");
        XCTAssertEqualObjects(layer.hillshadeIlluminationAnchor, constantStyleValue,
                              @"hillshadeIlluminationAnchor should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.hillshadeIlluminationAnchor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::HillshadeIlluminationAnchorType> intervalStops = { {{18, mbgl::style::HillshadeIlluminationAnchorType::Viewport}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::HillshadeIlluminationAnchorType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getHillshadeIlluminationAnchor(), propertyValue,
                       @"Setting hillshadeIlluminationAnchor to a camera function should update hillshade-illumination-anchor.");
        XCTAssertEqualObjects(layer.hillshadeIlluminationAnchor, functionStyleValue,
                              @"hillshadeIlluminationAnchor should round-trip camera functions.");

                              

        layer.hillshadeIlluminationAnchor = nil;
        XCTAssertTrue(rawLayer->getHillshadeIlluminationAnchor().isUndefined(),
                      @"Unsetting hillshadeIlluminationAnchor should return hillshade-illumination-anchor to the default value.");
        XCTAssertEqualObjects(layer.hillshadeIlluminationAnchor, defaultStyleValue,
                              @"hillshadeIlluminationAnchor should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.hillshadeIlluminationAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.hillshadeIlluminationAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // hillshade-illumination-direction
    {
        XCTAssertTrue(rawLayer->getHillshadeIlluminationDirection().isUndefined(),
                      @"hillshade-illumination-direction should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.hillshadeIlluminationDirection;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.hillshadeIlluminationDirection = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getHillshadeIlluminationDirection(), propertyValue,
                       @"Setting hillshadeIlluminationDirection to a constant value should update hillshade-illumination-direction.");
        XCTAssertEqualObjects(layer.hillshadeIlluminationDirection, constantStyleValue,
                              @"hillshadeIlluminationDirection should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.hillshadeIlluminationDirection = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getHillshadeIlluminationDirection(), propertyValue,
                       @"Setting hillshadeIlluminationDirection to a camera function should update hillshade-illumination-direction.");
        XCTAssertEqualObjects(layer.hillshadeIlluminationDirection, functionStyleValue,
                              @"hillshadeIlluminationDirection should round-trip camera functions.");

                              

        layer.hillshadeIlluminationDirection = nil;
        XCTAssertTrue(rawLayer->getHillshadeIlluminationDirection().isUndefined(),
                      @"Unsetting hillshadeIlluminationDirection should return hillshade-illumination-direction to the default value.");
        XCTAssertEqualObjects(layer.hillshadeIlluminationDirection, defaultStyleValue,
                              @"hillshadeIlluminationDirection should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.hillshadeIlluminationDirection = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.hillshadeIlluminationDirection = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        // Transition property test
        layer.hillshadeIlluminationDirectionTransition = transitionTest;
        auto toptions = rawLayer->getHillshadeIlluminationDirectionTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition hillshadeIlluminationDirectionTransition = layer.hillshadeIlluminationDirectionTransition;
        XCTAssertEqual(hillshadeIlluminationDirectionTransition.delay, transitionTest.delay);
        XCTAssertEqual(hillshadeIlluminationDirectionTransition.duration, transitionTest.duration);
    }

    // hillshade-shadow-color
    {
        XCTAssertTrue(rawLayer->getHillshadeShadowColor().isUndefined(),
                      @"hillshade-shadow-color should be unset initially.");
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.hillshadeShadowColor;

        MGLStyleValue<MGLColor *> *constantStyleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.hillshadeShadowColor = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getHillshadeShadowColor(), propertyValue,
                       @"Setting hillshadeShadowColor to a constant value should update hillshade-shadow-color.");
        XCTAssertEqualObjects(layer.hillshadeShadowColor, constantStyleValue,
                              @"hillshadeShadowColor should round-trip constant values.");

        MGLStyleValue<MGLColor *> * functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.hillshadeShadowColor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = { {{18, { 1, 0, 0, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };
        
        XCTAssertEqual(rawLayer->getHillshadeShadowColor(), propertyValue,
                       @"Setting hillshadeShadowColor to a camera function should update hillshade-shadow-color.");
        XCTAssertEqualObjects(layer.hillshadeShadowColor, functionStyleValue,
                              @"hillshadeShadowColor should round-trip camera functions.");

                              

        layer.hillshadeShadowColor = nil;
        XCTAssertTrue(rawLayer->getHillshadeShadowColor().isUndefined(),
                      @"Unsetting hillshadeShadowColor should return hillshade-shadow-color to the default value.");
        XCTAssertEqualObjects(layer.hillshadeShadowColor, defaultStyleValue,
                              @"hillshadeShadowColor should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.hillshadeShadowColor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.hillshadeShadowColor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
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
