// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/circle_layer.hpp>

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
    
    // circle-blur
    {
        XCTAssertTrue(rawLayer->getCircleBlur().isUndefined(),
                      @"circle-blur should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.circleBlur;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.circleBlur = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getCircleBlur(), propertyValue,
                       @"Setting circleBlur to a constant value should update circle-blur.");
        XCTAssertEqualObjects(layer.circleBlur, styleValue,
                              @"circleBlur should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.circleBlur = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getCircleBlur(), propertyValue,
                       @"Setting circleBlur to a function should update circle-blur.");
        XCTAssertEqualObjects(layer.circleBlur, styleValue,
                              @"circleBlur should round-trip functions.");
        
        layer.circleBlur = nil;
        XCTAssertTrue(rawLayer->getCircleBlur().isUndefined(),
                      @"Unsetting circleBlur should return circle-blur to the default value.");
        XCTAssertEqualObjects(layer.circleBlur, defaultStyleValue,
                              @"circleBlur should return the default value after being unset.");
    }
    
    // circle-color
    {
        XCTAssertTrue(rawLayer->getCircleColor().isUndefined(),
                      @"circle-color should be unset initially.");
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.circleColor;
        
        MGLStyleValue<MGLColor *> *styleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.circleColor = styleValue;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { .r = 1, .g = 0, .b = 0, .a = 1 } };
        XCTAssertEqual(rawLayer->getCircleColor(), propertyValue,
                       @"Setting circleColor to a constant value should update circle-color.");
        XCTAssertEqualObjects(layer.circleColor, styleValue,
                              @"circleColor should round-trip constant values.");
        
        styleValue = [MGLStyleValue<MGLColor *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.circleColor = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::Color> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getCircleColor(), propertyValue,
                       @"Setting circleColor to a function should update circle-color.");
        XCTAssertEqualObjects(layer.circleColor, styleValue,
                              @"circleColor should round-trip functions.");
        
        layer.circleColor = nil;
        XCTAssertTrue(rawLayer->getCircleColor().isUndefined(),
                      @"Unsetting circleColor should return circle-color to the default value.");
        XCTAssertEqualObjects(layer.circleColor, defaultStyleValue,
                              @"circleColor should return the default value after being unset.");
    }
    
    // circle-opacity
    {
        XCTAssertTrue(rawLayer->getCircleOpacity().isUndefined(),
                      @"circle-opacity should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.circleOpacity;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.circleOpacity = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getCircleOpacity(), propertyValue,
                       @"Setting circleOpacity to a constant value should update circle-opacity.");
        XCTAssertEqualObjects(layer.circleOpacity, styleValue,
                              @"circleOpacity should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.circleOpacity = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getCircleOpacity(), propertyValue,
                       @"Setting circleOpacity to a function should update circle-opacity.");
        XCTAssertEqualObjects(layer.circleOpacity, styleValue,
                              @"circleOpacity should round-trip functions.");
        
        layer.circleOpacity = nil;
        XCTAssertTrue(rawLayer->getCircleOpacity().isUndefined(),
                      @"Unsetting circleOpacity should return circle-opacity to the default value.");
        XCTAssertEqualObjects(layer.circleOpacity, defaultStyleValue,
                              @"circleOpacity should return the default value after being unset.");
    }
    
    // circle-radius
    {
        XCTAssertTrue(rawLayer->getCircleRadius().isUndefined(),
                      @"circle-radius should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.circleRadius;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.circleRadius = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getCircleRadius(), propertyValue,
                       @"Setting circleRadius to a constant value should update circle-radius.");
        XCTAssertEqualObjects(layer.circleRadius, styleValue,
                              @"circleRadius should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.circleRadius = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getCircleRadius(), propertyValue,
                       @"Setting circleRadius to a function should update circle-radius.");
        XCTAssertEqualObjects(layer.circleRadius, styleValue,
                              @"circleRadius should round-trip functions.");
        
        layer.circleRadius = nil;
        XCTAssertTrue(rawLayer->getCircleRadius().isUndefined(),
                      @"Unsetting circleRadius should return circle-radius to the default value.");
        XCTAssertEqualObjects(layer.circleRadius, defaultStyleValue,
                              @"circleRadius should return the default value after being unset.");
    }
    
    // circle-pitch-scale
    {
        XCTAssertTrue(rawLayer->getCirclePitchScale().isUndefined(),
                      @"circle-pitch-scale should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.circleScaleAlignment;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLCircleScaleAlignment:MGLCircleScaleAlignmentViewport]];
        layer.circleScaleAlignment = styleValue;
        mbgl::style::PropertyValue<mbgl::style::CirclePitchScaleType> propertyValue = { mbgl::style::CirclePitchScaleType::Viewport };
        XCTAssertEqual(rawLayer->getCirclePitchScale(), propertyValue,
                       @"Setting circleScaleAlignment to a constant value should update circle-pitch-scale.");
        XCTAssertEqualObjects(layer.circleScaleAlignment, styleValue,
                              @"circleScaleAlignment should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.circleScaleAlignment = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::style::CirclePitchScaleType> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getCirclePitchScale(), propertyValue,
                       @"Setting circleScaleAlignment to a function should update circle-pitch-scale.");
        XCTAssertEqualObjects(layer.circleScaleAlignment, styleValue,
                              @"circleScaleAlignment should round-trip functions.");
        
        layer.circleScaleAlignment = nil;
        XCTAssertTrue(rawLayer->getCirclePitchScale().isUndefined(),
                      @"Unsetting circleScaleAlignment should return circle-pitch-scale to the default value.");
        XCTAssertEqualObjects(layer.circleScaleAlignment, defaultStyleValue,
                              @"circleScaleAlignment should return the default value after being unset.");
    }
    
    // circle-translate
    {
        XCTAssertTrue(rawLayer->getCircleTranslate().isUndefined(),
                      @"circle-translate should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.circleTranslation;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.circleTranslation = styleValue;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getCircleTranslate(), propertyValue,
                       @"Setting circleTranslation to a constant value should update circle-translate.");
        XCTAssertEqualObjects(layer.circleTranslation, styleValue,
                              @"circleTranslation should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.circleTranslation = styleValue;
        propertyValue = { mbgl::style::Function<std::array<float, 2>> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getCircleTranslate(), propertyValue,
                       @"Setting circleTranslation to a function should update circle-translate.");
        XCTAssertEqualObjects(layer.circleTranslation, styleValue,
                              @"circleTranslation should round-trip functions.");
        
        layer.circleTranslation = nil;
        XCTAssertTrue(rawLayer->getCircleTranslate().isUndefined(),
                      @"Unsetting circleTranslation should return circle-translate to the default value.");
        XCTAssertEqualObjects(layer.circleTranslation, defaultStyleValue,
                              @"circleTranslation should return the default value after being unset.");
    }
    
    // circle-translate-anchor
    {
        XCTAssertTrue(rawLayer->getCircleTranslateAnchor().isUndefined(),
                      @"circle-translate-anchor should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.circleTranslationAnchor;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLCircleTranslationAnchor:MGLCircleTranslationAnchorViewport]];
        layer.circleTranslationAnchor = styleValue;
        mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType> propertyValue = { mbgl::style::TranslateAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getCircleTranslateAnchor(), propertyValue,
                       @"Setting circleTranslationAnchor to a constant value should update circle-translate-anchor.");
        XCTAssertEqualObjects(layer.circleTranslationAnchor, styleValue,
                              @"circleTranslationAnchor should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.circleTranslationAnchor = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::style::TranslateAnchorType> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getCircleTranslateAnchor(), propertyValue,
                       @"Setting circleTranslationAnchor to a function should update circle-translate-anchor.");
        XCTAssertEqualObjects(layer.circleTranslationAnchor, styleValue,
                              @"circleTranslationAnchor should round-trip functions.");
        
        layer.circleTranslationAnchor = nil;
        XCTAssertTrue(rawLayer->getCircleTranslateAnchor().isUndefined(),
                      @"Unsetting circleTranslationAnchor should return circle-translate-anchor to the default value.");
        XCTAssertEqualObjects(layer.circleTranslationAnchor, defaultStyleValue,
                              @"circleTranslationAnchor should return the default value after being unset.");
    }
}

- (void)testPropertyNames {
    [self testPropertyName:@"circle-blur" isBoolean:NO];
    [self testPropertyName:@"circle-color" isBoolean:NO];
    [self testPropertyName:@"circle-opacity" isBoolean:NO];
    [self testPropertyName:@"circle-radius" isBoolean:NO];
    [self testPropertyName:@"circle-scale-alignment" isBoolean:NO];
    [self testPropertyName:@"circle-translation" isBoolean:NO];
    [self testPropertyName:@"circle-translation-anchor" isBoolean:NO];
}

- (void)testValueAdditions {
    XCTAssertEqual([NSValue valueWithMGLCircleScaleAlignment:MGLCircleScaleAlignmentMap].MGLCircleScaleAlignmentValue, MGLCircleScaleAlignmentMap);
    XCTAssertEqual([NSValue valueWithMGLCircleScaleAlignment:MGLCircleScaleAlignmentViewport].MGLCircleScaleAlignmentValue, MGLCircleScaleAlignmentViewport);
    XCTAssertEqual([NSValue valueWithMGLCircleTranslationAnchor:MGLCircleTranslationAnchorMap].MGLCircleTranslationAnchorValue, MGLCircleTranslationAnchorMap);
    XCTAssertEqual([NSValue valueWithMGLCircleTranslationAnchor:MGLCircleTranslationAnchorViewport].MGLCircleTranslationAnchorValue, MGLCircleTranslationAnchorViewport);
}

@end
