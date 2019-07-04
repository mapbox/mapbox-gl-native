#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>
#import <TargetConditionals.h>

#if TARGET_OS_IPHONE
    #define MGLEdgeInsetsZero UIEdgeInsetsZero
#else
    #define MGLEdgeInsetsZero NSEdgeInsetsZero
#endif

static MGLMapView *mapView;

@interface MGLMapViewTests : XCTestCase
@end

@implementation MGLMapViewTests

- (void)setUp {
    [super setUp];
    
    [MGLAccountManager setAccessToken:@"pk.feedcafedeadbeefbadebede"];
    NSURL *styleURL = [[NSBundle bundleForClass:[self class]] URLForResource:@"one-liner" withExtension:@"json"];
    mapView = [[MGLMapView alloc] initWithFrame:CGRectMake(0, 0, 64, 64) styleURL:styleURL];
}

- (void)tearDown {
    mapView = nil;
    [MGLAccountManager setAccessToken:nil];
    [super tearDown];
}

- (void)testCoordinateBoundsConversion {
    [mapView setCenterCoordinate:CLLocationCoordinate2DMake(33, 179)];

    MGLCoordinateBounds leftAntimeridianBounds = MGLCoordinateBoundsMake(CLLocationCoordinate2DMake(-75, 175), CLLocationCoordinate2DMake(75, 180));
    CGRect leftAntimeridianBoundsRect = [mapView convertCoordinateBounds:leftAntimeridianBounds toRectToView:mapView];

    MGLCoordinateBounds rightAntimeridianBounds = MGLCoordinateBoundsMake(CLLocationCoordinate2DMake(-75, -180), CLLocationCoordinate2DMake(75, -175));
    CGRect rightAntimeridianBoundsRect = [mapView convertCoordinateBounds:rightAntimeridianBounds toRectToView:mapView];

    MGLCoordinateBounds spanningBounds = MGLCoordinateBoundsMake(CLLocationCoordinate2DMake(24, 140), CLLocationCoordinate2DMake(44, 240));
    CGRect spanningBoundsRect = [mapView convertCoordinateBounds:spanningBounds toRectToView:mapView];

    // If the resulting CGRect from -convertCoordinateBounds:toRectToView:
    // intersects the set of bounds to the left and right of the
    // antimeridian, then we know that the CGRect spans across the antimeridian
    XCTAssertTrue(CGRectIntersectsRect(spanningBoundsRect, leftAntimeridianBoundsRect), @"Resulting");
    XCTAssertTrue(CGRectIntersectsRect(spanningBoundsRect, rightAntimeridianBoundsRect), @"Something");
}

#if TARGET_OS_IPHONE
- (void)testUserTrackingModeCompletion {
    __block BOOL completed = NO;
    [mapView setUserTrackingMode:MGLUserTrackingModeNone animated:NO completionHandler:^{
        completed = YES;
    }];
    XCTAssertTrue(completed, @"Completion block should get called synchronously when the mode is unchanged.");

    completed = NO;
    [mapView setUserTrackingMode:MGLUserTrackingModeNone animated:YES completionHandler:^{
        completed = YES;
    }];
    XCTAssertTrue(completed, @"Completion block should get called synchronously when the mode is unchanged.");

    completed = NO;
    [mapView setUserTrackingMode:MGLUserTrackingModeFollow animated:NO completionHandler:^{
        completed = YES;
    }];
    XCTAssertTrue(completed, @"Completion block should get called synchronously when there’s no location.");

    completed = NO;
    [mapView setUserTrackingMode:MGLUserTrackingModeFollowWithHeading animated:YES completionHandler:^{
        completed = YES;
    }];
    XCTAssertTrue(completed, @"Completion block should get called synchronously when there’s no location.");
}

- (void)testTargetCoordinateCompletion {
    __block BOOL completed = NO;
    [mapView setTargetCoordinate:kCLLocationCoordinate2DInvalid animated:NO completionHandler:^{
        completed = YES;
    }];
    XCTAssertTrue(completed, @"Completion block should get called synchronously when the target coordinate is unchanged.");

    completed = NO;
    [mapView setTargetCoordinate:kCLLocationCoordinate2DInvalid animated:YES completionHandler:^{
        completed = YES;
    }];
    XCTAssertTrue(completed, @"Completion block should get called synchronously when the target coordinate is unchanged.");

    completed = NO;
    [mapView setUserTrackingMode:MGLUserTrackingModeFollow animated:NO completionHandler:nil];
    [mapView setTargetCoordinate:CLLocationCoordinate2DMake(39.128106, -84.516293) animated:YES completionHandler:^{
        completed = YES;
    }];
    XCTAssertTrue(completed, @"Completion block should get called synchronously when not tracking user course.");

    completed = NO;
    [mapView setUserTrackingMode:MGLUserTrackingModeFollowWithCourse animated:NO completionHandler:nil];
    [mapView setTargetCoordinate:CLLocationCoordinate2DMake(39.224407, -84.394957) animated:YES completionHandler:^{
        completed = YES;
    }];
    XCTAssertTrue(completed, @"Completion block should get called synchronously when there’s no location.");
}
#endif

- (void)testVisibleCoordinatesCompletion {
    XCTestExpectation *expectation = [self expectationWithDescription:@"Completion block should get called when not animated"];
    MGLCoordinateBounds unitBounds = MGLCoordinateBoundsMake(CLLocationCoordinate2DMake(0, 0), CLLocationCoordinate2DMake(1, 1));
    [mapView setVisibleCoordinateBounds:unitBounds edgePadding:MGLEdgeInsetsZero animated:NO completionHandler:^{
        [expectation fulfill];
    }];
    [self waitForExpectations:@[expectation] timeout:1];

#if TARGET_OS_IPHONE
    expectation = [self expectationWithDescription:@"Completion block should get called when animated"];
    CLLocationCoordinate2D antiunitCoordinates[] = {
        CLLocationCoordinate2DMake(0, 0),
        CLLocationCoordinate2DMake(-1, -1),
    };
    [mapView setVisibleCoordinates:antiunitCoordinates
                             count:sizeof(antiunitCoordinates) / sizeof(antiunitCoordinates[0])
                       edgePadding:UIEdgeInsetsZero
                         direction:0
                          duration:0
           animationTimingFunction:nil
                 completionHandler:^{
        [expectation fulfill];
    }];
    [self waitForExpectations:@[expectation] timeout:1];
#endif
}

- (void)testShowAnnotationsCompletion {
    __block BOOL completed = NO;
    [mapView showAnnotations:@[] edgePadding:MGLEdgeInsetsZero animated:NO completionHandler:^{
        completed = YES;
    }];
    XCTAssertTrue(completed, @"Completion block should get called synchronously when there are no annotations to show.");

    XCTestExpectation *expectation = [self expectationWithDescription:@"Completion block should get called when not animated"];
    MGLPointAnnotation *annotation = [[MGLPointAnnotation alloc] init];
    [mapView showAnnotations:@[annotation] edgePadding:MGLEdgeInsetsZero animated:NO completionHandler:^{
        [expectation fulfill];
    }];
    [self waitForExpectations:@[expectation] timeout:1];

    expectation = [self expectationWithDescription:@"Completion block should get called when animated."];
    [mapView showAnnotations:@[annotation] edgePadding:MGLEdgeInsetsZero animated:YES completionHandler:^{
        [expectation fulfill];
    }];
    [self waitForExpectations:@[expectation] timeout:1];
}

@end
