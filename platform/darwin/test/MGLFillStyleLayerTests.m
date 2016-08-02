// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import <Mapbox/Mapbox.h>
#import "MGLRuntimeStylingHelper.h"
#import <XCTest/XCTest.h>

@interface MGLFillLayerTests : XCTestCase <MGLMapViewDelegate>
@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) XCTestExpectation *expectation;
@end

@implementation MGLFillLayerTests

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

- (void)testFillLayer
{
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceID:@"sourceID" url:geoJSONURL];    
    MGLFillStyleLayer *layer = [[MGLFillStyleLayer alloc] initWithLayerID:@"layerID" sourceID:@"sourceID"];
    // Layout properties
    // Paint properties
    layer.fillAntialias = MGLRuntimeStylingHelper.testBool;XCTAssert([layer.fillAntialias isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    layer.fillOpacity = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.fillOpacity isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.fillColor = MGLRuntimeStylingHelper.testColor;XCTAssert([layer.fillColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    layer.fillOutlineColor = MGLRuntimeStylingHelper.testColor;XCTAssert([layer.fillOutlineColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    layer.fillTranslate = MGLRuntimeStylingHelper.testOffset;
    // TODO: setterEnum
    layer.fillPattern = MGLRuntimeStylingHelper.testString;XCTAssert([layer.fillPattern isEqual:MGLRuntimeStylingHelper.testString], @"Should be equal");
}

@end
