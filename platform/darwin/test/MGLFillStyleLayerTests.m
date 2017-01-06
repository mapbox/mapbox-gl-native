// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import "MGLStyleLayerTests.h"

@interface MGLFillLayerTests : MGLStyleLayerTests
@end

@implementation MGLFillLayerTests

+ (NSString *)layerType {
    return @"fill";
}

- (void)testFillLayer {
    NSString *filePath = [[NSBundle bundleForClass:self.class] pathForResource:@"amsterdam" ofType:@"geojson"];
    NSURL *url = [NSURL fileURLWithPath:filePath];
    MGLShapeSource *source = [[MGLShapeSource alloc] initWithIdentifier:@"sourceID" URL:url options:nil];
    [self.mapView.style addSource:source];
    MGLFillStyleLayer *layer = [[MGLFillStyleLayer alloc] initWithIdentifier:@"layerID" source:source];
    [self.mapView.style addLayer:layer];

    layer.fillAntialiased = [MGLRuntimeStylingHelper testBool];
    layer.fillColor = [MGLRuntimeStylingHelper testColor];
    layer.fillOpacity = [MGLRuntimeStylingHelper testNumber];
    layer.fillOutlineColor = [MGLRuntimeStylingHelper testColor];
    layer.fillPattern = [MGLRuntimeStylingHelper testString];
    layer.fillTranslation = [MGLRuntimeStylingHelper testOffset];
    layer.fillTranslationAnchor = [MGLRuntimeStylingHelper testEnum:MGLFillTranslationAnchorViewport type:@encode(MGLFillTranslationAnchor)];

    MGLFillStyleLayer *gLayer = (MGLFillStyleLayer *)[self.mapView.style layerWithIdentifier:@"layerID"];
    XCTAssertTrue([gLayer isKindOfClass:[MGLFillStyleLayer class]]);
    XCTAssertEqualObjects(gLayer.fillAntialiased, [MGLRuntimeStylingHelper testBool]);
    XCTAssertEqualObjects(gLayer.fillColor, [MGLRuntimeStylingHelper testColor]);
    XCTAssertEqualObjects(gLayer.fillOpacity, [MGLRuntimeStylingHelper testNumber]);
    XCTAssertEqualObjects(gLayer.fillOutlineColor, [MGLRuntimeStylingHelper testColor]);
    XCTAssertEqualObjects(gLayer.fillPattern, [MGLRuntimeStylingHelper testString]);
    XCTAssertEqualObjects(gLayer.fillTranslation, [MGLRuntimeStylingHelper testOffset]);
    XCTAssert([gLayer.fillTranslationAnchor isKindOfClass:[MGLStyleConstantValue class]]);
    XCTAssertEqualObjects(gLayer.fillTranslationAnchor, [MGLRuntimeStylingHelper testEnum:MGLFillTranslationAnchorViewport type:@encode(MGLFillTranslationAnchor)]);

    layer.fillAntialiased = [MGLRuntimeStylingHelper testBoolFunction];
    layer.fillColor = [MGLRuntimeStylingHelper testColorFunction];
    layer.fillOpacity = [MGLRuntimeStylingHelper testNumberFunction];
    layer.fillOutlineColor = [MGLRuntimeStylingHelper testColorFunction];
    layer.fillPattern = [MGLRuntimeStylingHelper testStringFunction];
    layer.fillTranslation = [MGLRuntimeStylingHelper testOffsetFunction];
    layer.fillTranslationAnchor = [MGLRuntimeStylingHelper testEnumFunction:MGLFillTranslationAnchorViewport type:@encode(MGLFillTranslationAnchor)];

    XCTAssertEqualObjects(gLayer.fillAntialiased, [MGLRuntimeStylingHelper testBoolFunction]);
    XCTAssertEqualObjects(gLayer.fillColor, [MGLRuntimeStylingHelper testColorFunction]);
    XCTAssertEqualObjects(gLayer.fillOpacity, [MGLRuntimeStylingHelper testNumberFunction]);
    XCTAssertEqualObjects(gLayer.fillOutlineColor, [MGLRuntimeStylingHelper testColorFunction]);
    XCTAssertEqualObjects(gLayer.fillPattern, [MGLRuntimeStylingHelper testStringFunction]);
    XCTAssertEqualObjects(gLayer.fillTranslation, [MGLRuntimeStylingHelper testOffsetFunction]);
    XCTAssertEqualObjects(gLayer.fillTranslationAnchor, [MGLRuntimeStylingHelper testEnumFunction:MGLFillTranslationAnchorViewport type:@encode(MGLFillTranslationAnchor)]);
}

- (void)testPropertyNames {
    [self testPropertyName:@"is-fill-antialiased" isBoolean:YES];
    [self testPropertyName:@"fill-color" isBoolean:NO];
    [self testPropertyName:@"fill-opacity" isBoolean:NO];
    [self testPropertyName:@"fill-outline-color" isBoolean:NO];
    [self testPropertyName:@"fill-pattern" isBoolean:NO];
    [self testPropertyName:@"fill-translation" isBoolean:NO];
    [self testPropertyName:@"fill-translation-anchor" isBoolean:NO];
}

@end
