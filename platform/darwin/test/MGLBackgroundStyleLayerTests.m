// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLBackgroundLayerTests : MGLStyleLayerTests
@end

@implementation MGLBackgroundLayerTests

- (void)testBackgroundLayer {
    MGLBackgroundStyleLayer *layer = [[MGLBackgroundStyleLayer alloc] initWithIdentifier:@"layerID"];
    [self.mapView.style addLayer:layer];

    layer.backgroundColor = [MGLRuntimeStylingHelper testColor];
    layer.backgroundOpacity = [MGLRuntimeStylingHelper testNumber];
    layer.backgroundPattern = [MGLRuntimeStylingHelper testString];

    MGLBackgroundStyleLayer *gLayer = (MGLBackgroundStyleLayer *)[self.mapView.style layerWithIdentifier:@"layerID"];
    XCTAssertTrue([gLayer isKindOfClass:[MGLBackgroundStyleLayer class]]);
    XCTAssertEqualObjects(gLayer.backgroundColor, [MGLRuntimeStylingHelper testColor]);
    XCTAssertEqualObjects(gLayer.backgroundOpacity, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.backgroundPattern, [MGLRuntimeStylingHelper testString]);

    layer.backgroundColor = [MGLRuntimeStylingHelper testColorFunction];
    layer.backgroundOpacity = [MGLRuntimeStylingHelper testNumberFunction];
    layer.backgroundPattern = [MGLRuntimeStylingHelper testStringFunction];

    XCTAssertEqualObjects(gLayer.backgroundColor, [MGLRuntimeStylingHelper testColorFunction]);
    XCTAssertEqualObjects(gLayer.backgroundOpacity, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.backgroundPattern, [MGLRuntimeStylingHelper testStringFunction]);
}

@end
