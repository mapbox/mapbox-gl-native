// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

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

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)mapViewDidFinishLoadingMap:(MGLMapView *)mapView
{
    [_expectation fulfill];
}

- (void)mapViewDidFinishRenderingMap:(MGLMapView *)mapView fullyRendered:(BOOL)fullyRendered
{
    [_expectation fulfill];
}

- (void)testRasterLayer
{
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceID:@"sourceID" url:geoJSONURL];    
    MGLRasterStyleLayer *layer = [[MGLRasterStyleLayer alloc] initWithLayerID:@"layerID" sourceID:@"sourceID"];
    // Layout properties
    // Paint properties
    layer.rasterOpacity = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.rasterOpacity isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.rasterHueRotate = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.rasterHueRotate isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.rasterBrightnessMin = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.rasterBrightnessMin isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.rasterBrightnessMax = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.rasterBrightnessMax isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.rasterSaturation = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.rasterSaturation isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.rasterContrast = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.rasterContrast isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.rasterFadeDuration = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.rasterFadeDuration isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
}

@end
