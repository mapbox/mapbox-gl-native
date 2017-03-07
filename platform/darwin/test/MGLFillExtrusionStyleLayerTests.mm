// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLStyleLayerTests.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/fill-extrusion_layer.hpp>

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
    }

    // fill-extrusion-translate
    {
        XCTAssertTrue(rawLayer->getFillExtrusionTranslate().isUndefined(),
                      @"fill-extrusion-translate should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.fillExtrusionTranslate;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.fillExtrusionTranslate = constantStyleValue;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getFillExtrusionTranslate(), propertyValue,
                       @"Setting fillExtrusionTranslate to a constant value should update fill-extrusion-translate.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslate, constantStyleValue,
                              @"fillExtrusionTranslate should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.fillExtrusionTranslate = functionStyleValue;

        mbgl::style::IntervalStops<std::array<float, 2>> intervalStops = { {{18, { 1, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<std::array<float, 2>> { intervalStops };
        
        XCTAssertEqual(rawLayer->getFillExtrusionTranslate(), propertyValue,
                       @"Setting fillExtrusionTranslate to a camera function should update fill-extrusion-translate.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslate, functionStyleValue,
                              @"fillExtrusionTranslate should round-trip camera functions.");

                              

        layer.fillExtrusionTranslate = nil;
        XCTAssertTrue(rawLayer->getFillExtrusionTranslate().isUndefined(),
                      @"Unsetting fillExtrusionTranslate should return fill-extrusion-translate to the default value.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslate, defaultStyleValue,
                              @"fillExtrusionTranslate should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionTranslate = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionTranslate = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // fill-extrusion-translate-anchor
    {
        XCTAssertTrue(rawLayer->getFillExtrusionTranslateAnchor().isUndefined(),
                      @"fill-extrusion-translate-anchor should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.fillExtrusionTranslateAnchor;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLFillExtrusionTranslateAnchor:MGLFillExtrusionTranslateAnchorViewport]];
        layer.fillExtrusionTranslateAnchor = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType> propertyValue = { mbgl::style::TranslateAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getFillExtrusionTranslateAnchor(), propertyValue,
                       @"Setting fillExtrusionTranslateAnchor to a constant value should update fill-extrusion-translate-anchor.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslateAnchor, constantStyleValue,
                              @"fillExtrusionTranslateAnchor should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.fillExtrusionTranslateAnchor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::TranslateAnchorType> intervalStops = { {{18, mbgl::style::TranslateAnchorType::Viewport}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::TranslateAnchorType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getFillExtrusionTranslateAnchor(), propertyValue,
                       @"Setting fillExtrusionTranslateAnchor to a camera function should update fill-extrusion-translate-anchor.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslateAnchor, functionStyleValue,
                              @"fillExtrusionTranslateAnchor should round-trip camera functions.");

                              

        layer.fillExtrusionTranslateAnchor = nil;
        XCTAssertTrue(rawLayer->getFillExtrusionTranslateAnchor().isUndefined(),
                      @"Unsetting fillExtrusionTranslateAnchor should return fill-extrusion-translate-anchor to the default value.");
        XCTAssertEqualObjects(layer.fillExtrusionTranslateAnchor, defaultStyleValue,
                              @"fillExtrusionTranslateAnchor should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionTranslateAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.fillExtrusionTranslateAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }
}

- (void)testPropertyNames {
    [self testPropertyName:@"fill-extrusion-base" isBoolean:NO];
    [self testPropertyName:@"fill-extrusion-color" isBoolean:NO];
    [self testPropertyName:@"fill-extrusion-height" isBoolean:NO];
    [self testPropertyName:@"fill-extrusion-opacity" isBoolean:NO];
    [self testPropertyName:@"fill-extrusion-pattern" isBoolean:NO];
    [self testPropertyName:@"fill-extrusion-translate" isBoolean:NO];
    [self testPropertyName:@"fill-extrusion-translate-anchor" isBoolean:NO];
}

- (void)testValueAdditions {
    XCTAssertEqual([NSValue valueWithMGLFillExtrusionTranslateAnchor:MGLFillExtrusionTranslateAnchorMap].MGLFillExtrusionTranslateAnchorValue, MGLFillExtrusionTranslateAnchorMap);
    XCTAssertEqual([NSValue valueWithMGLFillExtrusionTranslateAnchor:MGLFillExtrusionTranslateAnchorViewport].MGLFillExtrusionTranslateAnchorValue, MGLFillExtrusionTranslateAnchorViewport);
}

@end
