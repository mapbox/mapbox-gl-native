// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLRasterLayerTests : MGLStyleLayerTests
@end

@implementation MGLRasterLayerTests

- (void)testRasterLayer {
    NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"amsterdam" ofType:@"geojson"];
    NSURL *url = [NSURL fileURLWithPath:filePath];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceIdentifier:@"sourceID" URL:url];
    MGLRasterStyleLayer *layer = [[MGLRasterStyleLayer alloc] initWithLayerIdentifier:@"layerID" sourceIdentifier:@"sourceID"];
    [self.mapView.style addSource:source];
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
