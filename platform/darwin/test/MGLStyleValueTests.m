#import <XCTest/XCTest.h>
#import <Mapbox/Mapbox.h>

@interface MGLStyleValueTests : XCTestCase
@end

@implementation MGLStyleValueTests

- (void)testStoplessFunction {
    XCTAssertThrowsSpecificNamed([MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential cameraStops:@{} options:nil], NSException, NSInvalidArgumentException, @"Stopless function should raise an exception");
}

- (void)testDeprecatedFunctions {
    MGLShapeSource *shapeSource = [[MGLShapeSource alloc] initWithIdentifier:@"test"
                                                                       shape:nil
                                                                     options:nil];
    MGLSymbolStyleLayer *symbolStyleLayer = [[MGLSymbolStyleLayer alloc] initWithIdentifier:@"symbolLayer"
                                                                                     source:shapeSource];
    MGLCircleStyleLayer *circleStyleLayer = [[MGLCircleStyleLayer alloc] initWithIdentifier:@"circleLayer"
                                                                                     source:shapeSource];
    
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    // deprecated function, stops with float values
    NSDictionary<NSNumber *, MGLStyleValue<NSNumber *> *> *stops = @{
        @1: [MGLStyleValue<NSNumber *> valueWithRawValue:@0],
        @2: [MGLStyleValue<NSNumber *> valueWithRawValue:@1],
        @3: [MGLStyleValue<NSNumber *> valueWithRawValue:@2],
        @4: [MGLStyleValue<NSNumber *> valueWithRawValue:@0],
    };
    MGLStyleValue<NSNumber *> *iconHaloBlurStyleValue =
        [MGLStyleValue<NSNumber *> valueWithInterpolationBase:1.0 stops:stops];
    symbolStyleLayer.iconHaloBlur = iconHaloBlurStyleValue;
    XCTAssertEqualObjects(symbolStyleLayer.iconHaloBlur, iconHaloBlurStyleValue);
    
    // deprecated function, stops with boolean values
    stops = @{
        @1: [MGLStyleValue<NSNumber *> valueWithRawValue:@YES],
        @2: [MGLStyleValue<NSNumber *> valueWithRawValue:@NO],
        @3: [MGLStyleValue<NSNumber *> valueWithRawValue:@YES],
        @4: [MGLStyleValue<NSNumber *> valueWithRawValue:@NO],
    };
    MGLStyleValue<NSNumber *> *iconAllowsOverlapStyleValue =
        [MGLStyleValue<NSNumber *> valueWithInterpolationBase:1.0 stops:stops];
    symbolStyleLayer.iconAllowsOverlap = iconAllowsOverlapStyleValue;
    // iconAllowsOverlap is boolean so mgl and mbgl conversions will coerce the developers stops into interval stops
    MGLStyleValue<NSNumber *> *expectedIconAllowsOverlapStyleValue =
        [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval
                                                  cameraStops:stops
                                                      options:nil];
    XCTAssertEqualObjects(symbolStyleLayer.iconAllowsOverlap, expectedIconAllowsOverlapStyleValue);
    
    ///
    // creating and using MGLStyleFunctions directly
    ///
    
    NSDictionary<NSNumber *, MGLStyleValue<NSNumber *> *> *circleRadiusStops = @{
        @0: [MGLStyleValue<NSNumber *> valueWithRawValue:@10],
        @20: [MGLStyleValue<NSNumber *> valueWithRawValue:@5],
    };
    MGLStyleFunction<NSNumber *> *circleRadiusFunction =
        [MGLStyleFunction<NSNumber *> functionWithInterpolationBase:1.0
                                                              stops:circleRadiusStops];
    circleStyleLayer.circleRadius = circleRadiusFunction;
    MGLStyleValue<NSNumber *> *expectedCircleRadiusFunction =
        [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential
                                                  cameraStops:circleRadiusStops
                                                      options:nil];
    // setting a data driven property to an MGLStyleFunction should return an exponential camera function
    XCTAssertEqualObjects(circleStyleLayer.circleRadius, expectedCircleRadiusFunction);
    
    CGVector circleTranslationOne = CGVectorMake(100, 0);
    CGVector circleTranslationTwo = CGVectorMake(0, 0);
#if TARGET_OS_IPHONE
    NSValue *circleTranslationValueOne = [NSValue valueWithCGVector:circleTranslationOne];
    NSValue *circleTranslationValueTwo = [NSValue valueWithCGVector:circleTranslationTwo];
#else
    NSValue *circleTranslationValueOne = [NSValue value:&circleTranslationOne withObjCType:@encode(CGVector)];
    NSValue *circleTranslationValueTwo = [NSValue value:&circleTranslationTwo withObjCType:@encode(CGVector)];
#endif
    
    NSDictionary<NSNumber *, MGLStyleValue<NSValue *> *> *circleTranslationStops = @{
        @0: [MGLStyleValue<NSValue *> valueWithRawValue:circleTranslationValueOne],
        @10: [MGLStyleValue<NSValue *> valueWithRawValue:circleTranslationValueTwo],
    };
    MGLStyleFunction<NSValue *> *circleTranslationFunction =
        [MGLStyleFunction<NSValue *> functionWithInterpolationBase:1.0
                                                             stops:circleTranslationStops];
    circleStyleLayer.circleTranslation = circleTranslationFunction;
    MGLStyleValue<NSValue *> *expectedCircleTranslationFunction =
        [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeExponential
                                                 cameraStops:circleTranslationStops
                                                     options:nil];
    // setting a non-data driven, interpolatable property to an MGLStyleFunction should return an exponential camera function
    XCTAssertEqualObjects(circleStyleLayer.circleTranslation, expectedCircleTranslationFunction);
    
    NSDictionary<NSNumber *, MGLStyleValue<NSNumber *> *> *iconOptionalStops = @{
        @0: [MGLStyleValue<NSNumber *> valueWithRawValue:@NO],
        @20: [MGLStyleValue<NSNumber *> valueWithRawValue:@YES],
    };
    MGLStyleFunction<NSNumber *> *iconOptionalFunction =
        [MGLStyleFunction<NSNumber *> valueWithInterpolationBase:1.0
                                                           stops:iconOptionalStops];
    symbolStyleLayer.iconOptional = iconOptionalFunction;
    MGLStyleValue<NSNumber *> *expectedIconOptionalFunction =
        [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval
                                                  cameraStops:iconOptionalStops
                                                      options:nil];
    XCTAssertEqualObjects(symbolStyleLayer.iconOptional, expectedIconOptionalFunction);
#pragma clang diagnostic pop
}

@end
