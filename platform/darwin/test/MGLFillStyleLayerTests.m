// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLFillLayerTests : MGLStyleLayerTests
@end

@implementation MGLFillLayerTests

- (void)testFillLayer {
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceIdentifier:@"sourceID" URL:geoJSONURL];
    MGLFillStyleLayer *layer = [[MGLFillStyleLayer alloc] initWithLayerIdentifier:@"layerID" sourceIdentifier:@"sourceID"];
    [self.mapView.style addLayer:layer];

    layer.fillAntialias = MGLRuntimeStylingHelper.testBool;
    layer.fillOpacity = MGLRuntimeStylingHelper.testNumber;
    layer.fillColor = MGLRuntimeStylingHelper.testColor;
    layer.fillOutlineColor = MGLRuntimeStylingHelper.testColor;
    layer.fillTranslate = MGLRuntimeStylingHelper.testOffset;
    // TODO: setterEnum
    layer.fillPattern = MGLRuntimeStylingHelper.testString;

    MGLFillStyleLayer *gLayer = [self.mapView.style layerWithIdentifier:@"layerID"];
    XCTAssertEqualObjects(gLayer.fillAntialias, MGLRuntimeStylingHelper.testBool);
    XCTAssertEqualObjects(gLayer.fillOpacity, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.fillColor, MGLRuntimeStylingHelper.testColor);
    XCTAssertEqualObjects(gLayer.fillOutlineColor, MGLRuntimeStylingHelper.testColor);
    XCTAssertEqualObjects(gLayer.fillTranslate, MGLRuntimeStylingHelper.testOffset);
    // TODO: getterEnum
    XCTAssertEqualObjects(gLayer.fillPattern, MGLRuntimeStylingHelper.testString);
}

@end
