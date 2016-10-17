// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLLineLayerTests : MGLStyleLayerTests
@end

@implementation MGLLineLayerTests

- (void)testLineLayer {
    NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"amsterdam" ofType:@"geojson"];
    NSURL *url = [NSURL fileURLWithPath:filePath];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"sourceID" URL:url options:nil];
    [self.mapView.style addSource:source];
    MGLLineStyleLayer *layer = [[MGLLineStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    [self.mapView.style addLayer:layer];

    layer.lineCap = [MGLRuntimeStylingHelper testEnum:MGLLineCapSquare type:@encode(MGLLineCap)];
    layer.lineJoin = [MGLRuntimeStylingHelper testEnum:MGLLineJoinMiter type:@encode(MGLLineJoin)];
    layer.lineMiterLimit = [MGLRuntimeStylingHelper testNumber];
    layer.lineRoundLimit = [MGLRuntimeStylingHelper testNumber];
    layer.lineOpacity = [MGLRuntimeStylingHelper testNumber];
    layer.lineColor = [MGLRuntimeStylingHelper testColor];
    layer.lineTranslate = [MGLRuntimeStylingHelper testOffset];
    layer.lineTranslateAnchor = [MGLRuntimeStylingHelper testEnum:MGLLineTranslateAnchorViewport type:@encode(MGLLineTranslateAnchor)];
    layer.lineWidth = [MGLRuntimeStylingHelper testNumber];
    layer.lineGapWidth = [MGLRuntimeStylingHelper testNumber];
    layer.lineOffset = [MGLRuntimeStylingHelper testNumber];
    layer.lineBlur = [MGLRuntimeStylingHelper testNumber];
    layer.lineDasharray = [MGLRuntimeStylingHelper testDashArray];
    layer.linePattern = [MGLRuntimeStylingHelper testString];

    MGLLineStyleLayer *gLayer = (MGLLineStyleLayer *)[self.mapView.style layerWithIdentifier:@"layerID"];
    XCTAssertTrue([gLayer isKindOfClass:[MGLLineStyleLayer class]]);
    XCTAssert([gLayer.lineCap isKindOfClass:[MGLStyleConstantValue class]]);
    XCTAssertEqualObjects(gLayer.lineCap, [MGLRuntimeStylingHelper testEnum:MGLLineCapSquare type:@encode(MGLLineCap)]);
    XCTAssert([gLayer.lineJoin isKindOfClass:[MGLStyleConstantValue class]]);
    XCTAssertEqualObjects(gLayer.lineJoin, [MGLRuntimeStylingHelper testEnum:MGLLineJoinMiter type:@encode(MGLLineJoin)]);
    XCTAssertEqualObjects(gLayer.lineMiterLimit, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.lineRoundLimit, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.lineOpacity, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.lineColor, [MGLRuntimeStylingHelper testColor]);
    XCTAssertEqualObjects(gLayer.lineTranslate, [MGLRuntimeStylingHelper testOffset]);
    XCTAssert([gLayer.lineTranslateAnchor isKindOfClass:[MGLStyleConstantValue class]]);
    XCTAssertEqualObjects(gLayer.lineTranslateAnchor, [MGLRuntimeStylingHelper testEnum:MGLLineTranslateAnchorViewport type:@encode(MGLLineTranslateAnchor)]);
    XCTAssertEqualObjects(gLayer.lineWidth, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.lineGapWidth, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.lineOffset, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.lineBlur, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.lineDasharray, [MGLRuntimeStylingHelper testDashArray]);
    XCTAssertEqualObjects(gLayer.linePattern, [MGLRuntimeStylingHelper testString]);

    layer.lineCap = [MGLRuntimeStylingHelper testEnumFunction:MGLLineCapSquare type:@encode(MGLLineCap)];
    layer.lineJoin = [MGLRuntimeStylingHelper testEnumFunction:MGLLineJoinMiter type:@encode(MGLLineJoin)];
    layer.lineMiterLimit = [MGLRuntimeStylingHelper testNumberFunction];
    layer.lineRoundLimit = [MGLRuntimeStylingHelper testNumberFunction];
    layer.lineOpacity = [MGLRuntimeStylingHelper testNumberFunction];
    layer.lineColor = [MGLRuntimeStylingHelper testColorFunction];
    layer.lineTranslate = [MGLRuntimeStylingHelper testOffsetFunction];
    layer.lineTranslateAnchor = [MGLRuntimeStylingHelper testEnumFunction:MGLLineTranslateAnchorViewport type:@encode(MGLLineTranslateAnchor)];
    layer.lineWidth = [MGLRuntimeStylingHelper testNumberFunction];
    layer.lineGapWidth = [MGLRuntimeStylingHelper testNumberFunction];
    layer.lineOffset = [MGLRuntimeStylingHelper testNumberFunction];
    layer.lineBlur = [MGLRuntimeStylingHelper testNumberFunction];
    layer.lineDasharray = [MGLRuntimeStylingHelper testDashArrayFunction];
    layer.linePattern = [MGLRuntimeStylingHelper testStringFunction];

    XCTAssertEqualObjects(gLayer.lineCap, [MGLRuntimeStylingHelper testEnumFunction:MGLLineCapSquare type:@encode(MGLLineCap)]);
    XCTAssertEqualObjects(gLayer.lineJoin, [MGLRuntimeStylingHelper testEnumFunction:MGLLineJoinMiter type:@encode(MGLLineJoin)]);
    XCTAssertEqualObjects(gLayer.lineMiterLimit, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.lineRoundLimit, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.lineOpacity, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.lineColor, [MGLRuntimeStylingHelper testColorFunction]);
    XCTAssertEqualObjects(gLayer.lineTranslate, [MGLRuntimeStylingHelper testOffsetFunction]);
    XCTAssertEqualObjects(gLayer.lineTranslateAnchor, [MGLRuntimeStylingHelper testEnumFunction:MGLLineTranslateAnchorViewport type:@encode(MGLLineTranslateAnchor)]);
    XCTAssertEqualObjects(gLayer.lineWidth, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.lineGapWidth, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.lineOffset, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.lineBlur, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.lineDasharray, [MGLRuntimeStylingHelper testDashArrayFunction]);
    XCTAssertEqualObjects(gLayer.linePattern, [MGLRuntimeStylingHelper testStringFunction]);
}

@end
