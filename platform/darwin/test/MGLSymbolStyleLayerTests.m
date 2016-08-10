// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLSymbolLayerTests : MGLStyleLayerTests
@end

@implementation MGLSymbolLayerTests

- (void)testSymbolLayer {
    NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"amsterdam" ofType:@"geojson"];
    NSURL *url = [NSURL fileURLWithPath:filePath];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceIdentifier:@"sourceID" URL:url];
    MGLSymbolStyleLayer *layer = [[MGLSymbolStyleLayer alloc] initWithLayerIdentifier:@"layerID" sourceIdentifier:@"sourceID"];
    [self.mapView.style addSource:source];
    [self.mapView.style addLayer:layer];

    layer.symbolPlacement = [MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerSymbolPlacementLine type:@encode(MGLSymbolStyleLayerSymbolPlacement)];
    layer.symbolSpacing = MGLRuntimeStylingHelper.testNumber;
    layer.symbolAvoidEdges = MGLRuntimeStylingHelper.testBool;
    layer.iconAllowOverlap = MGLRuntimeStylingHelper.testBool;
    layer.iconIgnorePlacement = MGLRuntimeStylingHelper.testBool;
    layer.iconOptional = MGLRuntimeStylingHelper.testBool;
    layer.iconRotationAlignment = [MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerIconRotationAlignmentViewport type:@encode(MGLSymbolStyleLayerIconRotationAlignment)];
    layer.iconSize = MGLRuntimeStylingHelper.testNumber;
    layer.iconTextFit = [MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerIconTextFitHeight type:@encode(MGLSymbolStyleLayerIconTextFit)];
    layer.iconTextFitPadding = MGLRuntimeStylingHelper.testPadding;
    layer.iconImage = MGLRuntimeStylingHelper.testString;
    layer.iconRotate = MGLRuntimeStylingHelper.testNumber;
    layer.iconPadding = MGLRuntimeStylingHelper.testNumber;
    layer.iconKeepUpright = MGLRuntimeStylingHelper.testBool;
    layer.iconOffset = MGLRuntimeStylingHelper.testOffset;
    layer.textPitchAlignment = [MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerTextPitchAlignmentViewport type:@encode(MGLSymbolStyleLayerTextPitchAlignment)];
    layer.textRotationAlignment = [MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerTextRotationAlignmentViewport type:@encode(MGLSymbolStyleLayerTextRotationAlignment)];
    layer.textField = MGLRuntimeStylingHelper.testString;
    layer.textFont = MGLRuntimeStylingHelper.testFont;
    layer.textSize = MGLRuntimeStylingHelper.testNumber;
    layer.textMaxWidth = MGLRuntimeStylingHelper.testNumber;
    layer.textLineHeight = MGLRuntimeStylingHelper.testNumber;
    layer.textLetterSpacing = MGLRuntimeStylingHelper.testNumber;
    layer.textJustify = [MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerTextJustifyRight type:@encode(MGLSymbolStyleLayerTextJustify)];
    layer.textAnchor = [MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerTextAnchorBottomRight type:@encode(MGLSymbolStyleLayerTextAnchor)];
    layer.textMaxAngle = MGLRuntimeStylingHelper.testNumber;
    layer.textRotate = MGLRuntimeStylingHelper.testNumber;
    layer.textPadding = MGLRuntimeStylingHelper.testNumber;
    layer.textKeepUpright = MGLRuntimeStylingHelper.testBool;
    layer.textTransform = [MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerTextTransformLowercase type:@encode(MGLSymbolStyleLayerTextTransform)];
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
    layer.iconTranslateAnchor = [MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerIconTranslateAnchorViewport type:@encode(MGLSymbolStyleLayerIconTranslateAnchor)];
    layer.textOpacity = MGLRuntimeStylingHelper.testNumber;
    layer.textColor = MGLRuntimeStylingHelper.testColor;
    layer.textHaloColor = MGLRuntimeStylingHelper.testColor;
    layer.textHaloWidth = MGLRuntimeStylingHelper.testNumber;
    layer.textHaloBlur = MGLRuntimeStylingHelper.testNumber;
    layer.textTranslate = MGLRuntimeStylingHelper.testOffset;
    layer.textTranslateAnchor = [MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerTextTranslateAnchorViewport type:@encode(MGLSymbolStyleLayerTextTranslateAnchor)];

    MGLSymbolStyleLayer *gLayer = [self.mapView.style layerWithIdentifier:@"layerID"];
    XCTAssert([(NSValue *)gLayer.symbolPlacement objCType] == [[MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerSymbolPlacementLine type:@encode(MGLSymbolStyleLayerSymbolPlacement)] objCType]);
    XCTAssertEqualObjects(gLayer.symbolSpacing, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.symbolAvoidEdges, MGLRuntimeStylingHelper.testBool);
    XCTAssertEqualObjects(gLayer.iconAllowOverlap, MGLRuntimeStylingHelper.testBool);
    XCTAssertEqualObjects(gLayer.iconIgnorePlacement, MGLRuntimeStylingHelper.testBool);
    XCTAssertEqualObjects(gLayer.iconOptional, MGLRuntimeStylingHelper.testBool);
    XCTAssert([(NSValue *)gLayer.iconRotationAlignment objCType] == [[MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerIconRotationAlignmentViewport type:@encode(MGLSymbolStyleLayerIconRotationAlignment)] objCType]);
    XCTAssertEqualObjects(gLayer.iconSize, MGLRuntimeStylingHelper.testNumber);
    XCTAssert([(NSValue *)gLayer.iconTextFit objCType] == [[MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerIconTextFitHeight type:@encode(MGLSymbolStyleLayerIconTextFit)] objCType]);
    XCTAssertEqualObjects(gLayer.iconTextFitPadding, MGLRuntimeStylingHelper.testPadding);
    XCTAssertEqualObjects(gLayer.iconImage, MGLRuntimeStylingHelper.testString);
    XCTAssertEqualObjects(gLayer.iconRotate, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.iconPadding, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.iconKeepUpright, MGLRuntimeStylingHelper.testBool);
    XCTAssertEqualObjects(gLayer.iconOffset, MGLRuntimeStylingHelper.testOffset);
    XCTAssert([(NSValue *)gLayer.textPitchAlignment objCType] == [[MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerTextPitchAlignmentViewport type:@encode(MGLSymbolStyleLayerTextPitchAlignment)] objCType]);
    XCTAssert([(NSValue *)gLayer.textRotationAlignment objCType] == [[MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerTextRotationAlignmentViewport type:@encode(MGLSymbolStyleLayerTextRotationAlignment)] objCType]);
    XCTAssertEqualObjects(gLayer.textField, MGLRuntimeStylingHelper.testString);
    XCTAssertEqualObjects(gLayer.textFont, MGLRuntimeStylingHelper.testFont);
    XCTAssertEqualObjects(gLayer.textSize, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textMaxWidth, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textLineHeight, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textLetterSpacing, MGLRuntimeStylingHelper.testNumber);
    XCTAssert([(NSValue *)gLayer.textJustify objCType] == [[MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerTextJustifyRight type:@encode(MGLSymbolStyleLayerTextJustify)] objCType]);
    XCTAssert([(NSValue *)gLayer.textAnchor objCType] == [[MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerTextAnchorBottomRight type:@encode(MGLSymbolStyleLayerTextAnchor)] objCType]);
    XCTAssertEqualObjects(gLayer.textMaxAngle, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textRotate, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textPadding, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textKeepUpright, MGLRuntimeStylingHelper.testBool);
    XCTAssert([(NSValue *)gLayer.textTransform objCType] == [[MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerTextTransformLowercase type:@encode(MGLSymbolStyleLayerTextTransform)] objCType]);
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
    XCTAssert([(NSValue *)gLayer.iconTranslateAnchor objCType] == [[MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerIconTranslateAnchorViewport type:@encode(MGLSymbolStyleLayerIconTranslateAnchor)] objCType]);
    XCTAssertEqualObjects(gLayer.textOpacity, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textColor, MGLRuntimeStylingHelper.testColor);
    XCTAssertEqualObjects(gLayer.textHaloColor, MGLRuntimeStylingHelper.testColor);
    XCTAssertEqualObjects(gLayer.textHaloWidth, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textHaloBlur, MGLRuntimeStylingHelper.testNumber);
    XCTAssertEqualObjects(gLayer.textTranslate, MGLRuntimeStylingHelper.testOffset);
    XCTAssert([(NSValue *)gLayer.textTranslateAnchor objCType] == [[MGLRuntimeStylingHelper testEnum:MGLSymbolStyleLayerTextTranslateAnchorViewport type:@encode(MGLSymbolStyleLayerTextTranslateAnchor)] objCType]);
}

@end
