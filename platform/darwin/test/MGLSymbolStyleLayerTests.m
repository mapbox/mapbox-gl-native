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

- (void)testSymbolLayer
{
    NSURL *geoJSONURL = [NSURL URLWithString:@"https://dl.dropboxusercontent.com/u/5285447/amsterdam.geojson"];
    MGLGeoJSONSource *source = [[MGLGeoJSONSource alloc] initWithSourceID:@"sourceID" url:geoJSONURL];    
    MGLSymbolStyleLayer *layer = [[MGLSymbolStyleLayer alloc] initWithLayerID:@"layerID" sourceID:@"sourceID"];
    [_mapView.style addLayer:layer];

    // TODO: setterEnum
    layer.symbolSpacing = MGLRuntimeStylingHelper.testNumber;
    layer.symbolAvoidEdges = MGLRuntimeStylingHelper.testBool;
    layer.iconAllowOverlap = MGLRuntimeStylingHelper.testBool;
    layer.iconIgnorePlacement = MGLRuntimeStylingHelper.testBool;
    layer.iconOptional = MGLRuntimeStylingHelper.testBool;
    // TODO: setterEnum
    layer.iconSize = MGLRuntimeStylingHelper.testNumber;
    // TODO: setterEnum
    layer.iconTextFitPadding = MGLRuntimeStylingHelper.testPadding;
    layer.iconImage = MGLRuntimeStylingHelper.testString;
    layer.iconRotate = MGLRuntimeStylingHelper.testNumber;
    layer.iconPadding = MGLRuntimeStylingHelper.testNumber;
    layer.iconKeepUpright = MGLRuntimeStylingHelper.testBool;
    layer.iconOffset = MGLRuntimeStylingHelper.testOffset;
    // TODO: setterEnum
    // TODO: setterEnum
    layer.textField = MGLRuntimeStylingHelper.testString;
    layer.textFont = MGLRuntimeStylingHelper.testFont;
    layer.textSize = MGLRuntimeStylingHelper.testNumber;
    layer.textMaxWidth = MGLRuntimeStylingHelper.testNumber;
    layer.textLineHeight = MGLRuntimeStylingHelper.testNumber;
    layer.textLetterSpacing = MGLRuntimeStylingHelper.testNumber;
    // TODO: setterEnum
    // TODO: setterEnum
    layer.textMaxAngle = MGLRuntimeStylingHelper.testNumber;
    layer.textRotate = MGLRuntimeStylingHelper.testNumber;
    layer.textPadding = MGLRuntimeStylingHelper.testNumber;
    layer.textKeepUpright = MGLRuntimeStylingHelper.testBool;
    // TODO: setterEnum
    layer.textOffset = MGLRuntimeStylingHelper.testOffset;
    layer.textAllowOverlap = MGLRuntimeStylingHelper.testBool;
    layer.textIgnorePlacement = MGLRuntimeStylingHelper.testBool;
    layer.textOptional = MGLRuntimeStylingHelper.testBool;
    layer.iconOpacity = MGLRuntimeStylingHelper.testNumber;
    layer.iconColor = MGLRuntimeStylingHelper.testColor;
    layer.iconHaloColor = MGLRuntimeStylingHelper.testColor;
    layer.iconHaloWidth = MGLRuntimeStylingHelper.testNumber;
    layer.iconHaloBlur = MGLRuntimeStylingHelper.testNumber;
    layer.iconTranslate = MGLRuntimeStylingHelper.testOffset;
    // TODO: setterEnum
    layer.textOpacity = MGLRuntimeStylingHelper.testNumber;
    layer.textColor = MGLRuntimeStylingHelper.testColor;
    layer.textHaloColor = MGLRuntimeStylingHelper.testColor;
    layer.textHaloWidth = MGLRuntimeStylingHelper.testNumber;
    layer.textHaloBlur = MGLRuntimeStylingHelper.testNumber;
    layer.textTranslate = MGLRuntimeStylingHelper.testOffset;
    // TODO: setterEnum

    MGLSymbolStyleLayer *gLayer = [_mapView.style layerWithIdentifier:@"layerID"];
    // TODO: setterEnum
    XCTAssert([gLayer.symbolSpacing isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.symbolAvoidEdges isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    XCTAssert([gLayer.iconAllowOverlap isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    XCTAssert([gLayer.iconIgnorePlacement isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    XCTAssert([gLayer.iconOptional isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    // TODO: setterEnum
    XCTAssert([gLayer.iconSize isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    // TODO: setterEnum
    XCTAssert([gLayer.iconTextFitPadding isEqual:MGLRuntimeStylingHelper.testPadding], @"Should be equal");
    XCTAssert([gLayer.iconImage isEqual:MGLRuntimeStylingHelper.testString], @"Should be equal");
    XCTAssert([gLayer.iconRotate isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.iconPadding isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.iconKeepUpright isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    XCTAssert([gLayer.iconOffset isEqual:MGLRuntimeStylingHelper.testOffset], @"Should be equal");
    // TODO: setterEnum
    // TODO: setterEnum
    XCTAssert([gLayer.textField isEqual:MGLRuntimeStylingHelper.testString], @"Should be equal");
    XCTAssert([gLayer.textFont isEqual:MGLRuntimeStylingHelper.testFont], @"Should be equal");
    XCTAssert([gLayer.textSize isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.textMaxWidth isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.textLineHeight isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.textLetterSpacing isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    // TODO: setterEnum
    // TODO: setterEnum
    XCTAssert([gLayer.textMaxAngle isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.textRotate isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.textPadding isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.textKeepUpright isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    // TODO: setterEnum
    XCTAssert([gLayer.textOffset isEqual:MGLRuntimeStylingHelper.testOffset], @"Should be equal");
    XCTAssert([gLayer.textAllowOverlap isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    XCTAssert([gLayer.textIgnorePlacement isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    XCTAssert([gLayer.textOptional isEqual:MGLRuntimeStylingHelper.testBool], @"Should be equal");
    XCTAssert([gLayer.iconOpacity isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.iconColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    XCTAssert([gLayer.iconHaloColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    XCTAssert([gLayer.iconHaloWidth isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.iconHaloBlur isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.iconTranslate isEqual:MGLRuntimeStylingHelper.testOffset], @"Should be equal");
    // TODO: setterEnum
    XCTAssert([gLayer.textOpacity isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.textColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    XCTAssert([gLayer.textHaloColor isEqual:MGLRuntimeStylingHelper.testColor], @"Should be equal");
    XCTAssert([gLayer.textHaloWidth isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.textHaloBlur isEqual:MGLRuntimeStylingHelper.testNumber], @"Should be equal");
    XCTAssert([gLayer.textTranslate isEqual:MGLRuntimeStylingHelper.testOffset], @"Should be equal");
    // TODO: setterEnum
}

@end
