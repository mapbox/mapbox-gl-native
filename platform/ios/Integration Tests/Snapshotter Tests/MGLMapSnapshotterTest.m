#import "MGLMapViewIntegrationTest.h"
#import "MGLMapSnapshotter_Private.h"

@interface MGLMapSnapshotter ()
@property (nonatomic) BOOL cancelled;
@end


@interface MGLMapSnapshotterTest : MGLMapViewIntegrationTest
@end

// Convenience func to create snapshotter
MGLMapSnapshotter* snapshotterWithCoordinates(CLLocationCoordinate2D coordinates, CGSize size) {
    // Create snapshot options
    MGLMapCamera* mapCamera    = [[MGLMapCamera alloc] init];
    mapCamera.pitch            = 20;
    mapCamera.centerCoordinate = coordinates;
    MGLMapSnapshotOptions* options = [[MGLMapSnapshotOptions alloc] initWithStyleURL:[MGLStyle satelliteStreetsStyleURL]
                                                                              camera:mapCamera
                                                                                size:size];
    options.zoomLevel = 10;

    // Create and start the snapshotter
    MGLMapSnapshotter* snapshotter = [[MGLMapSnapshotter alloc] initWithOptions:options];
    return snapshotter;
}

MGLMapSnapshotter* snapshotterWithBounds(MGLCoordinateBounds bounds, CGSize size) {

    MGLMapCamera* mapCamera = [[MGLMapCamera alloc] init];
    MGLMapSnapshotOptions* options = [[MGLMapSnapshotOptions alloc] initWithStyleURL:[MGLStyle satelliteStreetsStyleURL]
                                                                              camera:mapCamera
                                                                                size:size];
    options.coordinateBounds = bounds;

    // Create and start the snapshotter
    MGLMapSnapshotter* snapshotter = [[MGLMapSnapshotter alloc] initWithOptions:options];
    return snapshotter;
}



@implementation MGLMapSnapshotterTest

- (void)testMultipleSnapshotsWithASingleSnapshotter🔒 {
    CGSize size = self.mapView.bounds.size;

    XCTestExpectation *expectation = [self expectationWithDescription:@"snapshots"];
    expectation.expectedFulfillmentCount = 2;
    expectation.assertForOverFulfill = YES;

    CLLocationCoordinate2D coord = CLLocationCoordinate2DMake(30.0, 30.0);

    MGLMapSnapshotter *snapshotter = snapshotterWithCoordinates(coord, size);
    XCTAssertNotNil(snapshotter);

    [snapshotter startWithCompletionHandler:^(MGLMapSnapshot * _Nullable snapshot, NSError * _Nullable error) {
        [expectation fulfill];
    }];

    @try {
        [snapshotter startWithCompletionHandler:^(MGLMapSnapshot * _Nullable snapshot, NSError * _Nullable error) {
            XCTFail(@"Should not be called - but should it?");
        }];
        XCTFail(@"Should not be called");
    }
    @catch (NSException *exception) {
        XCTAssert(exception.name == NSInternalInconsistencyException);
        [expectation fulfill];
    }

    [self waitForExpectations:@[expectation] timeout:10.0];
}

- (void)testDeallocatingSnapshotterDuringSnapshot🔒 {
    // See also https://github.com/mapbox/mapbox-gl-native/issues/12336

    NSTimeInterval timeout         = 10.0;
    XCTestExpectation *expectation = [self expectationWithDescription:@"snapshot"];
    CGSize size                    = self.mapView.bounds.size;
    CLLocationCoordinate2D coord   = CLLocationCoordinate2DMake(30.0, 30.0);

    // Test triggering to main queue
    dispatch_queue_t backgroundQueue = dispatch_get_main_queue();
//  dispatch_queue_t backgroundQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);

    __weak __typeof__(self) weakself = self;

    dispatch_async(backgroundQueue, ^{

        dispatch_group_t dg = dispatch_group_create();
        dispatch_group_enter(dg);

        dispatch_async(dispatch_get_main_queue(), ^{
            MGLMapSnapshotter *snapshotter = snapshotterWithCoordinates(coord, size);
            __weak MGLMapSnapshotter *weakSnapshotter = snapshotter;
            
            [snapshotter startWithCompletionHandler:^(MGLMapSnapshot * _Nullable snapshot, NSError * _Nullable error) {
                // We expect this completion block to be called with an error
                __typeof__(self) strongself = weakself;

                MGLTestAssertNil(strongself, snapshot);
                MGLTestAssert(strongself,
                              ([error.domain isEqualToString:MGLErrorDomain] && error.code == MGLErrorCodeSnapshotFailed),
                              @"Should have errored");
                MGLTestAssertNil(strongself, weakSnapshotter, @"Snapshotter should have been deallocated");

                dispatch_group_leave(dg);
            }];
        });

        dispatch_group_notify(dg, dispatch_get_main_queue(), ^{
            [expectation fulfill];
        });
    });

    [self waitForExpectations:@[expectation] timeout:timeout];
}

- (void)testSnapshotterUsingNestedDispatchQueues🔒 {
    // This is the opposite pair to the above test `testDeallocatingSnapshotterDuringSnapshot`
    // The only significant difference is that the snapshotter is a `__block` variable, so
    // its lifetime should continue until it's set to nil in the completion block.
    // See also https://github.com/mapbox/mapbox-gl-native/issues/12336

    NSTimeInterval timeout         = 10.0;
    XCTestExpectation *expectation = [self expectationWithDescription:@"snapshot"];
    CGSize size                    = self.mapView.bounds.size;
    CLLocationCoordinate2D coord   = CLLocationCoordinate2DMake(30.0, 30.0);

    // Test triggering to main queue
    dispatch_queue_t backgroundQueue = dispatch_get_main_queue();
    //  dispatch_queue_t backgroundQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);

    __weak __typeof__(self) weakself = self;

    dispatch_async(backgroundQueue, ^{

        dispatch_group_t dg = dispatch_group_create();
        dispatch_group_enter(dg);

        dispatch_async(dispatch_get_main_queue(), ^{

            __block MGLMapSnapshotter *snapshotter = snapshotterWithCoordinates(coord, size);

            [snapshotter startWithCompletionHandler:^(MGLMapSnapshot * _Nullable snapshot, NSError * _Nullable error) {
                // We expect this completion block to be called with an error
                __typeof__(self) strongself = weakself;
                MGLTestAssertNotNil(strongself, snapshot);
                MGLTestAssertNil(strongself, error, @"Snapshotter should have completed");
                dispatch_group_leave(dg);
                snapshotter = nil;
            }];
        });

        dispatch_group_notify(dg, dispatch_get_main_queue(), ^{
            [expectation fulfill];
        });
    });

    [self waitForExpectations:@[expectation] timeout:timeout];
}

- (void)testCancellingSnapshot🔒 {
    XCTestExpectation *expectation = [self expectationWithDescription:@"snapshots"];
    expectation.assertForOverFulfill = YES;
    expectation.expectedFulfillmentCount = 1;

    CGSize size                    = self.mapView.bounds.size;
    CLLocationCoordinate2D coord   = CLLocationCoordinate2DMake(30.0, 30.0);

    MGLMapSnapshotter *snapshotter = snapshotterWithCoordinates(coord, size);

    __weak __typeof__(self) weakself = self;

    [snapshotter startWithCompletionHandler:^(MGLMapSnapshot * _Nullable snapshot, NSError * _Nullable error) {
        // We expect this completion block to be called with an error
        __typeof__(self) strongself = weakself;

        MGLTestAssertNil(strongself, snapshot);
        MGLTestAssert(strongself,
                      ([error.domain isEqualToString:MGLErrorDomain] && error.code == MGLErrorCodeSnapshotFailed),
                      @"Should have been cancelled");
        MGLTestAssert(strongself, snapshotter.cancelled, @"Should have been cancelled");
        [expectation fulfill];
    }];

    [snapshotter cancel];

    [self waitForExpectations:@[expectation] timeout:5.0];
}

- (void)testAllocatingSnapshotOnBackgroundQueue🔒 {
    XCTestExpectation *expectation = [self expectationWithDescription:@"snapshots"];

    CGSize size                  = self.mapView.bounds.size;
    CLLocationCoordinate2D coord = CLLocationCoordinate2DMake(30.0, 30.0);

    dispatch_queue_attr_t attr = dispatch_queue_attr_make_with_qos_class(DISPATCH_QUEUE_SERIAL, QOS_CLASS_USER_INITIATED, QOS_MIN_RELATIVE_PRIORITY);
    dispatch_queue_t backgroundQueue = dispatch_queue_create(__PRETTY_FUNCTION__, attr);

    dispatch_async(backgroundQueue, ^{

        // Create the snapshotter - DO NOT START.
        MGLMapSnapshotter* snapshotter = snapshotterWithCoordinates(coord, size);

        dispatch_group_t group = dispatch_group_create();
        dispatch_group_enter(group);

        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            dispatch_group_leave(group);
        });

        dispatch_group_wait(group, DISPATCH_TIME_FOREVER);

        snapshotter = nil;

        dispatch_sync(dispatch_get_main_queue(), ^{
            [expectation fulfill];
        });
    });

    [self waitForExpectations:@[expectation] timeout:2.0];
}

- (void)testSnapshotterFromBackgroundQueueShouldFail🔒 {
    CGSize size = self.mapView.bounds.size;
    CLLocationCoordinate2D coord = CLLocationCoordinate2DMake(30.0, 30.0);

    XCTestExpectation *expectation = [self expectationWithDescription:@"snapshots"];
    expectation.expectedFulfillmentCount = 1;
    expectation.assertForOverFulfill = YES;

    __weak __typeof__(self) weakself = self;

    dispatch_queue_attr_t attr = dispatch_queue_attr_make_with_qos_class(DISPATCH_QUEUE_SERIAL, QOS_CLASS_USER_INITIATED, QOS_MIN_RELATIVE_PRIORITY); // also for concurrent
    dispatch_queue_t backgroundQueue = dispatch_queue_create(__PRETTY_FUNCTION__, attr);

    // Use dispatch_group to keep the backgroundQueue block around (and
    // so also the MGLMapSnapshotter
    dispatch_group_t group = dispatch_group_create();
    dispatch_group_enter(group);

    dispatch_async(backgroundQueue, ^{

        MGLMapSnapshotter *snapshotter = snapshotterWithCoordinates(coord, size);
        XCTAssertNotNil(snapshotter);

        MGLMapSnapshotCompletionHandler completion = ^(MGLMapSnapshot * _Nullable snapshot, NSError * _Nullable error) {
            // The completion block should not be called
            MGLTestFail(weakself);
            dispatch_group_leave(group);
        };

        @try {
            [snapshotter startWithCompletionHandler:completion];
            MGLTestFail(weakself, @"startWithCompletionHandler: should raise an exception");
        }
        @catch (NSException *exception) {
            MGLTestAssert(weakself, exception.name == NSInvalidArgumentException);
            dispatch_group_leave(group);
        }

        // Wait for the snapshot to complete
        dispatch_group_wait(group, DISPATCH_TIME_FOREVER);

        snapshotter = nil;

        dispatch_sync(dispatch_get_main_queue(), ^{
            [expectation fulfill];
        });
    });

    [self waitForExpectations:@[expectation] timeout:60.0];
}

- (void)testMultipleSnapshotters🔒🙁 {
    NSUInteger numSnapshots = 8;
    CGSize size = self.mapView.bounds.size;

    XCTestExpectation *expectation = [self expectationWithDescription:@"snapshots"];
    expectation.expectedFulfillmentCount = numSnapshots;
    expectation.assertForOverFulfill = YES;

    __weak __typeof__(self) weakself = self;

    for (size_t run = 0; run < numSnapshots; run++) {

        float ratio = (float)run/(float)numSnapshots;
        float lon = (ratio*120.0) + ((1.0-ratio)*54.0);
        CLLocationCoordinate2D coord = CLLocationCoordinate2DMake(57.0, lon);

        __block MGLMapSnapshotter *snapshotter;

        // Allocate from an autorelease pool here, to avoid having
        // snapshotter retained for longer than we'd like to test.
        @autoreleasepool {
            snapshotter = snapshotterWithCoordinates(coord, size);
            XCTAssertNotNil(snapshotter);
        }

        [snapshotter startWithCompletionHandler:^(MGLMapSnapshot * _Nullable snapshot, NSError * _Nullable error) {

            // This should be the main queue
            __typeof__(self) strongself = weakself;

            MGLTestAssertNotNil(strongself, strongself);

            MGLTestAssertNotNil(strongself, snapshot);
            MGLTestAssertNotNil(strongself, snapshot.image);
            MGLTestAssertNil(strongself, error, @"Snapshot should not error with: %@", error);

            // Change this to XCTAttachmentLifetimeKeepAlways to be able to look at the snapshots after running
            XCTAttachment *attachment = [XCTAttachment attachmentWithImage:snapshot.image];
            attachment.lifetime = XCTAttachmentLifetimeDeleteOnSuccess;
            [strongself addAttachment:attachment];

            // Dealloc the snapshotter (by having this line in the block, we
            // also retained the snapshotter. Setting to nil should release, as
            // this block should be the only thing retaining it (since it was
            // allocated from the above autorelease pool)
            snapshotter = nil;

            [expectation fulfill];
        }];
    } // end for loop

    [self waitForExpectations:@[expectation] timeout:60.0];
}

- (void)testSnapshotPointConversion🔒 {
    CGSize size = self.mapView.bounds.size;

    XCTestExpectation *expectation = [self expectationWithDescription:@"snapshot"];
    expectation.expectedFulfillmentCount = 1;
    expectation.assertForOverFulfill = YES;

    CLLocationCoordinate2D coord = CLLocationCoordinate2DMake(30.0, 30.0);

    MGLMapSnapshotter *snapshotter = snapshotterWithCoordinates(coord, size);
    XCTAssertNotNil(snapshotter);

    __weak __typeof__(self) weakself = self;

    [snapshotter startWithCompletionHandler:^(MGLMapSnapshot * _Nullable snapshot, NSError * _Nullable error) {

        __typeof__(self) myself = weakself;

        MGLTestAssertNotNil(myself, snapshot);

        CGPoint point = [snapshot pointForCoordinate:coord];

        CGFloat epsilon = 0.000001;

        MGLTestAssertEqualWithAccuracy(myself, point.x, size.width/2.0, epsilon);
        MGLTestAssertEqualWithAccuracy(myself, point.y, size.height/2.0, epsilon);

        CLLocationCoordinate2D coord2 = [snapshot coordinateForPoint:point];

        MGLTestAssertEqualWithAccuracy(myself, coord.latitude, coord2.latitude, epsilon);
        MGLTestAssertEqualWithAccuracy(myself, coord.longitude, coord2.longitude, epsilon);

        [expectation fulfill];
    }];

    [self waitForExpectations:@[expectation] timeout:10.0];
}

- (void)testSnapshotPointConversionCoordinateOrdering🔒 {
    CGSize size = self.mapView.bounds.size;

    XCTestExpectation *expectation = [self expectationWithDescription:@"snapshot"];
    expectation.expectedFulfillmentCount = 1;
    expectation.assertForOverFulfill = YES;

    CLLocationCoordinate2D coord = CLLocationCoordinate2DMake(30.0, 30.0);

    MGLMapSnapshotter *snapshotter = snapshotterWithCoordinates(coord, size);
    XCTAssertNotNil(snapshotter);

    __weak __typeof__(self) weakself = self;

    [snapshotter startWithCompletionHandler:^(MGLMapSnapshot * _Nullable snapshot, NSError * _Nullable error) {

        __typeof__(self) myself = weakself;

        CGFloat epsilon = 0.000001;

        MGLTestAssertNotNil(myself, snapshot);

        CLLocationCoordinate2D coordTL = [snapshot coordinateForPoint:CGPointZero];

        MGLTestAssert(myself, coordTL.longitude < coord.longitude);
        MGLTestAssert(myself, coordTL.latitude > coord.latitude);

        // And check point
        CGPoint tl = [snapshot pointForCoordinate:coordTL];
        MGLTestAssertEqualWithAccuracy(myself, tl.x, 0.0, epsilon);
        MGLTestAssertEqualWithAccuracy(myself, tl.y, 0.0, epsilon);

        CLLocationCoordinate2D coordBR = [snapshot coordinateForPoint:CGPointMake(size.width, size.height)];

        MGLTestAssert(myself, coordBR.longitude > coord.longitude);
        MGLTestAssert(myself, coordBR.latitude < coord.latitude);

        [expectation fulfill];
    }];

    [self waitForExpectations:@[expectation] timeout:10.0];
}

- (void)testSnapshotWithOverlayHandlerFailure🔒 {
    CGSize size = self.mapView.bounds.size;

    XCTestExpectation *expectation = [self expectationWithDescription:@"snapshot with overlay fails"];
    expectation.expectedFulfillmentCount = 2;

    CLLocationCoordinate2D coord = CLLocationCoordinate2DMake(30.0, 30.0);

    MGLMapSnapshotter *snapshotter = snapshotterWithCoordinates(coord, size);
    XCTAssertNotNil(snapshotter);

    [snapshotter startWithOverlayHandler:^(MGLMapSnapshotOverlay *snapshotOverlay) {
        XCTAssertNotNil(snapshotOverlay);
        
        UIGraphicsEndImageContext();
        [expectation fulfill];
    } completionHandler:^(MGLMapSnapshot * _Nullable snapshot, NSError * _Nullable error) {
        XCTAssertNil(snapshot);
        XCTAssertNotNil(error);
        XCTAssertEqualObjects(error.domain, MGLErrorDomain);
        XCTAssertEqual(error.code, MGLErrorCodeSnapshotFailed);
        XCTAssertEqualObjects(error.localizedDescription, @"Failed to generate composited snapshot.");
                
        [expectation fulfill];
    }];

    [self waitForExpectations:@[expectation] timeout:10.0];
}

- (void)testSnapshotWithOverlayHandlerSuccess🔒 {
    CGSize size = self.mapView.bounds.size;
    CGRect snapshotRect = CGRectMake(0, 0, size.width, size.height);

    XCTestExpectation *expectation = [self expectationWithDescription:@"snapshot with overlay succeeds"];
    expectation.expectedFulfillmentCount = 2;

    CLLocationCoordinate2D coord = CLLocationCoordinate2DMake(30.0, 30.0);

    MGLMapSnapshotter *snapshotter = snapshotterWithCoordinates(coord, size);
    XCTAssertNotNil(snapshotter);
    
    CGFloat scale = snapshotter.options.scale;
    
    [snapshotter startWithOverlayHandler:^(MGLMapSnapshotOverlay *snapshotOverlay) {
        XCTAssertNotNil(snapshotOverlay);
        
        CGFloat width = CGBitmapContextGetWidth(snapshotOverlay.context);
        CGFloat height = CGBitmapContextGetHeight(snapshotOverlay.context);

        CGRect contextRect = CGContextConvertRectToDeviceSpace(snapshotOverlay.context, CGRectMake(0, 0, 1, 0));
        CGFloat scaleFromContext = contextRect.size.width;
        XCTAssertEqual(scale, scaleFromContext);
        XCTAssertEqual(width, size.width*scale);
        XCTAssertEqual(height, size.height*scale);
        
        CGContextSetFillColorWithColor(snapshotOverlay.context, [UIColor.greenColor CGColor]);
        CGContextSetAlpha(snapshotOverlay.context, 0.2);
        CGContextAddRect(snapshotOverlay.context, snapshotRect);
        CGContextFillRect(snapshotOverlay.context, snapshotRect);
        [expectation fulfill];
    } completionHandler:^(MGLMapSnapshot * _Nullable snapshot, NSError * _Nullable error) {
        XCTAssertNil(error);
        XCTAssertNotNil(snapshot);
        [expectation fulfill];
    }];

    [self waitForExpectations:@[expectation] timeout:10.0];
}

- (void)testSnapshotCoordinatesWithOverlayHandler🔒 {
    CGSize size = self.mapView.bounds.size;

    XCTestExpectation *expectation = [self expectationWithDescription:@"snapshot with overlay succeeds"];
    expectation.expectedFulfillmentCount = 2;

    CLLocationCoordinate2D london = { .latitude = 51.5074, .longitude = -0.1278 };
    CLLocationCoordinate2D paris  = { .latitude = 48.8566, .longitude = 2.3522 };

    MGLCoordinateBounds bounds = {
        .ne = london,
        .sw = paris
    };

    MGLMapSnapshotter *snapshotter = snapshotterWithBounds(bounds, size);
    XCTAssertNotNil(snapshotter);

    void (^testCoordinates)(id<MGLMapSnapshotProtocol>) = ^(id<MGLMapSnapshotProtocol> snapshot){
        XCTAssertNotNil(snapshot);

        CGPoint londonPoint = [snapshot pointForCoordinate:london];
        CGPoint parisPoint  = [snapshot pointForCoordinate:paris];

        XCTAssertEqualWithAccuracy(londonPoint.x, 0, 0.1);
        XCTAssertEqualWithAccuracy(parisPoint.x, size.width, 0.1);

        // Vertically, London and Paris are inset (due to the size vs coordinate bounds)
        XCTAssert(parisPoint.y > londonPoint.y);
        XCTAssert(londonPoint.y > 0.0);
        XCTAssert(parisPoint.y < size.height);

        CLLocationCoordinate2D london2 = [snapshot coordinateForPoint:londonPoint];
        CLLocationCoordinate2D paris2  = [snapshot coordinateForPoint:parisPoint];

        XCTAssertEqualWithAccuracy(london.latitude,  london2.latitude,  0.0000001);
        XCTAssertEqualWithAccuracy(london.longitude, london2.longitude, 0.0000001);
        XCTAssertEqualWithAccuracy(paris.latitude,   paris2.latitude,   0.0000001);
        XCTAssertEqualWithAccuracy(paris.longitude,  paris2.longitude,  0.0000001);
    };

    [snapshotter startWithOverlayHandler:^(MGLMapSnapshotOverlay *snapshotOverlay) {
        XCTAssert([snapshotOverlay conformsToProtocol:@protocol(MGLMapSnapshotProtocol)]);
        testCoordinates((id<MGLMapSnapshotProtocol>)snapshotOverlay);

        [expectation fulfill];
    } completionHandler:^(MGLMapSnapshot * _Nullable snapshot, NSError * _Nullable error) {
        XCTAssert([snapshot conformsToProtocol:@protocol(MGLMapSnapshotProtocol)]);
        testCoordinates((id<MGLMapSnapshotProtocol>)snapshot);

        [expectation fulfill];
    }];

    [self waitForExpectations:@[expectation] timeout:10.0];
}


@end
