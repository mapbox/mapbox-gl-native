// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLBackgroundLayerTests : MGLStyleLayerTests
@end

@implementation MGLBackgroundLayerTests

- (void)testBackgroundLayer {
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceIdentifier:@"sourceID" URL:geoJSONURL];
    MGLBackgroundStyleLayer *layer = [[MGLBackgroundStyleLayer alloc] initWithLayerIdentifier:@"layerID" sourceIdentifier:@"sourceID"];
    [self.mapView.style addSource:source];
    [self.mapView.style addLayer:layer];

    layer.backgroundColor = MGLRuntimeStylingHelper.testColor;
    layer.backgroundPattern = MGLRuntimeStylingHelper.testString;
    layer.backgroundOpacity = MGLRuntimeStylingHelper.testNumber;

    MGLBackgroundStyleLayer *gLayer = [self.mapView.style layerWithIdentifier:@"layerID"];
    XCTAssertEqualObjects(gLayer.backgroundColor, MGLRuntimeStylingHelper.testColor);
    XCTAssertEqualObjects(gLayer.backgroundPattern, MGLRuntimeStylingHelper.testString);
    XCTAssertEqualObjects(gLayer.backgroundOpacity, MGLRuntimeStylingHelper.testNumber);
}

@end
