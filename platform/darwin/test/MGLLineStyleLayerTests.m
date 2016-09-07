// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLLineLayerTests : MGLStyleLayerTests
@end

@implementation MGLLineLayerTests

- (void)testLineLayer {
    NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"amsterdam" ofType:@"geojson"];
    NSURL *url = [NSURL fileURLWithPath:filePath];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceIdentifier:@"sourceID" URL:url];
    MGLLineStyleLayer *layer = [[MGLLineStyleLayer alloc] initWithLayerIdentifier:@"layerID" sourceIdentifier:@"sourceID"];
    [self.mapView.style addSource:source];
    [self.mapView.style addLayer:layer];

    layer.lineCap = [MGLRuntimeStylingHelper testEnum:MGLLineStyleLayerLineCapSquare type:@encode(MGLLineStyleLayerLineCap)];
    layer.lineJoin = [MGLRuntimeStylingHelper testEnum:MGLLineStyleLayerLineJoinMiter type:@encode(MGLLineStyleLayerLineJoin)];
    layer.lineMiterLimit = MGLRuntimeStylingHelper.testNumber;
    layer.lineRoundLimit = MGLRuntimeStylingHelper.testNumber;
    layer.lineOpacity = MGLRuntimeStylingHelper.testNumber;
    layer.lineColor = MGLRuntimeStylingHelper.testColor;
    layer.lineTranslate = MGLRuntimeStylingHelper.testOffset;
    layer.lineTranslateAnchor = [MGLRuntimeStylingHelper testEnum:MGLLineStyleLayerLineTranslateAnchorViewport type:@encode(MGLLineStyleLayerLineTranslateAnchor)];
    layer.lineWidth = MGLRuntimeStylingHelper.testNumber;
    layer.lineGapWidth = MGLRuntimeStylingHelper.testNumber;
    layer.lineOffset = MGLRuntimeStylingHelper.testNumber;
    layer.lineBlur = MGLRuntimeStylingHelper.testNumber;
    layer.lineDasharray = MGLRuntimeStylingHelper.testDashArray;
    layer.linePattern = MGLRuntimeStylingHelper.testString;

    MGLLineStyleLayer *gLayer = [self.mapView.style layerWithIdentifier:@"layerID"];
    XCTAssert([(NSValue *)gLayer.lineCap objCType] == [[MGLRuntimeStylingHelper testEnum:MGLLineStyleLayerLineCapSquare type:@encode(MGLLineStyleLayerLineCap)] objCType]);
    XCTAssert([(NSValue *)gLayer.lineJoin objCType] == [[MGLRuntimeStylingHelper testEnum:MGLLineStyleLayerLineJoinMiter type:@encode(MGLLineStyleLayerLineJoin)] objCType]);
    XCTAssertEqualObjects(gLayer.lineMiterLimit, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.lineRoundLimit, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.lineOpacity, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.lineColor, MGLRuntimeStylingHelper.testColor);
    XCTAssertEqualObjects(gLayer.lineTranslate, MGLRuntimeStylingHelper.testOffset);
    XCTAssert([(NSValue *)gLayer.lineTranslateAnchor objCType] == [[MGLRuntimeStylingHelper testEnum:MGLLineStyleLayerLineTranslateAnchorViewport type:@encode(MGLLineStyleLayerLineTranslateAnchor)] objCType]);
    XCTAssertEqualObjects(gLayer.lineWidth, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.lineGapWidth, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.lineOffset, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.lineBlur, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.lineDasharray, MGLRuntimeStylingHelper.testDashArray);
    XCTAssertEqualObjects(gLayer.linePattern, MGLRuntimeStylingHelper.testString);
}

@end
