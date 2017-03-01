// This file is generated.
// Edit platform/darwin/scripts/generate-style-code.js, then run `make darwin-style-code`.

#import "MGLStyleLayerTests.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/symbol_layer.hpp>

@interface MGLSymbolLayerTests : MGLStyleLayerTests
@end

@implementation MGLSymbolLayerTests

+ (NSString *)layerType {
    return @"symbol";
}

- (void)testPredicates {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];
    MGLSymbolStyleLayer *layer = [[MGLSymbolStyleLayer alloc] initWithIdentifier:@"layerID" source:source];

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

    MGLSymbolStyleLayer *layer = [[MGLSymbolStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    XCTAssertNotEqual(layer.rawLayer, nullptr);
    XCTAssertTrue(layer.rawLayer->is<mbgl::style::SymbolLayer>());
    auto rawLayer = layer.rawLayer->as<mbgl::style::SymbolLayer>();

    // icon-allow-overlap
    {
        XCTAssertTrue(rawLayer->getIconAllowOverlap().isUndefined(),
                      @"icon-allow-overlap should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.iconAllowsOverlap;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@YES];
        layer.iconAllowsOverlap = constantStyleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getIconAllowOverlap(), propertyValue,
                       @"Setting iconAllowsOverlap to a constant value should update icon-allow-overlap.");
        XCTAssertEqualObjects(layer.iconAllowsOverlap, constantStyleValue,
                              @"iconAllowsOverlap should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconAllowsOverlap = functionStyleValue;

        mbgl::style::IntervalStops<bool> intervalStops = { {{18, true}} };
        propertyValue = mbgl::style::CameraFunction<bool> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconAllowOverlap(), propertyValue,
                       @"Setting iconAllowsOverlap to a camera function should update icon-allow-overlap.");
        XCTAssertEqualObjects(layer.iconAllowsOverlap, functionStyleValue,
                              @"iconAllowsOverlap should round-trip camera functions.");

                              

        layer.iconAllowsOverlap = nil;
        XCTAssertTrue(rawLayer->getIconAllowOverlap().isUndefined(),
                      @"Unsetting iconAllowsOverlap should return icon-allow-overlap to the default value.");
        XCTAssertEqualObjects(layer.iconAllowsOverlap, defaultStyleValue,
                              @"iconAllowsOverlap should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconAllowsOverlap = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconAllowsOverlap = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // icon-ignore-placement
    {
        XCTAssertTrue(rawLayer->getIconIgnorePlacement().isUndefined(),
                      @"icon-ignore-placement should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.iconIgnoresPlacement;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@YES];
        layer.iconIgnoresPlacement = constantStyleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getIconIgnorePlacement(), propertyValue,
                       @"Setting iconIgnoresPlacement to a constant value should update icon-ignore-placement.");
        XCTAssertEqualObjects(layer.iconIgnoresPlacement, constantStyleValue,
                              @"iconIgnoresPlacement should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconIgnoresPlacement = functionStyleValue;

        mbgl::style::IntervalStops<bool> intervalStops = { {{18, true}} };
        propertyValue = mbgl::style::CameraFunction<bool> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconIgnorePlacement(), propertyValue,
                       @"Setting iconIgnoresPlacement to a camera function should update icon-ignore-placement.");
        XCTAssertEqualObjects(layer.iconIgnoresPlacement, functionStyleValue,
                              @"iconIgnoresPlacement should round-trip camera functions.");

                              

        layer.iconIgnoresPlacement = nil;
        XCTAssertTrue(rawLayer->getIconIgnorePlacement().isUndefined(),
                      @"Unsetting iconIgnoresPlacement should return icon-ignore-placement to the default value.");
        XCTAssertEqualObjects(layer.iconIgnoresPlacement, defaultStyleValue,
                              @"iconIgnoresPlacement should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconIgnoresPlacement = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconIgnoresPlacement = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // icon-image
    {
        XCTAssertTrue(rawLayer->getIconImage().isUndefined(),
                      @"icon-image should be unset initially.");
        MGLStyleValue<NSString *> *defaultStyleValue = layer.iconImageName;

        MGLStyleValue<NSString *> *constantStyleValue = [MGLStyleValue<NSString *> valueWithRawValue:@"Icon Image"];
        layer.iconImageName = constantStyleValue;
        mbgl::style::PropertyValue<std::string> propertyValue = { "Icon Image" };
        XCTAssertEqual(rawLayer->getIconImage(), propertyValue,
                       @"Setting iconImageName to a constant value should update icon-image.");
        XCTAssertEqualObjects(layer.iconImageName, constantStyleValue,
                              @"iconImageName should round-trip constant values.");

        MGLStyleValue<NSString *> * functionStyleValue = [MGLStyleValue<NSString *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconImageName = functionStyleValue;

        mbgl::style::IntervalStops<std::string> intervalStops = { {{18, "Icon Image"}} };
        propertyValue = mbgl::style::CameraFunction<std::string> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconImage(), propertyValue,
                       @"Setting iconImageName to a camera function should update icon-image.");
        XCTAssertEqualObjects(layer.iconImageName, functionStyleValue,
                              @"iconImageName should round-trip camera functions.");

                              

        layer.iconImageName = nil;
        XCTAssertTrue(rawLayer->getIconImage().isUndefined(),
                      @"Unsetting iconImageName should return icon-image to the default value.");
        XCTAssertEqualObjects(layer.iconImageName, defaultStyleValue,
                              @"iconImageName should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSString *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconImageName = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSString *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconImageName = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // icon-offset
    {
        XCTAssertTrue(rawLayer->getIconOffset().isUndefined(),
                      @"icon-offset should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.iconOffset;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.iconOffset = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getIconOffset(), propertyValue,
                       @"Setting iconOffset to a constant value should update icon-offset.");
        XCTAssertEqualObjects(layer.iconOffset, constantStyleValue,
                              @"iconOffset should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconOffset = functionStyleValue;

        mbgl::style::IntervalStops<std::array<float, 2>> intervalStops = { {{18, { 1, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<std::array<float, 2>> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconOffset(), propertyValue,
                       @"Setting iconOffset to a camera function should update icon-offset.");
        XCTAssertEqualObjects(layer.iconOffset, functionStyleValue,
                              @"iconOffset should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.iconOffset = functionStyleValue;

        mbgl::style::ExponentialStops<std::array<float, 2>> exponentialStops = { {{18, { 1, 1 }}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<std::array<float, 2>> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getIconOffset(), propertyValue,
                       @"Setting iconOffset to a source function should update icon-offset.");
        XCTAssertEqualObjects(layer.iconOffset, functionStyleValue,
                              @"iconOffset should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.iconOffset = functionStyleValue;

        std::map<float, std::array<float, 2>> innerStops { {18, { 1, 1 }} };
        mbgl::style::CompositeExponentialStops<std::array<float, 2>> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<std::array<float, 2>> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getIconOffset(), propertyValue,
                       @"Setting iconOffset to a composite function should update icon-offset.");
        XCTAssertEqualObjects(layer.iconOffset, functionStyleValue,
                              @"iconOffset should round-trip composite functions.");                                                                                                          
                              

        layer.iconOffset = nil;
        XCTAssertTrue(rawLayer->getIconOffset().isUndefined(),
                      @"Unsetting iconOffset should return icon-offset to the default value.");
        XCTAssertEqualObjects(layer.iconOffset, defaultStyleValue,
                              @"iconOffset should return the default value after being unset.");
    }

    // icon-optional
    {
        XCTAssertTrue(rawLayer->getIconOptional().isUndefined(),
                      @"icon-optional should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.iconOptional;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@YES];
        layer.iconOptional = constantStyleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getIconOptional(), propertyValue,
                       @"Setting iconOptional to a constant value should update icon-optional.");
        XCTAssertEqualObjects(layer.iconOptional, constantStyleValue,
                              @"iconOptional should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconOptional = functionStyleValue;

        mbgl::style::IntervalStops<bool> intervalStops = { {{18, true}} };
        propertyValue = mbgl::style::CameraFunction<bool> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconOptional(), propertyValue,
                       @"Setting iconOptional to a camera function should update icon-optional.");
        XCTAssertEqualObjects(layer.iconOptional, functionStyleValue,
                              @"iconOptional should round-trip camera functions.");

                              

        layer.iconOptional = nil;
        XCTAssertTrue(rawLayer->getIconOptional().isUndefined(),
                      @"Unsetting iconOptional should return icon-optional to the default value.");
        XCTAssertEqualObjects(layer.iconOptional, defaultStyleValue,
                              @"iconOptional should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconOptional = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconOptional = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // icon-padding
    {
        XCTAssertTrue(rawLayer->getIconPadding().isUndefined(),
                      @"icon-padding should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.iconPadding;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.iconPadding = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getIconPadding(), propertyValue,
                       @"Setting iconPadding to a constant value should update icon-padding.");
        XCTAssertEqualObjects(layer.iconPadding, constantStyleValue,
                              @"iconPadding should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconPadding = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconPadding(), propertyValue,
                       @"Setting iconPadding to a camera function should update icon-padding.");
        XCTAssertEqualObjects(layer.iconPadding, functionStyleValue,
                              @"iconPadding should round-trip camera functions.");

                              

        layer.iconPadding = nil;
        XCTAssertTrue(rawLayer->getIconPadding().isUndefined(),
                      @"Unsetting iconPadding should return icon-padding to the default value.");
        XCTAssertEqualObjects(layer.iconPadding, defaultStyleValue,
                              @"iconPadding should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconPadding = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconPadding = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // icon-rotate
    {
        XCTAssertTrue(rawLayer->getIconRotate().isUndefined(),
                      @"icon-rotate should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.iconRotation;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.iconRotation = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getIconRotate(), propertyValue,
                       @"Setting iconRotation to a constant value should update icon-rotate.");
        XCTAssertEqualObjects(layer.iconRotation, constantStyleValue,
                              @"iconRotation should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconRotation = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconRotate(), propertyValue,
                       @"Setting iconRotation to a camera function should update icon-rotate.");
        XCTAssertEqualObjects(layer.iconRotation, functionStyleValue,
                              @"iconRotation should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.iconRotation = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getIconRotate(), propertyValue,
                       @"Setting iconRotation to a source function should update icon-rotate.");
        XCTAssertEqualObjects(layer.iconRotation, functionStyleValue,
                              @"iconRotation should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.iconRotation = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getIconRotate(), propertyValue,
                       @"Setting iconRotation to a composite function should update icon-rotate.");
        XCTAssertEqualObjects(layer.iconRotation, functionStyleValue,
                              @"iconRotation should round-trip composite functions.");                                                                                                          
                              

        layer.iconRotation = nil;
        XCTAssertTrue(rawLayer->getIconRotate().isUndefined(),
                      @"Unsetting iconRotation should return icon-rotate to the default value.");
        XCTAssertEqualObjects(layer.iconRotation, defaultStyleValue,
                              @"iconRotation should return the default value after being unset.");
    }

    // icon-rotation-alignment
    {
        XCTAssertTrue(rawLayer->getIconRotationAlignment().isUndefined(),
                      @"icon-rotation-alignment should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.iconRotationAlignment;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLIconRotationAlignment:MGLIconRotationAlignmentAuto]];
        layer.iconRotationAlignment = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::AlignmentType> propertyValue = { mbgl::style::AlignmentType::Auto };
        XCTAssertEqual(rawLayer->getIconRotationAlignment(), propertyValue,
                       @"Setting iconRotationAlignment to a constant value should update icon-rotation-alignment.");
        XCTAssertEqualObjects(layer.iconRotationAlignment, constantStyleValue,
                              @"iconRotationAlignment should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconRotationAlignment = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::AlignmentType> intervalStops = { {{18, mbgl::style::AlignmentType::Auto}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::AlignmentType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconRotationAlignment(), propertyValue,
                       @"Setting iconRotationAlignment to a camera function should update icon-rotation-alignment.");
        XCTAssertEqualObjects(layer.iconRotationAlignment, functionStyleValue,
                              @"iconRotationAlignment should round-trip camera functions.");

                              

        layer.iconRotationAlignment = nil;
        XCTAssertTrue(rawLayer->getIconRotationAlignment().isUndefined(),
                      @"Unsetting iconRotationAlignment should return icon-rotation-alignment to the default value.");
        XCTAssertEqualObjects(layer.iconRotationAlignment, defaultStyleValue,
                              @"iconRotationAlignment should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconRotationAlignment = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconRotationAlignment = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // icon-size
    {
        XCTAssertTrue(rawLayer->getIconSize().isUndefined(),
                      @"icon-size should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.iconScale;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.iconScale = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getIconSize(), propertyValue,
                       @"Setting iconScale to a constant value should update icon-size.");
        XCTAssertEqualObjects(layer.iconScale, constantStyleValue,
                              @"iconScale should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconScale = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconSize(), propertyValue,
                       @"Setting iconScale to a camera function should update icon-size.");
        XCTAssertEqualObjects(layer.iconScale, functionStyleValue,
                              @"iconScale should round-trip camera functions.");

                              

        layer.iconScale = nil;
        XCTAssertTrue(rawLayer->getIconSize().isUndefined(),
                      @"Unsetting iconScale should return icon-size to the default value.");
        XCTAssertEqualObjects(layer.iconScale, defaultStyleValue,
                              @"iconScale should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconScale = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconScale = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // icon-text-fit
    {
        XCTAssertTrue(rawLayer->getIconTextFit().isUndefined(),
                      @"icon-text-fit should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.iconTextFit;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLIconTextFit:MGLIconTextFitBoth]];
        layer.iconTextFit = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::IconTextFitType> propertyValue = { mbgl::style::IconTextFitType::Both };
        XCTAssertEqual(rawLayer->getIconTextFit(), propertyValue,
                       @"Setting iconTextFit to a constant value should update icon-text-fit.");
        XCTAssertEqualObjects(layer.iconTextFit, constantStyleValue,
                              @"iconTextFit should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconTextFit = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::IconTextFitType> intervalStops = { {{18, mbgl::style::IconTextFitType::Both}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::IconTextFitType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconTextFit(), propertyValue,
                       @"Setting iconTextFit to a camera function should update icon-text-fit.");
        XCTAssertEqualObjects(layer.iconTextFit, functionStyleValue,
                              @"iconTextFit should round-trip camera functions.");

                              

        layer.iconTextFit = nil;
        XCTAssertTrue(rawLayer->getIconTextFit().isUndefined(),
                      @"Unsetting iconTextFit should return icon-text-fit to the default value.");
        XCTAssertEqualObjects(layer.iconTextFit, defaultStyleValue,
                              @"iconTextFit should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconTextFit = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconTextFit = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // icon-text-fit-padding
    {
        XCTAssertTrue(rawLayer->getIconTextFitPadding().isUndefined(),
                      @"icon-text-fit-padding should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.iconTextFitPadding;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithUIEdgeInsets:UIEdgeInsetsMake(1, 1, 1, 1)]
#else
            [NSValue valueWithEdgeInsets:NSEdgeInsetsMake(1, 1, 1, 1)]
#endif
        ];
        layer.iconTextFitPadding = constantStyleValue;
        mbgl::style::PropertyValue<std::array<float, 4>> propertyValue = { { 1, 1, 1, 1 } };
        XCTAssertEqual(rawLayer->getIconTextFitPadding(), propertyValue,
                       @"Setting iconTextFitPadding to a constant value should update icon-text-fit-padding.");
        XCTAssertEqualObjects(layer.iconTextFitPadding, constantStyleValue,
                              @"iconTextFitPadding should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconTextFitPadding = functionStyleValue;

        mbgl::style::IntervalStops<std::array<float, 4>> intervalStops = { {{18, { 1, 1, 1, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<std::array<float, 4>> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconTextFitPadding(), propertyValue,
                       @"Setting iconTextFitPadding to a camera function should update icon-text-fit-padding.");
        XCTAssertEqualObjects(layer.iconTextFitPadding, functionStyleValue,
                              @"iconTextFitPadding should round-trip camera functions.");

                              

        layer.iconTextFitPadding = nil;
        XCTAssertTrue(rawLayer->getIconTextFitPadding().isUndefined(),
                      @"Unsetting iconTextFitPadding should return icon-text-fit-padding to the default value.");
        XCTAssertEqualObjects(layer.iconTextFitPadding, defaultStyleValue,
                              @"iconTextFitPadding should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconTextFitPadding = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconTextFitPadding = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // icon-keep-upright
    {
        XCTAssertTrue(rawLayer->getIconKeepUpright().isUndefined(),
                      @"icon-keep-upright should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.keepsIconUpright;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@YES];
        layer.keepsIconUpright = constantStyleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getIconKeepUpright(), propertyValue,
                       @"Setting keepsIconUpright to a constant value should update icon-keep-upright.");
        XCTAssertEqualObjects(layer.keepsIconUpright, constantStyleValue,
                              @"keepsIconUpright should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.keepsIconUpright = functionStyleValue;

        mbgl::style::IntervalStops<bool> intervalStops = { {{18, true}} };
        propertyValue = mbgl::style::CameraFunction<bool> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconKeepUpright(), propertyValue,
                       @"Setting keepsIconUpright to a camera function should update icon-keep-upright.");
        XCTAssertEqualObjects(layer.keepsIconUpright, functionStyleValue,
                              @"keepsIconUpright should round-trip camera functions.");

                              

        layer.keepsIconUpright = nil;
        XCTAssertTrue(rawLayer->getIconKeepUpright().isUndefined(),
                      @"Unsetting keepsIconUpright should return icon-keep-upright to the default value.");
        XCTAssertEqualObjects(layer.keepsIconUpright, defaultStyleValue,
                              @"keepsIconUpright should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.keepsIconUpright = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.keepsIconUpright = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-keep-upright
    {
        XCTAssertTrue(rawLayer->getTextKeepUpright().isUndefined(),
                      @"text-keep-upright should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.keepsTextUpright;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@NO];
        layer.keepsTextUpright = constantStyleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { false };
        XCTAssertEqual(rawLayer->getTextKeepUpright(), propertyValue,
                       @"Setting keepsTextUpright to a constant value should update text-keep-upright.");
        XCTAssertEqualObjects(layer.keepsTextUpright, constantStyleValue,
                              @"keepsTextUpright should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.keepsTextUpright = functionStyleValue;

        mbgl::style::IntervalStops<bool> intervalStops = { {{18, false}} };
        propertyValue = mbgl::style::CameraFunction<bool> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextKeepUpright(), propertyValue,
                       @"Setting keepsTextUpright to a camera function should update text-keep-upright.");
        XCTAssertEqualObjects(layer.keepsTextUpright, functionStyleValue,
                              @"keepsTextUpright should round-trip camera functions.");

                              

        layer.keepsTextUpright = nil;
        XCTAssertTrue(rawLayer->getTextKeepUpright().isUndefined(),
                      @"Unsetting keepsTextUpright should return text-keep-upright to the default value.");
        XCTAssertEqualObjects(layer.keepsTextUpright, defaultStyleValue,
                              @"keepsTextUpright should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.keepsTextUpright = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.keepsTextUpright = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-max-angle
    {
        XCTAssertTrue(rawLayer->getTextMaxAngle().isUndefined(),
                      @"text-max-angle should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.maximumTextAngle;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.maximumTextAngle = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextMaxAngle(), propertyValue,
                       @"Setting maximumTextAngle to a constant value should update text-max-angle.");
        XCTAssertEqualObjects(layer.maximumTextAngle, constantStyleValue,
                              @"maximumTextAngle should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.maximumTextAngle = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextMaxAngle(), propertyValue,
                       @"Setting maximumTextAngle to a camera function should update text-max-angle.");
        XCTAssertEqualObjects(layer.maximumTextAngle, functionStyleValue,
                              @"maximumTextAngle should round-trip camera functions.");

                              

        layer.maximumTextAngle = nil;
        XCTAssertTrue(rawLayer->getTextMaxAngle().isUndefined(),
                      @"Unsetting maximumTextAngle should return text-max-angle to the default value.");
        XCTAssertEqualObjects(layer.maximumTextAngle, defaultStyleValue,
                              @"maximumTextAngle should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.maximumTextAngle = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.maximumTextAngle = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-max-width
    {
        XCTAssertTrue(rawLayer->getTextMaxWidth().isUndefined(),
                      @"text-max-width should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.maximumTextWidth;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.maximumTextWidth = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextMaxWidth(), propertyValue,
                       @"Setting maximumTextWidth to a constant value should update text-max-width.");
        XCTAssertEqualObjects(layer.maximumTextWidth, constantStyleValue,
                              @"maximumTextWidth should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.maximumTextWidth = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextMaxWidth(), propertyValue,
                       @"Setting maximumTextWidth to a camera function should update text-max-width.");
        XCTAssertEqualObjects(layer.maximumTextWidth, functionStyleValue,
                              @"maximumTextWidth should round-trip camera functions.");

                              

        layer.maximumTextWidth = nil;
        XCTAssertTrue(rawLayer->getTextMaxWidth().isUndefined(),
                      @"Unsetting maximumTextWidth should return text-max-width to the default value.");
        XCTAssertEqualObjects(layer.maximumTextWidth, defaultStyleValue,
                              @"maximumTextWidth should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.maximumTextWidth = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.maximumTextWidth = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // symbol-avoid-edges
    {
        XCTAssertTrue(rawLayer->getSymbolAvoidEdges().isUndefined(),
                      @"symbol-avoid-edges should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.symbolAvoidsEdges;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@YES];
        layer.symbolAvoidsEdges = constantStyleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getSymbolAvoidEdges(), propertyValue,
                       @"Setting symbolAvoidsEdges to a constant value should update symbol-avoid-edges.");
        XCTAssertEqualObjects(layer.symbolAvoidsEdges, constantStyleValue,
                              @"symbolAvoidsEdges should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.symbolAvoidsEdges = functionStyleValue;

        mbgl::style::IntervalStops<bool> intervalStops = { {{18, true}} };
        propertyValue = mbgl::style::CameraFunction<bool> { intervalStops };
        
        XCTAssertEqual(rawLayer->getSymbolAvoidEdges(), propertyValue,
                       @"Setting symbolAvoidsEdges to a camera function should update symbol-avoid-edges.");
        XCTAssertEqualObjects(layer.symbolAvoidsEdges, functionStyleValue,
                              @"symbolAvoidsEdges should round-trip camera functions.");

                              

        layer.symbolAvoidsEdges = nil;
        XCTAssertTrue(rawLayer->getSymbolAvoidEdges().isUndefined(),
                      @"Unsetting symbolAvoidsEdges should return symbol-avoid-edges to the default value.");
        XCTAssertEqualObjects(layer.symbolAvoidsEdges, defaultStyleValue,
                              @"symbolAvoidsEdges should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.symbolAvoidsEdges = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.symbolAvoidsEdges = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // symbol-placement
    {
        XCTAssertTrue(rawLayer->getSymbolPlacement().isUndefined(),
                      @"symbol-placement should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.symbolPlacement;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLSymbolPlacement:MGLSymbolPlacementLine]];
        layer.symbolPlacement = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::SymbolPlacementType> propertyValue = { mbgl::style::SymbolPlacementType::Line };
        XCTAssertEqual(rawLayer->getSymbolPlacement(), propertyValue,
                       @"Setting symbolPlacement to a constant value should update symbol-placement.");
        XCTAssertEqualObjects(layer.symbolPlacement, constantStyleValue,
                              @"symbolPlacement should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.symbolPlacement = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::SymbolPlacementType> intervalStops = { {{18, mbgl::style::SymbolPlacementType::Line}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::SymbolPlacementType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getSymbolPlacement(), propertyValue,
                       @"Setting symbolPlacement to a camera function should update symbol-placement.");
        XCTAssertEqualObjects(layer.symbolPlacement, functionStyleValue,
                              @"symbolPlacement should round-trip camera functions.");

                              

        layer.symbolPlacement = nil;
        XCTAssertTrue(rawLayer->getSymbolPlacement().isUndefined(),
                      @"Unsetting symbolPlacement should return symbol-placement to the default value.");
        XCTAssertEqualObjects(layer.symbolPlacement, defaultStyleValue,
                              @"symbolPlacement should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.symbolPlacement = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.symbolPlacement = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // symbol-spacing
    {
        XCTAssertTrue(rawLayer->getSymbolSpacing().isUndefined(),
                      @"symbol-spacing should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.symbolSpacing;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.symbolSpacing = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getSymbolSpacing(), propertyValue,
                       @"Setting symbolSpacing to a constant value should update symbol-spacing.");
        XCTAssertEqualObjects(layer.symbolSpacing, constantStyleValue,
                              @"symbolSpacing should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.symbolSpacing = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getSymbolSpacing(), propertyValue,
                       @"Setting symbolSpacing to a camera function should update symbol-spacing.");
        XCTAssertEqualObjects(layer.symbolSpacing, functionStyleValue,
                              @"symbolSpacing should round-trip camera functions.");

                              

        layer.symbolSpacing = nil;
        XCTAssertTrue(rawLayer->getSymbolSpacing().isUndefined(),
                      @"Unsetting symbolSpacing should return symbol-spacing to the default value.");
        XCTAssertEqualObjects(layer.symbolSpacing, defaultStyleValue,
                              @"symbolSpacing should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.symbolSpacing = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.symbolSpacing = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-field
    {
        XCTAssertTrue(rawLayer->getTextField().isUndefined(),
                      @"text-field should be unset initially.");
        MGLStyleValue<NSString *> *defaultStyleValue = layer.text;

        MGLStyleValue<NSString *> *constantStyleValue = [MGLStyleValue<NSString *> valueWithRawValue:@"Text Field"];
        layer.text = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<std::string> propertyValue = { "Text Field" };
        XCTAssertEqual(rawLayer->getTextField(), propertyValue,
                       @"Setting text to a constant value should update text-field.");
        XCTAssertEqualObjects(layer.text, constantStyleValue,
                              @"text should round-trip constant values.");

        MGLStyleValue<NSString *> * functionStyleValue = [MGLStyleValue<NSString *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.text = functionStyleValue;

        mbgl::style::IntervalStops<std::string> intervalStops = { {{18, "Text Field"}} };
        propertyValue = mbgl::style::CameraFunction<std::string> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextField(), propertyValue,
                       @"Setting text to a camera function should update text-field.");
        XCTAssertEqualObjects(layer.text, functionStyleValue,
                              @"text should round-trip camera functions.");

                              

        layer.text = nil;
        XCTAssertTrue(rawLayer->getTextField().isUndefined(),
                      @"Unsetting text should return text-field to the default value.");
        XCTAssertEqualObjects(layer.text, defaultStyleValue,
                              @"text should return the default value after being unset.");
    }

    // text-allow-overlap
    {
        XCTAssertTrue(rawLayer->getTextAllowOverlap().isUndefined(),
                      @"text-allow-overlap should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textAllowsOverlap;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@YES];
        layer.textAllowsOverlap = constantStyleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getTextAllowOverlap(), propertyValue,
                       @"Setting textAllowsOverlap to a constant value should update text-allow-overlap.");
        XCTAssertEqualObjects(layer.textAllowsOverlap, constantStyleValue,
                              @"textAllowsOverlap should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textAllowsOverlap = functionStyleValue;

        mbgl::style::IntervalStops<bool> intervalStops = { {{18, true}} };
        propertyValue = mbgl::style::CameraFunction<bool> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextAllowOverlap(), propertyValue,
                       @"Setting textAllowsOverlap to a camera function should update text-allow-overlap.");
        XCTAssertEqualObjects(layer.textAllowsOverlap, functionStyleValue,
                              @"textAllowsOverlap should round-trip camera functions.");

                              

        layer.textAllowsOverlap = nil;
        XCTAssertTrue(rawLayer->getTextAllowOverlap().isUndefined(),
                      @"Unsetting textAllowsOverlap should return text-allow-overlap to the default value.");
        XCTAssertEqualObjects(layer.textAllowsOverlap, defaultStyleValue,
                              @"textAllowsOverlap should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textAllowsOverlap = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textAllowsOverlap = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-anchor
    {
        XCTAssertTrue(rawLayer->getTextAnchor().isUndefined(),
                      @"text-anchor should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.textAnchor;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLTextAnchor:MGLTextAnchorBottomRight]];
        layer.textAnchor = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::TextAnchorType> propertyValue = { mbgl::style::TextAnchorType::BottomRight };
        XCTAssertEqual(rawLayer->getTextAnchor(), propertyValue,
                       @"Setting textAnchor to a constant value should update text-anchor.");
        XCTAssertEqualObjects(layer.textAnchor, constantStyleValue,
                              @"textAnchor should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textAnchor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::TextAnchorType> intervalStops = { {{18, mbgl::style::TextAnchorType::BottomRight}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::TextAnchorType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextAnchor(), propertyValue,
                       @"Setting textAnchor to a camera function should update text-anchor.");
        XCTAssertEqualObjects(layer.textAnchor, functionStyleValue,
                              @"textAnchor should round-trip camera functions.");

                              

        layer.textAnchor = nil;
        XCTAssertTrue(rawLayer->getTextAnchor().isUndefined(),
                      @"Unsetting textAnchor should return text-anchor to the default value.");
        XCTAssertEqualObjects(layer.textAnchor, defaultStyleValue,
                              @"textAnchor should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-font
    {
        XCTAssertTrue(rawLayer->getTextFont().isUndefined(),
                      @"text-font should be unset initially.");
        MGLStyleValue<NSArray<NSString *> *> *defaultStyleValue = layer.textFontNames;

        MGLStyleValue<NSArray<NSString *> *> *constantStyleValue = [MGLStyleValue<NSArray<NSString *> *> valueWithRawValue:@[@"Text Font", @"Tnof Txet"]];
        layer.textFontNames = constantStyleValue;
        mbgl::style::PropertyValue<std::vector<std::string>> propertyValue = { { "Text Font", "Tnof Txet" } };
        XCTAssertEqual(rawLayer->getTextFont(), propertyValue,
                       @"Setting textFontNames to a constant value should update text-font.");
        XCTAssertEqualObjects(layer.textFontNames, constantStyleValue,
                              @"textFontNames should round-trip constant values.");

        MGLStyleValue<NSArray<NSString *> *> * functionStyleValue = [MGLStyleValue<NSArray<NSString *> *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textFontNames = functionStyleValue;

        mbgl::style::IntervalStops<std::vector<std::string>> intervalStops = { {{18, { "Text Font", "Tnof Txet" }}} };
        propertyValue = mbgl::style::CameraFunction<std::vector<std::string>> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextFont(), propertyValue,
                       @"Setting textFontNames to a camera function should update text-font.");
        XCTAssertEqualObjects(layer.textFontNames, functionStyleValue,
                              @"textFontNames should round-trip camera functions.");

                              

        layer.textFontNames = nil;
        XCTAssertTrue(rawLayer->getTextFont().isUndefined(),
                      @"Unsetting textFontNames should return text-font to the default value.");
        XCTAssertEqualObjects(layer.textFontNames, defaultStyleValue,
                              @"textFontNames should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSArray<NSString *> *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textFontNames = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSArray<NSString *> *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textFontNames = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-size
    {
        XCTAssertTrue(rawLayer->getTextSize().isUndefined(),
                      @"text-size should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textFontSize;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.textFontSize = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextSize(), propertyValue,
                       @"Setting textFontSize to a constant value should update text-size.");
        XCTAssertEqualObjects(layer.textFontSize, constantStyleValue,
                              @"textFontSize should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textFontSize = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextSize(), propertyValue,
                       @"Setting textFontSize to a camera function should update text-size.");
        XCTAssertEqualObjects(layer.textFontSize, functionStyleValue,
                              @"textFontSize should round-trip camera functions.");

                              

        layer.textFontSize = nil;
        XCTAssertTrue(rawLayer->getTextSize().isUndefined(),
                      @"Unsetting textFontSize should return text-size to the default value.");
        XCTAssertEqualObjects(layer.textFontSize, defaultStyleValue,
                              @"textFontSize should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textFontSize = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textFontSize = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-ignore-placement
    {
        XCTAssertTrue(rawLayer->getTextIgnorePlacement().isUndefined(),
                      @"text-ignore-placement should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textIgnoresPlacement;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@YES];
        layer.textIgnoresPlacement = constantStyleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getTextIgnorePlacement(), propertyValue,
                       @"Setting textIgnoresPlacement to a constant value should update text-ignore-placement.");
        XCTAssertEqualObjects(layer.textIgnoresPlacement, constantStyleValue,
                              @"textIgnoresPlacement should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textIgnoresPlacement = functionStyleValue;

        mbgl::style::IntervalStops<bool> intervalStops = { {{18, true}} };
        propertyValue = mbgl::style::CameraFunction<bool> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextIgnorePlacement(), propertyValue,
                       @"Setting textIgnoresPlacement to a camera function should update text-ignore-placement.");
        XCTAssertEqualObjects(layer.textIgnoresPlacement, functionStyleValue,
                              @"textIgnoresPlacement should round-trip camera functions.");

                              

        layer.textIgnoresPlacement = nil;
        XCTAssertTrue(rawLayer->getTextIgnorePlacement().isUndefined(),
                      @"Unsetting textIgnoresPlacement should return text-ignore-placement to the default value.");
        XCTAssertEqualObjects(layer.textIgnoresPlacement, defaultStyleValue,
                              @"textIgnoresPlacement should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textIgnoresPlacement = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textIgnoresPlacement = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-justify
    {
        XCTAssertTrue(rawLayer->getTextJustify().isUndefined(),
                      @"text-justify should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.textJustification;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLTextJustification:MGLTextJustificationRight]];
        layer.textJustification = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::TextJustifyType> propertyValue = { mbgl::style::TextJustifyType::Right };
        XCTAssertEqual(rawLayer->getTextJustify(), propertyValue,
                       @"Setting textJustification to a constant value should update text-justify.");
        XCTAssertEqualObjects(layer.textJustification, constantStyleValue,
                              @"textJustification should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textJustification = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::TextJustifyType> intervalStops = { {{18, mbgl::style::TextJustifyType::Right}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::TextJustifyType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextJustify(), propertyValue,
                       @"Setting textJustification to a camera function should update text-justify.");
        XCTAssertEqualObjects(layer.textJustification, functionStyleValue,
                              @"textJustification should round-trip camera functions.");

                              

        layer.textJustification = nil;
        XCTAssertTrue(rawLayer->getTextJustify().isUndefined(),
                      @"Unsetting textJustification should return text-justify to the default value.");
        XCTAssertEqualObjects(layer.textJustification, defaultStyleValue,
                              @"textJustification should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textJustification = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textJustification = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-letter-spacing
    {
        XCTAssertTrue(rawLayer->getTextLetterSpacing().isUndefined(),
                      @"text-letter-spacing should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textLetterSpacing;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.textLetterSpacing = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextLetterSpacing(), propertyValue,
                       @"Setting textLetterSpacing to a constant value should update text-letter-spacing.");
        XCTAssertEqualObjects(layer.textLetterSpacing, constantStyleValue,
                              @"textLetterSpacing should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textLetterSpacing = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextLetterSpacing(), propertyValue,
                       @"Setting textLetterSpacing to a camera function should update text-letter-spacing.");
        XCTAssertEqualObjects(layer.textLetterSpacing, functionStyleValue,
                              @"textLetterSpacing should round-trip camera functions.");

                              

        layer.textLetterSpacing = nil;
        XCTAssertTrue(rawLayer->getTextLetterSpacing().isUndefined(),
                      @"Unsetting textLetterSpacing should return text-letter-spacing to the default value.");
        XCTAssertEqualObjects(layer.textLetterSpacing, defaultStyleValue,
                              @"textLetterSpacing should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textLetterSpacing = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textLetterSpacing = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-line-height
    {
        XCTAssertTrue(rawLayer->getTextLineHeight().isUndefined(),
                      @"text-line-height should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textLineHeight;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.textLineHeight = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextLineHeight(), propertyValue,
                       @"Setting textLineHeight to a constant value should update text-line-height.");
        XCTAssertEqualObjects(layer.textLineHeight, constantStyleValue,
                              @"textLineHeight should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textLineHeight = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextLineHeight(), propertyValue,
                       @"Setting textLineHeight to a camera function should update text-line-height.");
        XCTAssertEqualObjects(layer.textLineHeight, functionStyleValue,
                              @"textLineHeight should round-trip camera functions.");

                              

        layer.textLineHeight = nil;
        XCTAssertTrue(rawLayer->getTextLineHeight().isUndefined(),
                      @"Unsetting textLineHeight should return text-line-height to the default value.");
        XCTAssertEqualObjects(layer.textLineHeight, defaultStyleValue,
                              @"textLineHeight should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textLineHeight = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textLineHeight = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-offset
    {
        XCTAssertTrue(rawLayer->getTextOffset().isUndefined(),
                      @"text-offset should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.textOffset;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.textOffset = constantStyleValue;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getTextOffset(), propertyValue,
                       @"Setting textOffset to a constant value should update text-offset.");
        XCTAssertEqualObjects(layer.textOffset, constantStyleValue,
                              @"textOffset should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textOffset = functionStyleValue;

        mbgl::style::IntervalStops<std::array<float, 2>> intervalStops = { {{18, { 1, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<std::array<float, 2>> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextOffset(), propertyValue,
                       @"Setting textOffset to a camera function should update text-offset.");
        XCTAssertEqualObjects(layer.textOffset, functionStyleValue,
                              @"textOffset should round-trip camera functions.");

                              

        layer.textOffset = nil;
        XCTAssertTrue(rawLayer->getTextOffset().isUndefined(),
                      @"Unsetting textOffset should return text-offset to the default value.");
        XCTAssertEqualObjects(layer.textOffset, defaultStyleValue,
                              @"textOffset should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textOffset = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textOffset = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-optional
    {
        XCTAssertTrue(rawLayer->getTextOptional().isUndefined(),
                      @"text-optional should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textOptional;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@YES];
        layer.textOptional = constantStyleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getTextOptional(), propertyValue,
                       @"Setting textOptional to a constant value should update text-optional.");
        XCTAssertEqualObjects(layer.textOptional, constantStyleValue,
                              @"textOptional should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textOptional = functionStyleValue;

        mbgl::style::IntervalStops<bool> intervalStops = { {{18, true}} };
        propertyValue = mbgl::style::CameraFunction<bool> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextOptional(), propertyValue,
                       @"Setting textOptional to a camera function should update text-optional.");
        XCTAssertEqualObjects(layer.textOptional, functionStyleValue,
                              @"textOptional should round-trip camera functions.");

                              

        layer.textOptional = nil;
        XCTAssertTrue(rawLayer->getTextOptional().isUndefined(),
                      @"Unsetting textOptional should return text-optional to the default value.");
        XCTAssertEqualObjects(layer.textOptional, defaultStyleValue,
                              @"textOptional should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textOptional = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textOptional = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-padding
    {
        XCTAssertTrue(rawLayer->getTextPadding().isUndefined(),
                      @"text-padding should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textPadding;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.textPadding = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextPadding(), propertyValue,
                       @"Setting textPadding to a constant value should update text-padding.");
        XCTAssertEqualObjects(layer.textPadding, constantStyleValue,
                              @"textPadding should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textPadding = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextPadding(), propertyValue,
                       @"Setting textPadding to a camera function should update text-padding.");
        XCTAssertEqualObjects(layer.textPadding, functionStyleValue,
                              @"textPadding should round-trip camera functions.");

                              

        layer.textPadding = nil;
        XCTAssertTrue(rawLayer->getTextPadding().isUndefined(),
                      @"Unsetting textPadding should return text-padding to the default value.");
        XCTAssertEqualObjects(layer.textPadding, defaultStyleValue,
                              @"textPadding should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textPadding = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textPadding = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-pitch-alignment
    {
        XCTAssertTrue(rawLayer->getTextPitchAlignment().isUndefined(),
                      @"text-pitch-alignment should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.textPitchAlignment;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLTextPitchAlignment:MGLTextPitchAlignmentAuto]];
        layer.textPitchAlignment = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::AlignmentType> propertyValue = { mbgl::style::AlignmentType::Auto };
        XCTAssertEqual(rawLayer->getTextPitchAlignment(), propertyValue,
                       @"Setting textPitchAlignment to a constant value should update text-pitch-alignment.");
        XCTAssertEqualObjects(layer.textPitchAlignment, constantStyleValue,
                              @"textPitchAlignment should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textPitchAlignment = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::AlignmentType> intervalStops = { {{18, mbgl::style::AlignmentType::Auto}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::AlignmentType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextPitchAlignment(), propertyValue,
                       @"Setting textPitchAlignment to a camera function should update text-pitch-alignment.");
        XCTAssertEqualObjects(layer.textPitchAlignment, functionStyleValue,
                              @"textPitchAlignment should round-trip camera functions.");

                              

        layer.textPitchAlignment = nil;
        XCTAssertTrue(rawLayer->getTextPitchAlignment().isUndefined(),
                      @"Unsetting textPitchAlignment should return text-pitch-alignment to the default value.");
        XCTAssertEqualObjects(layer.textPitchAlignment, defaultStyleValue,
                              @"textPitchAlignment should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textPitchAlignment = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textPitchAlignment = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-rotate
    {
        XCTAssertTrue(rawLayer->getTextRotate().isUndefined(),
                      @"text-rotate should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textRotation;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.textRotation = constantStyleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextRotate(), propertyValue,
                       @"Setting textRotation to a constant value should update text-rotate.");
        XCTAssertEqualObjects(layer.textRotation, constantStyleValue,
                              @"textRotation should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textRotation = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextRotate(), propertyValue,
                       @"Setting textRotation to a camera function should update text-rotate.");
        XCTAssertEqualObjects(layer.textRotation, functionStyleValue,
                              @"textRotation should round-trip camera functions.");

                              

        layer.textRotation = nil;
        XCTAssertTrue(rawLayer->getTextRotate().isUndefined(),
                      @"Unsetting textRotation should return text-rotate to the default value.");
        XCTAssertEqualObjects(layer.textRotation, defaultStyleValue,
                              @"textRotation should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textRotation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textRotation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-rotation-alignment
    {
        XCTAssertTrue(rawLayer->getTextRotationAlignment().isUndefined(),
                      @"text-rotation-alignment should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.textRotationAlignment;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLTextRotationAlignment:MGLTextRotationAlignmentAuto]];
        layer.textRotationAlignment = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::AlignmentType> propertyValue = { mbgl::style::AlignmentType::Auto };
        XCTAssertEqual(rawLayer->getTextRotationAlignment(), propertyValue,
                       @"Setting textRotationAlignment to a constant value should update text-rotation-alignment.");
        XCTAssertEqualObjects(layer.textRotationAlignment, constantStyleValue,
                              @"textRotationAlignment should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textRotationAlignment = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::AlignmentType> intervalStops = { {{18, mbgl::style::AlignmentType::Auto}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::AlignmentType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextRotationAlignment(), propertyValue,
                       @"Setting textRotationAlignment to a camera function should update text-rotation-alignment.");
        XCTAssertEqualObjects(layer.textRotationAlignment, functionStyleValue,
                              @"textRotationAlignment should round-trip camera functions.");

                              

        layer.textRotationAlignment = nil;
        XCTAssertTrue(rawLayer->getTextRotationAlignment().isUndefined(),
                      @"Unsetting textRotationAlignment should return text-rotation-alignment to the default value.");
        XCTAssertEqualObjects(layer.textRotationAlignment, defaultStyleValue,
                              @"textRotationAlignment should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textRotationAlignment = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textRotationAlignment = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-transform
    {
        XCTAssertTrue(rawLayer->getTextTransform().isUndefined(),
                      @"text-transform should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.textTransform;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLTextTransform:MGLTextTransformLowercase]];
        layer.textTransform = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<mbgl::style::TextTransformType> propertyValue = { mbgl::style::TextTransformType::Lowercase };
        XCTAssertEqual(rawLayer->getTextTransform(), propertyValue,
                       @"Setting textTransform to a constant value should update text-transform.");
        XCTAssertEqualObjects(layer.textTransform, constantStyleValue,
                              @"textTransform should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textTransform = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::TextTransformType> intervalStops = { {{18, mbgl::style::TextTransformType::Lowercase}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::TextTransformType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextTransform(), propertyValue,
                       @"Setting textTransform to a camera function should update text-transform.");
        XCTAssertEqualObjects(layer.textTransform, functionStyleValue,
                              @"textTransform should round-trip camera functions.");

                              

        layer.textTransform = nil;
        XCTAssertTrue(rawLayer->getTextTransform().isUndefined(),
                      @"Unsetting textTransform should return text-transform to the default value.");
        XCTAssertEqualObjects(layer.textTransform, defaultStyleValue,
                              @"textTransform should return the default value after being unset.");
    }

    // icon-color
    {
        XCTAssertTrue(rawLayer->getIconColor().isUndefined(),
                      @"icon-color should be unset initially.");
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.iconColor;

        MGLStyleValue<MGLColor *> *constantStyleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.iconColor = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getIconColor(), propertyValue,
                       @"Setting iconColor to a constant value should update icon-color.");
        XCTAssertEqualObjects(layer.iconColor, constantStyleValue,
                              @"iconColor should round-trip constant values.");

        MGLStyleValue<MGLColor *> * functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconColor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = { {{18, { 1, 0, 0, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconColor(), propertyValue,
                       @"Setting iconColor to a camera function should update icon-color.");
        XCTAssertEqualObjects(layer.iconColor, functionStyleValue,
                              @"iconColor should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.iconColor = functionStyleValue;

        mbgl::style::ExponentialStops<mbgl::Color> exponentialStops = { {{18, { 1, 0, 0, 1 }}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<mbgl::Color> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getIconColor(), propertyValue,
                       @"Setting iconColor to a source function should update icon-color.");
        XCTAssertEqualObjects(layer.iconColor, functionStyleValue,
                              @"iconColor should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.iconColor = functionStyleValue;

        std::map<float, mbgl::Color> innerStops { {18, { 1, 0, 0, 1 }} };
        mbgl::style::CompositeExponentialStops<mbgl::Color> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<mbgl::Color> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getIconColor(), propertyValue,
                       @"Setting iconColor to a composite function should update icon-color.");
        XCTAssertEqualObjects(layer.iconColor, functionStyleValue,
                              @"iconColor should round-trip composite functions.");                                                                                                          
                              

        layer.iconColor = nil;
        XCTAssertTrue(rawLayer->getIconColor().isUndefined(),
                      @"Unsetting iconColor should return icon-color to the default value.");
        XCTAssertEqualObjects(layer.iconColor, defaultStyleValue,
                              @"iconColor should return the default value after being unset.");
    }

    // icon-halo-blur
    {
        XCTAssertTrue(rawLayer->getIconHaloBlur().isUndefined(),
                      @"icon-halo-blur should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.iconHaloBlur;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.iconHaloBlur = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getIconHaloBlur(), propertyValue,
                       @"Setting iconHaloBlur to a constant value should update icon-halo-blur.");
        XCTAssertEqualObjects(layer.iconHaloBlur, constantStyleValue,
                              @"iconHaloBlur should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconHaloBlur = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconHaloBlur(), propertyValue,
                       @"Setting iconHaloBlur to a camera function should update icon-halo-blur.");
        XCTAssertEqualObjects(layer.iconHaloBlur, functionStyleValue,
                              @"iconHaloBlur should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.iconHaloBlur = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getIconHaloBlur(), propertyValue,
                       @"Setting iconHaloBlur to a source function should update icon-halo-blur.");
        XCTAssertEqualObjects(layer.iconHaloBlur, functionStyleValue,
                              @"iconHaloBlur should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.iconHaloBlur = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getIconHaloBlur(), propertyValue,
                       @"Setting iconHaloBlur to a composite function should update icon-halo-blur.");
        XCTAssertEqualObjects(layer.iconHaloBlur, functionStyleValue,
                              @"iconHaloBlur should round-trip composite functions.");                                                                                                          
                              

        layer.iconHaloBlur = nil;
        XCTAssertTrue(rawLayer->getIconHaloBlur().isUndefined(),
                      @"Unsetting iconHaloBlur should return icon-halo-blur to the default value.");
        XCTAssertEqualObjects(layer.iconHaloBlur, defaultStyleValue,
                              @"iconHaloBlur should return the default value after being unset.");
    }

    // icon-halo-color
    {
        XCTAssertTrue(rawLayer->getIconHaloColor().isUndefined(),
                      @"icon-halo-color should be unset initially.");
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.iconHaloColor;

        MGLStyleValue<MGLColor *> *constantStyleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.iconHaloColor = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getIconHaloColor(), propertyValue,
                       @"Setting iconHaloColor to a constant value should update icon-halo-color.");
        XCTAssertEqualObjects(layer.iconHaloColor, constantStyleValue,
                              @"iconHaloColor should round-trip constant values.");

        MGLStyleValue<MGLColor *> * functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconHaloColor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = { {{18, { 1, 0, 0, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconHaloColor(), propertyValue,
                       @"Setting iconHaloColor to a camera function should update icon-halo-color.");
        XCTAssertEqualObjects(layer.iconHaloColor, functionStyleValue,
                              @"iconHaloColor should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.iconHaloColor = functionStyleValue;

        mbgl::style::ExponentialStops<mbgl::Color> exponentialStops = { {{18, { 1, 0, 0, 1 }}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<mbgl::Color> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getIconHaloColor(), propertyValue,
                       @"Setting iconHaloColor to a source function should update icon-halo-color.");
        XCTAssertEqualObjects(layer.iconHaloColor, functionStyleValue,
                              @"iconHaloColor should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.iconHaloColor = functionStyleValue;

        std::map<float, mbgl::Color> innerStops { {18, { 1, 0, 0, 1 }} };
        mbgl::style::CompositeExponentialStops<mbgl::Color> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<mbgl::Color> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getIconHaloColor(), propertyValue,
                       @"Setting iconHaloColor to a composite function should update icon-halo-color.");
        XCTAssertEqualObjects(layer.iconHaloColor, functionStyleValue,
                              @"iconHaloColor should round-trip composite functions.");                                                                                                          
                              

        layer.iconHaloColor = nil;
        XCTAssertTrue(rawLayer->getIconHaloColor().isUndefined(),
                      @"Unsetting iconHaloColor should return icon-halo-color to the default value.");
        XCTAssertEqualObjects(layer.iconHaloColor, defaultStyleValue,
                              @"iconHaloColor should return the default value after being unset.");
    }

    // icon-halo-width
    {
        XCTAssertTrue(rawLayer->getIconHaloWidth().isUndefined(),
                      @"icon-halo-width should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.iconHaloWidth;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.iconHaloWidth = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getIconHaloWidth(), propertyValue,
                       @"Setting iconHaloWidth to a constant value should update icon-halo-width.");
        XCTAssertEqualObjects(layer.iconHaloWidth, constantStyleValue,
                              @"iconHaloWidth should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconHaloWidth = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconHaloWidth(), propertyValue,
                       @"Setting iconHaloWidth to a camera function should update icon-halo-width.");
        XCTAssertEqualObjects(layer.iconHaloWidth, functionStyleValue,
                              @"iconHaloWidth should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.iconHaloWidth = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getIconHaloWidth(), propertyValue,
                       @"Setting iconHaloWidth to a source function should update icon-halo-width.");
        XCTAssertEqualObjects(layer.iconHaloWidth, functionStyleValue,
                              @"iconHaloWidth should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.iconHaloWidth = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getIconHaloWidth(), propertyValue,
                       @"Setting iconHaloWidth to a composite function should update icon-halo-width.");
        XCTAssertEqualObjects(layer.iconHaloWidth, functionStyleValue,
                              @"iconHaloWidth should round-trip composite functions.");                                                                                                          
                              

        layer.iconHaloWidth = nil;
        XCTAssertTrue(rawLayer->getIconHaloWidth().isUndefined(),
                      @"Unsetting iconHaloWidth should return icon-halo-width to the default value.");
        XCTAssertEqualObjects(layer.iconHaloWidth, defaultStyleValue,
                              @"iconHaloWidth should return the default value after being unset.");
    }

    // icon-opacity
    {
        XCTAssertTrue(rawLayer->getIconOpacity().isUndefined(),
                      @"icon-opacity should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.iconOpacity;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.iconOpacity = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getIconOpacity(), propertyValue,
                       @"Setting iconOpacity to a constant value should update icon-opacity.");
        XCTAssertEqualObjects(layer.iconOpacity, constantStyleValue,
                              @"iconOpacity should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconOpacity = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconOpacity(), propertyValue,
                       @"Setting iconOpacity to a camera function should update icon-opacity.");
        XCTAssertEqualObjects(layer.iconOpacity, functionStyleValue,
                              @"iconOpacity should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.iconOpacity = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getIconOpacity(), propertyValue,
                       @"Setting iconOpacity to a source function should update icon-opacity.");
        XCTAssertEqualObjects(layer.iconOpacity, functionStyleValue,
                              @"iconOpacity should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.iconOpacity = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getIconOpacity(), propertyValue,
                       @"Setting iconOpacity to a composite function should update icon-opacity.");
        XCTAssertEqualObjects(layer.iconOpacity, functionStyleValue,
                              @"iconOpacity should round-trip composite functions.");                                                                                                          
                              

        layer.iconOpacity = nil;
        XCTAssertTrue(rawLayer->getIconOpacity().isUndefined(),
                      @"Unsetting iconOpacity should return icon-opacity to the default value.");
        XCTAssertEqualObjects(layer.iconOpacity, defaultStyleValue,
                              @"iconOpacity should return the default value after being unset.");
    }

    // icon-translate
    {
        XCTAssertTrue(rawLayer->getIconTranslate().isUndefined(),
                      @"icon-translate should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.iconTranslation;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.iconTranslation = constantStyleValue;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getIconTranslate(), propertyValue,
                       @"Setting iconTranslation to a constant value should update icon-translate.");
        XCTAssertEqualObjects(layer.iconTranslation, constantStyleValue,
                              @"iconTranslation should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconTranslation = functionStyleValue;

        mbgl::style::IntervalStops<std::array<float, 2>> intervalStops = { {{18, { 1, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<std::array<float, 2>> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconTranslate(), propertyValue,
                       @"Setting iconTranslation to a camera function should update icon-translate.");
        XCTAssertEqualObjects(layer.iconTranslation, functionStyleValue,
                              @"iconTranslation should round-trip camera functions.");

                              

        layer.iconTranslation = nil;
        XCTAssertTrue(rawLayer->getIconTranslate().isUndefined(),
                      @"Unsetting iconTranslation should return icon-translate to the default value.");
        XCTAssertEqualObjects(layer.iconTranslation, defaultStyleValue,
                              @"iconTranslation should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconTranslation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconTranslation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // icon-translate-anchor
    {
        XCTAssertTrue(rawLayer->getIconTranslateAnchor().isUndefined(),
                      @"icon-translate-anchor should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.iconTranslationAnchor;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLIconTranslationAnchor:MGLIconTranslationAnchorViewport]];
        layer.iconTranslationAnchor = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType> propertyValue = { mbgl::style::TranslateAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getIconTranslateAnchor(), propertyValue,
                       @"Setting iconTranslationAnchor to a constant value should update icon-translate-anchor.");
        XCTAssertEqualObjects(layer.iconTranslationAnchor, constantStyleValue,
                              @"iconTranslationAnchor should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.iconTranslationAnchor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::TranslateAnchorType> intervalStops = { {{18, mbgl::style::TranslateAnchorType::Viewport}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::TranslateAnchorType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getIconTranslateAnchor(), propertyValue,
                       @"Setting iconTranslationAnchor to a camera function should update icon-translate-anchor.");
        XCTAssertEqualObjects(layer.iconTranslationAnchor, functionStyleValue,
                              @"iconTranslationAnchor should round-trip camera functions.");

                              

        layer.iconTranslationAnchor = nil;
        XCTAssertTrue(rawLayer->getIconTranslateAnchor().isUndefined(),
                      @"Unsetting iconTranslationAnchor should return icon-translate-anchor to the default value.");
        XCTAssertEqualObjects(layer.iconTranslationAnchor, defaultStyleValue,
                              @"iconTranslationAnchor should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconTranslationAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.iconTranslationAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-color
    {
        XCTAssertTrue(rawLayer->getTextColor().isUndefined(),
                      @"text-color should be unset initially.");
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.textColor;

        MGLStyleValue<MGLColor *> *constantStyleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.textColor = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getTextColor(), propertyValue,
                       @"Setting textColor to a constant value should update text-color.");
        XCTAssertEqualObjects(layer.textColor, constantStyleValue,
                              @"textColor should round-trip constant values.");

        MGLStyleValue<MGLColor *> * functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textColor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = { {{18, { 1, 0, 0, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextColor(), propertyValue,
                       @"Setting textColor to a camera function should update text-color.");
        XCTAssertEqualObjects(layer.textColor, functionStyleValue,
                              @"textColor should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.textColor = functionStyleValue;

        mbgl::style::ExponentialStops<mbgl::Color> exponentialStops = { {{18, { 1, 0, 0, 1 }}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<mbgl::Color> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getTextColor(), propertyValue,
                       @"Setting textColor to a source function should update text-color.");
        XCTAssertEqualObjects(layer.textColor, functionStyleValue,
                              @"textColor should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.textColor = functionStyleValue;

        std::map<float, mbgl::Color> innerStops { {18, { 1, 0, 0, 1 }} };
        mbgl::style::CompositeExponentialStops<mbgl::Color> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<mbgl::Color> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getTextColor(), propertyValue,
                       @"Setting textColor to a composite function should update text-color.");
        XCTAssertEqualObjects(layer.textColor, functionStyleValue,
                              @"textColor should round-trip composite functions.");                                                                                                          
                              

        layer.textColor = nil;
        XCTAssertTrue(rawLayer->getTextColor().isUndefined(),
                      @"Unsetting textColor should return text-color to the default value.");
        XCTAssertEqualObjects(layer.textColor, defaultStyleValue,
                              @"textColor should return the default value after being unset.");
    }

    // text-halo-blur
    {
        XCTAssertTrue(rawLayer->getTextHaloBlur().isUndefined(),
                      @"text-halo-blur should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textHaloBlur;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.textHaloBlur = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextHaloBlur(), propertyValue,
                       @"Setting textHaloBlur to a constant value should update text-halo-blur.");
        XCTAssertEqualObjects(layer.textHaloBlur, constantStyleValue,
                              @"textHaloBlur should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textHaloBlur = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextHaloBlur(), propertyValue,
                       @"Setting textHaloBlur to a camera function should update text-halo-blur.");
        XCTAssertEqualObjects(layer.textHaloBlur, functionStyleValue,
                              @"textHaloBlur should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.textHaloBlur = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getTextHaloBlur(), propertyValue,
                       @"Setting textHaloBlur to a source function should update text-halo-blur.");
        XCTAssertEqualObjects(layer.textHaloBlur, functionStyleValue,
                              @"textHaloBlur should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.textHaloBlur = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getTextHaloBlur(), propertyValue,
                       @"Setting textHaloBlur to a composite function should update text-halo-blur.");
        XCTAssertEqualObjects(layer.textHaloBlur, functionStyleValue,
                              @"textHaloBlur should round-trip composite functions.");                                                                                                          
                              

        layer.textHaloBlur = nil;
        XCTAssertTrue(rawLayer->getTextHaloBlur().isUndefined(),
                      @"Unsetting textHaloBlur should return text-halo-blur to the default value.");
        XCTAssertEqualObjects(layer.textHaloBlur, defaultStyleValue,
                              @"textHaloBlur should return the default value after being unset.");
    }

    // text-halo-color
    {
        XCTAssertTrue(rawLayer->getTextHaloColor().isUndefined(),
                      @"text-halo-color should be unset initially.");
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.textHaloColor;

        MGLStyleValue<MGLColor *> *constantStyleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.textHaloColor = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getTextHaloColor(), propertyValue,
                       @"Setting textHaloColor to a constant value should update text-halo-color.");
        XCTAssertEqualObjects(layer.textHaloColor, constantStyleValue,
                              @"textHaloColor should round-trip constant values.");

        MGLStyleValue<MGLColor *> * functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textHaloColor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::Color> intervalStops = { {{18, { 1, 0, 0, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::Color> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextHaloColor(), propertyValue,
                       @"Setting textHaloColor to a camera function should update text-halo-color.");
        XCTAssertEqualObjects(layer.textHaloColor, functionStyleValue,
                              @"textHaloColor should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.textHaloColor = functionStyleValue;

        mbgl::style::ExponentialStops<mbgl::Color> exponentialStops = { {{18, { 1, 0, 0, 1 }}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<mbgl::Color> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getTextHaloColor(), propertyValue,
                       @"Setting textHaloColor to a source function should update text-halo-color.");
        XCTAssertEqualObjects(layer.textHaloColor, functionStyleValue,
                              @"textHaloColor should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<MGLColor *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.textHaloColor = functionStyleValue;

        std::map<float, mbgl::Color> innerStops { {18, { 1, 0, 0, 1 }} };
        mbgl::style::CompositeExponentialStops<mbgl::Color> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<mbgl::Color> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getTextHaloColor(), propertyValue,
                       @"Setting textHaloColor to a composite function should update text-halo-color.");
        XCTAssertEqualObjects(layer.textHaloColor, functionStyleValue,
                              @"textHaloColor should round-trip composite functions.");                                                                                                          
                              

        layer.textHaloColor = nil;
        XCTAssertTrue(rawLayer->getTextHaloColor().isUndefined(),
                      @"Unsetting textHaloColor should return text-halo-color to the default value.");
        XCTAssertEqualObjects(layer.textHaloColor, defaultStyleValue,
                              @"textHaloColor should return the default value after being unset.");
    }

    // text-halo-width
    {
        XCTAssertTrue(rawLayer->getTextHaloWidth().isUndefined(),
                      @"text-halo-width should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textHaloWidth;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.textHaloWidth = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextHaloWidth(), propertyValue,
                       @"Setting textHaloWidth to a constant value should update text-halo-width.");
        XCTAssertEqualObjects(layer.textHaloWidth, constantStyleValue,
                              @"textHaloWidth should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textHaloWidth = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextHaloWidth(), propertyValue,
                       @"Setting textHaloWidth to a camera function should update text-halo-width.");
        XCTAssertEqualObjects(layer.textHaloWidth, functionStyleValue,
                              @"textHaloWidth should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.textHaloWidth = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getTextHaloWidth(), propertyValue,
                       @"Setting textHaloWidth to a source function should update text-halo-width.");
        XCTAssertEqualObjects(layer.textHaloWidth, functionStyleValue,
                              @"textHaloWidth should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.textHaloWidth = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getTextHaloWidth(), propertyValue,
                       @"Setting textHaloWidth to a composite function should update text-halo-width.");
        XCTAssertEqualObjects(layer.textHaloWidth, functionStyleValue,
                              @"textHaloWidth should round-trip composite functions.");                                                                                                          
                              

        layer.textHaloWidth = nil;
        XCTAssertTrue(rawLayer->getTextHaloWidth().isUndefined(),
                      @"Unsetting textHaloWidth should return text-halo-width to the default value.");
        XCTAssertEqualObjects(layer.textHaloWidth, defaultStyleValue,
                              @"textHaloWidth should return the default value after being unset.");
    }

    // text-opacity
    {
        XCTAssertTrue(rawLayer->getTextOpacity().isUndefined(),
                      @"text-opacity should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textOpacity;

        MGLStyleValue<NSNumber *> *constantStyleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.textOpacity = constantStyleValue;
        mbgl::style::DataDrivenPropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextOpacity(), propertyValue,
                       @"Setting textOpacity to a constant value should update text-opacity.");
        XCTAssertEqualObjects(layer.textOpacity, constantStyleValue,
                              @"textOpacity should round-trip constant values.");

        MGLStyleValue<NSNumber *> * functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textOpacity = functionStyleValue;

        mbgl::style::IntervalStops<float> intervalStops = { {{18, 0xff}} };
        propertyValue = mbgl::style::CameraFunction<float> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextOpacity(), propertyValue,
                       @"Setting textOpacity to a camera function should update text-opacity.");
        XCTAssertEqualObjects(layer.textOpacity, functionStyleValue,
                              @"textOpacity should round-trip camera functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential sourceStops:@{@18: constantStyleValue} attributeName:@"keyName" options:nil];
        layer.textOpacity = functionStyleValue;

        mbgl::style::ExponentialStops<float> exponentialStops = { {{18, 0xff}}, 1.0 };
        propertyValue = mbgl::style::SourceFunction<float> { "keyName", exponentialStops };

        XCTAssertEqual(rawLayer->getTextOpacity(), propertyValue,
                       @"Setting textOpacity to a source function should update text-opacity.");
        XCTAssertEqualObjects(layer.textOpacity, functionStyleValue,
                              @"textOpacity should round-trip source functions.");

        functionStyleValue = [MGLStyleValue<NSNumber *> valueWithInterpolationMode:MGLInterpolationModeExponential compositeStops:@{@10: @{@18: constantStyleValue}} attributeName:@"keyName" options:nil];
        layer.textOpacity = functionStyleValue;

        std::map<float, float> innerStops { {18, 0xff} };
        mbgl::style::CompositeExponentialStops<float> compositeStops { { {10.0, innerStops} }, 1.0 };

        propertyValue = mbgl::style::CompositeFunction<float> { "keyName", compositeStops };

        XCTAssertEqual(rawLayer->getTextOpacity(), propertyValue,
                       @"Setting textOpacity to a composite function should update text-opacity.");
        XCTAssertEqualObjects(layer.textOpacity, functionStyleValue,
                              @"textOpacity should round-trip composite functions.");                                                                                                          
                              

        layer.textOpacity = nil;
        XCTAssertTrue(rawLayer->getTextOpacity().isUndefined(),
                      @"Unsetting textOpacity should return text-opacity to the default value.");
        XCTAssertEqualObjects(layer.textOpacity, defaultStyleValue,
                              @"textOpacity should return the default value after being unset.");
    }

    // text-translate
    {
        XCTAssertTrue(rawLayer->getTextTranslate().isUndefined(),
                      @"text-translate should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.textTranslation;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.textTranslation = constantStyleValue;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getTextTranslate(), propertyValue,
                       @"Setting textTranslation to a constant value should update text-translate.");
        XCTAssertEqualObjects(layer.textTranslation, constantStyleValue,
                              @"textTranslation should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textTranslation = functionStyleValue;

        mbgl::style::IntervalStops<std::array<float, 2>> intervalStops = { {{18, { 1, 1 }}} };
        propertyValue = mbgl::style::CameraFunction<std::array<float, 2>> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextTranslate(), propertyValue,
                       @"Setting textTranslation to a camera function should update text-translate.");
        XCTAssertEqualObjects(layer.textTranslation, functionStyleValue,
                              @"textTranslation should round-trip camera functions.");

                              

        layer.textTranslation = nil;
        XCTAssertTrue(rawLayer->getTextTranslate().isUndefined(),
                      @"Unsetting textTranslation should return text-translate to the default value.");
        XCTAssertEqualObjects(layer.textTranslation, defaultStyleValue,
                              @"textTranslation should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textTranslation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textTranslation = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }

    // text-translate-anchor
    {
        XCTAssertTrue(rawLayer->getTextTranslateAnchor().isUndefined(),
                      @"text-translate-anchor should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.textTranslationAnchor;

        MGLStyleValue<NSValue *> *constantStyleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLTextTranslationAnchor:MGLTextTranslationAnchorViewport]];
        layer.textTranslationAnchor = constantStyleValue;
        mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType> propertyValue = { mbgl::style::TranslateAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getTextTranslateAnchor(), propertyValue,
                       @"Setting textTranslationAnchor to a constant value should update text-translate-anchor.");
        XCTAssertEqualObjects(layer.textTranslationAnchor, constantStyleValue,
                              @"textTranslationAnchor should round-trip constant values.");

        MGLStyleValue<NSValue *> * functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval cameraStops:@{@18: constantStyleValue} options:nil];
        layer.textTranslationAnchor = functionStyleValue;

        mbgl::style::IntervalStops<mbgl::style::TranslateAnchorType> intervalStops = { {{18, mbgl::style::TranslateAnchorType::Viewport}} };
        propertyValue = mbgl::style::CameraFunction<mbgl::style::TranslateAnchorType> { intervalStops };
        
        XCTAssertEqual(rawLayer->getTextTranslateAnchor(), propertyValue,
                       @"Setting textTranslationAnchor to a camera function should update text-translate-anchor.");
        XCTAssertEqualObjects(layer.textTranslationAnchor, functionStyleValue,
                              @"textTranslationAnchor should round-trip camera functions.");

                              

        layer.textTranslationAnchor = nil;
        XCTAssertTrue(rawLayer->getTextTranslateAnchor().isUndefined(),
                      @"Unsetting textTranslationAnchor should return text-translate-anchor to the default value.");
        XCTAssertEqualObjects(layer.textTranslationAnchor, defaultStyleValue,
                              @"textTranslationAnchor should return the default value after being unset.");

        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeIdentity sourceStops:nil attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textTranslationAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
        functionStyleValue = [MGLStyleValue<NSValue *> valueWithInterpolationMode:MGLInterpolationModeInterval compositeStops:@{@18: constantStyleValue} attributeName:@"" options:nil];
        XCTAssertThrowsSpecificNamed(layer.textTranslationAnchor = functionStyleValue, NSException, NSInvalidArgumentException, @"MGLStyleValue should raise an exception if it is applied to a property that cannot support it");
    }
}

- (void)testPropertyNames {
    [self testPropertyName:@"icon-allows-overlap" isBoolean:YES];
    [self testPropertyName:@"icon-ignores-placement" isBoolean:YES];
    [self testPropertyName:@"icon-image-name" isBoolean:NO];
    [self testPropertyName:@"icon-offset" isBoolean:NO];
    [self testPropertyName:@"is-icon-optional" isBoolean:YES];
    [self testPropertyName:@"icon-padding" isBoolean:NO];
    [self testPropertyName:@"icon-rotation" isBoolean:NO];
    [self testPropertyName:@"icon-rotation-alignment" isBoolean:NO];
    [self testPropertyName:@"icon-scale" isBoolean:NO];
    [self testPropertyName:@"icon-text-fit" isBoolean:NO];
    [self testPropertyName:@"icon-text-fit-padding" isBoolean:NO];
    [self testPropertyName:@"keeps-icon-upright" isBoolean:YES];
    [self testPropertyName:@"keeps-text-upright" isBoolean:YES];
    [self testPropertyName:@"maximum-text-angle" isBoolean:NO];
    [self testPropertyName:@"maximum-text-width" isBoolean:NO];
    [self testPropertyName:@"symbol-avoids-edges" isBoolean:YES];
    [self testPropertyName:@"symbol-placement" isBoolean:NO];
    [self testPropertyName:@"symbol-spacing" isBoolean:NO];
    [self testPropertyName:@"text" isBoolean:NO];
    [self testPropertyName:@"text-allows-overlap" isBoolean:YES];
    [self testPropertyName:@"text-anchor" isBoolean:NO];
    [self testPropertyName:@"text-font-names" isBoolean:NO];
    [self testPropertyName:@"text-font-size" isBoolean:NO];
    [self testPropertyName:@"text-ignores-placement" isBoolean:YES];
    [self testPropertyName:@"text-justification" isBoolean:NO];
    [self testPropertyName:@"text-letter-spacing" isBoolean:NO];
    [self testPropertyName:@"text-line-height" isBoolean:NO];
    [self testPropertyName:@"text-offset" isBoolean:NO];
    [self testPropertyName:@"is-text-optional" isBoolean:YES];
    [self testPropertyName:@"text-padding" isBoolean:NO];
    [self testPropertyName:@"text-pitch-alignment" isBoolean:NO];
    [self testPropertyName:@"text-rotation" isBoolean:NO];
    [self testPropertyName:@"text-rotation-alignment" isBoolean:NO];
    [self testPropertyName:@"text-transform" isBoolean:NO];
    [self testPropertyName:@"icon-color" isBoolean:NO];
    [self testPropertyName:@"icon-halo-blur" isBoolean:NO];
    [self testPropertyName:@"icon-halo-color" isBoolean:NO];
    [self testPropertyName:@"icon-halo-width" isBoolean:NO];
    [self testPropertyName:@"icon-opacity" isBoolean:NO];
    [self testPropertyName:@"icon-translation" isBoolean:NO];
    [self testPropertyName:@"icon-translation-anchor" isBoolean:NO];
    [self testPropertyName:@"text-color" isBoolean:NO];
    [self testPropertyName:@"text-halo-blur" isBoolean:NO];
    [self testPropertyName:@"text-halo-color" isBoolean:NO];
    [self testPropertyName:@"text-halo-width" isBoolean:NO];
    [self testPropertyName:@"text-opacity" isBoolean:NO];
    [self testPropertyName:@"text-translation" isBoolean:NO];
    [self testPropertyName:@"text-translation-anchor" isBoolean:NO];
}

- (void)testValueAdditions {
    XCTAssertEqual([NSValue valueWithMGLIconRotationAlignment:MGLIconRotationAlignmentMap].MGLIconRotationAlignmentValue, MGLIconRotationAlignmentMap);
    XCTAssertEqual([NSValue valueWithMGLIconRotationAlignment:MGLIconRotationAlignmentViewport].MGLIconRotationAlignmentValue, MGLIconRotationAlignmentViewport);
    XCTAssertEqual([NSValue valueWithMGLIconRotationAlignment:MGLIconRotationAlignmentAuto].MGLIconRotationAlignmentValue, MGLIconRotationAlignmentAuto);
    XCTAssertEqual([NSValue valueWithMGLIconTextFit:MGLIconTextFitNone].MGLIconTextFitValue, MGLIconTextFitNone);
    XCTAssertEqual([NSValue valueWithMGLIconTextFit:MGLIconTextFitWidth].MGLIconTextFitValue, MGLIconTextFitWidth);
    XCTAssertEqual([NSValue valueWithMGLIconTextFit:MGLIconTextFitHeight].MGLIconTextFitValue, MGLIconTextFitHeight);
    XCTAssertEqual([NSValue valueWithMGLIconTextFit:MGLIconTextFitBoth].MGLIconTextFitValue, MGLIconTextFitBoth);
    XCTAssertEqual([NSValue valueWithMGLSymbolPlacement:MGLSymbolPlacementPoint].MGLSymbolPlacementValue, MGLSymbolPlacementPoint);
    XCTAssertEqual([NSValue valueWithMGLSymbolPlacement:MGLSymbolPlacementLine].MGLSymbolPlacementValue, MGLSymbolPlacementLine);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorCenter].MGLTextAnchorValue, MGLTextAnchorCenter);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorLeft].MGLTextAnchorValue, MGLTextAnchorLeft);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorRight].MGLTextAnchorValue, MGLTextAnchorRight);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorTop].MGLTextAnchorValue, MGLTextAnchorTop);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorBottom].MGLTextAnchorValue, MGLTextAnchorBottom);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorTopLeft].MGLTextAnchorValue, MGLTextAnchorTopLeft);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorTopRight].MGLTextAnchorValue, MGLTextAnchorTopRight);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorBottomLeft].MGLTextAnchorValue, MGLTextAnchorBottomLeft);
    XCTAssertEqual([NSValue valueWithMGLTextAnchor:MGLTextAnchorBottomRight].MGLTextAnchorValue, MGLTextAnchorBottomRight);
    XCTAssertEqual([NSValue valueWithMGLTextJustification:MGLTextJustificationLeft].MGLTextJustificationValue, MGLTextJustificationLeft);
    XCTAssertEqual([NSValue valueWithMGLTextJustification:MGLTextJustificationCenter].MGLTextJustificationValue, MGLTextJustificationCenter);
    XCTAssertEqual([NSValue valueWithMGLTextJustification:MGLTextJustificationRight].MGLTextJustificationValue, MGLTextJustificationRight);
    XCTAssertEqual([NSValue valueWithMGLTextPitchAlignment:MGLTextPitchAlignmentMap].MGLTextPitchAlignmentValue, MGLTextPitchAlignmentMap);
    XCTAssertEqual([NSValue valueWithMGLTextPitchAlignment:MGLTextPitchAlignmentViewport].MGLTextPitchAlignmentValue, MGLTextPitchAlignmentViewport);
    XCTAssertEqual([NSValue valueWithMGLTextPitchAlignment:MGLTextPitchAlignmentAuto].MGLTextPitchAlignmentValue, MGLTextPitchAlignmentAuto);
    XCTAssertEqual([NSValue valueWithMGLTextRotationAlignment:MGLTextRotationAlignmentMap].MGLTextRotationAlignmentValue, MGLTextRotationAlignmentMap);
    XCTAssertEqual([NSValue valueWithMGLTextRotationAlignment:MGLTextRotationAlignmentViewport].MGLTextRotationAlignmentValue, MGLTextRotationAlignmentViewport);
    XCTAssertEqual([NSValue valueWithMGLTextRotationAlignment:MGLTextRotationAlignmentAuto].MGLTextRotationAlignmentValue, MGLTextRotationAlignmentAuto);
    XCTAssertEqual([NSValue valueWithMGLTextTransform:MGLTextTransformNone].MGLTextTransformValue, MGLTextTransformNone);
    XCTAssertEqual([NSValue valueWithMGLTextTransform:MGLTextTransformUppercase].MGLTextTransformValue, MGLTextTransformUppercase);
    XCTAssertEqual([NSValue valueWithMGLTextTransform:MGLTextTransformLowercase].MGLTextTransformValue, MGLTextTransformLowercase);
    XCTAssertEqual([NSValue valueWithMGLIconTranslationAnchor:MGLIconTranslationAnchorMap].MGLIconTranslationAnchorValue, MGLIconTranslationAnchorMap);
    XCTAssertEqual([NSValue valueWithMGLIconTranslationAnchor:MGLIconTranslationAnchorViewport].MGLIconTranslationAnchorValue, MGLIconTranslationAnchorViewport);
    XCTAssertEqual([NSValue valueWithMGLTextTranslationAnchor:MGLTextTranslationAnchorMap].MGLTextTranslationAnchorValue, MGLTextTranslationAnchorMap);
    XCTAssertEqual([NSValue valueWithMGLTextTranslationAnchor:MGLTextTranslationAnchorViewport].MGLTextTranslationAnchorValue, MGLTextTranslationAnchorViewport);
}

@end
