// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLCircleLayerTests : MGLStyleLayerTests
@end

@implementation MGLCircleLayerTests

- (void)testCircleLayer {
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceIdentifier:@"sourceID" URL:geoJSONURL];
    MGLCircleStyleLayer *layer = [[MGLCircleStyleLayer alloc] initWithLayerIdentifier:@"layerID" sourceIdentifier:@"sourceID"];
    [self.mapView.style addSource:source];
    [self.mapView.style addLayer:layer];

    layer.circleRadius = MGLRuntimeStylingHelper.testNumber;
    layer.circleColor = MGLRuntimeStylingHelper.testColor;
    layer.circleBlur = MGLRuntimeStylingHelper.testNumber;
    layer.circleOpacity = MGLRuntimeStylingHelper.testNumber;
    layer.circleTranslate = MGLRuntimeStylingHelper.testOffset;
    layer.circleTranslateAnchor = [MGLRuntimeStylingHelper testEnum:MGLCircleStyleLayerCircleTranslateAnchorViewport type:@encode(MGLCircleStyleLayerCircleTranslateAnchor)];
    layer.circlePitchScale = [MGLRuntimeStylingHelper testEnum:MGLCircleStyleLayerCirclePitchScaleViewport type:@encode(MGLCircleStyleLayerCirclePitchScale)];

    MGLCircleStyleLayer *gLayer = [self.mapView.style layerWithIdentifier:@"layerID"];
    XCTAssertEqualObjects(gLayer.circleRadius, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.circleColor, MGLRuntimeStylingHelper.testColor);
    XCTAssertEqualObjects(gLayer.circleBlur, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.circleOpacity, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.circleTranslate, MGLRuntimeStylingHelper.testOffset);
    XCTAssertEqualObjects(gLayer.circleTranslateAnchor, [MGLRuntimeStylingHelper testEnum:MGLCircleStyleLayerCircleTranslateAnchorViewport type:@encode(MGLCircleStyleLayerCircleTranslateAnchor)]);
    XCTAssertEqualObjects(gLayer.circlePitchScale, [MGLRuntimeStylingHelper testEnum:MGLCircleStyleLayerCirclePitchScaleViewport type:@encode(MGLCircleStyleLayerCirclePitchScale)]);
}

@end
