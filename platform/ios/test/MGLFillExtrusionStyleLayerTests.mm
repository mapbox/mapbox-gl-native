// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLStyleLayerTests.h"
#import "../../darwin/src/NSDate+MGLAdditions.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/fill-extrusion_layer.hpp>
#include <mbgl/style/transition_options.hpp>

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
    layer.predicate = [NSPredicate predicateWithValue:NO];
    XCTAssertEqualObjects(layer.predicate, [NSPredicate predicateWithValue:NO]);
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
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.fillExtrusionBase;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.fillExtrusionBase = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getFillExtrusionBase(), propertyValue,
                       @"Setting fillExtrusionBase to a constant value should update fill-extrusion-base.");
        XCTAssertEqualObjects(layer.fillExtrusionBase, constantStyleValue,
                              @"fillExtrusionBase should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.fillExtrusionBase = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getFillExtrusionBase(), propertyValue,
                       @"Setting fillExtrusionBase to a camera function should update fill-extrusion-base.");
        XCTAssertEqualObjects(layer.fillExtrusionBase, functionStyleValue,
                              @"fillExtrusionBase should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.fillExtrusionBase = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getFillExtrusionBase(), propertyValue,
                       @"Setting fillExtrusionBase to a source function should update fill-extrusion-base.");
        XCTAssertEqualObjects(layer.fillExtrusionBase, functionStyleValue,
                              @"fillExtrusionBase should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.fillExtrusionBase = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getFillExtrusionBase(), propertyValue,
                       @"Setting fillExtrusionBase to a composite function should update fill-extrusion-base.");
        XCTAssertEqualObjects(layer.fillExtrusionBase, functionStyleValue,
                              @"fillExtrusionBase should round-trip composite functions.");                                                                                                          
                              

        layer.fillExtrusionBase = nil;
        XCTAssertTrue(rawLayer->getFillExtrusionBase().isUndefined(),
                      @"Unsetting fillExtrusionBase should return fill-extrusion-base to the default value.");
        XCTAssertEqualObjects(layer.fillExtrusionBase, defaultStyleValue,
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
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.fillExtrusionColor;

        MGLStyleValue<MGLColor *> *constantStyleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.fillExtrusionColor = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getFillExtrusionColor(), propertyValue,
                       @"Setting fillExtrusionColor to a constant value should update fill-extrusion-color.");
        XCTAssertEqualObjects(layer.fillExtrusionColor, constantStyleValue,
                              @"fillExtrusionColor should round-trip constant values.");

        MGLStyleValue<MGLColor *> * functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.fillExtrusionColor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = { {{18, { 1, 0, 0, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };
        
        XCTAssertEqual(rawLayer->getFillExtrusionColor(), propertyValue,
                       @"Setting fillExtrusionColor to a camera function should update fill-extrusion-color.");
        XCTAssertEqualObjects(layer.fillExtrusionColor, functionStyleValue,
                              @"fillExtrusionColor should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.fillExtrusionColor = functionStyleValue;

        mbgl::style::ExponentialStops<mbgl::Color> exponentialStops = { {{18, { 1, 0, 0, 1 }}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<mbgl::Color> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getFillExtrusionColor(), propertyValue,
                       @"Setting fillExtrusionColor to a source function should update fill-extrusion-color.");
        XCTAssertEqualObjects(layer.fillExtrusionColor, functionStyleValue,
                              @"fillExtrusionColor should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.fillExtrusionColor = functionStyleValue;

        std::map<float, mbgl::Color> innerStops { {18, { 1, 0, 0, 1 }} };
        mbgl::style::CompositeExponentialStops<mbgl::Color> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<mbgl::Color> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getFillExtrusionColor(), propertyValue,
                       @"Setting fillExtrusionColor to a composite function should update fill-extrusion-color.");
        XCTAssertEqualObjects(layer.fillExtrusionColor, functionStyleValue,
                              @"fillExtrusionColor should round-trip composite functions.");                                                                                                          
                              

        layer.fillExtrusionColor = nil;
        XCTAssertTrue(rawLayer->getFillExtrusionColor().isUndefined(),
                      @"Unsetting fillExtrusionColor should return fill-extrusion-color to the default value.");
        XCTAssertEqualObjects(layer.fillExtrusionColor, defaultStyleValue,
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
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.fillExtrusionHeight;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.fillExtrusionHeight = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getFillExtrusionHeight(), propertyValue,
                       @"Setting fillExtrusionHeight to a constant value should update fill-extrusion-height.");
        XCTAssertEqualObjects(layer.fillExtrusionHeight, constantStyleValue,
                              @"fillExtrusionHeight should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.fillExtrusionHeight = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getFillExtrusionHeight(), propertyValue,
                       @"Setting fillExtrusionHeight to a camera function should update fill-extrusion-height.");
        XCTAssertEqualObjects(layer.fillExtrusionHeight, functionStyleValue,
                              @"fillExtrusionHeight should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.fillExtrusionHeight = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getFillExtrusionHeight(), propertyValue,
                       @"Setting fillExtrusionHeight to a source function should update fill-extrusion-height.");
        XCTAssertEqualObjects(layer.fillExtrusionHeight, functionStyleValue,
                              @"fillExtrusionHeight should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.fillExtrusionHeight = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getFillExtrusionHeight(), propertyValue,
                       @"Setting fillExtrusionHeight to a composite function should update fill-extrusion-height.");
        XCTAssertEqualObjects(layer.fillExtrusionHeight, functionStyleValue,
                              @"fillExtrusionHeight should round-trip composite functions.");                                                                                                          
                              

        layer.fillExtrusionHeight = nil;
        XCTAssertTrue(rawLayer->getFillExtrusionHeight().isUndefined(),
                      @"Unsetting fillExtrusionHeight should return fill-extrusion-height to the default value.");
        XCTAssertEqualObjects(layer.fillExtrusionHeight, defaultStyleValue,
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
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.fillExtrusionOpacity;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.fillExtrusionOpacity = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getFillExtrusionOpacity(), propertyValue,
                       @"Setting fillExtrusionOpacity to a constant value should update fill-extrusion-opacity.");
        XCTAssertEqualObjects(layer.fillExtrusionOpacity, constantStyleValue,
                              @"fillExtrusionOpacity should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.fillExtrusionOpacity = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getFillExtrusionOpacity(), propertyValue,
                       @"Setting fillExtrusionOpacity to a camera function should update fill-extrusion-opacity.");
        XCTAssertEqualObjects(layer.fillExtrusionOpacity, functionStyleValue,
                              @"fillExtrusionOpacity should round-trip camera functions.");

                              

        layer.fillExtrusionOpacity = nil;
        XCTAssertTrue(rawLayer->getFillExtrusionOpacity().isUndefined(),
                      @"Unsetting fillExtrusionOpacity should return fill-extrusion-opacity to the default value.");
        XCTAssertEqualObjects(layer.fillExtrusionOpacity, defaultStyleValue,
                              @"fillExtrusionOpacity should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionOpacity = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionOpacity = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
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
        MGLStyleValue<NSString *> *defaultStyleValue = layer.fillExtrusionPattern;

        MGLStyleValue<NSString *> *constantStyleValue = [MGLStyleValue<NSString *> valueWithRawValue:@"Fill Extrusion Pattern"];
        layer.fillExtrusionPattern = constantStyleValue;
        mbgl::style::PropertyValue<std::string> propertyValue = { "Fill Extrusion Pattern" };
        XCTAssertEqual(rawLayer->getFillExtrusionPattern(), propertyValue,
                       @"Setting fillExtrusionPattern to a constant value should update fill-extrusion-pattern.");
        XCTAssertEqualObjects(layer.fillExtrusionPattern, constantStyleValue,
                              @"fillExtrusionPattern should round-trip constant values.");

        MGLStyleValue<NSString *> * functionStyleValue = [MGLStyleValue<NSString *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.fillExtrusionPattern = functionStyleValue;

        mbgl::style::IntervalStops<std::string> intervalStops = { {{18, "Fill Extrusion Pattern"}} };
        propertyValue = mbgl::style::CameraFunction<std::string> { intervalStops };
        
        XCTAssertEqual(rawLayer->getFillExtrusionPattern(), propertyValue,
                       @"Setting fillExtrusionPattern to a camera function should update fill-extrusion-pattern.");
        XCTAssertEqualObjects(layer.fillExtrusionPattern, functionStyleValue,
                              @"fillExtrusionPattern should round-trip camera functions.");

                              

        layer.fillExtrusionPattern = nil;
        XCTAssertTrue(rawLayer->getFillExtrusionPattern().isUndefined(),
                      @"Unsetting fillExtrusionPattern should return fill-extrusion-pattern to the default value.");
        XCTAssertEqualObjects(layer.fillExtrusionPattern, defaultStyleValue,
                              @"fillExtrusionPattern should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSString *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionPattern = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSString *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionPattern = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
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
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.fillExtrusionTranslation;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.fillExtrusionTranslation = constantStyleValue;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getFillExtrusionTranslate(), propertyValue,
                       @"Setting fillExtrusionTranslation to a constant value should update fill-extrusion-translate.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslation, constantStyleValue,
                              @"fillExtrusionTranslation should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.fillExtrusionTranslation = functionStyleValue;

        mbgl::style::IntervalStops<std::array<float, 2>> intervalStops = { {{18, { 1, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<std::array<float, 2>> { intervalStops };
        
        XCTAssertEqual(rawLayer->getFillExtrusionTranslate(), propertyValue,
                       @"Setting fillExtrusionTranslation to a camera function should update fill-extrusion-translate.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslation, functionStyleValue,
                              @"fillExtrusionTranslation should round-trip camera functions.");

                              

        layer.fillExtrusionTranslation = nil;
        XCTAssertTrue(rawLayer->getFillExtrusionTranslate().isUndefined(),
                      @"Unsetting fillExtrusionTranslation should return fill-extrusion-translate to the default value.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslation, defaultStyleValue,
                              @"fillExtrusionTranslation should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionTranslation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionTranslation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // fill-extrusion-translate-anchor
    {
        XCTAssertTrue(rawLayer->getFillExtrusionTranslateAnchor().isUndefined(),
                      @"fill-extrusion-translate-anchor should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.fillExtrusionTranslationAnchor;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLFillExtrusionTranslationAnchor:MGLFillExtrusionTranslationAnchorViewport]];
        layer.fillExtrusionTranslationAnchor = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType> propertyValue = { mbgl::style::TranslateAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getFillExtrusionTranslateAnchor(), propertyValue,
                       @"Setting fillExtrusionTranslationAnchor to a constant value should update fill-extrusion-translate-anchor.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslationAnchor, constantStyleValue,
                              @"fillExtrusionTranslationAnchor should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.fillExtrusionTranslationAnchor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::TranslateAnchorType> intervalStops = { {{18, mbgl::style::TranslateAnchorType::Viewport}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::TranslateAnchorType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getFillExtrusionTranslateAnchor(), propertyValue,
                       @"Setting fillExtrusionTranslationAnchor to a camera function should update fill-extrusion-translate-anchor.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslationAnchor, functionStyleValue,
                              @"fillExtrusionTranslationAnchor should round-trip camera functions.");

                              

        layer.fillExtrusionTranslationAnchor = nil;
        XCTAssertTrue(rawLayer->getFillExtrusionTranslateAnchor().isUndefined(),
                      @"Unsetting fillExtrusionTranslationAnchor should return fill-extrusion-translate-anchor to the default value.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslationAnchor, defaultStyleValue,
                              @"fillExtrusionTranslationAnchor should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionTranslationAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionTranslationAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
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
