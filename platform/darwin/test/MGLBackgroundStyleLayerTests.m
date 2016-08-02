// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

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

- (void)testBackgroundLayer
{
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceID:@"sourceID" url:geoJSONURL];    
    MGLBackgroundStyleLayer *layer = [[MGLBackgroundStyleLayer alloc] initWithLayerID:@"layerID" sourceID:@"sourceID"];
    // Layout properties
    // Paint properties
    layer.backgroundColor = MGLRuntimeStylingHelper.testColor;XCTAssert([layer.backgroundColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    layer.backgroundPattern = MGLRuntimeStylingHelper.testString;XCTAssert([layer.backgroundPattern isEqual:MGLRuntimeStylingHelper.testString], @"Should be equal");
    layer.backgroundOpacity = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.backgroundOpacity isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
}

@end
