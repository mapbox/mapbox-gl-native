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

- (void)testFillLayer
{
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceID:@"sourceID" url:geoJSONURL];    
    MGLFillStyleLayer *layer = [[MGLFillStyleLayer alloc] initWithLayerID:@"layerID" sourceID:@"sourceID"];
    [_mapView.style addLayer:layer];

    layer.fillAntialias = MGLRuntimeStylingHelper.testBool;
    layer.fillOpacity = MGLRuntimeStylingHelper.testNumber;
    layer.fillColor = MGLRuntimeStylingHelper.testColor;
    layer.fillOutlineColor = MGLRuntimeStylingHelper.testColor;
    layer.fillTranslate = MGLRuntimeStylingHelper.testOffset;
    // TODO: setterEnum
    layer.fillPattern = MGLRuntimeStylingHelper.testString;

    MGLFillStyleLayer *gLayer = [_mapView.style layerWithIdentifier:@"layerID"];
    XCTAssert([gLayer.fillAntialias isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    XCTAssert([gLayer.fillOpacity isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.fillColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    XCTAssert([gLayer.fillOutlineColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    XCTAssert([gLayer.fillTranslate isEqual:MGLRuntimeStylingHelper.testOffset], @"Should be equal");
    // TODO: setterEnum
    XCTAssert([gLayer.fillPattern isEqual:MGLRuntimeStylingHelper.testString], @"Should be equal");
}

@end
