// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLFillLayerTests : MGLStyleLayerTests
@end

@implementation MGLFillLayerTests

- (void)testFillLayer {
    NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"amsterdam" ofType:@"geojson"];
    NSURL *url = [NSURL fileURLWithPath:filePath];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceIdentifier:@"sourceID" URL:url];
    MGLFillStyleLayer *layer = [[MGLFillStyleLayer alloc] initWithLayerIdentifier:@"layerID" sourceIdentifier:@"sourceID"];
    [self.mapView.style addSource:source];
    [self.mapView.style addLayer:layer];

    layer.fillAntialias = MGLRuntimeStylingHelper.testBool;
    layer.fillOpacity = MGLRuntimeStylingHelper.testNumber;
    layer.fillColor = MGLRuntimeStylingHelper.testColor;
    layer.fillOutlineColor = MGLRuntimeStylingHelper.testColor;
    layer.fillTranslate = MGLRuntimeStylingHelper.testOffset;
    layer.fillTranslateAnchor = [MGLRuntimeStylingHelper testEnum:MGLFillStyleLayerFillTranslateAnchorViewport type:@encode(MGLFillStyleLayerFillTranslateAnchor)];
    layer.fillPattern = MGLRuntimeStylingHelper.testString;

    MGLFillStyleLayer *gLayer = [self.mapView.style layerWithIdentifier:@"layerID"];
    XCTAssertEqualObjects(gLayer.fillAntialias, MGLRuntimeStylingHelper.testBool);
    XCTAssertEqualObjects(gLayer.fillOpacity, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.fillColor, MGLRuntimeStylingHelper.testColor);
    XCTAssertEqualObjects(gLayer.fillOutlineColor, MGLRuntimeStylingHelper.testColor);
    XCTAssertEqualObjects(gLayer.fillTranslate, MGLRuntimeStylingHelper.testOffset);
    XCTAssert([(NSValue *)gLayer.fillTranslateAnchor objCType] == [[MGLRuntimeStylingHelper testEnum:MGLFillStyleLayerFillTranslateAnchorViewport type:@encode(MGLFillStyleLayerFillTranslateAnchor)] objCType]);
    XCTAssertEqualObjects(gLayer.fillPattern, MGLRuntimeStylingHelper.testString);
}

@end
