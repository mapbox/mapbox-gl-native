// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import <Mapbox/Mapbox.h>
#import "MGLRuntimeStylingHelper.h"
#import <XCTest/XCTest.h>

@interface MGLLineLayerTests : XCTestCase <MGLMapViewDelegate>

@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) XCTestExpectation *expectation;

@end

@implementation MGLLineLayerTests

- (void)setUp {
    [super setUp];
    UIApplication *app = [[UIApplication sharedApplication] delegate];
    UIViewController *vc = [[UIViewController alloc] init];
    app.keyWindow.rootViewController = vc;
    [vc view]; // Force load xib
    _mapView = [[MGLMapView alloc] initWithFrame:CGRectMake(0, 0, 256, 256)];
    [vc.view addSubview:_mapView];
    _mapView.delegate = self;
}

- (void)testLineLayer {
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceIdentifier:@"sourceID" URL:geoJSONURL];
    MGLLineStyleLayer *layer = [[MGLLineStyleLayer alloc] initWithLayerIdentifier:@"layerID" sourceIdentifier:@"sourceID"];
    [_mapView.style addLayer:layer];

    // TODO: setterEnum
    // TODO: setterEnum
    layer.lineMiterLimit = MGLRuntimeStylingHelper.testNumber;
    layer.lineRoundLimit = MGLRuntimeStylingHelper.testNumber;
    layer.lineOpacity = MGLRuntimeStylingHelper.testNumber;
    layer.lineColor = MGLRuntimeStylingHelper.testColor;
    layer.lineTranslate = MGLRuntimeStylingHelper.testOffset;
    // TODO: setterEnum
    layer.lineWidth = MGLRuntimeStylingHelper.testNumber;
    layer.lineGapWidth = MGLRuntimeStylingHelper.testNumber;
    layer.lineOffset = MGLRuntimeStylingHelper.testNumber;
    layer.lineBlur = MGLRuntimeStylingHelper.testNumber;
    layer.lineDasharray = MGLRuntimeStylingHelper.testDashArray;
    layer.linePattern = MGLRuntimeStylingHelper.testString;

    MGLLineStyleLayer *gLayer = [_mapView.style layerWithIdentifier:@"layerID"];
    // TODO: getterEnum
    // TODO: getterEnum
    XCTAssert([gLayer.lineMiterLimit isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.lineRoundLimit isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.lineOpacity isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.lineColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    XCTAssert([gLayer.lineTranslate isEqual:MGLRuntimeStylingHelper.testOffset], @"Should be equal");
    // TODO: getterEnum
    XCTAssert([gLayer.lineWidth isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.lineGapWidth isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.lineOffset isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.lineBlur isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.lineDasharray isEqual:MGLRuntimeStylingHelper.testDashArray], @"Should be equal");
    XCTAssert([gLayer.linePattern isEqual:MGLRuntimeStylingHelper.testString], @"Should be equal");
}

@end
