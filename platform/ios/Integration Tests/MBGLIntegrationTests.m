#import <XCTest/XCTest.h>
#import <objc/message.h>
#import <Mapbox/Mapbox.h>

@interface MBGLIntegrationTests : XCTestCase <MGLMapViewDelegate>

@property (nonatomic) MGLMapView *mapView;
@property (nonatomic) MGLStyle *style;

@end

@implementation MBGLIntegrationTests {
    XCTestExpectation *_styleLoadingExpectation;
    XCTestExpectation *_renderFinishedExpectation;
}

#pragma mark - Setup/Teardown

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

- (void)tearDown {
    _styleLoadingExpectation = nil;
    self.mapView = nil;
    self.style = nil;

    [super tearDown];
}

#pragma mark - MGLMapViewDelegate

- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    XCTAssertNotNil(mapView.style);
    XCTAssertEqual(mapView.style, style);

    [_styleLoadingExpectation fulfill];
}

- (void)mapViewDidFinishRenderingFrame:(MGLMapView *)mapView fullyRendered:(__unused BOOL)fullyRendered {
    [_renderFinishedExpectation fulfill];
    _renderFinishedExpectation = nil;
}

#pragma mark - Utilities

- (void)waitForMapViewToBeRendered {
    [self.mapView setNeedsDisplay];
    _renderFinishedExpectation = [self expectationWithDescription:@"Map view should be rendered"];
    [self waitForExpectations:@[_renderFinishedExpectation] timeout:1];
}

- (MGLStyle *)style {
    return self.mapView.style;
}

#pragma mark - Tests

// This test does not strictly need to be in this test file/target. Including here for convenience.
- (void)testOpenGLLayerDoesNotLeakWhenCreatedAndDestroyedWithoutAddingToStyle {
    MGLOpenGLStyleLayer *layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
    __weak id weakLayer = layer;
    layer = nil;

    XCTAssertNil(weakLayer);
}

- (void)testAddingRemovingOpenGLLayerWithoutRendering {
    XCTAssertNotNil(self.style);

    void(^addRemoveGLLayer)(void) = ^{
        __weak id weakLayer = nil;

        @autoreleasepool {
            MGLOpenGLStyleLayer *layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
            [self.style insertLayer:layer atIndex:0];
            weakLayer = layer;

            // Nil the layer prior to remove to ensure it's being retained
            layer = nil;
            [self.style removeLayer:weakLayer];
        }

        XCTAssertNil(weakLayer);
    };

    addRemoveGLLayer();
    addRemoveGLLayer();
    addRemoveGLLayer();
}

- (void)testReusingOpenGLLayerIdentifier {
    __weak MGLOpenGLStyleLayer *weakLayer2;

    @autoreleasepool {
        MGLOpenGLStyleLayer *layer1 = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
        [self.style insertLayer:layer1 atIndex:0];
        [self waitForMapViewToBeRendered];
        [self.style removeLayer:layer1];

        MGLOpenGLStyleLayer *layer2 = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
        weakLayer2 = layer2;

        XCTAssertNotNil(layer2);
        XCTAssert(layer1 != layer2);

        [self.style insertLayer:layer2 atIndex:0];
        [self waitForMapViewToBeRendered];
        [self.style removeLayer:layer2];

        XCTAssertNil([layer1 style]);
        XCTAssertNil([layer2 style]);
    }

    // At this point, layer2 (and layer1) should still be around,
    // since the render process needs to keep a reference to them.
    XCTAssertNotNil(weakLayer2);

    // Let render loop run enough to release the layers
    [self waitForMapViewToBeRendered];
    XCTAssertNil(weakLayer2);
}

- (void)testAddingRemovingOpenGLLayer {
    XCTAssertNotNil(self.style);

    void(^addRemoveGLLayer)(void) = ^{

        __weak id retrievedLayer = nil;
        
        @autoreleasepool {
            MGLOpenGLStyleLayer *layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
            [self.style insertLayer:layer atIndex:0];
            layer = nil;
            
            [self waitForMapViewToBeRendered];

            retrievedLayer = [self.style layerWithIdentifier:@"gl-layer"];
            XCTAssertNotNil(retrievedLayer);

            [self.style removeLayer:retrievedLayer];
            [self waitForMapViewToBeRendered];
        }

        XCTAssertNil(retrievedLayer);
    };

    addRemoveGLLayer();
    addRemoveGLLayer();
    addRemoveGLLayer();
}

- (void)testReusingOpenGLLayer {
    MGLOpenGLStyleLayer *layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
    [self.style insertLayer:layer atIndex:0];
    [self waitForMapViewToBeRendered];
    
    [self.style removeLayer:layer];
    [self waitForMapViewToBeRendered];
    
    [self.style insertLayer:layer atIndex:0];
    [self waitForMapViewToBeRendered];

    [self.style removeLayer:layer];
    [self waitForMapViewToBeRendered];
}

- (void)testOpenGLLayerDoesNotLeakWhenRemovedFromStyle {
    __weak id weakLayer;
    @autoreleasepool {
        MGLOpenGLStyleLayer *layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
        weakLayer = layer;
        [self.style insertLayer:layer atIndex:0];
        layer = nil;

        [self waitForMapViewToBeRendered];
        [self.style removeLayer:[self.style layerWithIdentifier:@"gl-layer"]];
    }

    MGLStyleLayer *layer2 = weakLayer;

    XCTAssertNotNil(weakLayer);
    [self waitForMapViewToBeRendered];

    layer2 = nil;
    XCTAssertNil(weakLayer);
}

- (void)testOpenGLLayerDoesNotLeakWhenStyleChanged {
    __weak MGLOpenGLStyleLayer *weakLayer;

    @autoreleasepool {
        {
            MGLOpenGLStyleLayer *layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
            weakLayer = layer;
            [self.style insertLayer:layer atIndex:0];
            layer = nil;
        }
    }

    XCTAssertNotNil(weakLayer);

    [self waitForMapViewToBeRendered];

    MGLStyleLayer *layer2 = [self.mapView.style layerWithIdentifier:@"gl-layer"];

    NSURL *styleURL = [[NSBundle bundleForClass:[self class]] URLForResource:@"one-liner" withExtension:@"json"];
    _styleLoadingExpectation = [self expectationWithDescription:@"Map view should finish loading style."];
    [self.mapView setStyleURL:styleURL];
    [self waitForExpectations:@[_styleLoadingExpectation] timeout:10];

    // At this point the C++ CustomLayer will have been destroyed, and the rawLayer pointer has been NULLed
    XCTAssert(weakLayer == layer2);
    XCTAssertNotNil(weakLayer);

    // Asking the style for the layer should return nil
    MGLStyleLayer *layer3 = [self.mapView.style layerWithIdentifier:@"gl-layer"];
    XCTAssertNil(layer3);
}


- (void)testOpenGLLayerDoesNotLeakWhenMapViewDeallocs {
    __weak id weakLayer;
 
    @autoreleasepool {

        NSURL *styleURL = [[NSBundle bundleForClass:[self class]] URLForResource:@"one-liner" withExtension:@"json"];
        MGLMapView *mapView2 = [[MGLMapView alloc] initWithFrame:UIScreen.mainScreen.bounds styleURL:styleURL];
        mapView2.delegate = self;

        XCTAssertNil(mapView2.style);

        _styleLoadingExpectation = [self expectationWithDescription:@"Map view should finish loading style."];
        [self waitForExpectationsWithTimeout:1 handler:nil];

        MGLOpenGLStyleLayer *layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
        weakLayer = layer;
        [mapView2.style insertLayer:layer atIndex:0];
        layer = nil;

        [self waitForMapViewToBeRendered];
    }
    XCTAssertNil(weakLayer);
}

@end

