#import <KIF/KIF.h>
#import <KIF/KIFTestStepValidation.h>

#import "KIFTestActor+MapboxGL.h"

#import <Mapbox/Mapbox.h>
#import "MGLTViewController.h"

#import <CoreLocation/CoreLocation.h>

@interface MapViewTests : KIFTestCase <MGLMapViewDelegate>

@end

@implementation MapViewTests

- (NSNotification *)waitForNotificationThatRegionDidChangeAnimatedWhileExecutingBlock:(void (^)())block {
    [[NSNotificationCenter defaultCenter] addObserverForName:@"regionDidChangeAnimated"
                                                      object:tester.mapView
                                                       queue:nil
                                                  usingBlock:^(NSNotification * _Nonnull note) {}];
    NSNotification *notification = [system waitForNotificationName:@"regionDidChangeAnimated"
                                                            object:tester.mapView whileExecutingBlock:block];
    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:@"regionDidChangeAnimated"
                                                  object:tester.mapView];
    return notification;
}

- (void)beforeEach {
    [system simulateDeviceRotationToOrientation:UIDeviceOrientationPortrait];
    [tester.viewController resetMapView];

    tester.mapView.centerCoordinate = CLLocationCoordinate2DMake(38.913175, -77.032458);
    tester.mapView.zoomLevel = 14;
    tester.mapView.direction = 0;

    tester.mapView.zoomEnabled = YES;
    tester.mapView.scrollEnabled = YES;
    tester.mapView.rotateEnabled = YES;

    [tester.mapView removeAnnotations:tester.mapView.annotations];

    tester.viewController.navigationController.navigationBarHidden = YES;
    tester.viewController.navigationController.toolbarHidden = YES;

    tester.mapView.delegate = self;
}

- (void)approveLocationIfNeeded {
    if ([CLLocationManager authorizationStatus] == kCLAuthorizationStatusNotDetermined) {
        [tester acknowledgeSystemAlert];
        [tester waitForTimeInterval:1];
    }
    XCTAssertTrue([CLLocationManager locationServicesEnabled]);
    XCTAssertEqual([CLLocationManager authorizationStatus], kCLAuthorizationStatusAuthorizedAlways);
}

- (void)testDirectionSet {
    [self waitForNotificationThatRegionDidChangeAnimatedWhileExecutingBlock:^{
        [tester.mapView setDirection:270 animated:YES];
    }];

    XCTAssertEqual(tester.mapView.direction,
                   270,
                   @"setting direction should take effect");

    [tester waitForAnimationsToFinish];
    XCTAssertEqual(tester.compass.alpha,
                   1,
                   @"compass should be visible when map is rotated");

    XCTAssertEqualObjects([NSValue valueWithCGAffineTransform:tester.compass.transform],
                          [NSValue valueWithCGAffineTransform:CGAffineTransformMakeRotation(M_PI * 0.5)],
                          @"compass rotation should indicate map rotation");
}

- (void)testDirectionReset {
    [self waitForNotificationThatRegionDidChangeAnimatedWhileExecutingBlock:^{
        [tester.mapView setDirection:90 animated:YES];
    }];

    XCTAssertEqual(tester.mapView.direction,
                   90,
                   @"setting direction should take effect");

    [self waitForNotificationThatRegionDidChangeAnimatedWhileExecutingBlock:^{
        [tester.mapView resetNorth];
    }];

    XCTAssertEqual(tester.mapView.direction,
                   0,
                   @"resetting north should reset map direction");

    [tester waitForAnimationsToFinish];
    XCTAssertEqual(tester.compass.alpha,
                   0,
                   @"compass should not be visible when map is unrotated");

    XCTAssert(CGAffineTransformEqualToTransform(tester.compass.transform, CGAffineTransformIdentity),
              @"compass rotation should indicate map rotation");
}

- (void)testZoom {
    double zoom = tester.mapView.zoomLevel;

    [tester.mapView zoomAtPoint:CGPointMake(tester.mapView.bounds.size.width  / 2,
                                            tester.mapView.bounds.size.height / 2)
                       distance:50
                          steps:10];

    XCTAssertGreaterThan(tester.mapView.zoomLevel,
                         zoom,
                         @"zoom gesture should increase zoom level");

    zoom = tester.mapView.zoomLevel;

    [tester.mapView pinchAtPoint:CGPointMake(tester.mapView.bounds.size.width  / 2,
                                             tester.mapView.bounds.size.height / 2)
                        distance:50
                           steps:10];

    XCTAssertLessThan(tester.mapView.zoomLevel,
                      zoom,
                      @"pinch gesture should decrease zoom level");
}

- (void)testZoomDisabled {
    tester.mapView.zoomEnabled = NO;
    double zoom = tester.mapView.zoomLevel;

    [tester.mapView zoomAtPoint:CGPointMake(tester.mapView.bounds.size.width  / 2,
                                            tester.mapView.bounds.size.height / 2)
                       distance:50
                          steps:10];

    XCTAssertEqual(tester.mapView.zoomLevel,
                   zoom,
                   @"disabling zoom gesture should disallow zooming");

    [tester.mapView pinchAtPoint:CGPointMake(tester.mapView.bounds.size.width  / 2,
                                             tester.mapView.bounds.size.height / 2)
                        distance:50
                           steps:10];

    XCTAssertEqual(tester.mapView.zoomLevel,
                   zoom,
                   @"disabling zoom gesture should disallow pinching");
}

- (void)testFitToBounds {
    // No-op
    MGLCoordinateBounds initialBounds = tester.mapView.visibleCoordinateBounds;
    [tester.mapView setVisibleCoordinateBounds:initialBounds animated:NO];
    XCTAssertEqualObjects(MGLStringFromCoordinateBounds(initialBounds),
                          MGLStringFromCoordinateBounds(tester.mapView.visibleCoordinateBounds),
                          @"setting visible coordinate bounds to currently visible coordinate bounds should be a no-op");

    // Roundtrip after zooming
    tester.mapView.zoomLevel -= 3;
    [tester.mapView setVisibleCoordinateBounds:initialBounds animated:NO];
    XCTAssertEqualObjects(MGLStringFromCoordinateBounds(initialBounds),
                          MGLStringFromCoordinateBounds(tester.mapView.visibleCoordinateBounds),
                          @"after zooming out, setting visible coordinate bounds back to %@ should not leave them at %@",
                          MGLStringFromCoordinateBounds(initialBounds),
                          MGLStringFromCoordinateBounds(tester.mapView.visibleCoordinateBounds));
    tester.mapView.zoomLevel += 3;
    [tester.mapView setVisibleCoordinateBounds:initialBounds animated:NO];
    XCTAssertEqualObjects(MGLStringFromCoordinateBounds(initialBounds),
                          MGLStringFromCoordinateBounds(tester.mapView.visibleCoordinateBounds),
                          @"after zooming in, setting visible coordinate bounds back to %@ should not leave them at %@",
                          MGLStringFromCoordinateBounds(initialBounds),
                          MGLStringFromCoordinateBounds(tester.mapView.visibleCoordinateBounds));

    // Roundtrip after panning
    MGLCoordinateBounds offsetBounds = MGLCoordinateBoundsOffset(initialBounds, MGLCoordinateSpanMake(0, 30));
    [tester.mapView setVisibleCoordinateBounds:offsetBounds animated:NO];
    [tester.mapView setVisibleCoordinateBounds:initialBounds animated:NO];
    XCTAssertEqualObjects(MGLStringFromCoordinateBounds(initialBounds),
                          MGLStringFromCoordinateBounds(tester.mapView.visibleCoordinateBounds),
                          @"after panning 30° to the east, setting visible coordinate bounds back to %@ should not leave them at %@",
                          MGLStringFromCoordinateBounds(initialBounds),
                          MGLStringFromCoordinateBounds(tester.mapView.visibleCoordinateBounds));

    // Inscribed shapes with rotation
    tester.mapView.direction = 45;
    // https://en.wikipedia.org/wiki/Boundary_Markers_of_the_Original_District_of_Columbia
    CLLocationCoordinate2D dcCoordinates[] = {
        {38.790339, -77.040583},
        {38.893219, -77.172304},
        {38.995946, -77.040947},
        {38.892829, -76.909229},
    };
    MGLCoordinateBounds dcBounds = {{38.790339, -77.172304}, {38.995946, -76.909229}};
    [tester.mapView setVisibleCoordinateBounds:dcBounds
                                      animated:NO];
    double zoomLevel = tester.mapView.zoomLevel;
    [tester.mapView setVisibleCoordinates:dcCoordinates
                                    count:sizeof(dcCoordinates) / sizeof(dcCoordinates[0])
                              edgePadding:UIEdgeInsetsZero
                                 animated:NO];
    XCTAssertGreaterThan(tester.mapView.zoomLevel, zoomLevel,
                         @"when the map is rotated, DC should fit at a zoom level higher than %f, but instead the zoom level is %f",
                         zoomLevel, tester.mapView.zoomLevel);
}

- (void)testPan {
    CLLocationCoordinate2D centerCoordinate = tester.mapView.centerCoordinate;

    [tester.mapView dragFromPoint:CGPointMake(10, 10) toPoint:CGPointMake(300, 300) steps:10];

    XCTAssertGreaterThan(tester.mapView.centerCoordinate.latitude,
                         centerCoordinate.latitude,
                         @"panning map down should increase center latitude");

    XCTAssertLessThan(tester.mapView.centerCoordinate.longitude,
                      centerCoordinate.longitude,
                      @"panning map right should decrease center longitude");
}

- (void)testSetCenterCancelsTransitions {
    XCTestExpectation *cameraIsInDCExpectation = [self expectationWithDescription:@"camera reset to DC"];

    CLLocationCoordinate2D dc = CLLocationCoordinate2DMake(38.894368, -77.036487);
    CLLocationCoordinate2D dc_west = CLLocationCoordinate2DMake(38.894368, -77.076487);
    [tester.mapView setCenterCoordinate:dc animated:NO];
    [tester.mapView setCenterCoordinate:dc_west animated:YES];
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 0.15 * NSEC_PER_SEC),
                   dispatch_get_main_queue(),
                   ^{
                       [tester.mapView setCenterCoordinate:dc animated:NO];
                       XCTAssertEqualWithAccuracy(dc.latitude,
                                                  tester.mapView.centerCoordinate.latitude,
                                                  0.0005,
                                                  @"setting center coordinate should cancel transitions");
                       XCTAssertEqualWithAccuracy(dc.longitude,
                                                  tester.mapView.centerCoordinate.longitude,
                                                  0.0005,
                                                  @"setting center coordinate should cancel transitions");
                       [cameraIsInDCExpectation fulfill];
                   });

    [self waitForExpectationsWithTimeout:1.0 handler:nil];
}

- (void)testPanDisabled {
    tester.mapView.scrollEnabled = NO;
    CLLocationCoordinate2D centerCoordinate = tester.mapView.centerCoordinate;

    [tester.mapView dragFromPoint:CGPointMake(10, 10) toPoint:CGPointMake(300, 300) steps:10];

    XCTAssertEqualWithAccuracy(centerCoordinate.latitude,
                               tester.mapView.centerCoordinate.latitude,
                               0.005,
                               @"disabling pan gesture should disallow vertical panning");

    XCTAssertEqualWithAccuracy(centerCoordinate.longitude,
                               tester.mapView.centerCoordinate.longitude,
                               0.005,
                               @"disabling pan gesture should disallow horizontal panning");
}

- (void)testRotateClockwise {
    CLLocationDirection startAngle = tester.mapView.direction;

    XCTAssertNotEqual(startAngle,
                      45,
                      @"start angle must not be destination angle");

    [tester.mapView twoFingerRotateAtPoint:tester.mapView.center angle:45];

    XCTAssertGreaterThanOrEqual(fabs(tester.mapView.direction - startAngle),
                                20,
                                @"rotating map should change angle");
}

- (void)testRotateCounterclockwise {
    CLLocationDirection startAngle = tester.mapView.direction;

    XCTAssertNotEqual(startAngle,
                      -45,
                      @"start angle must not be destination angle");

    [tester.mapView twoFingerRotateAtPoint:tester.mapView.center angle:-45];

    XCTAssertGreaterThanOrEqual(fabs(startAngle - tester.mapView.direction),
                                20,
                                @"rotating map should change angle");
    XCTAssertGreaterThan(tester.mapView.camera.heading, 0, @"camera should not go negative");
}

- (void)testRotateDisabled {
    tester.mapView.rotateEnabled = NO;
    CLLocationDirection startAngle = tester.mapView.direction;

    XCTAssertNotEqual(startAngle,
                      45,
                      @"start angle must not be destination angle");

    [tester.mapView twoFingerRotateAtPoint:tester.mapView.center angle:45];

    XCTAssertEqualWithAccuracy(tester.mapView.direction,
                               startAngle,
                               0.005,
                               @"disabling rotation show disallow rotation gestures");
}

- (void)testZoomSet {
    double newZoom = 11.65;

    XCTAssertNotEqual(tester.mapView.zoomLevel,
                      newZoom,
                      @"initial setup should have differing zoom");

    tester.mapView.zoomLevel = newZoom;

    XCTAssertEqualWithAccuracy(tester.mapView.zoomLevel,
                               newZoom,
                               0.01,
                               @"setting zoom should take effect");
}

- (void)testCameraDebouncing {
    MGLMapCamera *camera = [MGLMapCamera cameraLookingAtCenterCoordinate:CLLocationCoordinate2DMake(45, -122)
                                                            fromDistance:100
                                                                   pitch:30
                                                                 heading:45];
    tester.mapView.camera = camera;
    XCTAssertEqualObjects(tester.mapView.camera, camera);
    
    [tester.mapView setCamera:camera withDuration:10 animationTimingFunction:nil completionHandler:^{
        XCTAssert(NO, @"Camera animation should not be canceled by redundantly setting the camera to the current camera.");
    }];
    XCTAssertEqualObjects(tester.mapView.camera, camera);
    
    tester.mapView.camera = camera;
    XCTAssertEqualObjects(tester.mapView.camera, camera);
}

- (void)testMarkerSelection {
    CGPoint point = CGPointMake(100, 100);
    MGLPointAnnotation *marker = [MGLPointAnnotation new];
    marker.coordinate = [tester.mapView convertPoint:point toCoordinateFromView:tester.mapView];
    marker.title = @"test"; // title required for callout
    [tester.mapView addAnnotation:marker];

    XCTAssertEqual(tester.mapView.selectedAnnotations.count, 0);

    [tester.mapView selectAnnotation:marker animated:NO];
    XCTAssertEqualObjects(tester.mapView.selectedAnnotations.firstObject, marker);

    [tester.mapView deselectAnnotation:marker animated:NO];
    XCTAssertEqual(tester.mapView.selectedAnnotations.count, 0);
}

- (void)testMarkerAddWithoutDelegate {
    XCTAssertFalse([tester.viewController respondsToSelector:@selector(mapView:imageForAnnotation:)]);

    MGLPointAnnotation *marker = [MGLPointAnnotation new];
    marker.coordinate = tester.mapView.centerCoordinate;
    [tester.mapView addAnnotation:marker];

    [tester.mapView selectAnnotation:marker animated:NO];
    XCTAssertEqualObjects(tester.mapView.selectedAnnotations.firstObject, marker);
    XCTAssertEqual([[tester.mapView subviewsWithClassNamePrefix:@"SM"] count], 0); // no callout for no title

    [tester.mapView deselectAnnotation:marker animated:NO];
    marker.title = @"test";
    [tester.mapView selectAnnotation:marker animated:NO];
    XCTAssertEqualObjects(tester.mapView.selectedAnnotations.firstObject, marker);
    XCTAssertGreaterThan([[tester.mapView subviewsWithClassNamePrefix:@"SM"] count], 0);
}

- (BOOL)mapView:(MGLMapView *)mapView annotationCanShowCallout:(id<MGLAnnotation>)annotation {
    return YES;
}

- (void)testTopLayoutGuide {
    CGRect statusBarFrame, navigationBarFrame, compassFrame;
    UINavigationBar *navigationBar = tester.viewController.navigationController.navigationBar;

    compassFrame = [tester.compass.superview convertRect:tester.compass.frame toView:nil];
    statusBarFrame = [tester.window convertRect:[[UIApplication sharedApplication] statusBarFrame] toView:nil];
    XCTAssertFalse(CGRectIntersectsRect(compassFrame, statusBarFrame),
                   @"compass should not be under status bar");

    tester.viewController.navigationController.navigationBarHidden = NO;
    compassFrame = [tester.compass.superview convertRect:tester.compass.frame toView:nil];
    navigationBarFrame = [tester.window convertRect:navigationBar.frame toView:nil];
    XCTAssertFalse(CGRectIntersectsRect(compassFrame, navigationBarFrame),
                   @"compass should not be under navigation bar");

    [system simulateDeviceRotationToOrientation:UIDeviceOrientationLandscapeLeft];

    compassFrame = [tester.compass.superview convertRect:tester.compass.frame toView:nil];
    navigationBarFrame = [tester.window convertRect:navigationBar.frame toView:nil];
    XCTAssertFalse(CGRectIntersectsRect(compassFrame, navigationBarFrame),
                   @"rotated device should not have compass under navigation bar");

    tester.viewController.navigationController.navigationBarHidden = YES;
    compassFrame = [tester.compass.superview convertRect:tester.compass.frame toView:nil];
    statusBarFrame = [tester.window convertRect:[[UIApplication sharedApplication] statusBarFrame] toView:nil];
    XCTAssertFalse(CGRectIntersectsRect(compassFrame, statusBarFrame),
                   @"rotated device should not have compass under status bar");
}

- (void)testBottomLayoutGuide {
    CGRect logoBugFrame, toolbarFrame, attributionButtonFrame;
    UIView *logoBug = (UIView *)[tester waitForViewWithAccessibilityLabel:@"Mapbox"];
    UIToolbar *toolbar = tester.viewController.navigationController.toolbar;
    UIView *attributionButton = (UIView *)[tester waitForViewWithAccessibilityLabel:@"About this map"];

    tester.viewController.navigationController.toolbarHidden = NO;

    logoBugFrame = [logoBug.superview convertRect:logoBug.frame toView:nil];
    toolbarFrame = [tester.window convertRect:toolbar.frame toView:nil];
    XCTAssertFalse(CGRectIntersectsRect(logoBugFrame, toolbarFrame),
                   @"logo bug should not be under toolbar");

    attributionButtonFrame = [attributionButton.superview convertRect:attributionButton.frame toView:nil];
    XCTAssertFalse(CGRectIntersectsRect(attributionButtonFrame, toolbarFrame),
                   @"attribution button should not be under toolbar");

    [system simulateDeviceRotationToOrientation:UIDeviceOrientationLandscapeRight];

    logoBugFrame = [logoBug.superview convertRect:logoBug.frame toView:nil];
    toolbarFrame = [tester.window convertRect:toolbar.frame toView:nil];
    XCTAssertFalse(CGRectIntersectsRect(logoBugFrame, toolbarFrame),
                   @"rotated device should not have logo bug under toolbar");

    attributionButtonFrame = [attributionButton.superview convertRect:attributionButton.frame toView:nil];
    XCTAssertFalse(CGRectIntersectsRect(attributionButtonFrame, toolbarFrame),
                   @"rotated device should not have attribution button under toolbar");
}

- (void)testInsetMapView {
    [tester.viewController insetMapView];
    [tester waitForAnimationsToFinish];

    UIView *logoBug = (UIView *)[tester waitForViewWithAccessibilityLabel:@"Mapbox"];
    UIView *attributionButton = (UIView *)[tester waitForViewWithAccessibilityLabel:@"About this map"];

    CGRect mapViewFrame = [tester.mapView.superview convertRect:tester.mapView.frame toView:nil];

    CGRect logoBugFrame = [logoBug.superview convertRect:logoBug.frame toView:nil];
    XCTAssertTrue(CGRectIntersectsRect(logoBugFrame, mapViewFrame),
                  @"logo bug should lie inside shrunken map view");

    CGRect attributionButtonFrame = [attributionButton.superview convertRect:attributionButton.frame toView:nil];
    XCTAssertTrue(CGRectIntersectsRect(attributionButtonFrame, mapViewFrame),
                  @"attribution button should lie inside shrunken map view");

    CGRect compassFrame = [tester.compass.superview convertRect:tester.compass.frame toView:nil];
    XCTAssertTrue(CGRectIntersectsRect(compassFrame, mapViewFrame),
                  @"compass should lie inside shrunken map view");
}

- (void)testContentInsetsWithTinyMapView {
    [tester.viewController tinyMapView];
    [self keyValueObservingExpectationForObject:tester.mapView keyPath:@"contentInset" handler:^BOOL(id observedObject, NSDictionary *change) {
        XCTAssertEqual(tester.mapView.contentInset.top,
                       0,
                       @"map should not have top content inset");
        XCTAssertEqual(tester.mapView.contentInset.bottom,
                       0,
                       @"map should not have bottom content inset");
        return YES;
    }];
    [self waitForExpectationsWithTimeout:2.0 handler:nil];

    tester.mapView.frame = CGRectMake(0, 0, tester.mapView.frame.size.width, tester.mapView.frame.size.height);
    [self keyValueObservingExpectationForObject:tester.mapView keyPath:@"contentInset" handler:^BOOL(id observedObject, NSDictionary *change) {
        XCTAssertEqual(tester.mapView.contentInset.top,
                       tester.viewController.topLayoutGuide.length,
                       @"map should have top content inset equal to the top layout guide");
        return YES;
    }];
    [self waitForExpectationsWithTimeout:2.0 handler:nil];
}

- (void)testDelegateRegionWillChange {
    __block NSUInteger unanimatedCount;
    __block NSUInteger animatedCount;
    [[NSNotificationCenter defaultCenter] addObserverForName:@"regionWillChangeAnimated"
                                                      object:tester.mapView
                                                       queue:nil
                                                  usingBlock:^(NSNotification *note) {
                                                      if ([note.userInfo[@"animated"] boolValue]) {
                                                          animatedCount++;
                                                      } else {
                                                          unanimatedCount++;
                                                      }
                                                  }];

    [tester waitForTimeInterval:1];

    unanimatedCount = 0;
    animatedCount = 0;

    NSNotification *notification = [system waitForNotificationName:@"regionWillChangeAnimated"
                                                            object:tester.mapView
                                               whileExecutingBlock:^{
                                                   tester.mapView.centerCoordinate = CLLocationCoordinate2DMake(0, 0);
                                               }];

    [tester waitForTimeInterval:1];

    XCTAssertEqual([notification.userInfo[@"animated"] boolValue],
                   NO,
                   @"regionWillChange delegate should not indicate animated change");
    XCTAssertEqual(unanimatedCount,
                   1,
                   @"regionWillChange delegate should indicate one unanimated change");

    notification = [system waitForNotificationName:@"regionWillChangeAnimated"
                                            object:tester.mapView
                               whileExecutingBlock:^{
                                   [tester.mapView setCenterCoordinate:CLLocationCoordinate2DMake(45, 100) animated:YES];
                               }];

    [tester waitForTimeInterval:1];

    XCTAssertEqual([notification.userInfo[@"animated"] boolValue],
                   YES,
                   @"regionWillChange delegate should indicate an animated change");
    XCTAssertEqual(animatedCount,
                   1,
                   @"regionWillChange delegate should indicate one animated change");

    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:@"regionWillChangeAnimated"
                                                  object:tester.mapView];
}

- (void)mapView:(MGLMapView *)mapView regionWillChangeAnimated:(BOOL)animated {
    [[NSNotificationCenter defaultCenter] postNotificationName:@"regionWillChangeAnimated"
                                                        object:mapView
                                                      userInfo:@{ @"animated" : @(animated) }];
}

- (void)mapView:(MGLMapView *)mapView regionDidChangeWithReason:(MGLCameraChangeReason)reason animated:(BOOL)animated {

    [[NSNotificationCenter defaultCenter] postNotificationName:@"regionDidChangeAnimated"
                                                        object:mapView
                                                      userInfo:@{ @"animated" : @(animated),
                                                                  @"reason" : @(reason)
                                                                  }];
}

- (void)testDelegatesStartStopLocatingUser {
    NSNotification *notification = [system waitForNotificationName:@"mapViewWillStartLocatingUser"
                                                            object:tester.mapView
                                               whileExecutingBlock:^{
                                                   tester.mapView.showsUserLocation = YES;
                                                   [self approveLocationIfNeeded];
                                               }];

    XCTAssertEqualObjects(notification.name,
                          @"mapViewWillStartLocatingUser",
                          @"mapViewWillStartLocatingUser delegate should receive message");
    XCTAssertNotNil([tester.mapView valueForKeyPath:@"locationManager"],
                 "map view location manager should not be nil");

    notification = [system waitForNotificationName:@"mapViewDidStopLocatingUser"
                                            object:tester.mapView
                               whileExecutingBlock:^{
                                   tester.mapView.showsUserLocation = NO;
                               }];

    XCTAssertEqualObjects(notification.name,
                          @"mapViewDidStopLocatingUser",
                          @"mapViewDidStopLocatingUser delegate should receive message");
    XCTAssertEqual(tester.mapView.userTrackingMode,
                   MGLUserTrackingModeNone,
                   @"user tracking mode should be none");
    XCTAssertNil([tester.mapView valueForKeyPath:@"locationManager"],
                 "map view location manager should be nil");
}

- (void)mapViewWillStartLocatingUser:(MGLMapView *)mapView {
    [[NSNotificationCenter defaultCenter] postNotificationName:@"mapViewWillStartLocatingUser" object:mapView];
}

- (void)mapViewDidStopLocatingUser:(MGLMapView *)mapView {
    [[NSNotificationCenter defaultCenter] postNotificationName:@"mapViewDidStopLocatingUser" object:mapView];
}

@end
