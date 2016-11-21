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

    layer.maximumRasterBrightness = [MGLRuntimeStylingHelper testNumber];
    layer.minimumRasterBrightness = [MGLRuntimeStylingHelper testNumber];
    layer.rasterContrast = [MGLRuntimeStylingHelper testNumber];
    layer.rasterFadeDuration = [MGLRuntimeStylingHelper testNumber];
    layer.rasterHueRotate = [MGLRuntimeStylingHelper testNumber];
    layer.rasterOpacity = [MGLRuntimeStylingHelper testNumber];
    layer.rasterSaturation = [MGLRuntimeStylingHelper testNumber];

    MGLRasterStyleLayer *gLayer = (MGLRasterStyleLayer *)[self.mapView.style layerWithIdentifier:@"layerID"];
    XCTAssertTrue([gLayer isKindOfClass:[MGLRasterStyleLayer class]]);
    XCTAssertEqualObjects(gLayer.maximumRasterBrightness, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.minimumRasterBrightness, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.rasterContrast, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.rasterFadeDuration, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.rasterHueRotate, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.rasterOpacity, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.rasterSaturation, [MGLRuntimeStylingHelper testNumber]);

    layer.maximumRasterBrightness = [MGLRuntimeStylingHelper testNumberFunction];
    layer.minimumRasterBrightness = [MGLRuntimeStylingHelper testNumberFunction];
    layer.rasterContrast = [MGLRuntimeStylingHelper testNumberFunction];
    layer.rasterFadeDuration = [MGLRuntimeStylingHelper testNumberFunction];
    layer.rasterHueRotate = [MGLRuntimeStylingHelper testNumberFunction];
    layer.rasterOpacity = [MGLRuntimeStylingHelper testNumberFunction];
    layer.rasterSaturation = [MGLRuntimeStylingHelper testNumberFunction];

    XCTAssertEqualObjects(gLayer.maximumRasterBrightness, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.minimumRasterBrightness, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.rasterContrast, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.rasterFadeDuration, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.rasterHueRotate, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.rasterOpacity, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.rasterSaturation, [MGLRuntimeStylingHelper testNumberFunction]);
}

@end
