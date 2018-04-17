#import "MGLMapViewIntegrationTest.h"

@interface MBCameraTransitionTests : MGLMapViewIntegrationTest
@end

@implementation MBCameraTransitionTests

- (void)testSetAndResetNorthWithDispatchAsyncInDelegateMethod {

    XCTestExpectation *expectation = [self expectationWithDescription:@"regionDidChange expectation"];

    __weak typeof(self) weakself = self;
    __block NSInteger delegateCallCount = 0;

    self.regionDidChange = ^(MGLMapView *mapView, MGLCameraChangeReason reason, BOOL animated) {

        MBCameraTransitionTests *strongSelf = weakself;

        if (!strongSelf) return;

        delegateCallCount++;

        MGLTestAssert(strongSelf, mapView.userTrackingMode != MGLUserTrackingModeFollowWithHeading);
        if (mapView.direction != 0.0) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [mapView resetNorth];
            });
        }

        [NSObject cancelPreviousPerformRequestsWithTarget:expectation selector:@selector(fulfill) object:nil];
        [expectation performSelector:@selector(fulfill) withObject:nil afterDelay:5.0];
    };

    [self.mapView setDirection:90 animated:YES];

    // loop, render, and wait
    [self waitForExpectations:@[expectation] timeout:10.0];

    XCTAssert(delegateCallCount == 2, @"Expecting 2 regionDidChange callbacks, got %ld", delegateCallCount); // Once for the setDirection and once for the reset north
}


- (void)testSetAndResetNorthInDelegateMethod {

    XCTestExpectation *expectation = [self expectationWithDescription:@"regionDidChange expectation"];

    __weak typeof(self) weakself = self;
    __block NSInteger delegateCallCount = 0;

    self.regionDidChange = ^(MGLMapView *mapView, MGLCameraChangeReason reason, BOOL animated) {

        MBCameraTransitionTests *strongSelf = weakself;

        if (!strongSelf) return;

        delegateCallCount++;

        MGLTestAssert(strongSelf, mapView.userTrackingMode != MGLUserTrackingModeFollowWithHeading);
        if (mapView.direction != 0.0) {
            NSLog(@"Reset to north");
            [mapView resetNorth];
        }

        [NSObject cancelPreviousPerformRequestsWithTarget:expectation selector:@selector(fulfill) object:nil];
        [expectation performSelector:@selector(fulfill) withObject:nil afterDelay:5.0];
    };

    [self.mapView setDirection:90 animated:YES];
    [self waitForExpectations:@[expectation] timeout:10.0];

    XCTAssert(delegateCallCount == 2, @"Expecting 2 regionDidChange callbacks, got %ld", delegateCallCount); // Once for the setDirection and once for the reset north
}


- (void)testSetCenterCancelsTransitions {
    XCTestExpectation *cameraIsInDCExpectation = [self expectationWithDescription:@"camera reset to DC"];

    CLLocationCoordinate2D dc = CLLocationCoordinate2DMake(38.894368, -77.036487);
    CLLocationCoordinate2D dc_west = CLLocationCoordinate2DMake(38.894368, -77.076487);

    double zoomLevel = 15.0;

    [self.mapView setCenterCoordinate:dc zoomLevel:zoomLevel animated:NO];
    [self.mapView setCenterCoordinate:dc_west zoomLevel:zoomLevel animated:YES];

    __weak typeof(self) weakself = self;

    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 0.15 * NSEC_PER_SEC),
                   dispatch_get_main_queue(),
                   ^{
                       MBCameraTransitionTests *strongSelf = weakself;

                       [strongSelf.mapView setCenterCoordinate:dc zoomLevel:zoomLevel animated:NO];
                       MGLTestAssertEqualWithAccuracy(strongSelf,
                                                      dc.latitude,
                                                      strongSelf.mapView.centerCoordinate.latitude,
                                                      0.0005,
                                                      @"setting center coordinate should cancel transitions");
                       MGLTestAssertEqualWithAccuracy(strongSelf,
                                                      dc.longitude,
                                                      strongSelf.mapView.centerCoordinate.longitude,
                                                      0.0005,
                                                      @"setting center coordinate should cancel transitions");
                       [cameraIsInDCExpectation fulfill];
                   });

    [self waitForExpectations:@[cameraIsInDCExpectation] timeout:10.0];
}

- (void)testSetCenterCoordinateInDelegateMethod {

    XCTestExpectation *expectation = [self expectationWithDescription:@"regionDidChange expectation"];

    __weak typeof(self) weakself = self;
    __block NSInteger delegateCallCount = 0;

    CLLocationCoordinate2D target = CLLocationCoordinate2DMake(40.0, 40.0);
    CLLocationCoordinate2D target2 = CLLocationCoordinate2DMake(-40.0, -40.0);

    self.regionDidChange = ^(MGLMapView *mapView, MGLCameraChangeReason reason, BOOL animated) {

        MBCameraTransitionTests *strongSelf = weakself;

        if (!strongSelf) return;

        MGLTestAssert(strongSelf, mapView.userTrackingMode != MGLUserTrackingModeFollowWithHeading);

        CLLocationCoordinate2D center = mapView.centerCoordinate;

        switch(delegateCallCount) {
            case 0:
            {
                // Our center coordinate should match our target (assuming we're not
                // constrained by zoom level)
                MGLTestAssertEqualWithAccuracy(strongSelf,
                                               target.longitude,
                                               center.longitude,
                                               0.0005,
                                               @"center coordinate longitude should be at target");

                MGLTestAssertEqualWithAccuracy(strongSelf,
                                               target.latitude,
                                               center.latitude,
                                               0.0005,
                                               @"center coordinate latitude should be at target");

                // Now set another coordinate.
                [mapView setCenterCoordinate:target2 animated:YES];
                break;
            }

            case 1:
            {
                // Our center coordinate should match our target (assuming we're not
                // constrained by zoom level)
                MGLTestAssertEqualWithAccuracy(strongSelf,
                                               target2.longitude,
                                               center.longitude,
                                               0.0005,
                                               @"center coordinate longitude should be at target2");

                MGLTestAssertEqualWithAccuracy(strongSelf,
                                               target2.latitude,
                                               center.latitude,
                                               0.0005,
                                               @"center coordinate latitude should be at target2");
                break;

            }

            default:
                MGLTestFail(strongSelf);
                break;
        }

        delegateCallCount++;

        [NSObject cancelPreviousPerformRequestsWithTarget:expectation selector:@selector(fulfill) object:nil];
        [expectation performSelector:@selector(fulfill) withObject:nil afterDelay:5.0];
    };

    [self.mapView setCenterCoordinate:target zoomLevel:15.0 animated:YES];
    [self waitForExpectations:@[expectation] timeout:10.0];

    XCTAssert(delegateCallCount == 2, @"Expecting 2 regionDidChange callbacks, got %ld", delegateCallCount); // Once for the setDirection and once for the reset north
}

- (void)testFlyToCameraInDelegateMethod {

    XCTestExpectation *expectation = [self expectationWithDescription:@"regionDidChange expectation"];

    __weak typeof(self) weakself = self;
    __block NSInteger delegateCallCount = 0;

    CLLocationCoordinate2D target = CLLocationCoordinate2DMake(40.0, 40.0);
    CLLocationCoordinate2D target2 = CLLocationCoordinate2DMake(-40.0, -40.0);

    __block BOOL runloop = YES;

    self.regionDidChange = ^(MGLMapView *mapView, MGLCameraChangeReason reason, BOOL animated) {

        MBCameraTransitionTests *strongSelf = weakself;

        if (!strongSelf) return;

        MGLTestAssert(strongSelf, mapView.userTrackingMode != MGLUserTrackingModeFollowWithHeading);

        CLLocationCoordinate2D center = mapView.centerCoordinate;

        switch(delegateCallCount) {
            case 0:
            {
                // Our center coordinate should match our target (assuming we're not
                // constrained by zoom level)
                MGLTestAssertEqualWithAccuracy(strongSelf,
                                               target.longitude,
                                               center.longitude,
                                               0.0005,
                                               @"center coordinate longitude should be at target");

                MGLTestAssertEqualWithAccuracy(strongSelf,
                                               target.latitude,
                                               center.latitude,
                                               0.0005,
                                               @"center coordinate latitude should be at target");

                // Now set another coordinate.
                MGLMapCamera *camera = [MGLMapCamera cameraLookingAtCenterCoordinate:target2
                                                                        fromDistance:10
                                                                               pitch:0.0
                                                                             heading:0.0];

                [mapView flyToCamera:camera completionHandler:^{
                    MGLTestAssert(strongSelf, !runloop, @"Completion block should be called after delegate method");
                    [expectation fulfill];
                }];
                break;
            }

            case 1:
            {
                // Our center coordinate should match our target (assuming we're not
                // constrained by zoom level)
                MGLTestAssertEqualWithAccuracy(strongSelf,
                                               target2.longitude,
                                               center.longitude,
                                               0.0005,
                                               @"center coordinate longitude should be at target2");

                MGLTestAssertEqualWithAccuracy(strongSelf,
                                               target2.latitude,
                                               center.latitude,
                                               0.0005,
                                               @"center coordinate latitude should be at target2");

                runloop = NO;
                break;
            }

            default:
                MGLTestFail(strongSelf);
                break;
        }

        delegateCallCount++;
    };

    [self.mapView setCenterCoordinate:target zoomLevel:15.0 animated:YES];

    // Run the loop, so the camera can fly to the new camera
    while (runloop) {
        [[NSRunLoop mainRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
    }

    [self waitForExpectations:@[expectation] timeout:1.0];

    XCTAssert(delegateCallCount == 2, @"Expecting 2 regionDidChange callbacks, got %ld", delegateCallCount); // Once for the setDirection and once for the reset north
}

@end
