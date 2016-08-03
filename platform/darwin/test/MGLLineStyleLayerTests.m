// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLLineLayerTests : MGLStyleLayerTests
@end

@implementation MGLLineLayerTests

- (void)testLineLayer {
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceIdentifier:@"sourceID" URL:geoJSONURL];
    MGLLineStyleLayer *layer = [[MGLLineStyleLayer alloc] initWithLayerIdentifier:@"layerID" sourceIdentifier:@"sourceID"];
    [self.mapView.style addLayer:layer];

    #warning Missing setter for lineCap enum
    #warning Missing setter for lineJoin enum
    layer.lineMiterLimit = MGLRuntimeStylingHelper.testNumber;
    layer.lineRoundLimit = MGLRuntimeStylingHelper.testNumber;
    layer.lineOpacity = MGLRuntimeStylingHelper.testNumber;
    layer.lineColor = MGLRuntimeStylingHelper.testColor;
    layer.lineTranslate = MGLRuntimeStylingHelper.testOffset;
    #warning Missing setter for lineTranslateAnchor enum
    layer.lineWidth = MGLRuntimeStylingHelper.testNumber;
    layer.lineGapWidth = MGLRuntimeStylingHelper.testNumber;
    layer.lineOffset = MGLRuntimeStylingHelper.testNumber;
    layer.lineBlur = MGLRuntimeStylingHelper.testNumber;
    layer.lineDasharray = MGLRuntimeStylingHelper.testDashArray;
    layer.linePattern = MGLRuntimeStylingHelper.testString;

    MGLLineStyleLayer *gLayer = [self.mapView.style layerWithIdentifier:@"layerID"];
    #warning Missing getter for lineCap enum
    #warning Missing getter for lineJoin enum
    XCTAssertEqualObjects(gLayer.lineMiterLimit, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.lineRoundLimit, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.lineOpacity, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.lineColor, MGLRuntimeStylingHelper.testColor);
    XCTAssertEqualObjects(gLayer.lineTranslate, MGLRuntimeStylingHelper.testOffset);
    #warning Missing getter for lineTranslateAnchor enum
    XCTAssertEqualObjects(gLayer.lineWidth, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.lineGapWidth, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.lineOffset, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.lineBlur, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.lineDasharray, MGLRuntimeStylingHelper.testDashArray);
    XCTAssertEqualObjects(gLayer.linePattern, MGLRuntimeStylingHelper.testString);
}

@end
