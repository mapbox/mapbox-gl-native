// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

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

- (void)testLineLayer
{
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceID:@"sourceID" url:geoJSONURL];    
    MGLLineStyleLayer *layer = [[MGLLineStyleLayer alloc] initWithLayerID:@"layerID" sourceID:@"sourceID"];
    // Layout properties
    // TODO: setterEnum
    // TODO: setterEnum
    layer.lineMiterLimit = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.lineMiterLimit isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.lineRoundLimit = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.lineRoundLimit isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    // Paint properties
    layer.lineOpacity = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.lineOpacity isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.lineColor = MGLRuntimeStylingHelper.testColor;XCTAssert([layer.lineColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    layer.lineTranslate = MGLRuntimeStylingHelper.testOffset;
    // TODO: setterEnum
    layer.lineWidth = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.lineWidth isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.lineGapWidth = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.lineGapWidth isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.lineOffset = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.lineOffset isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.lineBlur = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.lineBlur isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.lineDasharray = MGLRuntimeStylingHelper.testDashArray;
    layer.linePattern = MGLRuntimeStylingHelper.testString;XCTAssert([layer.linePattern isEqual:MGLRuntimeStylingHelper.testString], @"Should be equal");
}

@end
