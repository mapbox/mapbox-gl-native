#import <XCTest/XCTest.h>
#import <objc/message.h>
#import <Mapbox/Mapbox.h>

#import "src/MGLStyleLayer_Private.h"
#import "../ios/src/MGLMapView_Private.h"

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
    [self.mapView setNeedsGLDisplay];
    _renderFinishedExpectation = [self expectationWithDescription:@"Map view should be rendered"];
    [self waitForExpectations:@[_renderFinishedExpectation] timeout:10000];
}

- (void)waitForManagedLayersToExpire {
    //    NSInteger count = MGLStyleLayerRetentionManagerDefaultLifetime;
    //    while (count--) {
    [self waitForMapViewToBeRendered];
    //    }
}

- (BOOL)isLayerBeingManaged:(MGLStyleLayer*)layer {

    MGLOpenGLStyleLayer *glLayer = [layer isKindOfClass:[MGLOpenGLStyleLayer class]] ? (MGLOpenGLStyleLayer*)layer : nil;

    return [glLayer isBeingManaged];
}

- (MGLStyle *)style {
    return self.mapView.style;
}

#pragma mark - Tests


- (void)testLayerRetainer {

    __weak MGLOpenGLStyleLayer *weaklayer1 = nil;
    __weak MGLOpenGLStyleLayerRetainer *weakLayerRetainer = nil;
    __unsafe_unretained MGLOpenGLStyleLayer *unsafelayer1 = nil;


    // For test purposes, we look at the retain count, but for this to make any
    // sense we also need to use unsafe unretained (to stop the call from retaining
    // the very value we're intereested in. We also use autorelease pools. Do
    // NOT use this in production code!!!
    SEL retainCountSelector = NSSelectorFromString(@"retainCount");
    NSInteger (*retainCount)(id, SEL) = (NSInteger (*)(id, SEL))objc_msgSend;

    #define ASSERT_RETAIN_COUNT(object, expectedCount) { \
            NSInteger count = retainCount(object, retainCountSelector); \
            XCTAssert(count == (expectedCount), @"Expected retain count mismatch for %s. Expected %d, got %ld", #object, expectedCount, count); \
        }



    @autoreleasepool {

        // Setup
        TRACE(@"MGLOpenGLStyleLayer allocation:");
        MGLOpenGLStyleLayer *layer1 = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"test"];
        weaklayer1 = layer1;
        unsafelayer1 = layer1;

        ASSERT_RETAIN_COUNT(unsafelayer1, 1);

        // First check to ensure a retainer gets released
        {
            TRACE(@"testing retainer dealloc");

            @autoreleasepool {
                MGLOpenGLStyleLayerRetainer *layerRetainer = [[MGLOpenGLStyleLayerRetainer alloc] initWithLayer:layer1];
                weakLayerRetainer = layerRetainer;
            }

            XCTAssertNil(weakLayerRetainer);
        }

        // Second check to check setting/unsetting owner
        {
            TRACE(@"testing retainer dealloc, after setting/clearing owner");

            @autoreleasepool {
                MGLOpenGLStyleLayerRetainer *layerRetainer = [[MGLOpenGLStyleLayerRetainer alloc] initWithLayer:layer1];
                weakLayerRetainer = layerRetainer;
                layerRetainer.owner = (__bridge void*)self;
            }
            XCTAssertNotNil(weakLayerRetainer);
            weakLayerRetainer.owner = NULL;
            XCTAssertNil(weakLayerRetainer);
        }

        // Now test "retaining" (used by layer during render & adding to style)
        @autoreleasepool {
            TRACE(@"testing retaining");

            // Create a retainer
            MGLOpenGLStyleLayerRetainer *layerRetainer = [[MGLOpenGLStyleLayerRetainer alloc] initWithLayer:layer1];
            weakLayerRetainer = layerRetainer;

            ASSERT_RETAIN_COUNT(unsafelayer1, 1);

            layerRetainer.owner = (__bridge void*)self;

            ASSERT_RETAIN_COUNT(unsafelayer1, 1);

            [layerRetainer retainLayer];

            ASSERT_RETAIN_COUNT(unsafelayer1, 2);
        }

        ASSERT_RETAIN_COUNT(unsafelayer1, 2);

        // Get rid of the layer, but we should still hold on to it via our
        // retainer.
        layer1 = nil;
    }

    XCTAssertNotNil(weaklayer1);
    XCTAssertNotNil(weakLayerRetainer);

    @autoreleasepool {
        XCTAssertNotNil(weakLayerRetainer.layer);
    }

    ASSERT_RETAIN_COUNT(unsafelayer1, 1);

    weakLayerRetainer.owner = NULL;

    XCTAssertNil(weakLayerRetainer);
    XCTAssertNil(weaklayer1);

    #undef ASSERT_RETAIN_COUNT
}


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

- (void)testUsingMapTableWithLayersAsKeys {
    __weak MGLStyleLayer *weakLayer = nil;

    @autoreleasepool {
        NSMapTable *mapTable =  [[NSMapTable alloc] initWithKeyOptions:NSPointerFunctionsStrongMemory|NSPointerFunctionsObjectPointerPersonality
                                                          valueOptions:NSPointerFunctionsStrongMemory
                                                              capacity:100];

        MGLStyleLayer *layer1 = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];

        NSInteger (*retainCount)(id, SEL) = (NSInteger (*)(id, SEL))objc_msgSend;
        SEL retainCountSelector = NSSelectorFromString(@"retainCount");

        NSInteger retainCount1 = retainCount(layer1, retainCountSelector);
        XCTAssert(retainCount1 == 1);

        [mapTable setObject:@(1) forKey:layer1];

        NSInteger retainCount2 = retainCount(layer1, retainCountSelector);
        XCTAssert(retainCount2 == 2);

        NSNumber *number1 = [mapTable objectForKey:layer1];
        XCTAssertNotNil(number1);
        XCTAssert([number1 integerValue] == 1);

        // Try adding a second layer, and ensuring it's there...
        MGLStyleLayer *layer2 = [[MGLOpenGLStyleLayer alloc] initWithIdentifier:@"gl-layer"];
        [mapTable setObject:@(2) forKey:layer2];

        NSNumber *number2 = [mapTable objectForKey:layer2];
        XCTAssertNotNil(number1);
        XCTAssertNotEqual(number1, number2);
        XCTAssertNotEqualObjects(number1, number2);
        XCTAssert([number2 integerValue] == 2);
    }

    XCTAssertNil(weakLayer);
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
    [self waitForManagedLayersToExpire];
    XCTAssertNil(weakLayer2);
}

// Unlike MGLOpenGLStyleLayers other layers are NOT retained, and should be re-created when fetching
// with layerWithIdentifier. This test is to check that assumption (and should fire if we change how
// and if other layer types are retained)
- (void)testUserAddedLineLayersAreNotEqual {
    NSString *filename = [[NSBundle mainBundle] pathForResource:@"polyline" ofType:@"geojson"];
    NSData *shapeData  = [NSData dataWithContentsOfFile:filename];
    MGLShape *shape    = [MGLShape shapeWithData:shapeData encoding:NSUTF8StringEncoding error:nil];
    MGLSource *source1 = [[MGLShapeSource alloc] initWithIdentifier:@"polyline" shape:shape options:nil];

    [self.style addSource:source1];

    MGLStyleLayer *lineLayer1 = [[MGLLineStyleLayer alloc] initWithIdentifier:@"line-layer" source:source1];
    [self.style insertLayer:lineLayer1 atIndex:0];

    // Disconnect the raw layer from the MGLStyleLayer, so that -layerWithIdentifier: is forced to
    // re-create the Obj-C layer.
    [lineLayer1 debugResetRawLayerPeer];

    MGLStyleLayer *lineLayer2 = [self.style layerWithIdentifier:@"line-layer"];
    XCTAssertNotNil(lineLayer2);
    XCTAssert(lineLayer1 != lineLayer2);
    XCTAssertEqualObjects(lineLayer1.identifier, lineLayer2.identifier);
    XCTAssertNotEqualObjects(lineLayer1, lineLayer2); // Not currently considered equal.

    // Now fetch the source. This time we don't reset the peer ptr, so the following should return the
    // original source object
    MGLSource *source2 = [self.style sourceWithIdentifier:@"polyline"];
    XCTAssertNotNil(source2);
    XCTAssert(source1 == source2);
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
            [self waitForManagedLayersToExpire];
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
    XCTAssert([self isLayerBeingManaged:weakLayer]);
    [self waitForMapViewToBeRendered];


    //    XCTAssertNotNil(weakLayer);
    //    XCTAssert([self isLayerBeingManaged:weakLayer]);
    //    [self waitForMapViewToBeRendered];

    XCTAssertNotNil(weakLayer);
    XCTAssert(![self isLayerBeingManaged:weakLayer]);

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
    __weak id weakLayer;

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

    // At this point the C++ CustomLayer will have been destroyed, BUT we're holding on
    // to the obj-c layer. We don't currently clear the rawLayer, so it could now be
    // pointing at garbage. And the following line will crash (due to description
    // using .rawLayer)

//    (void)layer2;
    XCTFail();
//    [self.style insertLayer:layer2 atIndex:0];


    [self waitForManagedLayersToExpire];

    // Asking the style for the layer should return nil
    MGLStyleLayer *layer3 = [self.mapView.style layerWithIdentifier:@"gl-layer"];
    XCTAssertNil(layer3);
//    XCTAssertNil(weakLayer);
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
