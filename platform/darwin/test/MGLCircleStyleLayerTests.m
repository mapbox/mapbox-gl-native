// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

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

- (void)testCircleLayer
{
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceID:@"sourceID" url:geoJSONURL];    
    MGLCircleStyleLayer *layer = [[MGLCircleStyleLayer alloc] initWithLayerID:@"layerID" sourceID:@"sourceID"];
    // Layout properties
    // Paint properties
    layer.circleRadius = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.circleRadius isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.circleColor = MGLRuntimeStylingHelper.testColor;XCTAssert([layer.circleColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    layer.circleBlur = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.circleBlur isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.circleOpacity = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.circleOpacity isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.circleTranslate = MGLRuntimeStylingHelper.testOffset;
    // TODO: setterEnum
    // TODO: setterEnum
}

@end
