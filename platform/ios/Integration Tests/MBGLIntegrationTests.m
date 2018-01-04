#import <XCTest/XCTest.h>

@import Mapbox;

@interface MBGLIntegrationTests : XCTestCase <MGLMapViewDelegate>

@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) MGLStyle *style;

@end

@implementation MBGLIntegrationTests {
    XCTestExpectation *_styleLoadingExpectation;
}

- (void)setUp {
    [super setUp];

    [MGLAccountManager setAccessToken:@"pk.feedcafedeadbeefbadebede"];
    NSURL *styleURL = [[NSBundle bundleForClass:[self class]] URLForResource:@"one-liner" withExtension:@"json"];
    self.mapView = [[MGLMapView alloc] initWithFrame:UIScreen.mainScreen.bounds styleURL:styleURL];
    self.mapView.delegate = self;
    if (!self.mapView.style) {
        _styleLoadingExpectation = [self expectationWithDescription:@"Map view should finish loading style."];
        [self waitForExpectationsWithTimeout:1 handler:nil];
    }

    UIView *superView = [[UIView alloc] initWithFrame:UIScreen.mainScreen.bounds];
    [superView addSubview:self.mapView];
    UIWindow *window = [[UIWindow alloc] initWithFrame:UIScreen.mainScreen.bounds];
    [window addSubview:superView];
    [window makeKeyAndVisible];
}

- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    XCTAssertNotNil(mapView.style);
    XCTAssertEqual(mapView.style, style);

    [_styleLoadingExpectation fulfill];
}

- (void)tearDown {
    _styleLoadingExpectation = nil;
    self.mapView = nil;

    [super tearDown];
}

- (MGLStyle *)style {
    return self.mapView.style;
}

- (void)testAddingRemovingOpenGLLayer {
    XCTAssertNotNil(self.style);

    void(^addRemoveGLLayer)(void) = ^{
        MGLOpenGLStyleLayer *layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
        [self.style insertLayer:layer atIndex:0];
        layer = nil;

        [[NSRunLoop currentRunLoop] runUntilDate:[NSDate date]];

        id retrievedLayer = [self.style layerWithIdentifier:@"gl-layer"];
        XCTAssertNotNil(retrievedLayer);
        [self.style removeLayer:retrievedLayer];
    };

    addRemoveGLLayer();
    addRemoveGLLayer();
    addRemoveGLLayer();
}

//- (void)testOpenGLLayerDoesNotLeakWhenCreatedAndDestroyedWithoutAddingToStyle {
//    XCTFail(@"Not yet implemented");
//}

@end
