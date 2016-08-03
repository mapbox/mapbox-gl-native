// This file is generated. 
// Edit platform/darwin/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import <Mapbox/Mapbox.h>
#import "MGLRuntimeStylingHelper.h"
#import <XCTest/XCTest.h>

@interface MGLRasterLayerTests : XCTestCase <MGLMapViewDelegate>
@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) XCTestExpectation *expectation;
@end

@implementation MGLRasterLayerTests

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

- (void)testRasterLayer
{
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceID:@"sourceID" url:geoJSONURL];    
    MGLRasterStyleLayer *layer = [[MGLRasterStyleLayer alloc] initWithLayerID:@"layerID" sourceID:@"sourceID"];
    [_mapView.style addLayer:layer];

    layer.rasterOpacity = MGLRuntimeStylingHelper.testNumber;
    layer.rasterHueRotate = MGLRuntimeStylingHelper.testNumber;
    layer.rasterBrightnessMin = MGLRuntimeStylingHelper.testNumber;
    layer.rasterBrightnessMax = MGLRuntimeStylingHelper.testNumber;
    layer.rasterSaturation = MGLRuntimeStylingHelper.testNumber;
    layer.rasterContrast = MGLRuntimeStylingHelper.testNumber;
    layer.rasterFadeDuration = MGLRuntimeStylingHelper.testNumber;

    MGLRasterStyleLayer *gLayer = [_mapView.style layerWithIdentifier:@"layerID"];
    XCTAssert([gLayer.rasterOpacity isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.rasterHueRotate isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.rasterBrightnessMin isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.rasterBrightnessMax isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.rasterSaturation isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.rasterContrast isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.rasterFadeDuration isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
}

@end
