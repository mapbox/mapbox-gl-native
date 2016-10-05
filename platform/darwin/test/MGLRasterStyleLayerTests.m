// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLRasterLayerTests : MGLStyleLayerTests
@end

@implementation MGLRasterLayerTests

- (void)testRasterLayer {
    NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"amsterdam" ofType:@"geojson"];
    NSURL *url = [NSURL fileURLWithPath:filePath];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"sourceID" URL:url options:nil];
    [self.mapView.style addSource:source];
    MGLRasterStyleLayer *layer = [[MGLRasterStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    [self.mapView.style addLayer:layer];

    layer.rasterOpacity = [MGLRuntimeStylingHelper testNumber];
    layer.rasterHueRotate = [MGLRuntimeStylingHelper testNumber];
    layer.rasterBrightnessMin = [MGLRuntimeStylingHelper testNumber];
    layer.rasterBrightnessMax = [MGLRuntimeStylingHelper testNumber];
    layer.rasterSaturation = [MGLRuntimeStylingHelper testNumber];
    layer.rasterContrast = [MGLRuntimeStylingHelper testNumber];
    layer.rasterFadeDuration = [MGLRuntimeStylingHelper testNumber];

    MGLRasterStyleLayer *gLayer = (MGLRasterStyleLayer *)[self.mapView.style layerWithIdentifier:@"layerID"];
    XCTAssertTrue([gLayer isKindOfClass:[MGLRasterStyleLayer class]]);
    XCTAssertEqualObjects(gLayer.rasterOpacity, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.rasterHueRotate, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.rasterBrightnessMin, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.rasterBrightnessMax, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.rasterSaturation, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.rasterContrast, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.rasterFadeDuration, [MGLRuntimeStylingHelper testNumber]);

    layer.rasterOpacity = [MGLRuntimeStylingHelper testNumberFunction];
    layer.rasterHueRotate = [MGLRuntimeStylingHelper testNumberFunction];
    layer.rasterBrightnessMin = [MGLRuntimeStylingHelper testNumberFunction];
    layer.rasterBrightnessMax = [MGLRuntimeStylingHelper testNumberFunction];
    layer.rasterSaturation = [MGLRuntimeStylingHelper testNumberFunction];
    layer.rasterContrast = [MGLRuntimeStylingHelper testNumberFunction];
    layer.rasterFadeDuration = [MGLRuntimeStylingHelper testNumberFunction];

    XCTAssertEqualObjects(gLayer.rasterOpacity, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.rasterHueRotate, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.rasterBrightnessMin, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.rasterBrightnessMax, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.rasterSaturation, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.rasterContrast, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.rasterFadeDuration, [MGLRuntimeStylingHelper testNumberFunction]);
}

@end
