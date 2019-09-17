#import "MGLMapViewIntegrationTest.h"
#import "MGLTestUtility.h"
#import "../../darwin/src/MGLGeometry_Private.h"

@interface MGLCameraTransitionTests : MGLMapViewIntegrationTest
@end

@implementation MGLCameraTransitionTests

- (void)testSetAndResetNorthWithDispatchAsyncInDelegateMethod {

    XCTestExpectation *expectation = [self expectationWithDescription:@"regionDidChange expectation"];
    expectation.expectedFulfillmentCount = 2;
    expectation.assertForOverFulfill = YES;

    __weak typeof(self) weakself = self;

    self.regionDidChange = ^(MGLMapView *mapView, MGLCameraChangeReason reason, BOOL animated) {

        MGLCameraTransitionTests *strongSelf = weakself;

        if (!strongSelf) return;

        [expectation fulfill];

        MGLTestAssert(strongSelf, mapView.userTrackingMode != MGLUserTrackingModeFollowWithHeading);
        if (mapView.direction != 0.0) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [mapView resetNorth];
            });
        }
    };

    [self.mapView setDirection:90 animated:YES];

    // loop, render, and wait
    [self waitForExpectations:@[expectation] timeout:10];
}


- (void)testSetAndResetNorthInDelegateMethod {

    XCTestExpectation *expectation = [self expectationWithDescription:@"regionDidChange expectation"];
    expectation.expectedFulfillmentCount = 2;
    expectation.assertForOverFulfill = YES;

    __weak typeof(self) weakself = self;

    self.regionDidChange = ^(MGLMapView *mapView, MGLCameraChangeReason reason, BOOL animated) {

        MGLCameraTransitionTests *strongSelf = weakself;

        if (!strongSelf) return;

        [expectation fulfill];

        MGLTestAssert(strongSelf, mapView.userTrackingMode != MGLUserTrackingModeFollowWithHeading);
        if (mapView.direction != 0.0) {
            NSLog(@"Reset to north");
            [mapView resetNorth];
        }
    };

    [self.mapView setDirection:90 animated:YES];
    [self waitForExpectations:@[expectation] timeout:10];
}

- (void)testInterruptingAndResetNorthOnlyOnceInIsChanging {

    // Reset to non-zero, prior to testing
    [self.mapView setDirection:45 animated:NO];

    XCTestExpectation *expectation = [self expectationWithDescription:@"regionDidChange expectation"];
    expectation.expectedFulfillmentCount = 1;
    expectation.assertForOverFulfill = YES;

    __weak typeof(self) weakself = self;
    __block BOOL startedReset = NO;
    __block BOOL finishedReset = NO;

    self.regionIsChanging = ^(MGLMapView *mapView) {
        MGLCameraTransitionTests *strongSelf = weakself;
        if (!strongSelf) return;

        if (!startedReset) {
            NSLog(@"Reset to north, interrupting the previous transition");
            startedReset = YES;
            [mapView resetNorth];
            finishedReset = YES;
        }
    };

    self.regionDidChange = ^(MGLMapView *mapView, MGLCameraChangeReason reason, BOOL animated) {
        MGLCameraTransitionTests *strongSelf = weakself;
        if (!strongSelf) return;

        MGLTestAssert(strongSelf, startedReset);

        if (finishedReset) {
            MGLTestAssert(strongSelf, !(reason & MGLCameraChangeReasonTransitionCancelled));
            [expectation fulfill];
        }
        else {
            MGLTestAssert(strongSelf, reason & MGLCameraChangeReasonTransitionCancelled);
        }
    };

    [self.mapView setDirection:90 animated:YES];
    [self waitForExpectations:@[expectation] timeout:10];

    XCTAssertEqualWithAccuracy(self.mapView.direction, 0.0, 0.001, @"Camera should have reset to north. %0.3f", self.mapView.direction);
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
                       MGLCameraTransitionTests *strongSelf = weakself;

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
    expectation.expectedFulfillmentCount = 2;
    expectation.assertForOverFulfill = YES;

    __weak typeof(self) weakself = self;
    __block NSInteger delegateCallCount = 0;

    CLLocationCoordinate2D target = CLLocationCoordinate2DMake(40.0, 40.0);
    CLLocationCoordinate2D target2 = CLLocationCoordinate2DMake(-40.0, -40.0);

    self.regionDidChange = ^(MGLMapView *mapView, MGLCameraChangeReason reason, BOOL animated) {

        MGLCameraTransitionTests *strongSelf = weakself;

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
                // Should take MGLAnimationDuration seconds (0.3s)
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

        [expectation fulfill];
    };

    // Should take MGLAnimationDuration seconds (0.3)
    [self.mapView setCenterCoordinate:target zoomLevel:15.0 animated:YES];
    [self waitForExpectations:@[expectation] timeout:10];
}

- (void)testFlyToCameraInDelegateMethod {

    XCTestExpectation *expectation = [self expectationWithDescription:@"regionDidChange expectation"];

    __weak typeof(self) weakself = self;
    __block NSInteger delegateCallCount = 0;
    expectation.expectedFulfillmentCount = 3;
    expectation.assertForOverFulfill = YES;

    CLLocationCoordinate2D target = CLLocationCoordinate2DMake(40.0, 40.0);
    CLLocationCoordinate2D target2 = CLLocationCoordinate2DMake(30.0, 30.0);

    __block BOOL runloop = YES;

    NSTimeInterval stop0 = CACurrentMediaTime();
    __block NSTimeInterval stop1 = 0.0;
    __block NSTimeInterval stop2 = 0.0;

    double zoomLevel = 5.0;
    double altitude = MGLAltitudeForZoomLevel(zoomLevel, 0.0, target.latitude, self.mapView.frame.size);

    self.regionDidChange = ^(MGLMapView *mapView, MGLCameraChangeReason reason, BOOL animated) {

        MGLCameraTransitionTests *strongSelf = weakself;

        if (!strongSelf) return;

        MGLTestAssert(strongSelf, mapView.userTrackingMode != MGLUserTrackingModeFollowWithHeading);

        CLLocationCoordinate2D center = mapView.centerCoordinate;

        switch(delegateCallCount) {
            case 0:
            {
                stop1 = CACurrentMediaTime();

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
                                                                            altitude:altitude
                                                                               pitch:0.0
                                                                             heading:0.0];

                // flyToCamera can take a while...
                [mapView flyToCamera:camera completionHandler:^{
                    MGLTestAssert(strongSelf, !runloop, @"Completion block should be called after delegate method");
                    [expectation fulfill];
                    stop2 = CACurrentMediaTime();
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

        [expectation fulfill];
    };

    // Should take MGLAnimationDuration
    [self.mapView setCenterCoordinate:target zoomLevel:zoomLevel animated:YES];

    [self waitForExpectations:@[expectation] timeout:10];

    NSLog(@"setCenterCoordinate: %0.4fs", stop1 - stop0);
    NSLog(@"flyToCamera: %0.4fs", stop2 - stop1);

    XCTAssert(delegateCallCount == 2, @"Expecting 2 regionDidChange callbacks, got %ld", (long)delegateCallCount); // Once for the setDirection and once for the reset north
}

#pragma mark - Pending tests

- (void)testContinuallyResettingNorthInIsChanging🙁{
    // See https://github.com/mapbox/mapbox-gl-native/pull/11614
    // This test currently fails, unsurprisingly, since we're continually
    // setting the camera to the same parameters during its update.
    //
    // Possible solutions/expectations:
    // - If you set camera parameters that match the *current* target parameters
    //      then the transition could be a no-op. We'd need to consider any completion
    //      block
    //  - Ideally we would detect this case and disallow it.

    // Reset to non-zero, prior to testing
    [self.mapView setDirection:45 animated:NO];

    XCTestExpectation *expectation = [self expectationWithDescription:@"regionDidChange expectation"];
    expectation.expectedFulfillmentCount = 2;
    expectation.assertForOverFulfill = YES;

    self.regionIsChanging = ^(MGLMapView *mapView) {
        [mapView resetNorth];
    };

    self.regionDidChange = ^(MGLMapView *mapView, MGLCameraChangeReason reason, BOOL animated) {
        [expectation fulfill];
    };

    [self.mapView setDirection:90 animated:YES];
    [self waitForExpectations:@[expectation] timeout:10];

    XCTAssertEqualWithAccuracy(self.mapView.direction, 0.0, 0.001, @"Camera should have reset to north. %0.3f", self.mapView.direction);
}

- (void)testContinuallySettingCoordinateInIsChanging🙁 {
    // See above comment in `-testContinuallyResettingNorthInIsChanging🙁`

    // Reset to non-zero, prior to testing
    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(0.0, 0.0) animated:NO];

    XCTestExpectation *expectation = [self expectationWithDescription:@"regionDidChange expectation"];
    expectation.expectedFulfillmentCount = 2;
    expectation.assertForOverFulfill = YES;

    __weak typeof(self) weakself = self;

    self.regionIsChanging = ^(MGLMapView *mapView) {
        [weakself.mapView setCenterCoordinate:CLLocationCoordinate2DMake(-40.0, -40.0) animated:YES];
    };

    self.regionDidChange = ^(MGLMapView *mapView, MGLCameraChangeReason reason, BOOL animated) {
        [expectation fulfill];
    };

    [self.mapView setCenterCoordinate:CLLocationCoordinate2DMake(40.0, 40.0) animated:YES];
    [self waitForExpectations:@[expectation] timeout:10];

    XCTAssertEqualWithAccuracy(self.mapView.direction, 0.0, 0.001, @"Camera should have reset to north. %0.3f", self.mapView.direction);
}

@end
