// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLStyleLayerTests.h"
#import "../../darwin/src/NSDate+MGLAdditions.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/transition_options.hpp>

@interface MGLFillLayerTests : MGLStyleLayerTests
@end

@implementation MGLFillLayerTests

+ (NSString *)layerType {
    return @"fill";
}

- (void)testPredicates {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];
    MGLFillStyleLayer *layer = [[MGLFillStyleLayer alloc] initWithIdentifier:@"layerID" source:source];

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

    MGLFillStyleLayer *layer = [[MGLFillStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    XCTAssertNotEqual(layer.rawLayer, nullptr);
    XCTAssertTrue(layer.rawLayer->is<mbgl::style::FillLayer>());
    auto rawLayer = layer.rawLayer->as<mbgl::style::FillLayer>();

    MGLTransition transitionTest = MGLTransitionMake(5, 4);


    // fill-antialias
    {
        XCTAssertTrue(rawLayer->getFillAntialias().isUndefined(),
                      @"fill-antialias should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.fillAntialiased;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@NO];
        layer.fillAntialiased = constantStyleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { false };
        XCTAssertEqual(rawLayer->getFillAntialias(), propertyValue,
                       @"Setting fillAntialiased to a constant value should update fill-antialias.");
        XCTAssertEqualObjects(layer.fillAntialiased, constantStyleValue,
                              @"fillAntialiased should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.fillAntialiased = functionStyleValue;

        mbgl::style::IntervalStops<bool> intervalStops = { {{18, false}} };
        propertyValue = mbgl::style::CameraFunction<bool> { intervalStops };
        
        XCTAssertEqual(rawLayer->getFillAntialias(), propertyValue,
                       @"Setting fillAntialiased to a camera function should update fill-antialias.");
        XCTAssertEqualObjects(layer.fillAntialiased, functionStyleValue,
                              @"fillAntialiased should round-trip camera functions.");

                              

        layer.fillAntialiased = nil;
        XCTAssertTrue(rawLayer->getFillAntialias().isUndefined(),
                      @"Unsetting fillAntialiased should return fill-antialias to the default value.");
        XCTAssertEqualObjects(layer.fillAntialiased, defaultStyleValue,
                              @"fillAntialiased should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillAntialiased = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillAntialiased = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // fill-color
    {
        XCTAssertTrue(rawLayer->getFillColor().isUndefined(),
                      @"fill-color should be unset initially.");
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.fillColor;

        MGLStyleValue<MGLColor *> *constantStyleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.fillColor = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getFillColor(), propertyValue,
                       @"Setting fillColor to a constant value should update fill-color.");
        XCTAssertEqualObjects(layer.fillColor, constantStyleValue,
                              @"fillColor should round-trip constant values.");

        MGLStyleValue<MGLColor *> * functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.fillColor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = { {{18, { 1, 0, 0, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };
        
        XCTAssertEqual(rawLayer->getFillColor(), propertyValue,
                       @"Setting fillColor to a camera function should update fill-color.");
        XCTAssertEqualObjects(layer.fillColor, functionStyleValue,
                              @"fillColor should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.fillColor = functionStyleValue;

        mbgl::style::ExponentialStops<mbgl::Color> exponentialStops = { {{18, { 1, 0, 0, 1 }}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<mbgl::Color> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getFillColor(), propertyValue,
                       @"Setting fillColor to a source function should update fill-color.");
        XCTAssertEqualObjects(layer.fillColor, functionStyleValue,
                              @"fillColor should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.fillColor = functionStyleValue;

        std::map<float, mbgl::Color> innerStops { {18, { 1, 0, 0, 1 }} };
        mbgl::style::CompositeExponentialStops<mbgl::Color> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<mbgl::Color> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getFillColor(), propertyValue,
                       @"Setting fillColor to a composite function should update fill-color.");
        XCTAssertEqualObjects(layer.fillColor, functionStyleValue,
                              @"fillColor should round-trip composite functions.");                                                                                                          
                              

        layer.fillColor = nil;
        XCTAssertTrue(rawLayer->getFillColor().isUndefined(),
                      @"Unsetting fillColor should return fill-color to the default value.");
        XCTAssertEqualObjects(layer.fillColor, defaultStyleValue,
                              @"fillColor should return the default value after being unset.");
        // Transition property test
        layer.fillColorTransition = transitionTest;
        auto toptions = rawLayer->getFillColorTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition fillColorTransition = layer.fillColorTransition;
        XCTAssertEqual(fillColorTransition.delay, transitionTest.delay);
        XCTAssertEqual(fillColorTransition.duration, transitionTest.duration);
    }

    // fill-opacity
    {
        XCTAssertTrue(rawLayer->getFillOpacity().isUndefined(),
                      @"fill-opacity should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.fillOpacity;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.fillOpacity = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getFillOpacity(), propertyValue,
                       @"Setting fillOpacity to a constant value should update fill-opacity.");
        XCTAssertEqualObjects(layer.fillOpacity, constantStyleValue,
                              @"fillOpacity should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.fillOpacity = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getFillOpacity(), propertyValue,
                       @"Setting fillOpacity to a camera function should update fill-opacity.");
        XCTAssertEqualObjects(layer.fillOpacity, functionStyleValue,
                              @"fillOpacity should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.fillOpacity = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getFillOpacity(), propertyValue,
                       @"Setting fillOpacity to a source function should update fill-opacity.");
        XCTAssertEqualObjects(layer.fillOpacity, functionStyleValue,
                              @"fillOpacity should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.fillOpacity = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getFillOpacity(), propertyValue,
                       @"Setting fillOpacity to a composite function should update fill-opacity.");
        XCTAssertEqualObjects(layer.fillOpacity, functionStyleValue,
                              @"fillOpacity should round-trip composite functions.");                                                                                                          
                              

        layer.fillOpacity = nil;
        XCTAssertTrue(rawLayer->getFillOpacity().isUndefined(),
                      @"Unsetting fillOpacity should return fill-opacity to the default value.");
        XCTAssertEqualObjects(layer.fillOpacity, defaultStyleValue,
                              @"fillOpacity should return the default value after being unset.");
        // Transition property test
        layer.fillOpacityTransition = transitionTest;
        auto toptions = rawLayer->getFillOpacityTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition fillOpacityTransition = layer.fillOpacityTransition;
        XCTAssertEqual(fillOpacityTransition.delay, transitionTest.delay);
        XCTAssertEqual(fillOpacityTransition.duration, transitionTest.duration);
    }

    // fill-outline-color
    {
        XCTAssertTrue(rawLayer->getFillOutlineColor().isUndefined(),
                      @"fill-outline-color should be unset initially.");
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.fillOutlineColor;

        MGLStyleValue<MGLColor *> *constantStyleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.fillOutlineColor = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getFillOutlineColor(), propertyValue,
                       @"Setting fillOutlineColor to a constant value should update fill-outline-color.");
        XCTAssertEqualObjects(layer.fillOutlineColor, constantStyleValue,
                              @"fillOutlineColor should round-trip constant values.");

        MGLStyleValue<MGLColor *> * functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.fillOutlineColor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = { {{18, { 1, 0, 0, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };
        
        XCTAssertEqual(rawLayer->getFillOutlineColor(), propertyValue,
                       @"Setting fillOutlineColor to a camera function should update fill-outline-color.");
        XCTAssertEqualObjects(layer.fillOutlineColor, functionStyleValue,
                              @"fillOutlineColor should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.fillOutlineColor = functionStyleValue;

        mbgl::style::ExponentialStops<mbgl::Color> exponentialStops = { {{18, { 1, 0, 0, 1 }}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<mbgl::Color> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getFillOutlineColor(), propertyValue,
                       @"Setting fillOutlineColor to a source function should update fill-outline-color.");
        XCTAssertEqualObjects(layer.fillOutlineColor, functionStyleValue,
                              @"fillOutlineColor should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.fillOutlineColor = functionStyleValue;

        std::map<float, mbgl::Color> innerStops { {18, { 1, 0, 0, 1 }} };
        mbgl::style::CompositeExponentialStops<mbgl::Color> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<mbgl::Color> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getFillOutlineColor(), propertyValue,
                       @"Setting fillOutlineColor to a composite function should update fill-outline-color.");
        XCTAssertEqualObjects(layer.fillOutlineColor, functionStyleValue,
                              @"fillOutlineColor should round-trip composite functions.");                                                                                                          
                              

        layer.fillOutlineColor = nil;
        XCTAssertTrue(rawLayer->getFillOutlineColor().isUndefined(),
                      @"Unsetting fillOutlineColor should return fill-outline-color to the default value.");
        XCTAssertEqualObjects(layer.fillOutlineColor, defaultStyleValue,
                              @"fillOutlineColor should return the default value after being unset.");
        // Transition property test
        layer.fillOutlineColorTransition = transitionTest;
        auto toptions = rawLayer->getFillOutlineColorTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition fillOutlineColorTransition = layer.fillOutlineColorTransition;
        XCTAssertEqual(fillOutlineColorTransition.delay, transitionTest.delay);
        XCTAssertEqual(fillOutlineColorTransition.duration, transitionTest.duration);
    }

    // fill-pattern
    {
        XCTAssertTrue(rawLayer->getFillPattern().isUndefined(),
                      @"fill-pattern should be unset initially.");
        MGLStyleValue<NSString *> *defaultStyleValue = layer.fillPattern;

        MGLStyleValue<NSString *> *constantStyleValue = [MGLStyleValue<NSString *> valueWithRawValue:@"Fill Pattern"];
        layer.fillPattern = constantStyleValue;
        mbgl::style::PropertyValue<std::string> propertyValue = { "Fill Pattern" };
        XCTAssertEqual(rawLayer->getFillPattern(), propertyValue,
                       @"Setting fillPattern to a constant value should update fill-pattern.");
        XCTAssertEqualObjects(layer.fillPattern, constantStyleValue,
                              @"fillPattern should round-trip constant values.");

        MGLStyleValue<NSString *> * functionStyleValue = [MGLStyleValue<NSString *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.fillPattern = functionStyleValue;

        mbgl::style::IntervalStops<std::string> intervalStops = { {{18, "Fill Pattern"}} };
        propertyValue = mbgl::style::CameraFunction<std::string> { intervalStops };
        
        XCTAssertEqual(rawLayer->getFillPattern(), propertyValue,
                       @"Setting fillPattern to a camera function should update fill-pattern.");
        XCTAssertEqualObjects(layer.fillPattern, functionStyleValue,
                              @"fillPattern should round-trip camera functions.");

                              

        layer.fillPattern = nil;
        XCTAssertTrue(rawLayer->getFillPattern().isUndefined(),
                      @"Unsetting fillPattern should return fill-pattern to the default value.");
        XCTAssertEqualObjects(layer.fillPattern, defaultStyleValue,
                              @"fillPattern should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSString *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillPattern = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSString *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillPattern = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        // Transition property test
        layer.fillPatternTransition = transitionTest;
        auto toptions = rawLayer->getFillPatternTransition();
        XCTAssert(toptions.delay && MGLTimeIntervalFromDuration(*toptions.delay) == transitionTest.delay);
        XCTAssert(toptions.duration && MGLTimeIntervalFromDuration(*toptions.duration) == transitionTest.duration);

        MGLTransition fillPatternTransition = layer.fillPatternTransition;
        XCTAssertEqual(fillPatternTransition.delay, transitionTest.delay);
        XCTAssertEqual(fillPatternTransition.duration, transitionTest.duration);
    }

    // fill-translate
    {
        XCTAssertTrue(rawLayer->getFillTranslate().isUndefined(),
                      @"fill-translate should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.fillTranslation;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.fillTranslation = constantStyleValue;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getFillTranslate(), propertyValue,
                       @"Setting fillTranslation to a constant value should update fill-translate.");
        XCTAssertEqualObjects(layer.fillTranslation, constantStyleValue,
                              @"fillTranslation should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.fillTranslation = functionStyleValue;

        mbgl::style::IntervalStops<std::array<float, 2>> intervalStops = { {{18, { 1, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<std::array<float, 2>> { intervalStops };
        
        XCTAssertEqual(rawLayer->getFillTranslate(), propertyValue,
                       @"Setting fillTranslation to a camera function should update fill-translate.");
        XCTAssertEqualObjects(layer.fillTranslation, functionStyleValue,
                              @"fillTranslation should round-trip camera functions.");

                              

        layer.fillTranslation = nil;
        XCTAssertTrue(rawLayer->getFillTranslate().isUndefined(),
                      @"Unsetting fillTranslation should return fill-translate to the default value.");
        XCTAssertEqualObjects(layer.fillTranslation, defaultStyleValue,
                              @"fillTranslation should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillTranslation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillTranslation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // fill-translate-anchor
    {
        XCTAssertTrue(rawLayer->getFillTranslateAnchor().isUndefined(),
                      @"fill-translate-anchor should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.fillTranslationAnchor;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLFillTranslationAnchor:MGLFillTranslationAnchorViewport]];
        layer.fillTranslationAnchor = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType> propertyValue = { mbgl::style::TranslateAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getFillTranslateAnchor(), propertyValue,
                       @"Setting fillTranslationAnchor to a constant value should update fill-translate-anchor.");
        XCTAssertEqualObjects(layer.fillTranslationAnchor, constantStyleValue,
                              @"fillTranslationAnchor should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.fillTranslationAnchor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::TranslateAnchorType> intervalStops = { {{18, mbgl::style::TranslateAnchorType::Viewport}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::TranslateAnchorType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getFillTranslateAnchor(), propertyValue,
                       @"Setting fillTranslationAnchor to a camera function should update fill-translate-anchor.");
        XCTAssertEqualObjects(layer.fillTranslationAnchor, functionStyleValue,
                              @"fillTranslationAnchor should round-trip camera functions.");

                              

        layer.fillTranslationAnchor = nil;
        XCTAssertTrue(rawLayer->getFillTranslateAnchor().isUndefined(),
                      @"Unsetting fillTranslationAnchor should return fill-translate-anchor to the default value.");
        XCTAssertEqualObjects(layer.fillTranslationAnchor, defaultStyleValue,
                              @"fillTranslationAnchor should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillTranslationAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillTranslationAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }
}

- (void)testPropertyNames {
    [self testPropertyName:@"is-fill-antialiased" isBoolean:YES];
    [self testPropertyName:@"fill-color" isBoolean:NO];
    [self testPropertyName:@"fill-opacity" isBoolean:NO];
    [self testPropertyName:@"fill-outline-color" isBoolean:NO];
    [self testPropertyName:@"fill-pattern" isBoolean:NO];
    [self testPropertyName:@"fill-translation" isBoolean:NO];
    [self testPropertyName:@"fill-translation-anchor" isBoolean:NO];
}

- (void)testValueAdditions {
    XCTAssertEqual([NSValue valueWithMGLFillTranslationAnchor:MGLFillTranslationAnchorMap].MGLFillTranslationAnchorValue, MGLFillTranslationAnchorMap);
    XCTAssertEqual([NSValue valueWithMGLFillTranslationAnchor:MGLFillTranslationAnchorViewport].MGLFillTranslationAnchorValue, MGLFillTranslationAnchorViewport);
}

@end
