// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLFillLayerTests : MGLStyleLayerTests
@end

@implementation MGLFillLayerTests

- (void)testFillLayer {
    NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"amsterdam" ofType:@"geojson"];
    NSURL *url = [NSURL fileURLWithPath:filePath];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"sourceID" URL:url options:nil];
    [self.mapView.style addSource:source];
    MGLFillStyleLayer *layer = [[MGLFillStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    [self.mapView.style addLayer:layer];

    layer.fillAntialias = [MGLRuntimeStylingHelper testBool];
    layer.fillOpacity = [MGLRuntimeStylingHelper testNumber];
    layer.fillColor = [MGLRuntimeStylingHelper testColor];
    layer.fillOutlineColor = [MGLRuntimeStylingHelper testColor];
    layer.fillTranslate = [MGLRuntimeStylingHelper testOffset];
    layer.fillTranslateAnchor = [MGLRuntimeStylingHelper testEnum:MGLFillTranslateAnchorViewport type:@encode(MGLFillTranslateAnchor)];
    layer.fillPattern = [MGLRuntimeStylingHelper testString];

    MGLFillStyleLayer *gLayer = (MGLFillStyleLayer *)[self.mapView.style layerWithIdentifier:@"layerID"];
    XCTAssertTrue([gLayer isKindOfClass:[MGLFillStyleLayer class]]);
    XCTAssertEqualObjects(gLayer.fillAntialias, [MGLRuntimeStylingHelper testBool]);
    XCTAssertEqualObjects(gLayer.fillOpacity, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.fillColor, [MGLRuntimeStylingHelper testColor]);
    XCTAssertEqualObjects(gLayer.fillOutlineColor, [MGLRuntimeStylingHelper testColor]);
    XCTAssertEqualObjects(gLayer.fillTranslate, [MGLRuntimeStylingHelper testOffset]);
    XCTAssert([gLayer.fillTranslateAnchor isKindOfClass:[MGLStyleConstantValue class]]);
    XCTAssertEqualObjects(gLayer.fillTranslateAnchor, [MGLRuntimeStylingHelper testEnum:MGLFillTranslateAnchorViewport type:@encode(MGLFillTranslateAnchor)]);
    XCTAssertEqualObjects(gLayer.fillPattern, [MGLRuntimeStylingHelper testString]);

    layer.fillAntialias = [MGLRuntimeStylingHelper testBoolFunction];
    layer.fillOpacity = [MGLRuntimeStylingHelper testNumberFunction];
    layer.fillColor = [MGLRuntimeStylingHelper testColorFunction];
    layer.fillOutlineColor = [MGLRuntimeStylingHelper testColorFunction];
    layer.fillTranslate = [MGLRuntimeStylingHelper testOffsetFunction];
    layer.fillTranslateAnchor = [MGLRuntimeStylingHelper testEnumFunction:MGLFillTranslateAnchorViewport type:@encode(MGLFillTranslateAnchor)];
    layer.fillPattern = [MGLRuntimeStylingHelper testStringFunction];

    XCTAssertEqualObjects(gLayer.fillAntialias, [MGLRuntimeStylingHelper testBoolFunction]);
    XCTAssertEqualObjects(gLayer.fillOpacity, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.fillColor, [MGLRuntimeStylingHelper testColorFunction]);
    XCTAssertEqualObjects(gLayer.fillOutlineColor, [MGLRuntimeStylingHelper testColorFunction]);
    XCTAssertEqualObjects(gLayer.fillTranslate, [MGLRuntimeStylingHelper testOffsetFunction]);
    XCTAssertEqualObjects(gLayer.fillTranslateAnchor, [MGLRuntimeStylingHelper testEnumFunction:MGLFillTranslateAnchorViewport type:@encode(MGLFillTranslateAnchor)]);
    XCTAssertEqualObjects(gLayer.fillPattern, [MGLRuntimeStylingHelper testStringFunction]);
}

@end
