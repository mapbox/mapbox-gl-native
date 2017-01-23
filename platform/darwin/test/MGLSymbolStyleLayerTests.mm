// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

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
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@YES];
        layer.iconAllowsOverlap = styleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getIconAllowOverlap(), propertyValue,
                       @"Setting iconAllowsOverlap to a constant value should update icon-allow-overlap.");
        XCTAssertEqualObjects(layer.iconAllowsOverlap, styleValue,
                              @"iconAllowsOverlap should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconAllowsOverlap = styleValue;
        propertyValue = { mbgl::style::Function<bool> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconAllowOverlap(), propertyValue,
                       @"Setting iconAllowsOverlap to a function should update icon-allow-overlap.");
        XCTAssertEqualObjects(layer.iconAllowsOverlap, styleValue,
                              @"iconAllowsOverlap should round-trip functions.");
        
        layer.iconAllowsOverlap = nil;
        XCTAssertTrue(rawLayer->getIconAllowOverlap().isUndefined(),
                      @"Unsetting iconAllowsOverlap should return icon-allow-overlap to the default value.");
        XCTAssertEqualObjects(layer.iconAllowsOverlap, defaultStyleValue,
                              @"iconAllowsOverlap should return the default value after being unset.");
    }
    
    // icon-ignore-placement
    {
        XCTAssertTrue(rawLayer->getIconIgnorePlacement().isUndefined(),
                      @"icon-ignore-placement should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.iconIgnoresPlacement;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@YES];
        layer.iconIgnoresPlacement = styleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getIconIgnorePlacement(), propertyValue,
                       @"Setting iconIgnoresPlacement to a constant value should update icon-ignore-placement.");
        XCTAssertEqualObjects(layer.iconIgnoresPlacement, styleValue,
                              @"iconIgnoresPlacement should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconIgnoresPlacement = styleValue;
        propertyValue = { mbgl::style::Function<bool> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconIgnorePlacement(), propertyValue,
                       @"Setting iconIgnoresPlacement to a function should update icon-ignore-placement.");
        XCTAssertEqualObjects(layer.iconIgnoresPlacement, styleValue,
                              @"iconIgnoresPlacement should round-trip functions.");
        
        layer.iconIgnoresPlacement = nil;
        XCTAssertTrue(rawLayer->getIconIgnorePlacement().isUndefined(),
                      @"Unsetting iconIgnoresPlacement should return icon-ignore-placement to the default value.");
        XCTAssertEqualObjects(layer.iconIgnoresPlacement, defaultStyleValue,
                              @"iconIgnoresPlacement should return the default value after being unset.");
    }
    
    // icon-image
    {
        XCTAssertTrue(rawLayer->getIconImage().isUndefined(),
                      @"icon-image should be unset initially.");
        MGLStyleValue<NSString *> *defaultStyleValue = layer.iconImageName;
        
        MGLStyleValue<NSString *> *styleValue = [MGLStyleValue<NSString *> valueWithRawValue:@"Icon Image"];
        layer.iconImageName = styleValue;
        mbgl::style::PropertyValue<std::string> propertyValue = { "Icon Image" };
        XCTAssertEqual(rawLayer->getIconImage(), propertyValue,
                       @"Setting iconImageName to a constant value should update icon-image.");
        XCTAssertEqualObjects(layer.iconImageName, styleValue,
                              @"iconImageName should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSString *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconImageName = styleValue;
        propertyValue = { mbgl::style::Function<std::string> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconImage(), propertyValue,
                       @"Setting iconImageName to a function should update icon-image.");
        XCTAssertEqualObjects(layer.iconImageName, styleValue,
                              @"iconImageName should round-trip functions.");
        
        layer.iconImageName = nil;
        XCTAssertTrue(rawLayer->getIconImage().isUndefined(),
                      @"Unsetting iconImageName should return icon-image to the default value.");
        XCTAssertEqualObjects(layer.iconImageName, defaultStyleValue,
                              @"iconImageName should return the default value after being unset.");
    }
    
    // icon-offset
    {
        XCTAssertTrue(rawLayer->getIconOffset().isUndefined(),
                      @"icon-offset should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.iconOffset;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.iconOffset = styleValue;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getIconOffset(), propertyValue,
                       @"Setting iconOffset to a constant value should update icon-offset.");
        XCTAssertEqualObjects(layer.iconOffset, styleValue,
                              @"iconOffset should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconOffset = styleValue;
        propertyValue = { mbgl::style::Function<std::array<float, 2>> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconOffset(), propertyValue,
                       @"Setting iconOffset to a function should update icon-offset.");
        XCTAssertEqualObjects(layer.iconOffset, styleValue,
                              @"iconOffset should round-trip functions.");
        
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
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@YES];
        layer.iconOptional = styleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getIconOptional(), propertyValue,
                       @"Setting iconOptional to a constant value should update icon-optional.");
        XCTAssertEqualObjects(layer.iconOptional, styleValue,
                              @"iconOptional should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconOptional = styleValue;
        propertyValue = { mbgl::style::Function<bool> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconOptional(), propertyValue,
                       @"Setting iconOptional to a function should update icon-optional.");
        XCTAssertEqualObjects(layer.iconOptional, styleValue,
                              @"iconOptional should round-trip functions.");
        
        layer.iconOptional = nil;
        XCTAssertTrue(rawLayer->getIconOptional().isUndefined(),
                      @"Unsetting iconOptional should return icon-optional to the default value.");
        XCTAssertEqualObjects(layer.iconOptional, defaultStyleValue,
                              @"iconOptional should return the default value after being unset.");
    }
    
    // icon-padding
    {
        XCTAssertTrue(rawLayer->getIconPadding().isUndefined(),
                      @"icon-padding should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.iconPadding;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.iconPadding = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getIconPadding(), propertyValue,
                       @"Setting iconPadding to a constant value should update icon-padding.");
        XCTAssertEqualObjects(layer.iconPadding, styleValue,
                              @"iconPadding should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconPadding = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconPadding(), propertyValue,
                       @"Setting iconPadding to a function should update icon-padding.");
        XCTAssertEqualObjects(layer.iconPadding, styleValue,
                              @"iconPadding should round-trip functions.");
        
        layer.iconPadding = nil;
        XCTAssertTrue(rawLayer->getIconPadding().isUndefined(),
                      @"Unsetting iconPadding should return icon-padding to the default value.");
        XCTAssertEqualObjects(layer.iconPadding, defaultStyleValue,
                              @"iconPadding should return the default value after being unset.");
    }
    
    // icon-rotate
    {
        XCTAssertTrue(rawLayer->getIconRotate().isUndefined(),
                      @"icon-rotate should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.iconRotation;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.iconRotation = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getIconRotate(), propertyValue,
                       @"Setting iconRotation to a constant value should update icon-rotate.");
        XCTAssertEqualObjects(layer.iconRotation, styleValue,
                              @"iconRotation should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconRotation = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconRotate(), propertyValue,
                       @"Setting iconRotation to a function should update icon-rotate.");
        XCTAssertEqualObjects(layer.iconRotation, styleValue,
                              @"iconRotation should round-trip functions.");
        
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
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLIconRotationAlignment:MGLIconRotationAlignmentAuto]];
        layer.iconRotationAlignment = styleValue;
        mbgl::style::PropertyValue<mbgl::style::AlignmentType> propertyValue = { mbgl::style::AlignmentType::Auto };
        XCTAssertEqual(rawLayer->getIconRotationAlignment(), propertyValue,
                       @"Setting iconRotationAlignment to a constant value should update icon-rotation-alignment.");
        XCTAssertEqualObjects(layer.iconRotationAlignment, styleValue,
                              @"iconRotationAlignment should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconRotationAlignment = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::style::AlignmentType> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconRotationAlignment(), propertyValue,
                       @"Setting iconRotationAlignment to a function should update icon-rotation-alignment.");
        XCTAssertEqualObjects(layer.iconRotationAlignment, styleValue,
                              @"iconRotationAlignment should round-trip functions.");
        
        layer.iconRotationAlignment = nil;
        XCTAssertTrue(rawLayer->getIconRotationAlignment().isUndefined(),
                      @"Unsetting iconRotationAlignment should return icon-rotation-alignment to the default value.");
        XCTAssertEqualObjects(layer.iconRotationAlignment, defaultStyleValue,
                              @"iconRotationAlignment should return the default value after being unset.");
    }
    
    // icon-size
    {
        XCTAssertTrue(rawLayer->getIconSize().isUndefined(),
                      @"icon-size should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.iconScale;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.iconScale = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getIconSize(), propertyValue,
                       @"Setting iconScale to a constant value should update icon-size.");
        XCTAssertEqualObjects(layer.iconScale, styleValue,
                              @"iconScale should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconScale = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconSize(), propertyValue,
                       @"Setting iconScale to a function should update icon-size.");
        XCTAssertEqualObjects(layer.iconScale, styleValue,
                              @"iconScale should round-trip functions.");
        
        layer.iconScale = nil;
        XCTAssertTrue(rawLayer->getIconSize().isUndefined(),
                      @"Unsetting iconScale should return icon-size to the default value.");
        XCTAssertEqualObjects(layer.iconScale, defaultStyleValue,
                              @"iconScale should return the default value after being unset.");
    }
    
    // icon-text-fit
    {
        XCTAssertTrue(rawLayer->getIconTextFit().isUndefined(),
                      @"icon-text-fit should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.iconTextFit;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLIconTextFit:MGLIconTextFitBoth]];
        layer.iconTextFit = styleValue;
        mbgl::style::PropertyValue<mbgl::style::IconTextFitType> propertyValue = { mbgl::style::IconTextFitType::Both };
        XCTAssertEqual(rawLayer->getIconTextFit(), propertyValue,
                       @"Setting iconTextFit to a constant value should update icon-text-fit.");
        XCTAssertEqualObjects(layer.iconTextFit, styleValue,
                              @"iconTextFit should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconTextFit = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::style::IconTextFitType> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconTextFit(), propertyValue,
                       @"Setting iconTextFit to a function should update icon-text-fit.");
        XCTAssertEqualObjects(layer.iconTextFit, styleValue,
                              @"iconTextFit should round-trip functions.");
        
        layer.iconTextFit = nil;
        XCTAssertTrue(rawLayer->getIconTextFit().isUndefined(),
                      @"Unsetting iconTextFit should return icon-text-fit to the default value.");
        XCTAssertEqualObjects(layer.iconTextFit, defaultStyleValue,
                              @"iconTextFit should return the default value after being unset.");
    }
    
    // icon-text-fit-padding
    {
        XCTAssertTrue(rawLayer->getIconTextFitPadding().isUndefined(),
                      @"icon-text-fit-padding should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.iconTextFitPadding;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithUIEdgeInsets:UIEdgeInsetsMake(1, 1, 1, 1)]
#else
            [NSValue valueWithEdgeInsets:NSEdgeInsetsMake(1, 1, 1, 1)]
#endif
        ];
        layer.iconTextFitPadding = styleValue;
        mbgl::style::PropertyValue<std::array<float, 4>> propertyValue = { { 1, 1, 1, 1 } };
        XCTAssertEqual(rawLayer->getIconTextFitPadding(), propertyValue,
                       @"Setting iconTextFitPadding to a constant value should update icon-text-fit-padding.");
        XCTAssertEqualObjects(layer.iconTextFitPadding, styleValue,
                              @"iconTextFitPadding should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconTextFitPadding = styleValue;
        propertyValue = { mbgl::style::Function<std::array<float, 4>> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconTextFitPadding(), propertyValue,
                       @"Setting iconTextFitPadding to a function should update icon-text-fit-padding.");
        XCTAssertEqualObjects(layer.iconTextFitPadding, styleValue,
                              @"iconTextFitPadding should round-trip functions.");
        
        layer.iconTextFitPadding = nil;
        XCTAssertTrue(rawLayer->getIconTextFitPadding().isUndefined(),
                      @"Unsetting iconTextFitPadding should return icon-text-fit-padding to the default value.");
        XCTAssertEqualObjects(layer.iconTextFitPadding, defaultStyleValue,
                              @"iconTextFitPadding should return the default value after being unset.");
    }
    
    // icon-keep-upright
    {
        XCTAssertTrue(rawLayer->getIconKeepUpright().isUndefined(),
                      @"icon-keep-upright should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.keepsIconUpright;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@YES];
        layer.keepsIconUpright = styleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getIconKeepUpright(), propertyValue,
                       @"Setting keepsIconUpright to a constant value should update icon-keep-upright.");
        XCTAssertEqualObjects(layer.keepsIconUpright, styleValue,
                              @"keepsIconUpright should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.keepsIconUpright = styleValue;
        propertyValue = { mbgl::style::Function<bool> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconKeepUpright(), propertyValue,
                       @"Setting keepsIconUpright to a function should update icon-keep-upright.");
        XCTAssertEqualObjects(layer.keepsIconUpright, styleValue,
                              @"keepsIconUpright should round-trip functions.");
        
        layer.keepsIconUpright = nil;
        XCTAssertTrue(rawLayer->getIconKeepUpright().isUndefined(),
                      @"Unsetting keepsIconUpright should return icon-keep-upright to the default value.");
        XCTAssertEqualObjects(layer.keepsIconUpright, defaultStyleValue,
                              @"keepsIconUpright should return the default value after being unset.");
    }
    
    // text-keep-upright
    {
        XCTAssertTrue(rawLayer->getTextKeepUpright().isUndefined(),
                      @"text-keep-upright should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.keepsTextUpright;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@NO];
        layer.keepsTextUpright = styleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { false };
        XCTAssertEqual(rawLayer->getTextKeepUpright(), propertyValue,
                       @"Setting keepsTextUpright to a constant value should update text-keep-upright.");
        XCTAssertEqualObjects(layer.keepsTextUpright, styleValue,
                              @"keepsTextUpright should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.keepsTextUpright = styleValue;
        propertyValue = { mbgl::style::Function<bool> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextKeepUpright(), propertyValue,
                       @"Setting keepsTextUpright to a function should update text-keep-upright.");
        XCTAssertEqualObjects(layer.keepsTextUpright, styleValue,
                              @"keepsTextUpright should round-trip functions.");
        
        layer.keepsTextUpright = nil;
        XCTAssertTrue(rawLayer->getTextKeepUpright().isUndefined(),
                      @"Unsetting keepsTextUpright should return text-keep-upright to the default value.");
        XCTAssertEqualObjects(layer.keepsTextUpright, defaultStyleValue,
                              @"keepsTextUpright should return the default value after being unset.");
    }
    
    // text-max-angle
    {
        XCTAssertTrue(rawLayer->getTextMaxAngle().isUndefined(),
                      @"text-max-angle should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.maximumTextAngle;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.maximumTextAngle = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextMaxAngle(), propertyValue,
                       @"Setting maximumTextAngle to a constant value should update text-max-angle.");
        XCTAssertEqualObjects(layer.maximumTextAngle, styleValue,
                              @"maximumTextAngle should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.maximumTextAngle = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextMaxAngle(), propertyValue,
                       @"Setting maximumTextAngle to a function should update text-max-angle.");
        XCTAssertEqualObjects(layer.maximumTextAngle, styleValue,
                              @"maximumTextAngle should round-trip functions.");
        
        layer.maximumTextAngle = nil;
        XCTAssertTrue(rawLayer->getTextMaxAngle().isUndefined(),
                      @"Unsetting maximumTextAngle should return text-max-angle to the default value.");
        XCTAssertEqualObjects(layer.maximumTextAngle, defaultStyleValue,
                              @"maximumTextAngle should return the default value after being unset.");
    }
    
    // text-max-width
    {
        XCTAssertTrue(rawLayer->getTextMaxWidth().isUndefined(),
                      @"text-max-width should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.maximumTextWidth;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.maximumTextWidth = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextMaxWidth(), propertyValue,
                       @"Setting maximumTextWidth to a constant value should update text-max-width.");
        XCTAssertEqualObjects(layer.maximumTextWidth, styleValue,
                              @"maximumTextWidth should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.maximumTextWidth = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextMaxWidth(), propertyValue,
                       @"Setting maximumTextWidth to a function should update text-max-width.");
        XCTAssertEqualObjects(layer.maximumTextWidth, styleValue,
                              @"maximumTextWidth should round-trip functions.");
        
        layer.maximumTextWidth = nil;
        XCTAssertTrue(rawLayer->getTextMaxWidth().isUndefined(),
                      @"Unsetting maximumTextWidth should return text-max-width to the default value.");
        XCTAssertEqualObjects(layer.maximumTextWidth, defaultStyleValue,
                              @"maximumTextWidth should return the default value after being unset.");
    }
    
    // symbol-avoid-edges
    {
        XCTAssertTrue(rawLayer->getSymbolAvoidEdges().isUndefined(),
                      @"symbol-avoid-edges should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.symbolAvoidsEdges;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@YES];
        layer.symbolAvoidsEdges = styleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getSymbolAvoidEdges(), propertyValue,
                       @"Setting symbolAvoidsEdges to a constant value should update symbol-avoid-edges.");
        XCTAssertEqualObjects(layer.symbolAvoidsEdges, styleValue,
                              @"symbolAvoidsEdges should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.symbolAvoidsEdges = styleValue;
        propertyValue = { mbgl::style::Function<bool> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getSymbolAvoidEdges(), propertyValue,
                       @"Setting symbolAvoidsEdges to a function should update symbol-avoid-edges.");
        XCTAssertEqualObjects(layer.symbolAvoidsEdges, styleValue,
                              @"symbolAvoidsEdges should round-trip functions.");
        
        layer.symbolAvoidsEdges = nil;
        XCTAssertTrue(rawLayer->getSymbolAvoidEdges().isUndefined(),
                      @"Unsetting symbolAvoidsEdges should return symbol-avoid-edges to the default value.");
        XCTAssertEqualObjects(layer.symbolAvoidsEdges, defaultStyleValue,
                              @"symbolAvoidsEdges should return the default value after being unset.");
    }
    
    // symbol-placement
    {
        XCTAssertTrue(rawLayer->getSymbolPlacement().isUndefined(),
                      @"symbol-placement should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.symbolPlacement;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLSymbolPlacement:MGLSymbolPlacementLine]];
        layer.symbolPlacement = styleValue;
        mbgl::style::PropertyValue<mbgl::style::SymbolPlacementType> propertyValue = { mbgl::style::SymbolPlacementType::Line };
        XCTAssertEqual(rawLayer->getSymbolPlacement(), propertyValue,
                       @"Setting symbolPlacement to a constant value should update symbol-placement.");
        XCTAssertEqualObjects(layer.symbolPlacement, styleValue,
                              @"symbolPlacement should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.symbolPlacement = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::style::SymbolPlacementType> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getSymbolPlacement(), propertyValue,
                       @"Setting symbolPlacement to a function should update symbol-placement.");
        XCTAssertEqualObjects(layer.symbolPlacement, styleValue,
                              @"symbolPlacement should round-trip functions.");
        
        layer.symbolPlacement = nil;
        XCTAssertTrue(rawLayer->getSymbolPlacement().isUndefined(),
                      @"Unsetting symbolPlacement should return symbol-placement to the default value.");
        XCTAssertEqualObjects(layer.symbolPlacement, defaultStyleValue,
                              @"symbolPlacement should return the default value after being unset.");
    }
    
    // symbol-spacing
    {
        XCTAssertTrue(rawLayer->getSymbolSpacing().isUndefined(),
                      @"symbol-spacing should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.symbolSpacing;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.symbolSpacing = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getSymbolSpacing(), propertyValue,
                       @"Setting symbolSpacing to a constant value should update symbol-spacing.");
        XCTAssertEqualObjects(layer.symbolSpacing, styleValue,
                              @"symbolSpacing should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.symbolSpacing = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getSymbolSpacing(), propertyValue,
                       @"Setting symbolSpacing to a function should update symbol-spacing.");
        XCTAssertEqualObjects(layer.symbolSpacing, styleValue,
                              @"symbolSpacing should round-trip functions.");
        
        layer.symbolSpacing = nil;
        XCTAssertTrue(rawLayer->getSymbolSpacing().isUndefined(),
                      @"Unsetting symbolSpacing should return symbol-spacing to the default value.");
        XCTAssertEqualObjects(layer.symbolSpacing, defaultStyleValue,
                              @"symbolSpacing should return the default value after being unset.");
    }
    
    // text-field
    {
        XCTAssertTrue(rawLayer->getTextField().isUndefined(),
                      @"text-field should be unset initially.");
        MGLStyleValue<NSString *> *defaultStyleValue = layer.text;
        
        MGLStyleValue<NSString *> *styleValue = [MGLStyleValue<NSString *> valueWithRawValue:@"Text Field"];
        layer.text = styleValue;
        mbgl::style::PropertyValue<std::string> propertyValue = { "Text Field" };
        XCTAssertEqual(rawLayer->getTextField(), propertyValue,
                       @"Setting text to a constant value should update text-field.");
        XCTAssertEqualObjects(layer.text, styleValue,
                              @"text should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSString *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.text = styleValue;
        propertyValue = { mbgl::style::Function<std::string> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextField(), propertyValue,
                       @"Setting text to a function should update text-field.");
        XCTAssertEqualObjects(layer.text, styleValue,
                              @"text should round-trip functions.");
        
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
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@YES];
        layer.textAllowsOverlap = styleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getTextAllowOverlap(), propertyValue,
                       @"Setting textAllowsOverlap to a constant value should update text-allow-overlap.");
        XCTAssertEqualObjects(layer.textAllowsOverlap, styleValue,
                              @"textAllowsOverlap should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textAllowsOverlap = styleValue;
        propertyValue = { mbgl::style::Function<bool> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextAllowOverlap(), propertyValue,
                       @"Setting textAllowsOverlap to a function should update text-allow-overlap.");
        XCTAssertEqualObjects(layer.textAllowsOverlap, styleValue,
                              @"textAllowsOverlap should round-trip functions.");
        
        layer.textAllowsOverlap = nil;
        XCTAssertTrue(rawLayer->getTextAllowOverlap().isUndefined(),
                      @"Unsetting textAllowsOverlap should return text-allow-overlap to the default value.");
        XCTAssertEqualObjects(layer.textAllowsOverlap, defaultStyleValue,
                              @"textAllowsOverlap should return the default value after being unset.");
    }
    
    // text-anchor
    {
        XCTAssertTrue(rawLayer->getTextAnchor().isUndefined(),
                      @"text-anchor should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.textAnchor;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLTextAnchor:MGLTextAnchorBottomRight]];
        layer.textAnchor = styleValue;
        mbgl::style::PropertyValue<mbgl::style::TextAnchorType> propertyValue = { mbgl::style::TextAnchorType::BottomRight };
        XCTAssertEqual(rawLayer->getTextAnchor(), propertyValue,
                       @"Setting textAnchor to a constant value should update text-anchor.");
        XCTAssertEqualObjects(layer.textAnchor, styleValue,
                              @"textAnchor should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textAnchor = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::style::TextAnchorType> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextAnchor(), propertyValue,
                       @"Setting textAnchor to a function should update text-anchor.");
        XCTAssertEqualObjects(layer.textAnchor, styleValue,
                              @"textAnchor should round-trip functions.");
        
        layer.textAnchor = nil;
        XCTAssertTrue(rawLayer->getTextAnchor().isUndefined(),
                      @"Unsetting textAnchor should return text-anchor to the default value.");
        XCTAssertEqualObjects(layer.textAnchor, defaultStyleValue,
                              @"textAnchor should return the default value after being unset.");
    }
    
    // text-font
    {
        XCTAssertTrue(rawLayer->getTextFont().isUndefined(),
                      @"text-font should be unset initially.");
        MGLStyleValue<NSArray<NSString *> *> *defaultStyleValue = layer.textFontNames;
        
        MGLStyleValue<NSArray<NSString *> *> *styleValue = [MGLStyleValue<NSArray<NSString *> *> valueWithRawValue:@[@"Text Font", @"Tnof Txet"]];
        layer.textFontNames = styleValue;
        mbgl::style::PropertyValue<std::vector<std::string>> propertyValue = { { "Text Font", "Tnof Txet" } };
        XCTAssertEqual(rawLayer->getTextFont(), propertyValue,
                       @"Setting textFontNames to a constant value should update text-font.");
        XCTAssertEqualObjects(layer.textFontNames, styleValue,
                              @"textFontNames should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSArray<NSString *> *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textFontNames = styleValue;
        propertyValue = { mbgl::style::Function<std::vector<std::string>> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextFont(), propertyValue,
                       @"Setting textFontNames to a function should update text-font.");
        XCTAssertEqualObjects(layer.textFontNames, styleValue,
                              @"textFontNames should round-trip functions.");
        
        layer.textFontNames = nil;
        XCTAssertTrue(rawLayer->getTextFont().isUndefined(),
                      @"Unsetting textFontNames should return text-font to the default value.");
        XCTAssertEqualObjects(layer.textFontNames, defaultStyleValue,
                              @"textFontNames should return the default value after being unset.");
    }
    
    // text-size
    {
        XCTAssertTrue(rawLayer->getTextSize().isUndefined(),
                      @"text-size should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textFontSize;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.textFontSize = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextSize(), propertyValue,
                       @"Setting textFontSize to a constant value should update text-size.");
        XCTAssertEqualObjects(layer.textFontSize, styleValue,
                              @"textFontSize should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textFontSize = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextSize(), propertyValue,
                       @"Setting textFontSize to a function should update text-size.");
        XCTAssertEqualObjects(layer.textFontSize, styleValue,
                              @"textFontSize should round-trip functions.");
        
        layer.textFontSize = nil;
        XCTAssertTrue(rawLayer->getTextSize().isUndefined(),
                      @"Unsetting textFontSize should return text-size to the default value.");
        XCTAssertEqualObjects(layer.textFontSize, defaultStyleValue,
                              @"textFontSize should return the default value after being unset.");
    }
    
    // text-ignore-placement
    {
        XCTAssertTrue(rawLayer->getTextIgnorePlacement().isUndefined(),
                      @"text-ignore-placement should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textIgnoresPlacement;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@YES];
        layer.textIgnoresPlacement = styleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getTextIgnorePlacement(), propertyValue,
                       @"Setting textIgnoresPlacement to a constant value should update text-ignore-placement.");
        XCTAssertEqualObjects(layer.textIgnoresPlacement, styleValue,
                              @"textIgnoresPlacement should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textIgnoresPlacement = styleValue;
        propertyValue = { mbgl::style::Function<bool> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextIgnorePlacement(), propertyValue,
                       @"Setting textIgnoresPlacement to a function should update text-ignore-placement.");
        XCTAssertEqualObjects(layer.textIgnoresPlacement, styleValue,
                              @"textIgnoresPlacement should round-trip functions.");
        
        layer.textIgnoresPlacement = nil;
        XCTAssertTrue(rawLayer->getTextIgnorePlacement().isUndefined(),
                      @"Unsetting textIgnoresPlacement should return text-ignore-placement to the default value.");
        XCTAssertEqualObjects(layer.textIgnoresPlacement, defaultStyleValue,
                              @"textIgnoresPlacement should return the default value after being unset.");
    }
    
    // text-justify
    {
        XCTAssertTrue(rawLayer->getTextJustify().isUndefined(),
                      @"text-justify should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.textJustification;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLTextJustification:MGLTextJustificationRight]];
        layer.textJustification = styleValue;
        mbgl::style::PropertyValue<mbgl::style::TextJustifyType> propertyValue = { mbgl::style::TextJustifyType::Right };
        XCTAssertEqual(rawLayer->getTextJustify(), propertyValue,
                       @"Setting textJustification to a constant value should update text-justify.");
        XCTAssertEqualObjects(layer.textJustification, styleValue,
                              @"textJustification should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textJustification = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::style::TextJustifyType> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextJustify(), propertyValue,
                       @"Setting textJustification to a function should update text-justify.");
        XCTAssertEqualObjects(layer.textJustification, styleValue,
                              @"textJustification should round-trip functions.");
        
        layer.textJustification = nil;
        XCTAssertTrue(rawLayer->getTextJustify().isUndefined(),
                      @"Unsetting textJustification should return text-justify to the default value.");
        XCTAssertEqualObjects(layer.textJustification, defaultStyleValue,
                              @"textJustification should return the default value after being unset.");
    }
    
    // text-letter-spacing
    {
        XCTAssertTrue(rawLayer->getTextLetterSpacing().isUndefined(),
                      @"text-letter-spacing should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textLetterSpacing;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.textLetterSpacing = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextLetterSpacing(), propertyValue,
                       @"Setting textLetterSpacing to a constant value should update text-letter-spacing.");
        XCTAssertEqualObjects(layer.textLetterSpacing, styleValue,
                              @"textLetterSpacing should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textLetterSpacing = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextLetterSpacing(), propertyValue,
                       @"Setting textLetterSpacing to a function should update text-letter-spacing.");
        XCTAssertEqualObjects(layer.textLetterSpacing, styleValue,
                              @"textLetterSpacing should round-trip functions.");
        
        layer.textLetterSpacing = nil;
        XCTAssertTrue(rawLayer->getTextLetterSpacing().isUndefined(),
                      @"Unsetting textLetterSpacing should return text-letter-spacing to the default value.");
        XCTAssertEqualObjects(layer.textLetterSpacing, defaultStyleValue,
                              @"textLetterSpacing should return the default value after being unset.");
    }
    
    // text-line-height
    {
        XCTAssertTrue(rawLayer->getTextLineHeight().isUndefined(),
                      @"text-line-height should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textLineHeight;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.textLineHeight = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextLineHeight(), propertyValue,
                       @"Setting textLineHeight to a constant value should update text-line-height.");
        XCTAssertEqualObjects(layer.textLineHeight, styleValue,
                              @"textLineHeight should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textLineHeight = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextLineHeight(), propertyValue,
                       @"Setting textLineHeight to a function should update text-line-height.");
        XCTAssertEqualObjects(layer.textLineHeight, styleValue,
                              @"textLineHeight should round-trip functions.");
        
        layer.textLineHeight = nil;
        XCTAssertTrue(rawLayer->getTextLineHeight().isUndefined(),
                      @"Unsetting textLineHeight should return text-line-height to the default value.");
        XCTAssertEqualObjects(layer.textLineHeight, defaultStyleValue,
                              @"textLineHeight should return the default value after being unset.");
    }
    
    // text-offset
    {
        XCTAssertTrue(rawLayer->getTextOffset().isUndefined(),
                      @"text-offset should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.textOffset;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.textOffset = styleValue;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getTextOffset(), propertyValue,
                       @"Setting textOffset to a constant value should update text-offset.");
        XCTAssertEqualObjects(layer.textOffset, styleValue,
                              @"textOffset should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textOffset = styleValue;
        propertyValue = { mbgl::style::Function<std::array<float, 2>> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextOffset(), propertyValue,
                       @"Setting textOffset to a function should update text-offset.");
        XCTAssertEqualObjects(layer.textOffset, styleValue,
                              @"textOffset should round-trip functions.");
        
        layer.textOffset = nil;
        XCTAssertTrue(rawLayer->getTextOffset().isUndefined(),
                      @"Unsetting textOffset should return text-offset to the default value.");
        XCTAssertEqualObjects(layer.textOffset, defaultStyleValue,
                              @"textOffset should return the default value after being unset.");
    }
    
    // text-optional
    {
        XCTAssertTrue(rawLayer->getTextOptional().isUndefined(),
                      @"text-optional should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textOptional;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@YES];
        layer.textOptional = styleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { true };
        XCTAssertEqual(rawLayer->getTextOptional(), propertyValue,
                       @"Setting textOptional to a constant value should update text-optional.");
        XCTAssertEqualObjects(layer.textOptional, styleValue,
                              @"textOptional should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textOptional = styleValue;
        propertyValue = { mbgl::style::Function<bool> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextOptional(), propertyValue,
                       @"Setting textOptional to a function should update text-optional.");
        XCTAssertEqualObjects(layer.textOptional, styleValue,
                              @"textOptional should round-trip functions.");
        
        layer.textOptional = nil;
        XCTAssertTrue(rawLayer->getTextOptional().isUndefined(),
                      @"Unsetting textOptional should return text-optional to the default value.");
        XCTAssertEqualObjects(layer.textOptional, defaultStyleValue,
                              @"textOptional should return the default value after being unset.");
    }
    
    // text-padding
    {
        XCTAssertTrue(rawLayer->getTextPadding().isUndefined(),
                      @"text-padding should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textPadding;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.textPadding = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextPadding(), propertyValue,
                       @"Setting textPadding to a constant value should update text-padding.");
        XCTAssertEqualObjects(layer.textPadding, styleValue,
                              @"textPadding should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textPadding = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextPadding(), propertyValue,
                       @"Setting textPadding to a function should update text-padding.");
        XCTAssertEqualObjects(layer.textPadding, styleValue,
                              @"textPadding should round-trip functions.");
        
        layer.textPadding = nil;
        XCTAssertTrue(rawLayer->getTextPadding().isUndefined(),
                      @"Unsetting textPadding should return text-padding to the default value.");
        XCTAssertEqualObjects(layer.textPadding, defaultStyleValue,
                              @"textPadding should return the default value after being unset.");
    }
    
    // text-pitch-alignment
    {
        XCTAssertTrue(rawLayer->getTextPitchAlignment().isUndefined(),
                      @"text-pitch-alignment should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.textPitchAlignment;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLTextPitchAlignment:MGLTextPitchAlignmentAuto]];
        layer.textPitchAlignment = styleValue;
        mbgl::style::PropertyValue<mbgl::style::AlignmentType> propertyValue = { mbgl::style::AlignmentType::Auto };
        XCTAssertEqual(rawLayer->getTextPitchAlignment(), propertyValue,
                       @"Setting textPitchAlignment to a constant value should update text-pitch-alignment.");
        XCTAssertEqualObjects(layer.textPitchAlignment, styleValue,
                              @"textPitchAlignment should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textPitchAlignment = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::style::AlignmentType> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextPitchAlignment(), propertyValue,
                       @"Setting textPitchAlignment to a function should update text-pitch-alignment.");
        XCTAssertEqualObjects(layer.textPitchAlignment, styleValue,
                              @"textPitchAlignment should round-trip functions.");
        
        layer.textPitchAlignment = nil;
        XCTAssertTrue(rawLayer->getTextPitchAlignment().isUndefined(),
                      @"Unsetting textPitchAlignment should return text-pitch-alignment to the default value.");
        XCTAssertEqualObjects(layer.textPitchAlignment, defaultStyleValue,
                              @"textPitchAlignment should return the default value after being unset.");
    }
    
    // text-rotate
    {
        XCTAssertTrue(rawLayer->getTextRotate().isUndefined(),
                      @"text-rotate should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.textRotation;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.textRotation = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextRotate(), propertyValue,
                       @"Setting textRotation to a constant value should update text-rotate.");
        XCTAssertEqualObjects(layer.textRotation, styleValue,
                              @"textRotation should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textRotation = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextRotate(), propertyValue,
                       @"Setting textRotation to a function should update text-rotate.");
        XCTAssertEqualObjects(layer.textRotation, styleValue,
                              @"textRotation should round-trip functions.");
        
        layer.textRotation = nil;
        XCTAssertTrue(rawLayer->getTextRotate().isUndefined(),
                      @"Unsetting textRotation should return text-rotate to the default value.");
        XCTAssertEqualObjects(layer.textRotation, defaultStyleValue,
                              @"textRotation should return the default value after being unset.");
    }
    
    // text-rotation-alignment
    {
        XCTAssertTrue(rawLayer->getTextRotationAlignment().isUndefined(),
                      @"text-rotation-alignment should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.textRotationAlignment;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLTextRotationAlignment:MGLTextRotationAlignmentAuto]];
        layer.textRotationAlignment = styleValue;
        mbgl::style::PropertyValue<mbgl::style::AlignmentType> propertyValue = { mbgl::style::AlignmentType::Auto };
        XCTAssertEqual(rawLayer->getTextRotationAlignment(), propertyValue,
                       @"Setting textRotationAlignment to a constant value should update text-rotation-alignment.");
        XCTAssertEqualObjects(layer.textRotationAlignment, styleValue,
                              @"textRotationAlignment should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textRotationAlignment = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::style::AlignmentType> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextRotationAlignment(), propertyValue,
                       @"Setting textRotationAlignment to a function should update text-rotation-alignment.");
        XCTAssertEqualObjects(layer.textRotationAlignment, styleValue,
                              @"textRotationAlignment should round-trip functions.");
        
        layer.textRotationAlignment = nil;
        XCTAssertTrue(rawLayer->getTextRotationAlignment().isUndefined(),
                      @"Unsetting textRotationAlignment should return text-rotation-alignment to the default value.");
        XCTAssertEqualObjects(layer.textRotationAlignment, defaultStyleValue,
                              @"textRotationAlignment should return the default value after being unset.");
    }
    
    // text-transform
    {
        XCTAssertTrue(rawLayer->getTextTransform().isUndefined(),
                      @"text-transform should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.textTransform;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLTextTransform:MGLTextTransformLowercase]];
        layer.textTransform = styleValue;
        mbgl::style::PropertyValue<mbgl::style::TextTransformType> propertyValue = { mbgl::style::TextTransformType::Lowercase };
        XCTAssertEqual(rawLayer->getTextTransform(), propertyValue,
                       @"Setting textTransform to a constant value should update text-transform.");
        XCTAssertEqualObjects(layer.textTransform, styleValue,
                              @"textTransform should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textTransform = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::style::TextTransformType> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextTransform(), propertyValue,
                       @"Setting textTransform to a function should update text-transform.");
        XCTAssertEqualObjects(layer.textTransform, styleValue,
                              @"textTransform should round-trip functions.");
        
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
        
        MGLStyleValue<MGLColor *> *styleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.iconColor = styleValue;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getIconColor(), propertyValue,
                       @"Setting iconColor to a constant value should update icon-color.");
        XCTAssertEqualObjects(layer.iconColor, styleValue,
                              @"iconColor should round-trip constant values.");
        
        styleValue = [MGLStyleValue<MGLColor *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconColor = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::Color> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconColor(), propertyValue,
                       @"Setting iconColor to a function should update icon-color.");
        XCTAssertEqualObjects(layer.iconColor, styleValue,
                              @"iconColor should round-trip functions.");
        
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
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.iconHaloBlur = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getIconHaloBlur(), propertyValue,
                       @"Setting iconHaloBlur to a constant value should update icon-halo-blur.");
        XCTAssertEqualObjects(layer.iconHaloBlur, styleValue,
                              @"iconHaloBlur should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconHaloBlur = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconHaloBlur(), propertyValue,
                       @"Setting iconHaloBlur to a function should update icon-halo-blur.");
        XCTAssertEqualObjects(layer.iconHaloBlur, styleValue,
                              @"iconHaloBlur should round-trip functions.");
        
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
        
        MGLStyleValue<MGLColor *> *styleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.iconHaloColor = styleValue;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getIconHaloColor(), propertyValue,
                       @"Setting iconHaloColor to a constant value should update icon-halo-color.");
        XCTAssertEqualObjects(layer.iconHaloColor, styleValue,
                              @"iconHaloColor should round-trip constant values.");
        
        styleValue = [MGLStyleValue<MGLColor *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconHaloColor = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::Color> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconHaloColor(), propertyValue,
                       @"Setting iconHaloColor to a function should update icon-halo-color.");
        XCTAssertEqualObjects(layer.iconHaloColor, styleValue,
                              @"iconHaloColor should round-trip functions.");
        
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
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.iconHaloWidth = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getIconHaloWidth(), propertyValue,
                       @"Setting iconHaloWidth to a constant value should update icon-halo-width.");
        XCTAssertEqualObjects(layer.iconHaloWidth, styleValue,
                              @"iconHaloWidth should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconHaloWidth = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconHaloWidth(), propertyValue,
                       @"Setting iconHaloWidth to a function should update icon-halo-width.");
        XCTAssertEqualObjects(layer.iconHaloWidth, styleValue,
                              @"iconHaloWidth should round-trip functions.");
        
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
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.iconOpacity = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getIconOpacity(), propertyValue,
                       @"Setting iconOpacity to a constant value should update icon-opacity.");
        XCTAssertEqualObjects(layer.iconOpacity, styleValue,
                              @"iconOpacity should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconOpacity = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconOpacity(), propertyValue,
                       @"Setting iconOpacity to a function should update icon-opacity.");
        XCTAssertEqualObjects(layer.iconOpacity, styleValue,
                              @"iconOpacity should round-trip functions.");
        
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
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.iconTranslation = styleValue;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getIconTranslate(), propertyValue,
                       @"Setting iconTranslation to a constant value should update icon-translate.");
        XCTAssertEqualObjects(layer.iconTranslation, styleValue,
                              @"iconTranslation should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconTranslation = styleValue;
        propertyValue = { mbgl::style::Function<std::array<float, 2>> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconTranslate(), propertyValue,
                       @"Setting iconTranslation to a function should update icon-translate.");
        XCTAssertEqualObjects(layer.iconTranslation, styleValue,
                              @"iconTranslation should round-trip functions.");
        
        layer.iconTranslation = nil;
        XCTAssertTrue(rawLayer->getIconTranslate().isUndefined(),
                      @"Unsetting iconTranslation should return icon-translate to the default value.");
        XCTAssertEqualObjects(layer.iconTranslation, defaultStyleValue,
                              @"iconTranslation should return the default value after being unset.");
    }
    
    // icon-translate-anchor
    {
        XCTAssertTrue(rawLayer->getIconTranslateAnchor().isUndefined(),
                      @"icon-translate-anchor should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.iconTranslationAnchor;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLIconTranslationAnchor:MGLIconTranslationAnchorViewport]];
        layer.iconTranslationAnchor = styleValue;
        mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType> propertyValue = { mbgl::style::TranslateAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getIconTranslateAnchor(), propertyValue,
                       @"Setting iconTranslationAnchor to a constant value should update icon-translate-anchor.");
        XCTAssertEqualObjects(layer.iconTranslationAnchor, styleValue,
                              @"iconTranslationAnchor should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.iconTranslationAnchor = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::style::TranslateAnchorType> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getIconTranslateAnchor(), propertyValue,
                       @"Setting iconTranslationAnchor to a function should update icon-translate-anchor.");
        XCTAssertEqualObjects(layer.iconTranslationAnchor, styleValue,
                              @"iconTranslationAnchor should round-trip functions.");
        
        layer.iconTranslationAnchor = nil;
        XCTAssertTrue(rawLayer->getIconTranslateAnchor().isUndefined(),
                      @"Unsetting iconTranslationAnchor should return icon-translate-anchor to the default value.");
        XCTAssertEqualObjects(layer.iconTranslationAnchor, defaultStyleValue,
                              @"iconTranslationAnchor should return the default value after being unset.");
    }
    
    // text-color
    {
        XCTAssertTrue(rawLayer->getTextColor().isUndefined(),
                      @"text-color should be unset initially.");
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.textColor;
        
        MGLStyleValue<MGLColor *> *styleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.textColor = styleValue;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getTextColor(), propertyValue,
                       @"Setting textColor to a constant value should update text-color.");
        XCTAssertEqualObjects(layer.textColor, styleValue,
                              @"textColor should round-trip constant values.");
        
        styleValue = [MGLStyleValue<MGLColor *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textColor = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::Color> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextColor(), propertyValue,
                       @"Setting textColor to a function should update text-color.");
        XCTAssertEqualObjects(layer.textColor, styleValue,
                              @"textColor should round-trip functions.");
        
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
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.textHaloBlur = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextHaloBlur(), propertyValue,
                       @"Setting textHaloBlur to a constant value should update text-halo-blur.");
        XCTAssertEqualObjects(layer.textHaloBlur, styleValue,
                              @"textHaloBlur should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textHaloBlur = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextHaloBlur(), propertyValue,
                       @"Setting textHaloBlur to a function should update text-halo-blur.");
        XCTAssertEqualObjects(layer.textHaloBlur, styleValue,
                              @"textHaloBlur should round-trip functions.");
        
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
        
        MGLStyleValue<MGLColor *> *styleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.textHaloColor = styleValue;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getTextHaloColor(), propertyValue,
                       @"Setting textHaloColor to a constant value should update text-halo-color.");
        XCTAssertEqualObjects(layer.textHaloColor, styleValue,
                              @"textHaloColor should round-trip constant values.");
        
        styleValue = [MGLStyleValue<MGLColor *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textHaloColor = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::Color> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextHaloColor(), propertyValue,
                       @"Setting textHaloColor to a function should update text-halo-color.");
        XCTAssertEqualObjects(layer.textHaloColor, styleValue,
                              @"textHaloColor should round-trip functions.");
        
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
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.textHaloWidth = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextHaloWidth(), propertyValue,
                       @"Setting textHaloWidth to a constant value should update text-halo-width.");
        XCTAssertEqualObjects(layer.textHaloWidth, styleValue,
                              @"textHaloWidth should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textHaloWidth = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextHaloWidth(), propertyValue,
                       @"Setting textHaloWidth to a function should update text-halo-width.");
        XCTAssertEqualObjects(layer.textHaloWidth, styleValue,
                              @"textHaloWidth should round-trip functions.");
        
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
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.textOpacity = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getTextOpacity(), propertyValue,
                       @"Setting textOpacity to a constant value should update text-opacity.");
        XCTAssertEqualObjects(layer.textOpacity, styleValue,
                              @"textOpacity should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textOpacity = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextOpacity(), propertyValue,
                       @"Setting textOpacity to a function should update text-opacity.");
        XCTAssertEqualObjects(layer.textOpacity, styleValue,
                              @"textOpacity should round-trip functions.");
        
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
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.textTranslation = styleValue;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getTextTranslate(), propertyValue,
                       @"Setting textTranslation to a constant value should update text-translate.");
        XCTAssertEqualObjects(layer.textTranslation, styleValue,
                              @"textTranslation should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textTranslation = styleValue;
        propertyValue = { mbgl::style::Function<std::array<float, 2>> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextTranslate(), propertyValue,
                       @"Setting textTranslation to a function should update text-translate.");
        XCTAssertEqualObjects(layer.textTranslation, styleValue,
                              @"textTranslation should round-trip functions.");
        
        layer.textTranslation = nil;
        XCTAssertTrue(rawLayer->getTextTranslate().isUndefined(),
                      @"Unsetting textTranslation should return text-translate to the default value.");
        XCTAssertEqualObjects(layer.textTranslation, defaultStyleValue,
                              @"textTranslation should return the default value after being unset.");
    }
    
    // text-translate-anchor
    {
        XCTAssertTrue(rawLayer->getTextTranslateAnchor().isUndefined(),
                      @"text-translate-anchor should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.textTranslationAnchor;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLTextTranslationAnchor:MGLTextTranslationAnchorViewport]];
        layer.textTranslationAnchor = styleValue;
        mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType> propertyValue = { mbgl::style::TranslateAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getTextTranslateAnchor(), propertyValue,
                       @"Setting textTranslationAnchor to a constant value should update text-translate-anchor.");
        XCTAssertEqualObjects(layer.textTranslationAnchor, styleValue,
                              @"textTranslationAnchor should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.textTranslationAnchor = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::style::TranslateAnchorType> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getTextTranslateAnchor(), propertyValue,
                       @"Setting textTranslationAnchor to a function should update text-translate-anchor.");
        XCTAssertEqualObjects(layer.textTranslationAnchor, styleValue,
                              @"textTranslationAnchor should round-trip functions.");
        
        layer.textTranslationAnchor = nil;
        XCTAssertTrue(rawLayer->getTextTranslateAnchor().isUndefined(),
                      @"Unsetting textTranslationAnchor should return text-translate-anchor to the default value.");
        XCTAssertEqualObjects(layer.textTranslationAnchor, defaultStyleValue,
                              @"textTranslationAnchor should return the default value after being unset.");
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
