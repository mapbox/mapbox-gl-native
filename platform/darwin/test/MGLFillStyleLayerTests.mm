// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/fill_layer.hpp>

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
    
    // fill-antialias
    {
        XCTAssertTrue(rawLayer->getFillAntialias().isUndefined(),
                      @"fill-antialias should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.fillAntialiased;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@NO];
        layer.fillAntialiased = styleValue;
        mbgl::style::PropertyValue<bool> propertyValue = { false };
        XCTAssertEqual(rawLayer->getFillAntialias(), propertyValue,
                       @"Setting fillAntialiased to a constant value should update fill-antialias.");
        XCTAssertEqualObjects(layer.fillAntialiased, styleValue,
                              @"fillAntialiased should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.fillAntialiased = styleValue;
        propertyValue = { mbgl::style::Function<bool> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getFillAntialias(), propertyValue,
                       @"Setting fillAntialiased to a function should update fill-antialias.");
        XCTAssertEqualObjects(layer.fillAntialiased, styleValue,
                              @"fillAntialiased should round-trip functions.");
        
        layer.fillAntialiased = nil;
        XCTAssertTrue(rawLayer->getFillAntialias().isUndefined(),
                      @"Unsetting fillAntialiased should return fill-antialias to the default value.");
        XCTAssertEqualObjects(layer.fillAntialiased, defaultStyleValue,
                              @"fillAntialiased should return the default value after being unset.");
    }
    
    // fill-color
    {
        XCTAssertTrue(rawLayer->getFillColor().isUndefined(),
                      @"fill-color should be unset initially.");
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.fillColor;
        
        MGLStyleValue<MGLColor *> *styleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.fillColor = styleValue;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getFillColor(), propertyValue,
                       @"Setting fillColor to a constant value should update fill-color.");
        XCTAssertEqualObjects(layer.fillColor, styleValue,
                              @"fillColor should round-trip constant values.");
        
        styleValue = [MGLStyleValue<MGLColor *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.fillColor = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::Color> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getFillColor(), propertyValue,
                       @"Setting fillColor to a function should update fill-color.");
        XCTAssertEqualObjects(layer.fillColor, styleValue,
                              @"fillColor should round-trip functions.");
        
        layer.fillColor = nil;
        XCTAssertTrue(rawLayer->getFillColor().isUndefined(),
                      @"Unsetting fillColor should return fill-color to the default value.");
        XCTAssertEqualObjects(layer.fillColor, defaultStyleValue,
                              @"fillColor should return the default value after being unset.");
    }
    
    // fill-opacity
    {
        XCTAssertTrue(rawLayer->getFillOpacity().isUndefined(),
                      @"fill-opacity should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.fillOpacity;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.fillOpacity = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getFillOpacity(), propertyValue,
                       @"Setting fillOpacity to a constant value should update fill-opacity.");
        XCTAssertEqualObjects(layer.fillOpacity, styleValue,
                              @"fillOpacity should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.fillOpacity = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getFillOpacity(), propertyValue,
                       @"Setting fillOpacity to a function should update fill-opacity.");
        XCTAssertEqualObjects(layer.fillOpacity, styleValue,
                              @"fillOpacity should round-trip functions.");
        
        layer.fillOpacity = nil;
        XCTAssertTrue(rawLayer->getFillOpacity().isUndefined(),
                      @"Unsetting fillOpacity should return fill-opacity to the default value.");
        XCTAssertEqualObjects(layer.fillOpacity, defaultStyleValue,
                              @"fillOpacity should return the default value after being unset.");
    }
    
    // fill-outline-color
    {
        XCTAssertTrue(rawLayer->getFillOutlineColor().isUndefined(),
                      @"fill-outline-color should be unset initially.");
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.fillOutlineColor;
        
        MGLStyleValue<MGLColor *> *styleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.fillOutlineColor = styleValue;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getFillOutlineColor(), propertyValue,
                       @"Setting fillOutlineColor to a constant value should update fill-outline-color.");
        XCTAssertEqualObjects(layer.fillOutlineColor, styleValue,
                              @"fillOutlineColor should round-trip constant values.");
        
        styleValue = [MGLStyleValue<MGLColor *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.fillOutlineColor = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::Color> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getFillOutlineColor(), propertyValue,
                       @"Setting fillOutlineColor to a function should update fill-outline-color.");
        XCTAssertEqualObjects(layer.fillOutlineColor, styleValue,
                              @"fillOutlineColor should round-trip functions.");
        
        layer.fillOutlineColor = nil;
        XCTAssertTrue(rawLayer->getFillOutlineColor().isUndefined(),
                      @"Unsetting fillOutlineColor should return fill-outline-color to the default value.");
        XCTAssertEqualObjects(layer.fillOutlineColor, defaultStyleValue,
                              @"fillOutlineColor should return the default value after being unset.");
    }
    
    // fill-pattern
    {
        XCTAssertTrue(rawLayer->getFillPattern().isUndefined(),
                      @"fill-pattern should be unset initially.");
        MGLStyleValue<NSString *> *defaultStyleValue = layer.fillPattern;
        
        MGLStyleValue<NSString *> *styleValue = [MGLStyleValue<NSString *> valueWithRawValue:@"Fill Pattern"];
        layer.fillPattern = styleValue;
        mbgl::style::PropertyValue<std::string> propertyValue = { "Fill Pattern" };
        XCTAssertEqual(rawLayer->getFillPattern(), propertyValue,
                       @"Setting fillPattern to a constant value should update fill-pattern.");
        XCTAssertEqualObjects(layer.fillPattern, styleValue,
                              @"fillPattern should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSString *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.fillPattern = styleValue;
        propertyValue = { mbgl::style::Function<std::string> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getFillPattern(), propertyValue,
                       @"Setting fillPattern to a function should update fill-pattern.");
        XCTAssertEqualObjects(layer.fillPattern, styleValue,
                              @"fillPattern should round-trip functions.");
        
        layer.fillPattern = nil;
        XCTAssertTrue(rawLayer->getFillPattern().isUndefined(),
                      @"Unsetting fillPattern should return fill-pattern to the default value.");
        XCTAssertEqualObjects(layer.fillPattern, defaultStyleValue,
                              @"fillPattern should return the default value after being unset.");
    }
    
    // fill-translate
    {
        XCTAssertTrue(rawLayer->getFillTranslate().isUndefined(),
                      @"fill-translate should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.fillTranslation;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.fillTranslation = styleValue;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getFillTranslate(), propertyValue,
                       @"Setting fillTranslation to a constant value should update fill-translate.");
        XCTAssertEqualObjects(layer.fillTranslation, styleValue,
                              @"fillTranslation should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.fillTranslation = styleValue;
        propertyValue = { mbgl::style::Function<std::array<float, 2>> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getFillTranslate(), propertyValue,
                       @"Setting fillTranslation to a function should update fill-translate.");
        XCTAssertEqualObjects(layer.fillTranslation, styleValue,
                              @"fillTranslation should round-trip functions.");
        
        layer.fillTranslation = nil;
        XCTAssertTrue(rawLayer->getFillTranslate().isUndefined(),
                      @"Unsetting fillTranslation should return fill-translate to the default value.");
        XCTAssertEqualObjects(layer.fillTranslation, defaultStyleValue,
                              @"fillTranslation should return the default value after being unset.");
    }
    
    // fill-translate-anchor
    {
        XCTAssertTrue(rawLayer->getFillTranslateAnchor().isUndefined(),
                      @"fill-translate-anchor should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.fillTranslationAnchor;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLFillTranslationAnchor:MGLFillTranslationAnchorViewport]];
        layer.fillTranslationAnchor = styleValue;
        mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType> propertyValue = { mbgl::style::TranslateAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getFillTranslateAnchor(), propertyValue,
                       @"Setting fillTranslationAnchor to a constant value should update fill-translate-anchor.");
        XCTAssertEqualObjects(layer.fillTranslationAnchor, styleValue,
                              @"fillTranslationAnchor should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.fillTranslationAnchor = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::style::TranslateAnchorType> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getFillTranslateAnchor(), propertyValue,
                       @"Setting fillTranslationAnchor to a function should update fill-translate-anchor.");
        XCTAssertEqualObjects(layer.fillTranslationAnchor, styleValue,
                              @"fillTranslationAnchor should round-trip functions.");
        
        layer.fillTranslationAnchor = nil;
        XCTAssertTrue(rawLayer->getFillTranslateAnchor().isUndefined(),
                      @"Unsetting fillTranslationAnchor should return fill-translate-anchor to the default value.");
        XCTAssertEqualObjects(layer.fillTranslationAnchor, defaultStyleValue,
                              @"fillTranslationAnchor should return the default value after being unset.");
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
