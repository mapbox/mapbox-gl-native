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
    self.style = nil;

    [super tearDown];
}

- (MGLStyle *)style {
    return self.mapView.style;
}

- (void)testAddingRemovingOpenGLLayer {
    XCTAssertNotNil(self.style);

    // Test fails with 0.1 (presumably because it's < one frame, ie. 1/60)
    NSTimeInterval waitInterval = 0.02;

    void(^addRemoveGLLayer)(void) = ^{

        MGLOpenGLStyleLayer *layer = nil;
        __weak id retrievedLayer = nil;
        
        @autoreleasepool {
            layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
            [self.style insertLayer:layer atIndex:0];
            layer = nil;
            
            [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];
            
            retrievedLayer = [self.style layerWithIdentifier:@"gl-layer"];
            XCTAssertNotNil(retrievedLayer);
            [self.style removeLayer:retrievedLayer];

            // We need to run the runloop for a little while so that the following assert will be correct
            // this is because although the layer has been removed from the style, there's still a pending
            // render (deinitialize) call, that will needs to be handled, which will finally release the
            // layer (and then the layer will be dealloc'd when the autorelease pool drains)
            [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];
        }
        
        XCTAssertNil(retrievedLayer);
    };

    addRemoveGLLayer();
    addRemoveGLLayer();
    addRemoveGLLayer();
}

- (void)testAddingRemovingOpenGLLayerWithoutRendering {
    XCTAssertNotNil(self.style);

    void(^addRemoveGLLayer)(void) = ^{
        MGLOpenGLStyleLayer *layer = nil;
        __weak id weakLayer = nil;
        
        @autoreleasepool {

            layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
            [self.style insertLayer:layer atIndex:0];
            weakLayer = layer;
            layer = nil;
            [self.style removeLayer:weakLayer];
        }
        
        XCTAssertNil(weakLayer);
    };

    addRemoveGLLayer();
    addRemoveGLLayer();
    addRemoveGLLayer();
}

- (void)testReusingOpenGLLayer {
    NSTimeInterval waitInterval = 0.02;

    MGLOpenGLStyleLayer *layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
    [self.style insertLayer:layer atIndex:0];

    [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];

    [self.style removeLayer:layer];

    [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];

    [self.style insertLayer:layer atIndex:0];

    [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];

    [self.style removeLayer:layer];

    [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];
}

// This test does not strictly need to be in this test file/target. Including here for convenience.
- (void)testOpenGLLayerDoesNotLeakWhenCreatedAndDestroyedWithoutAddingToStyle {
    MGLOpenGLStyleLayer *layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
    __weak id weakLayer = layer;
    layer = nil;
    
    XCTAssertNil(weakLayer);
}

- (void)testOpenGLLayerDoesNotLeakWhenMapViewDeallocs {
    NSTimeInterval waitInterval = 0.02;
    __weak id weakLayer;

    @autoreleasepool {

        NSURL *styleURL = [[NSBundle bundleForClass:[self class]] URLForResource:@"one-liner" withExtension:@"json"];
        MGLMapView *mapView2 = [[MGLMapView alloc] initWithFrame:UIScreen.mainScreen.bounds styleURL:styleURL];
        mapView2.delegate = self;

        XCTAssertNil(mapView2.style);

        _styleLoadingExpectation = [self expectationWithDescription:@"Map view should finish loading style."];
        [self waitForExpectationsWithTimeout:1 handler:nil];

//        UIView *superView = [[UIView alloc] initWithFrame:UIScreen.mainScreen.bounds];
//        [superView addSubview:self.mapView];
//        UIWindow *window = [[UIWindow alloc] initWithFrame:UIScreen.mainScreen.bounds];
//        [window addSubview:superView];
//        [window makeKeyAndVisible];

        MGLOpenGLStyleLayer *layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
        weakLayer = layer;
        [mapView2.style insertLayer:layer atIndex:0];
        layer = nil;

        [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];
        [self.mapView setNeedsDisplay];
        [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];
    }
    XCTAssertNil(weakLayer);
}





- (void)testOpenGLLayerDoesNotLeakWhenRemovedFromStyle {
    NSTimeInterval waitInterval = 0.02;

    MGLOpenGLStyleLayer *layer;
    __weak id weakLayer;
    @autoreleasepool {
        layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
        weakLayer = layer;
        [self.style insertLayer:layer atIndex:0];
        layer = nil;

        [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];

        [self.style removeLayer:[self.style layerWithIdentifier:@"gl-layer"]];
        [self.mapView setNeedsDisplay];

        [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];
    }
    XCTAssertNil(weakLayer);
}

- (void)testOpenGLLayerDoesNotLeakWhenStyleChanged {
    NSTimeInterval waitInterval = 0.02;
    __weak id weakLayer;

    @autoreleasepool {
        MGLOpenGLStyleLayer *layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
        weakLayer = layer;
        [self.style insertLayer:layer atIndex:0];
        layer = nil;

        [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];

        NSURL *styleURL = [[NSBundle bundleForClass:[self class]] URLForResource:@"one-liner" withExtension:@"json"];
        _styleLoadingExpectation = [self expectationWithDescription:@"Map view should finish loading style."];
        [self.mapView setStyleURL:styleURL];
    
        [self waitForExpectationsWithTimeout:1 handler:nil];
        [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];
    }
    XCTAssertNil(weakLayer);
}

- (void)testReusingOpenGLLayerIdentifier {
    NSTimeInterval waitInterval = 0.02;
    MGLOpenGLStyleLayer *layer1, *layer2;
    @autoreleasepool {
        layer1 = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
        [self.style insertLayer:layer1 atIndex:0];

        [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];

        [self.style removeLayer:layer1];

        layer2 = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
        [self.style insertLayer:layer2 atIndex:0];

        [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];

        [self.style removeLayer:layer2];

        [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];
    }
    XCTAssertNotNil(layer1);
    XCTAssertNotNil(layer2);
    XCTAssertNil([layer1 style]);
    XCTAssertNil([layer2 style]);
}

@end
