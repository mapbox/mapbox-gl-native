// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

#import "MGLStyleLayer_Private.h"

#include <mbgl/style/layers/line_layer.hpp>

@interface MGLLineLayerTests : MGLStyleLayerTests
@end

@implementation MGLLineLayerTests

+ (NSString *)layerType {
    return @"line";
}

- (void)testPredicates {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" shape:feature options:nil];
    MGLLineStyleLayer *layer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    
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
    
    MGLLineStyleLayer *layer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    XCTAssertNotEqual(layer.rawLayer, nullptr);
    XCTAssertTrue(layer.rawLayer->is<mbgl::style::LineLayer>());
    auto rawLayer = layer.rawLayer->as<mbgl::style::LineLayer>();
    
    // line-cap
    {
        XCTAssertTrue(rawLayer->getLineCap().isUndefined(),
                      @"line-cap should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.lineCap;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLLineCap:MGLLineCapSquare]];
        layer.lineCap = styleValue;
        mbgl::style::PropertyValue<mbgl::style::LineCapType> propertyValue = { mbgl::style::LineCapType::Square };
        XCTAssertEqual(rawLayer->getLineCap(), propertyValue,
                       @"Setting lineCap to a constant value should update line-cap.");
        XCTAssertEqualObjects(layer.lineCap, styleValue,
                              @"lineCap should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.lineCap = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::style::LineCapType> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getLineCap(), propertyValue,
                       @"Setting lineCap to a function should update line-cap.");
        XCTAssertEqualObjects(layer.lineCap, styleValue,
                              @"lineCap should round-trip functions.");
        
        layer.lineCap = nil;
        XCTAssertTrue(rawLayer->getLineCap().isUndefined(),
                      @"Unsetting lineCap should return line-cap to the default value.");
        XCTAssertEqualObjects(layer.lineCap, defaultStyleValue,
                              @"lineCap should return the default value after being unset.");
    }
    
    // line-join
    {
        XCTAssertTrue(rawLayer->getLineJoin().isUndefined(),
                      @"line-join should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.lineJoin;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLLineJoin:MGLLineJoinMiter]];
        layer.lineJoin = styleValue;
        mbgl::style::PropertyValue<mbgl::style::LineJoinType> propertyValue = { mbgl::style::LineJoinType::Miter };
        XCTAssertEqual(rawLayer->getLineJoin(), propertyValue,
                       @"Setting lineJoin to a constant value should update line-join.");
        XCTAssertEqualObjects(layer.lineJoin, styleValue,
                              @"lineJoin should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.lineJoin = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::style::LineJoinType> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getLineJoin(), propertyValue,
                       @"Setting lineJoin to a function should update line-join.");
        XCTAssertEqualObjects(layer.lineJoin, styleValue,
                              @"lineJoin should round-trip functions.");
        
        layer.lineJoin = nil;
        XCTAssertTrue(rawLayer->getLineJoin().isUndefined(),
                      @"Unsetting lineJoin should return line-join to the default value.");
        XCTAssertEqualObjects(layer.lineJoin, defaultStyleValue,
                              @"lineJoin should return the default value after being unset.");
    }
    
    // line-miter-limit
    {
        XCTAssertTrue(rawLayer->getLineMiterLimit().isUndefined(),
                      @"line-miter-limit should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.lineMiterLimit;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.lineMiterLimit = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineMiterLimit(), propertyValue,
                       @"Setting lineMiterLimit to a constant value should update line-miter-limit.");
        XCTAssertEqualObjects(layer.lineMiterLimit, styleValue,
                              @"lineMiterLimit should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.lineMiterLimit = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getLineMiterLimit(), propertyValue,
                       @"Setting lineMiterLimit to a function should update line-miter-limit.");
        XCTAssertEqualObjects(layer.lineMiterLimit, styleValue,
                              @"lineMiterLimit should round-trip functions.");
        
        layer.lineMiterLimit = nil;
        XCTAssertTrue(rawLayer->getLineMiterLimit().isUndefined(),
                      @"Unsetting lineMiterLimit should return line-miter-limit to the default value.");
        XCTAssertEqualObjects(layer.lineMiterLimit, defaultStyleValue,
                              @"lineMiterLimit should return the default value after being unset.");
    }
    
    // line-round-limit
    {
        XCTAssertTrue(rawLayer->getLineRoundLimit().isUndefined(),
                      @"line-round-limit should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.lineRoundLimit;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.lineRoundLimit = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineRoundLimit(), propertyValue,
                       @"Setting lineRoundLimit to a constant value should update line-round-limit.");
        XCTAssertEqualObjects(layer.lineRoundLimit, styleValue,
                              @"lineRoundLimit should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.lineRoundLimit = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getLineRoundLimit(), propertyValue,
                       @"Setting lineRoundLimit to a function should update line-round-limit.");
        XCTAssertEqualObjects(layer.lineRoundLimit, styleValue,
                              @"lineRoundLimit should round-trip functions.");
        
        layer.lineRoundLimit = nil;
        XCTAssertTrue(rawLayer->getLineRoundLimit().isUndefined(),
                      @"Unsetting lineRoundLimit should return line-round-limit to the default value.");
        XCTAssertEqualObjects(layer.lineRoundLimit, defaultStyleValue,
                              @"lineRoundLimit should return the default value after being unset.");
    }
    
    // line-blur
    {
        XCTAssertTrue(rawLayer->getLineBlur().isUndefined(),
                      @"line-blur should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.lineBlur;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.lineBlur = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineBlur(), propertyValue,
                       @"Setting lineBlur to a constant value should update line-blur.");
        XCTAssertEqualObjects(layer.lineBlur, styleValue,
                              @"lineBlur should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.lineBlur = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getLineBlur(), propertyValue,
                       @"Setting lineBlur to a function should update line-blur.");
        XCTAssertEqualObjects(layer.lineBlur, styleValue,
                              @"lineBlur should round-trip functions.");
        
        layer.lineBlur = nil;
        XCTAssertTrue(rawLayer->getLineBlur().isUndefined(),
                      @"Unsetting lineBlur should return line-blur to the default value.");
        XCTAssertEqualObjects(layer.lineBlur, defaultStyleValue,
                              @"lineBlur should return the default value after being unset.");
    }
    
    // line-color
    {
        XCTAssertTrue(rawLayer->getLineColor().isUndefined(),
                      @"line-color should be unset initially.");
        MGLStyleValue<MGLColor *> *defaultStyleValue = layer.lineColor;
        
        MGLStyleValue<MGLColor *> *styleValue = [MGLStyleValue<MGLColor *> valueWithRawValue:[MGLColor redColor]];
        layer.lineColor = styleValue;
        mbgl::style::PropertyValue<mbgl::Color> propertyValue = { { 1, 0, 0, 1 } };
        XCTAssertEqual(rawLayer->getLineColor(), propertyValue,
                       @"Setting lineColor to a constant value should update line-color.");
        XCTAssertEqualObjects(layer.lineColor, styleValue,
                              @"lineColor should round-trip constant values.");
        
        styleValue = [MGLStyleValue<MGLColor *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.lineColor = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::Color> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getLineColor(), propertyValue,
                       @"Setting lineColor to a function should update line-color.");
        XCTAssertEqualObjects(layer.lineColor, styleValue,
                              @"lineColor should round-trip functions.");
        
        layer.lineColor = nil;
        XCTAssertTrue(rawLayer->getLineColor().isUndefined(),
                      @"Unsetting lineColor should return line-color to the default value.");
        XCTAssertEqualObjects(layer.lineColor, defaultStyleValue,
                              @"lineColor should return the default value after being unset.");
    }
    
    // line-dasharray
    {
        XCTAssertTrue(rawLayer->getLineDasharray().isUndefined(),
                      @"line-dasharray should be unset initially.");
        MGLStyleValue<NSArray<NSNumber *> *> *defaultStyleValue = layer.lineDashPattern;
        
        MGLStyleValue<NSArray<NSNumber *> *> *styleValue = [MGLStyleValue<NSArray<NSNumber *> *> valueWithRawValue:@[@1, @2]];
        layer.lineDashPattern = styleValue;
        mbgl::style::PropertyValue<std::vector<float>> propertyValue = { {1, 2} };
        XCTAssertEqual(rawLayer->getLineDasharray(), propertyValue,
                       @"Setting lineDashPattern to a constant value should update line-dasharray.");
        XCTAssertEqualObjects(layer.lineDashPattern, styleValue,
                              @"lineDashPattern should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSArray<NSNumber *> *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.lineDashPattern = styleValue;
        propertyValue = { mbgl::style::Function<std::vector<float>> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getLineDasharray(), propertyValue,
                       @"Setting lineDashPattern to a function should update line-dasharray.");
        XCTAssertEqualObjects(layer.lineDashPattern, styleValue,
                              @"lineDashPattern should round-trip functions.");
        
        layer.lineDashPattern = nil;
        XCTAssertTrue(rawLayer->getLineDasharray().isUndefined(),
                      @"Unsetting lineDashPattern should return line-dasharray to the default value.");
        XCTAssertEqualObjects(layer.lineDashPattern, defaultStyleValue,
                              @"lineDashPattern should return the default value after being unset.");
    }
    
    // line-gap-width
    {
        XCTAssertTrue(rawLayer->getLineGapWidth().isUndefined(),
                      @"line-gap-width should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.lineGapWidth;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.lineGapWidth = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineGapWidth(), propertyValue,
                       @"Setting lineGapWidth to a constant value should update line-gap-width.");
        XCTAssertEqualObjects(layer.lineGapWidth, styleValue,
                              @"lineGapWidth should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.lineGapWidth = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getLineGapWidth(), propertyValue,
                       @"Setting lineGapWidth to a function should update line-gap-width.");
        XCTAssertEqualObjects(layer.lineGapWidth, styleValue,
                              @"lineGapWidth should round-trip functions.");
        
        layer.lineGapWidth = nil;
        XCTAssertTrue(rawLayer->getLineGapWidth().isUndefined(),
                      @"Unsetting lineGapWidth should return line-gap-width to the default value.");
        XCTAssertEqualObjects(layer.lineGapWidth, defaultStyleValue,
                              @"lineGapWidth should return the default value after being unset.");
    }
    
    // line-offset
    {
        XCTAssertTrue(rawLayer->getLineOffset().isUndefined(),
                      @"line-offset should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.lineOffset;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.lineOffset = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineOffset(), propertyValue,
                       @"Setting lineOffset to a constant value should update line-offset.");
        XCTAssertEqualObjects(layer.lineOffset, styleValue,
                              @"lineOffset should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.lineOffset = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getLineOffset(), propertyValue,
                       @"Setting lineOffset to a function should update line-offset.");
        XCTAssertEqualObjects(layer.lineOffset, styleValue,
                              @"lineOffset should round-trip functions.");
        
        layer.lineOffset = nil;
        XCTAssertTrue(rawLayer->getLineOffset().isUndefined(),
                      @"Unsetting lineOffset should return line-offset to the default value.");
        XCTAssertEqualObjects(layer.lineOffset, defaultStyleValue,
                              @"lineOffset should return the default value after being unset.");
    }
    
    // line-opacity
    {
        XCTAssertTrue(rawLayer->getLineOpacity().isUndefined(),
                      @"line-opacity should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.lineOpacity;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.lineOpacity = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineOpacity(), propertyValue,
                       @"Setting lineOpacity to a constant value should update line-opacity.");
        XCTAssertEqualObjects(layer.lineOpacity, styleValue,
                              @"lineOpacity should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.lineOpacity = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getLineOpacity(), propertyValue,
                       @"Setting lineOpacity to a function should update line-opacity.");
        XCTAssertEqualObjects(layer.lineOpacity, styleValue,
                              @"lineOpacity should round-trip functions.");
        
        layer.lineOpacity = nil;
        XCTAssertTrue(rawLayer->getLineOpacity().isUndefined(),
                      @"Unsetting lineOpacity should return line-opacity to the default value.");
        XCTAssertEqualObjects(layer.lineOpacity, defaultStyleValue,
                              @"lineOpacity should return the default value after being unset.");
    }
    
    // line-pattern
    {
        XCTAssertTrue(rawLayer->getLinePattern().isUndefined(),
                      @"line-pattern should be unset initially.");
        MGLStyleValue<NSString *> *defaultStyleValue = layer.linePattern;
        
        MGLStyleValue<NSString *> *styleValue = [MGLStyleValue<NSString *> valueWithRawValue:@"Line Pattern"];
        layer.linePattern = styleValue;
        mbgl::style::PropertyValue<std::string> propertyValue = { "Line Pattern" };
        XCTAssertEqual(rawLayer->getLinePattern(), propertyValue,
                       @"Setting linePattern to a constant value should update line-pattern.");
        XCTAssertEqualObjects(layer.linePattern, styleValue,
                              @"linePattern should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSString *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.linePattern = styleValue;
        propertyValue = { mbgl::style::Function<std::string> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getLinePattern(), propertyValue,
                       @"Setting linePattern to a function should update line-pattern.");
        XCTAssertEqualObjects(layer.linePattern, styleValue,
                              @"linePattern should round-trip functions.");
        
        layer.linePattern = nil;
        XCTAssertTrue(rawLayer->getLinePattern().isUndefined(),
                      @"Unsetting linePattern should return line-pattern to the default value.");
        XCTAssertEqualObjects(layer.linePattern, defaultStyleValue,
                              @"linePattern should return the default value after being unset.");
    }
    
    // line-translate
    {
        XCTAssertTrue(rawLayer->getLineTranslate().isUndefined(),
                      @"line-translate should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.lineTranslation;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:
#if TARGET_OS_IPHONE
            [NSValue valueWithCGVector:CGVectorMake(1, 1)]
#else
            [NSValue valueWithMGLVector:CGVectorMake(1, -1)]
#endif
        ];
        layer.lineTranslation = styleValue;
        mbgl::style::PropertyValue<std::array<float, 2>> propertyValue = { { 1, 1 } };
        XCTAssertEqual(rawLayer->getLineTranslate(), propertyValue,
                       @"Setting lineTranslation to a constant value should update line-translate.");
        XCTAssertEqualObjects(layer.lineTranslation, styleValue,
                              @"lineTranslation should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.lineTranslation = styleValue;
        propertyValue = { mbgl::style::Function<std::array<float, 2>> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getLineTranslate(), propertyValue,
                       @"Setting lineTranslation to a function should update line-translate.");
        XCTAssertEqualObjects(layer.lineTranslation, styleValue,
                              @"lineTranslation should round-trip functions.");
        
        layer.lineTranslation = nil;
        XCTAssertTrue(rawLayer->getLineTranslate().isUndefined(),
                      @"Unsetting lineTranslation should return line-translate to the default value.");
        XCTAssertEqualObjects(layer.lineTranslation, defaultStyleValue,
                              @"lineTranslation should return the default value after being unset.");
    }
    
    // line-translate-anchor
    {
        XCTAssertTrue(rawLayer->getLineTranslateAnchor().isUndefined(),
                      @"line-translate-anchor should be unset initially.");
        MGLStyleValue<NSValue *> *defaultStyleValue = layer.lineTranslationAnchor;
        
        MGLStyleValue<NSValue *> *styleValue = [MGLStyleValue<NSValue *> valueWithRawValue:[NSValue valueWithMGLLineTranslationAnchor:MGLLineTranslationAnchorViewport]];
        layer.lineTranslationAnchor = styleValue;
        mbgl::style::PropertyValue<mbgl::style::TranslateAnchorType> propertyValue = { mbgl::style::TranslateAnchorType::Viewport };
        XCTAssertEqual(rawLayer->getLineTranslateAnchor(), propertyValue,
                       @"Setting lineTranslationAnchor to a constant value should update line-translate-anchor.");
        XCTAssertEqualObjects(layer.lineTranslationAnchor, styleValue,
                              @"lineTranslationAnchor should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSValue *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.lineTranslationAnchor = styleValue;
        propertyValue = { mbgl::style::Function<mbgl::style::TranslateAnchorType> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getLineTranslateAnchor(), propertyValue,
                       @"Setting lineTranslationAnchor to a function should update line-translate-anchor.");
        XCTAssertEqualObjects(layer.lineTranslationAnchor, styleValue,
                              @"lineTranslationAnchor should round-trip functions.");
        
        layer.lineTranslationAnchor = nil;
        XCTAssertTrue(rawLayer->getLineTranslateAnchor().isUndefined(),
                      @"Unsetting lineTranslationAnchor should return line-translate-anchor to the default value.");
        XCTAssertEqualObjects(layer.lineTranslationAnchor, defaultStyleValue,
                              @"lineTranslationAnchor should return the default value after being unset.");
    }
    
    // line-width
    {
        XCTAssertTrue(rawLayer->getLineWidth().isUndefined(),
                      @"line-width should be unset initially.");
        MGLStyleValue<NSNumber *> *defaultStyleValue = layer.lineWidth;
        
        MGLStyleValue<NSNumber *> *styleValue = [MGLStyleValue<NSNumber *> valueWithRawValue:@0xff];
        layer.lineWidth = styleValue;
        mbgl::style::PropertyValue<float> propertyValue = { 0xff };
        XCTAssertEqual(rawLayer->getLineWidth(), propertyValue,
                       @"Setting lineWidth to a constant value should update line-width.");
        XCTAssertEqualObjects(layer.lineWidth, styleValue,
                              @"lineWidth should round-trip constant values.");
        
        styleValue = [MGLStyleValue<NSNumber *> valueWithStops:@{
            @18: styleValue,
        }];
        layer.lineWidth = styleValue;
        propertyValue = { mbgl::style::Function<float> {
            {{ 18, propertyValue.asConstant() }},
            1,
        }};
        XCTAssertEqual(rawLayer->getLineWidth(), propertyValue,
                       @"Setting lineWidth to a function should update line-width.");
        XCTAssertEqualObjects(layer.lineWidth, styleValue,
                              @"lineWidth should round-trip functions.");
        
        layer.lineWidth = nil;
        XCTAssertTrue(rawLayer->getLineWidth().isUndefined(),
                      @"Unsetting lineWidth should return line-width to the default value.");
        XCTAssertEqualObjects(layer.lineWidth, defaultStyleValue,
                              @"lineWidth should return the default value after being unset.");
    }
}

- (void)testPropertyNames {
    [self testPropertyName:@"line-cap" isBoolean:NO];
    [self testPropertyName:@"line-join" isBoolean:NO];
    [self testPropertyName:@"line-miter-limit" isBoolean:NO];
    [self testPropertyName:@"line-round-limit" isBoolean:NO];
    [self testPropertyName:@"line-blur" isBoolean:NO];
    [self testPropertyName:@"line-color" isBoolean:NO];
    [self testPropertyName:@"line-dash-pattern" isBoolean:NO];
    [self testPropertyName:@"line-gap-width" isBoolean:NO];
    [self testPropertyName:@"line-offset" isBoolean:NO];
    [self testPropertyName:@"line-opacity" isBoolean:NO];
    [self testPropertyName:@"line-pattern" isBoolean:NO];
    [self testPropertyName:@"line-translation" isBoolean:NO];
    [self testPropertyName:@"line-translation-anchor" isBoolean:NO];
    [self testPropertyName:@"line-width" isBoolean:NO];
}

- (void)testValueAdditions {
    XCTAssertEqual([NSValue valueWithMGLLineCap:MGLLineCapButt].MGLLineCapValue, MGLLineCapButt);
    XCTAssertEqual([NSValue valueWithMGLLineCap:MGLLineCapRound].MGLLineCapValue, MGLLineCapRound);
    XCTAssertEqual([NSValue valueWithMGLLineCap:MGLLineCapSquare].MGLLineCapValue, MGLLineCapSquare);
    XCTAssertEqual([NSValue valueWithMGLLineJoin:MGLLineJoinBevel].MGLLineJoinValue, MGLLineJoinBevel);
    XCTAssertEqual([NSValue valueWithMGLLineJoin:MGLLineJoinRound].MGLLineJoinValue, MGLLineJoinRound);
    XCTAssertEqual([NSValue valueWithMGLLineJoin:MGLLineJoinMiter].MGLLineJoinValue, MGLLineJoinMiter);
    XCTAssertEqual([NSValue valueWithMGLLineTranslationAnchor:MGLLineTranslationAnchorMap].MGLLineTranslationAnchorValue, MGLLineTranslationAnchorMap);
    XCTAssertEqual([NSValue valueWithMGLLineTranslationAnchor:MGLLineTranslationAnchorViewport].MGLLineTranslationAnchorValue, MGLLineTranslationAnchorViewport);
}

@end
