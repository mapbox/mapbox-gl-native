#import "MGLMapViewIntegrationTest.h"

@interface MBGLIntegrationTests : MGLMapViewIntegrationTest
@end

@implementation MBGLIntegrationTests

#pragma mark - Tests

- (void)waitForMapViewToBeRendered {
    [self waitForMapViewToBeRenderedWithTimeout:10];
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




@end

