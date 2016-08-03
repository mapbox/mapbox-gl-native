// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import <Mapbox/Mapbox.h>
#import "MGLRuntimeStylingHelper.h"
#import <XCTest/XCTest.h>

@interface MGLBackgroundLayerTests : XCTestCase <MGLMapViewDelegate>

@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) XCTestExpectation *expectation;

@end

@implementation MGLBackgroundLayerTests

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

- (void)testBackgroundLayer {
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceID:@"sourceID" url:geoJSONURL];    
    MGLBackgroundStyleLayer *layer = [[MGLBackgroundStyleLayer alloc] initWithLayerID:@"layerID" sourceID:@"sourceID"];
    [_mapView.style addLayer:layer];

    layer.backgroundColor = MGLRuntimeStylingHelper.testColor;
    layer.backgroundPattern = MGLRuntimeStylingHelper.testString;
    layer.backgroundOpacity = MGLRuntimeStylingHelper.testNumber;

    MGLBackgroundStyleLayer *gLayer = [_mapView.style layerWithIdentifier:@"layerID"];
    XCTAssert([gLayer.backgroundColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    XCTAssert([gLayer.backgroundPattern isEqual:MGLRuntimeStylingHelper.testString], @"Should be equal");
    XCTAssert([gLayer.backgroundOpacity isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
}

@end
