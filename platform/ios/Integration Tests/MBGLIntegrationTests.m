#import "MGLMapViewIntegrationTest.h"

@interface MBGLIntegrationTests : MGLMapViewIntegrationTest
@end

@implementation MBGLIntegrationTests

#pragma mark - Tests

- (void)waitForMapViewToBeRendered {
    [self waitForMapViewToBeRenderedWithTimeout:5];
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
    self.styleLoadingExpectation = [self expectationWithDescription:@"Map view should finish loading style."];
    [self.mapView setStyleURL:styleURL];
    [self waitForExpectations:@[self.styleLoadingExpectation] timeout:10];

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

        self.styleLoadingExpectation = [self expectationWithDescription:@"Map view should finish loading style."];
        [self waitForExpectationsWithTimeout:10 handler:nil];

        MGLOpenGLStyleLayer *layer = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
        weakLayer = layer;
        [mapView2.style insertLayer:layer atIndex:0];
        layer = nil;

        [self waitForMapViewToBeRendered];
    }
    XCTAssertNil(weakLayer);
}

@end

