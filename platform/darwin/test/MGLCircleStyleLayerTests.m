// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import <Mapbox/Mapbox.h>
#import "MGLRuntimeStylingHelper.h"
#import <XCTest/XCTest.h>

@interface MGLCircleLayerTests : XCTestCase <MGLMapViewDelegate>

@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) XCTestExpectation *expectation;

@end

@implementation MGLCircleLayerTests

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

- (void)testCircleLayer {
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceIdentifier:@"sourceID" URL:geoJSONURL];
    MGLCircleStyleLayer *layer = [[MGLCircleStyleLayer alloc] initWithLayerIdentifier:@"layerID" sourceIdentifier:@"sourceID"];
    [_mapView.style addLayer:layer];

    layer.circleRadius = MGLRuntimeStylingHelper.testNumber;
    layer.circleColor = MGLRuntimeStylingHelper.testColor;
    layer.circleBlur = MGLRuntimeStylingHelper.testNumber;
    layer.circleOpacity = MGLRuntimeStylingHelper.testNumber;
    layer.circleTranslate = MGLRuntimeStylingHelper.testOffset;
    // TODO: setterEnum
    // TODO: setterEnum

    MGLCircleStyleLayer *gLayer = [_mapView.style layerWithIdentifier:@"layerID"];
    XCTAssert([gLayer.circleRadius isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.circleColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    XCTAssert([gLayer.circleBlur isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.circleOpacity isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.circleTranslate isEqual:MGLRuntimeStylingHelper.testOffset], @"Should be equal");
    // TODO: getterEnum
    // TODO: getterEnum
}

@end
