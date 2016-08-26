// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLBackgroundLayerTests : MGLStyleLayerTests
@end

@implementation MGLBackgroundLayerTests

- (void)testBackgroundLayer {
    NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"amsterdam" ofType:@"geojson"];
    NSURL *url = [NSURL fileURLWithPath:filePath];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceIdentifier:@"sourceID" URL:url];
    MGLBackgroundStyleLayer *layer = [[MGLBackgroundStyleLayer alloc] initWithLayerIdentifier:@"layerID" sourceIdentifier:@"sourceID"];
    [self.mapView.style addSource:source];
    [self.mapView.style addLayer:layer];

    layer.backgroundColor = [MGLRuntimeStylingHelper testColor];
    layer.backgroundPattern = [MGLRuntimeStylingHelper testString];
    layer.backgroundOpacity = [MGLRuntimeStylingHelper testNumber];

    MGLBackgroundStyleLayer *gLayer = [self.mapView.style layerWithIdentifier:@"layerID"];
    XCTAssertEqualObjects(gLayer.backgroundColor, [MGLRuntimeStylingHelper testColor]);
    XCTAssertEqualObjects(gLayer.backgroundPattern, [MGLRuntimeStylingHelper testString]);
    XCTAssertEqualObjects(gLayer.backgroundOpacity, [MGLRuntimeStylingHelper testNumber]);

    layer.backgroundColor = [MGLRuntimeStylingHelper testColorFunction];
    layer.backgroundPattern = [MGLRuntimeStylingHelper testStringFunction];
    layer.backgroundOpacity = [MGLRuntimeStylingHelper testNumberFunction];

    XCTAssertEqualObjects(gLayer.backgroundColor, [MGLRuntimeStylingHelper testColorFunction]);
    XCTAssertEqualObjects(gLayer.backgroundPattern, [MGLRuntimeStylingHelper testStringFunction]);
    XCTAssertEqualObjects(gLayer.backgroundOpacity, [MGLRuntimeStylingHelper testNumberFunction]);
}

@end
