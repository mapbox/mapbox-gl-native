#import <XCTest/XCTest.h>
#import <objc/message.h>
#import <Mapbox/Mapbox.h>


#define TRACE(message) NSLog(@"%@: %@", NSStringFromSelector(_cmd), message)


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

- (void)testStoringOpenGLLayerInCollections {
    // If we change the meaning of equality for MGLStyleLayer we want to know about it - since we
    // store layers in both a NSMutableSet and in a NSMapTable
    MGLStyleLayer *layer1 = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
    MGLStyleLayer *layer2 = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];

    XCTAssertNotEqual(layer1, layer2);          // Test diff pointers, in case behind the scenes it tries to return the same object.
    XCTAssertNotEqualObjects(layer1, layer2);   // Although they have the same identifier, we DO NOT consider them equal.
    NSMutableSet *set = [NSMutableSet set];
    [set addObject:layer1];
    [set addObject:layer2];
    XCTAssert(set.count == 2);
}

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

    // At this point, layer2 (and layer1) should still be around, since the render process needs to
    // keep a reference to them.
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

            NSUInteger address1 = (NSUInteger)layer;
            layer = nil;
            
            // We want to ensure the MGLOpenGLStyleLayer's raw Layer's Impl gets handed to a RenderLayer
            // (which in turn has a raw context pointer back to the layer)
            [self waitForMapViewToBeRendered];

            retrievedLayer = [self.style layerWithIdentifier:@"gl-layer"];
            NSUInteger address2 = (NSUInteger)retrievedLayer;

            XCTAssertNotNil(retrievedLayer);

            // These two should be the same object in the case of MGLOpenGLStyleLayer
            XCTAssert(address1 == address2);

            [self.style removeLayer:retrievedLayer];

            // We need to run the runloop for a little while (2 trips through the renderer) to allow
            // the managed layer to be released. This is because it takes 2 renders before the RenderLayer
            // (and the associated Impl) are destroyed.
            [self waitForMapViewToBeRendered];
        }

        XCTAssertNil(retrievedLayer);
    };

    addRemoveGLLayer();
    addRemoveGLLayer();
    addRemoveGLLayer();
}

- (void)testReusingOpenGLLayer {
    NSTimeInterval waitInterval = 0.02;

    NSLog(@"testReusingOpenGLLayer - init and insert layer");
    MGLOpenGLStyleLayer *layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
    [self.style insertLayer:layer atIndex:0];

    [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];

    NSLog(@"testReusingOpenGLLayer - remove layer %p", layer);
    [self.style removeLayer:layer];

    [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];

    NSLog(@"testReusingOpenGLLayer - re-insert layer %p", layer);

    [self.style insertLayer:layer atIndex:0];

    [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];

    NSLog(@"testReusingOpenGLLayer - re-remove layer %p", layer);

    [self.style removeLayer:layer];

    [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];
}

- (void)testOpenGLLayerDoesNotLeakWhenRemovedFromStyle {

    //    MGLOpenGLStyleLayer *layer;
    __weak id weakLayer;
    @autoreleasepool {
        MGLOpenGLStyleLayer *layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
        weakLayer = layer;
        [self.style insertLayer:layer atIndex:0];
        layer = nil;

        // Run the render loop, so the layer gets used/managed.
        [self waitForMapViewToBeRendered];
        //        [[NSRunLoop currentRunLoop] runUntilDate:[[NSDate date] dateByAddingTimeInterval:waitInterval]];

        [self.style removeLayer:[self.style layerWithIdentifier:@"gl-layer"]];
    }

    MGLStyleLayer *layer2 = weakLayer;

    XCTAssertNotNil(weakLayer);
    [self waitForMapViewToBeRendered];

    layer2 = nil;
    XCTAssertNil(weakLayer);
}

- (void)testUserAddedLineLayerIsReleasedWhenStyleChanged {
    __weak MGLStyleLayer* weakLayer1 = nil;
    __weak MGLStyleLayer* weakLayer2 = nil;

    __unsafe_unretained MGLStyleLayer* unsafeLayer1 = nil;
    __unsafe_unretained MGLStyleLayer* unsafeLayer2 = nil;

    // Add a line layer
    @autoreleasepool {
        NSString *filename = [[NSBundle mainBundle] pathForResource:@"polyline" ofType:@"geojson"];
        NSData *shapeData = [NSData dataWithContentsOfFile:filename];
        MGLShape *shape = [MGLShape shapeWithData:shapeData encoding:NSUTF8StringEncoding error:nil];
        MGLSource *source1 = [[MGLShapeSource alloc] initWithIdentifier:@"polyline" shape:shape options:nil];

        [self.style addSource:source1];
        MGLStyleLayer *lineLayer1 = [[MGLLineStyleLayer alloc] initWithIdentifier:@"line-layer" source:source1];
        [self.style insertLayer:lineLayer1 atIndex:0];

        unsafeLayer1 = lineLayer1;
        weakLayer1 = lineLayer1;
    }
    XCTAssertNil(weakLayer1);

    // Look for the same layer by Id
    @autoreleasepool {
        MGLStyleLayer *lineLayer2 = [self.style layerWithIdentifier:@"line-layer"];
        MGLSource *source2 = [self.style sourceWithIdentifier:@"polyline"];

        XCTAssertNotNil(lineLayer2);
        XCTAssertNotNil(source2);

        unsafeLayer2 = lineLayer2;
        weakLayer2 = lineLayer2;
    }
    XCTAssertNil(weakLayer2);

    // Since we only add MGLOpenGLStyleLayers to our retain/release management set, the above layers
    // will have been released, but we want to check that the second one was re-created. So just
    // check pointer values
    XCTAssert(unsafeLayer1);
    XCTAssert(unsafeLayer2);
    XCTAssert(unsafeLayer1 != unsafeLayer2);

    // Swap style
    NSURL *styleURL = [[NSBundle bundleForClass:[self class]] URLForResource:@"one-liner" withExtension:@"json"];
    _styleLoadingExpectation = [self expectationWithDescription:@"Map view should finish loading style."];
    [self.mapView setStyleURL:styleURL];
    [self waitForExpectations:@[_styleLoadingExpectation] timeout:10];

    // Asking the style for the layer should return nil
    {
        MGLStyleLayer *lineLayer = [self.style layerWithIdentifier:@"line-layer"];
        MGLSource *lineSource = [self.style sourceWithIdentifier:@"polyline"];
        XCTAssertNil(lineLayer);
        XCTAssertNil(lineSource);
    }
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

    // Unlike other MGLStyleLayers, MGLOpenGLStyleLayers are managed for retain/release purposes
    XCTAssertNotNil(weakLayer);

    [self waitForMapViewToBeRendered];

    // should be identical to the original
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

        // Let the render process fire, to ensure a RenderLayer gets created with a raw pointer back
        // to the layer (and it should be retained).
        [self waitForMapViewToBeRendered];

        // We don't wait for the layers to be released here, we'll just let the MGLMapView dealloc
        // so everything should be cleaned up.
    }
    XCTAssertNil(weakLayer);
}

@end

