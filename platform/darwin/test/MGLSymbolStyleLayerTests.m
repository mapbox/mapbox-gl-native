// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLSymbolLayerTests : MGLStyleLayerTests
@end

@implementation MGLSymbolLayerTests

- (void)testSymbolLayer {
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceIdentifier:@"sourceID" URL:geoJSONURL];
    MGLSymbolStyleLayer *layer = [[MGLSymbolStyleLayer alloc] initWithLayerIdentifier:@"layerID" sourceIdentifier:@"sourceID"];
    [self.mapView.style addLayer:layer];

    // TODO: setterEnum
    layer.symbolSpacing = MGLRuntimeStylingHelper.testNumber;
    layer.symbolAvoidEdges = MGLRuntimeStylingHelper.testBool;
    layer.iconAllowOverlap = MGLRuntimeStylingHelper.testBool;
    layer.iconIgnorePlacement = MGLRuntimeStylingHelper.testBool;
    layer.iconOptional = MGLRuntimeStylingHelper.testBool;
    // TODO: setterEnum
    layer.iconSize = MGLRuntimeStylingHelper.testNumber;
    // TODO: setterEnum
    layer.iconTextFitPadding = MGLRuntimeStylingHelper.testPadding;
    layer.iconImage = MGLRuntimeStylingHelper.testString;
    layer.iconRotate = MGLRuntimeStylingHelper.testNumber;
    layer.iconPadding = MGLRuntimeStylingHelper.testNumber;
    layer.iconKeepUpright = MGLRuntimeStylingHelper.testBool;
    layer.iconOffset = MGLRuntimeStylingHelper.testOffset;
    // TODO: setterEnum
    // TODO: setterEnum
    layer.textField = MGLRuntimeStylingHelper.testString;
    layer.textFont = MGLRuntimeStylingHelper.testFont;
    layer.textSize = MGLRuntimeStylingHelper.testNumber;
    layer.textMaxWidth = MGLRuntimeStylingHelper.testNumber;
    layer.textLineHeight = MGLRuntimeStylingHelper.testNumber;
    layer.textLetterSpacing = MGLRuntimeStylingHelper.testNumber;
    // TODO: setterEnum
    // TODO: setterEnum
    layer.textMaxAngle = MGLRuntimeStylingHelper.testNumber;
    layer.textRotate = MGLRuntimeStylingHelper.testNumber;
    layer.textPadding = MGLRuntimeStylingHelper.testNumber;
    layer.textKeepUpright = MGLRuntimeStylingHelper.testBool;
    // TODO: setterEnum
    layer.textOffset = MGLRuntimeStylingHelper.testOffset;
    layer.textAllowOverlap = MGLRuntimeStylingHelper.testBool;
    layer.textIgnorePlacement = MGLRuntimeStylingHelper.testBool;
    layer.textOptional = MGLRuntimeStylingHelper.testBool;
    layer.iconOpacity = MGLRuntimeStylingHelper.testNumber;
    layer.iconColor = MGLRuntimeStylingHelper.testColor;
    layer.iconHaloColor = MGLRuntimeStylingHelper.testColor;
    layer.iconHaloWidth = MGLRuntimeStylingHelper.testNumber;
    layer.iconHaloBlur = MGLRuntimeStylingHelper.testNumber;
    layer.iconTranslate = MGLRuntimeStylingHelper.testOffset;
    // TODO: setterEnum
    layer.textOpacity = MGLRuntimeStylingHelper.testNumber;
    layer.textColor = MGLRuntimeStylingHelper.testColor;
    layer.textHaloColor = MGLRuntimeStylingHelper.testColor;
    layer.textHaloWidth = MGLRuntimeStylingHelper.testNumber;
    layer.textHaloBlur = MGLRuntimeStylingHelper.testNumber;
    layer.textTranslate = MGLRuntimeStylingHelper.testOffset;
    // TODO: setterEnum

    MGLSymbolStyleLayer *gLayer = [self.mapView.style layerWithIdentifier:@"layerID"];
    // TODO: getterEnum
    XCTAssertEqualObjects(gLayer.symbolSpacing, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.symbolAvoidEdges, MGLRuntimeStylingHelper.testBool);
    XCTAssertEqualObjects(gLayer.iconAllowOverlap, MGLRuntimeStylingHelper.testBool);
    XCTAssertEqualObjects(gLayer.iconIgnorePlacement, MGLRuntimeStylingHelper.testBool);
    XCTAssertEqualObjects(gLayer.iconOptional, MGLRuntimeStylingHelper.testBool);
    // TODO: getterEnum
    XCTAssertEqualObjects(gLayer.iconSize, MGLRuntimeStylingHelper.testNumber);
    // TODO: getterEnum
    XCTAssertEqualObjects(gLayer.iconTextFitPadding, MGLRuntimeStylingHelper.testPadding);
    XCTAssertEqualObjects(gLayer.iconImage, MGLRuntimeStylingHelper.testString);
    XCTAssertEqualObjects(gLayer.iconRotate, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.iconPadding, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.iconKeepUpright, MGLRuntimeStylingHelper.testBool);
    XCTAssertEqualObjects(gLayer.iconOffset, MGLRuntimeStylingHelper.testOffset);
    // TODO: getterEnum
    // TODO: getterEnum
    XCTAssertEqualObjects(gLayer.textField, MGLRuntimeStylingHelper.testString);
    XCTAssertEqualObjects(gLayer.textFont, MGLRuntimeStylingHelper.testFont);
    XCTAssertEqualObjects(gLayer.textSize, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textMaxWidth, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textLineHeight, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textLetterSpacing, MGLRuntimeStylingHelper.testNumber);
    // TODO: getterEnum
    // TODO: getterEnum
    XCTAssertEqualObjects(gLayer.textMaxAngle, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textRotate, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textPadding, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textKeepUpright, MGLRuntimeStylingHelper.testBool);
    // TODO: getterEnum
    XCTAssertEqualObjects(gLayer.textOffset, MGLRuntimeStylingHelper.testOffset);
    XCTAssertEqualObjects(gLayer.textAllowOverlap, MGLRuntimeStylingHelper.testBool);
    XCTAssertEqualObjects(gLayer.textIgnorePlacement, MGLRuntimeStylingHelper.testBool);
    XCTAssertEqualObjects(gLayer.textOptional, MGLRuntimeStylingHelper.testBool);
    XCTAssertEqualObjects(gLayer.iconOpacity, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.iconColor, MGLRuntimeStylingHelper.testColor);
    XCTAssertEqualObjects(gLayer.iconHaloColor, MGLRuntimeStylingHelper.testColor);
    XCTAssertEqualObjects(gLayer.iconHaloWidth, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.iconHaloBlur, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.iconTranslate, MGLRuntimeStylingHelper.testOffset);
    // TODO: getterEnum
    XCTAssertEqualObjects(gLayer.textOpacity, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textColor, MGLRuntimeStylingHelper.testColor);
    XCTAssertEqualObjects(gLayer.textHaloColor, MGLRuntimeStylingHelper.testColor);
    XCTAssertEqualObjects(gLayer.textHaloWidth, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textHaloBlur, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textTranslate, MGLRuntimeStylingHelper.testOffset);
    // TODO: getterEnum
}

@end
