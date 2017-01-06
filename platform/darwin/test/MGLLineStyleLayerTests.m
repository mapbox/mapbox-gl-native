// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLLineLayerTests : MGLStyleLayerTests
@end

@implementation MGLLineLayerTests

+ (NSString *)layerType {
    return @"line";
}

- (void)testLineLayer {
    NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"amsterdam" ofType:@"geojson"];
    NSURL *url = [NSURL fileURLWithPath:filePath];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" URL:url options:nil];
    [self.mapView.style addSource:source];
    MGLLineStyleLayer *layer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    [self.mapView.style addLayer:layer];

    layer.lineCap = [MGLRuntimeStylingHelper testEnum:MGLLineCapSquare type:@encode(MGLLineCap)];
    layer.lineJoin = [MGLRuntimeStylingHelper testEnum:MGLLineJoinMiter type:@encode(MGLLineJoin)];
    layer.lineMiterLimit = [MGLRuntimeStylingHelper testNumber];
    layer.lineRoundLimit = [MGLRuntimeStylingHelper testNumber];
    layer.lineBlur = [MGLRuntimeStylingHelper testNumber];
    layer.lineColor = [MGLRuntimeStylingHelper testColor];
    layer.lineDashPattern = [MGLRuntimeStylingHelper testDashArray];
    layer.lineGapWidth = [MGLRuntimeStylingHelper testNumber];
    layer.lineOffset = [MGLRuntimeStylingHelper testNumber];
    layer.lineOpacity = [MGLRuntimeStylingHelper testNumber];
    layer.linePattern = [MGLRuntimeStylingHelper testString];
    layer.lineTranslation = [MGLRuntimeStylingHelper testOffset];
    layer.lineTranslationAnchor = [MGLRuntimeStylingHelper testEnum:MGLLineTranslationAnchorViewport type:@encode(MGLLineTranslationAnchor)];
    layer.lineWidth = [MGLRuntimeStylingHelper testNumber];

    MGLLineStyleLayer *gLayer = (MGLLineStyleLayer *)[self.mapView.style layerWithIdentifier:@"layerID"];
    XCTAssertTrue([gLayer isKindOfClass:[MGLLineStyleLayer class]]);
    XCTAssert([gLayer.lineCap isKindOfClass:[MGLStyleConstantValue class]]);
    XCTAssertEqualObjects(gLayer.lineCap, [MGLRuntimeStylingHelper testEnum:MGLLineCapSquare type:@encode(MGLLineCap)]);
    XCTAssert([gLayer.lineJoin isKindOfClass:[MGLStyleConstantValue class]]);
    XCTAssertEqualObjects(gLayer.lineJoin, [MGLRuntimeStylingHelper testEnum:MGLLineJoinMiter type:@encode(MGLLineJoin)]);
    XCTAssertEqualObjects(gLayer.lineMiterLimit, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.lineRoundLimit, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.lineBlur, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.lineColor, [MGLRuntimeStylingHelper testColor]);
    XCTAssertEqualObjects(gLayer.lineDashPattern, [MGLRuntimeStylingHelper testDashArray]);
    XCTAssertEqualObjects(gLayer.lineGapWidth, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.lineOffset, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.lineOpacity, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.linePattern, [MGLRuntimeStylingHelper testString]);
    XCTAssertEqualObjects(gLayer.lineTranslation, [MGLRuntimeStylingHelper testOffset]);
    XCTAssert([gLayer.lineTranslationAnchor isKindOfClass:[MGLStyleConstantValue class]]);
    XCTAssertEqualObjects(gLayer.lineTranslationAnchor, [MGLRuntimeStylingHelper testEnum:MGLLineTranslationAnchorViewport type:@encode(MGLLineTranslationAnchor)]);
    XCTAssertEqualObjects(gLayer.lineWidth, [MGLRuntimeStylingHelper testNumber]);

    layer.lineCap = [MGLRuntimeStylingHelper testEnumFunction:MGLLineCapSquare type:@encode(MGLLineCap)];
    layer.lineJoin = [MGLRuntimeStylingHelper testEnumFunction:MGLLineJoinMiter type:@encode(MGLLineJoin)];
    layer.lineMiterLimit = [MGLRuntimeStylingHelper testNumberFunction];
    layer.lineRoundLimit = [MGLRuntimeStylingHelper testNumberFunction];
    layer.lineBlur = [MGLRuntimeStylingHelper testNumberFunction];
    layer.lineColor = [MGLRuntimeStylingHelper testColorFunction];
    layer.lineDashPattern = [MGLRuntimeStylingHelper testDashArrayFunction];
    layer.lineGapWidth = [MGLRuntimeStylingHelper testNumberFunction];
    layer.lineOffset = [MGLRuntimeStylingHelper testNumberFunction];
    layer.lineOpacity = [MGLRuntimeStylingHelper testNumberFunction];
    layer.linePattern = [MGLRuntimeStylingHelper testStringFunction];
    layer.lineTranslation = [MGLRuntimeStylingHelper testOffsetFunction];
    layer.lineTranslationAnchor = [MGLRuntimeStylingHelper testEnumFunction:MGLLineTranslationAnchorViewport type:@encode(MGLLineTranslationAnchor)];
    layer.lineWidth = [MGLRuntimeStylingHelper testNumberFunction];

    XCTAssertEqualObjects(gLayer.lineCap, [MGLRuntimeStylingHelper testEnumFunction:MGLLineCapSquare type:@encode(MGLLineCap)]);
    XCTAssertEqualObjects(gLayer.lineJoin, [MGLRuntimeStylingHelper testEnumFunction:MGLLineJoinMiter type:@encode(MGLLineJoin)]);
    XCTAssertEqualObjects(gLayer.lineMiterLimit, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.lineRoundLimit, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.lineBlur, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.lineColor, [MGLRuntimeStylingHelper testColorFunction]);
    XCTAssertEqualObjects(gLayer.lineDashPattern, [MGLRuntimeStylingHelper testDashArrayFunction]);
    XCTAssertEqualObjects(gLayer.lineGapWidth, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.lineOffset, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.lineOpacity, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.linePattern, [MGLRuntimeStylingHelper testStringFunction]);
    XCTAssertEqualObjects(gLayer.lineTranslation, [MGLRuntimeStylingHelper testOffsetFunction]);
    XCTAssertEqualObjects(gLayer.lineTranslationAnchor, [MGLRuntimeStylingHelper testEnumFunction:MGLLineTranslationAnchorViewport type:@encode(MGLLineTranslationAnchor)]);
    XCTAssertEqualObjects(gLayer.lineWidth, [MGLRuntimeStylingHelper testNumberFunction]);
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

@end
