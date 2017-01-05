// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLCircleLayerTests : MGLStyleLayerTests
@end

@implementation MGLCircleLayerTests

+ (NSString *)layerType {
    return @"circle";
}

- (void)testCircleLayer {
    NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"amsterdam" ofType:@"geojson"];
    NSURL *url = [NSURL fileURLWithPath:filePath];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" URL:url options:nil];
    [self.mapView.style addSource:source];
    MGLCircleStyleLayer *layer = [[MGLCircleStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    [self.mapView.style addLayer:layer];

    layer.circleBlur = [MGLRuntimeStylingHelper testNumber];
    layer.circleColor = [MGLRuntimeStylingHelper testColor];
    layer.circleOpacity = [MGLRuntimeStylingHelper testNumber];
    layer.circleRadius = [MGLRuntimeStylingHelper testNumber];
    layer.circleScaleAlignment = [MGLRuntimeStylingHelper testEnum:MGLCircleScaleAlignmentViewport type:@encode(MGLCircleScaleAlignment)];
    layer.circleTranslate = [MGLRuntimeStylingHelper testOffset];
    layer.circleTranslateAnchor = [MGLRuntimeStylingHelper testEnum:MGLCircleTranslateAnchorViewport type:@encode(MGLCircleTranslateAnchor)];

    MGLCircleStyleLayer *gLayer = (MGLCircleStyleLayer *)[self.mapView.style layerWithIdentifier:@"layerID"];
    XCTAssertTrue([gLayer isKindOfClass:[MGLCircleStyleLayer class]]);
    XCTAssertEqualObjects(gLayer.circleBlur, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.circleColor, [MGLRuntimeStylingHelper testColor]);
    XCTAssertEqualObjects(gLayer.circleOpacity, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.circleRadius, [MGLRuntimeStylingHelper testNumber]);
    XCTAssert([gLayer.circleScaleAlignment isKindOfClass:[MGLStyleConstantValue class]]);
    XCTAssertEqualObjects(gLayer.circleScaleAlignment, [MGLRuntimeStylingHelper testEnum:MGLCircleScaleAlignmentViewport type:@encode(MGLCircleScaleAlignment)]);
    XCTAssertEqualObjects(gLayer.circleTranslate, [MGLRuntimeStylingHelper testOffset]);
    XCTAssert([gLayer.circleTranslateAnchor isKindOfClass:[MGLStyleConstantValue class]]);
    XCTAssertEqualObjects(gLayer.circleTranslateAnchor, [MGLRuntimeStylingHelper testEnum:MGLCircleTranslateAnchorViewport type:@encode(MGLCircleTranslateAnchor)]);

    layer.circleBlur = [MGLRuntimeStylingHelper testNumberFunction];
    layer.circleColor = [MGLRuntimeStylingHelper testColorFunction];
    layer.circleOpacity = [MGLRuntimeStylingHelper testNumberFunction];
    layer.circleRadius = [MGLRuntimeStylingHelper testNumberFunction];
    layer.circleScaleAlignment = [MGLRuntimeStylingHelper testEnumFunction:MGLCircleScaleAlignmentViewport type:@encode(MGLCircleScaleAlignment)];
    layer.circleTranslate = [MGLRuntimeStylingHelper testOffsetFunction];
    layer.circleTranslateAnchor = [MGLRuntimeStylingHelper testEnumFunction:MGLCircleTranslateAnchorViewport type:@encode(MGLCircleTranslateAnchor)];

    XCTAssertEqualObjects(gLayer.circleBlur, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.circleColor, [MGLRuntimeStylingHelper testColorFunction]);
    XCTAssertEqualObjects(gLayer.circleOpacity, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.circleRadius, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.circleScaleAlignment, [MGLRuntimeStylingHelper testEnumFunction:MGLCircleScaleAlignmentViewport type:@encode(MGLCircleScaleAlignment)]);
    XCTAssertEqualObjects(gLayer.circleTranslate, [MGLRuntimeStylingHelper testOffsetFunction]);
    XCTAssertEqualObjects(gLayer.circleTranslateAnchor, [MGLRuntimeStylingHelper testEnumFunction:MGLCircleTranslateAnchorViewport type:@encode(MGLCircleTranslateAnchor)]);
}

- (void)testPropertyNames {
    [self testPropertyName:@"circle-blur" isBoolean:NO];
    [self testPropertyName:@"circle-color" isBoolean:NO];
    [self testPropertyName:@"circle-opacity" isBoolean:NO];
    [self testPropertyName:@"circle-radius" isBoolean:NO];
    [self testPropertyName:@"circle-scale-alignment" isBoolean:NO];
    [self testPropertyName:@"circle-translate" isBoolean:NO];
    [self testPropertyName:@"circle-translate-anchor" isBoolean:NO];
}

@end
