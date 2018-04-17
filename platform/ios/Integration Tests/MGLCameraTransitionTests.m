#import "MGLMapViewIntegrationTest.h"

@interface MBCameraTransitionTests : MGLMapViewIntegrationTest
@end

@implementation MBCameraTransitionTests

- (void)testSetAndResetNorthWithDispatchAsync {

    XCTestExpectation *expectation = [self expectationWithDescription:@"regionDidChange expectation"];

    __weak typeof(self) weakself = self;
    __block NSInteger delegateCallCount = 0;

    self.regionDidChange = ^(MGLMapView *mapView, MGLCameraChangeReason reason, BOOL animated) {

        MBCameraTransitionTests *strongSelf = weakself;

        if (!strongSelf) return;

        delegateCallCount++;

        if (mapView.userTrackingMode != MGLUserTrackingModeFollowWithHeading && mapView.direction != 0.0) {
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


- (void)testSetAndResetNorth {

    XCTestExpectation *expectation = [self expectationWithDescription:@"regionDidChange expectation"];

    __weak typeof(self) weakself = self;
    __block NSInteger delegateCallCount = 0;

    self.regionDidChange = ^(MGLMapView *mapView, MGLCameraChangeReason reason, BOOL animated) {

        MBCameraTransitionTests *strongSelf = weakself;

        if (!strongSelf) return;

        delegateCallCount++;

        if (mapView.userTrackingMode != MGLUserTrackingModeFollowWithHeading && mapView.direction != 0.0) {
            NSLog(@"resetting north");
            [mapView resetNorth];
        }

        [NSObject cancelPreviousPerformRequestsWithTarget:expectation selector:@selector(fulfill) object:nil];
        [expectation performSelector:@selector(fulfill) withObject:nil afterDelay:5.0];
    };

    [self.mapView setDirection:90 animated:YES];
    [self waitForExpectations:@[expectation] timeout:10.0];

    XCTAssert(delegateCallCount == 2, @"delegateCallCount = %ld", delegateCallCount); // Once for the setDirection and once for the reset north
}

@end
