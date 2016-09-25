// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLCircleLayerTests : MGLStyleLayerTests
@end

@implementation MGLCircleLayerTests

- (void)testCircleLayer {
    NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"amsterdam" ofType:@"geojson"];
    NSURL *url = [NSURL fileURLWithPath:filePath];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithIdentifier:@"sourceID" URL:url options:nil];
    [self.mapView.style addSource:source];
    MGLCircleStyleLayer *layer = [[MGLCircleStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    [self.mapView.style addLayer:layer];

    layer.circleRadius = [MGLRuntimeStylingHelper testNumber];
    layer.circleColor = [MGLRuntimeStylingHelper testColor];
    layer.circleBlur = [MGLRuntimeStylingHelper testNumber];
    layer.circleOpacity = [MGLRuntimeStylingHelper testNumber];
    layer.circleTranslate = [MGLRuntimeStylingHelper testOffset];
    layer.circleTranslateAnchor = [MGLRuntimeStylingHelper testEnum:MGLCircleTranslateAnchorViewport type:@encode(MGLCircleTranslateAnchor)];
    layer.circlePitchScale = [MGLRuntimeStylingHelper testEnum:MGLCirclePitchScaleViewport type:@encode(MGLCirclePitchScale)];

    MGLCircleStyleLayer *gLayer = (MGLCircleStyleLayer *)[self.mapView.style layerWithIdentifier:@"layerID"];
    XCTAssertTrue([gLayer isKindOfClass:[MGLCircleStyleLayer class]]);
    XCTAssertEqualObjects(gLayer.circleRadius, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.circleColor, [MGLRuntimeStylingHelper testColor]);
    XCTAssertEqualObjects(gLayer.circleBlur, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.circleOpacity, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.circleTranslate, [MGLRuntimeStylingHelper testOffset]);
    XCTAssert([gLayer.circleTranslateAnchor isKindOfClass:[MGLStyleConstantValue class]]);
    XCTAssertEqualObjects(gLayer.circleTranslateAnchor, [MGLRuntimeStylingHelper testEnum:MGLCircleTranslateAnchorViewport type:@encode(MGLCircleTranslateAnchor)]);
    XCTAssert([gLayer.circlePitchScale isKindOfClass:[MGLStyleConstantValue class]]);
    XCTAssertEqualObjects(gLayer.circlePitchScale, [MGLRuntimeStylingHelper testEnum:MGLCirclePitchScaleViewport type:@encode(MGLCirclePitchScale)]);

    layer.circleRadius = [MGLRuntimeStylingHelper testNumberFunction];
    layer.circleColor = [MGLRuntimeStylingHelper testColorFunction];
    layer.circleBlur = [MGLRuntimeStylingHelper testNumberFunction];
    layer.circleOpacity = [MGLRuntimeStylingHelper testNumberFunction];
    layer.circleTranslate = [MGLRuntimeStylingHelper testOffsetFunction];
    layer.circleTranslateAnchor = [MGLRuntimeStylingHelper testEnumFunction:MGLCircleTranslateAnchorViewport type:@encode(MGLCircleTranslateAnchor)];
    layer.circlePitchScale = [MGLRuntimeStylingHelper testEnumFunction:MGLCirclePitchScaleViewport type:@encode(MGLCirclePitchScale)];

    XCTAssertEqualObjects(gLayer.circleRadius, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.circleColor, [MGLRuntimeStylingHelper testColorFunction]);
    XCTAssertEqualObjects(gLayer.circleBlur, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.circleOpacity, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.circleTranslate, [MGLRuntimeStylingHelper testOffsetFunction]);
    XCTAssertEqualObjects(gLayer.circleTranslateAnchor, [MGLRuntimeStylingHelper testEnumFunction:MGLCircleTranslateAnchorViewport type:@encode(MGLCircleTranslateAnchor)]);
    XCTAssertEqualObjects(gLayer.circlePitchScale, [MGLRuntimeStylingHelper testEnumFunction:MGLCirclePitchScaleViewport type:@encode(MGLCirclePitchScale)]);
}

@end
