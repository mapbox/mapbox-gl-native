// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLRasterLayerTests : MGLStyleLayerTests
@end

@implementation MGLRasterLayerTests

- (void)testRasterLayer {
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceIdentifier:@"sourceID" URL:geoJSONURL];
    MGLRasterStyleLayer *layer = [[MGLRasterStyleLayer alloc] initWithLayerIdentifier:@"layerID" sourceIdentifier:@"sourceID"];
    [self.mapView.style addLayer:layer];

    layer.rasterOpacity = MGLRuntimeStylingHelper.testNumber;
    layer.rasterHueRotate = MGLRuntimeStylingHelper.testNumber;
    layer.rasterBrightnessMin = MGLRuntimeStylingHelper.testNumber;
    layer.rasterBrightnessMax = MGLRuntimeStylingHelper.testNumber;
    layer.rasterSaturation = MGLRuntimeStylingHelper.testNumber;
    layer.rasterContrast = MGLRuntimeStylingHelper.testNumber;
    layer.rasterFadeDuration = MGLRuntimeStylingHelper.testNumber;

    MGLRasterStyleLayer *gLayer = [self.mapView.style layerWithIdentifier:@"layerID"];
    XCTAssertEqualObjects(gLayer.rasterOpacity, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.rasterHueRotate, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.rasterBrightnessMin, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.rasterBrightnessMax, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.rasterSaturation, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.rasterContrast, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.rasterFadeDuration, MGLRuntimeStylingHelper.testNumber);
}

@end
