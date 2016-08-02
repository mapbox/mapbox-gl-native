// This file is generated. 
// Edit platform/ios/scripts/generate-style-code.js, then run `make style-code-darwin`.

#import <Mapbox/Mapbox.h>
#import "MGLRuntimeStylingHelper.h"
#import <XCTest/XCTest.h>

@interface MGLSymbolLayerTests : XCTestCase <MGLMapViewDelegate>
@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) XCTestExpectation *expectation;
@end

@implementation MGLSymbolLayerTests

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

- (void)testSymbolLayer
{
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceID:@"sourceID" url:geoJSONURL];    
    MGLSymbolStyleLayer *layer = [[MGLSymbolStyleLayer alloc] initWithLayerID:@"layerID" sourceID:@"sourceID"];
    // Layout properties
    // TODO: setterEnum
    layer.symbolSpacing = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.symbolSpacing isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.symbolAvoidEdges = MGLRuntimeStylingHelper.testBool;XCTAssert([layer.symbolAvoidEdges isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    layer.iconAllowOverlap = MGLRuntimeStylingHelper.testBool;XCTAssert([layer.iconAllowOverlap isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    layer.iconIgnorePlacement = MGLRuntimeStylingHelper.testBool;XCTAssert([layer.iconIgnorePlacement isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    layer.iconOptional = MGLRuntimeStylingHelper.testBool;XCTAssert([layer.iconOptional isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    // TODO: setterEnum
    layer.iconSize = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.iconSize isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    // TODO: setterEnum
    layer.iconTextFitPadding = MGLRuntimeStylingHelper.testPadding;
    layer.iconImage = MGLRuntimeStylingHelper.testString;XCTAssert([layer.iconImage isEqual:MGLRuntimeStylingHelper.testString], @"Should be equal");
    layer.iconRotate = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.iconRotate isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.iconPadding = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.iconPadding isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.iconKeepUpright = MGLRuntimeStylingHelper.testBool;XCTAssert([layer.iconKeepUpright isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    layer.iconOffset = MGLRuntimeStylingHelper.testOffset;
    // TODO: setterEnum
    // TODO: setterEnum
    layer.textField = MGLRuntimeStylingHelper.testString;XCTAssert([layer.textField isEqual:MGLRuntimeStylingHelper.testString], @"Should be equal");
    layer.textFont = MGLRuntimeStylingHelper.testFont;
    layer.textSize = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.textSize isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.textMaxWidth = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.textMaxWidth isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.textLineHeight = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.textLineHeight isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.textLetterSpacing = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.textLetterSpacing isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    // TODO: setterEnum
    // TODO: setterEnum
    layer.textMaxAngle = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.textMaxAngle isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.textRotate = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.textRotate isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.textPadding = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.textPadding isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.textKeepUpright = MGLRuntimeStylingHelper.testBool;XCTAssert([layer.textKeepUpright isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    // TODO: setterEnum
    layer.textOffset = MGLRuntimeStylingHelper.testOffset;
    layer.textAllowOverlap = MGLRuntimeStylingHelper.testBool;XCTAssert([layer.textAllowOverlap isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    layer.textIgnorePlacement = MGLRuntimeStylingHelper.testBool;XCTAssert([layer.textIgnorePlacement isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    layer.textOptional = MGLRuntimeStylingHelper.testBool;XCTAssert([layer.textOptional isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    // Paint properties
    layer.iconOpacity = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.iconOpacity isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.iconColor = MGLRuntimeStylingHelper.testColor;XCTAssert([layer.iconColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    layer.iconHaloColor = MGLRuntimeStylingHelper.testColor;XCTAssert([layer.iconHaloColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    layer.iconHaloWidth = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.iconHaloWidth isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.iconHaloBlur = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.iconHaloBlur isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.iconTranslate = MGLRuntimeStylingHelper.testOffset;
    // TODO: setterEnum
    layer.textOpacity = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.textOpacity isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.textColor = MGLRuntimeStylingHelper.testColor;XCTAssert([layer.textColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    layer.textHaloColor = MGLRuntimeStylingHelper.testColor;XCTAssert([layer.textHaloColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    layer.textHaloWidth = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.textHaloWidth isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.textHaloBlur = MGLRuntimeStylingHelper.testNumber;XCTAssert([layer.textHaloBlur isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    layer.textTranslate = MGLRuntimeStylingHelper.testOffset;
    // TODO: setterEnum
}

@end
