#import <KIF/KIF.h>
#import <KIF/KIFTestStepValidation.h>

#import "KIFTestActor+MapboxGL.h"

#import "MapboxGL.h"
#import "MGLTViewController.h"

#import "LocationMocker/LocationMocker.h"
#import <CoreLocation/CoreLocation.h>

@interface MGLMapView (LocationManager)

@property (nonatomic) CLLocationManager *locationManager;

@end

@interface MapViewTests : KIFTestCase <MGLMapViewDelegate>

@end

@implementation MapViewTests

- (void)beforeEach {
    [system simulateDeviceRotationToOrientation:UIDeviceOrientationPortrait];
    [tester.viewController resetMapView];

    tester.mapView.centerCoordinate = CLLocationCoordinate2DMake(38.913175, -77.032458);
    tester.mapView.zoomLevel = 14;
    tester.mapView.direction = 0;

    tester.mapView.zoomEnabled = YES;
    tester.mapView.scrollEnabled = YES;
    tester.mapView.rotateEnabled = YES;

    tester.viewController.navigationController.navigationBarHidden = YES;
    tester.viewController.navigationController.toolbarHidden = YES;

    tester.mapView.delegate = self;
}

- (void)testDirectionSet {
    tester.mapView.direction = 270;

    XCTAssertEqual(tester.mapView.direction,
                   270,
                   @"setting direction should take effect");

    [tester waitForTimeInterval:1];

    XCTAssertEqual(tester.compass.alpha,
                   1,
                   @"compass should be visible when map is rotated");

    XCTAssertEqualObjects([NSValue valueWithCGAffineTransform:tester.compass.transform],
                          [NSValue valueWithCGAffineTransform:CGAffineTransformMakeRotation(M_PI * 1.5)],
                          @"compass rotation should indicate map rotation");
}

- (void)testCompassTap {
    tester.mapView.direction = 180;

    XCTAssertEqual(tester.mapView.direction,
                   180,
                   @"setting direction should take effect");

    [tester.compass tap];

    [tester waitForTimeInterval:1];

    XCTAssertEqual(tester.mapView.direction,
                   0,
                   @"tapping compass should reset map direction");

    XCTAssertEqual(tester.compass.alpha,
                   0,
                   @"compass should not be visible when map is unrotated");

    XCTAssertEqualObjects([NSValue valueWithCGAffineTransform:tester.compass.transform],
                          [NSValue valueWithCGAffineTransform:CGAffineTransformIdentity],
                          @"compass rotation should indicate map rotation");
}

- (void)testDirectionReset {
    tester.mapView.direction = 90;

    XCTAssertEqual(tester.mapView.direction,
                   90,
                   @"setting direction should take effect");

    [tester.mapView resetNorth];

    [tester waitForTimeInterval:1];

    XCTAssertEqual(tester.mapView.direction,
                   0,
                   @"resetting north should reset map direction");

    XCTAssertEqual(tester.compass.alpha,
                   0,
                   @"compass should not be visible when map is unrotated");

    XCTAssertEqualObjects([NSValue valueWithCGAffineTransform:tester.compass.transform],
                          [NSValue valueWithCGAffineTransform:CGAffineTransformIdentity],
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

- (void)testRotate {
    CLLocationDirection startAngle = tester.mapView.direction;

    XCTAssertNotEqual(startAngle,
                      45,
                      @"start angle must not be destination angle");

    [tester.mapView twoFingerRotateAtPoint:tester.mapView.center angle:45];

    XCTAssertGreaterThanOrEqual(fabs(tester.mapView.direction - startAngle),
                                20,
                                @"rotating map should change angle");
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

- (void)testCenterSet {
    CLLocationCoordinate2D newCenterCoordinate = CLLocationCoordinate2DMake(45.23237263, -122.23287129);

    XCTAssertNotEqual(tester.mapView.centerCoordinate.latitude,
                      newCenterCoordinate.latitude,
                      @"initial setup should have differing center latitude");

    XCTAssertNotEqual(tester.mapView.centerCoordinate.longitude,
                      newCenterCoordinate.longitude,
                      @"initial setup should have differing center longitude");

    tester.mapView.centerCoordinate = newCenterCoordinate;

    XCTAssertEqual(tester.mapView.centerCoordinate.latitude,
                   newCenterCoordinate.latitude,
                   @"setting center should change latitude");

    XCTAssertEqual(tester.mapView.centerCoordinate.longitude,
                   newCenterCoordinate.longitude,
                   @"setting center should change longitude");
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
    UIView *logoBug = (UIView *)[tester waitForViewWithAccessibilityLabel:@"Mapbox logo"];
    UIToolbar *toolbar = tester.viewController.navigationController.toolbar;
    UIView *attributionButton = (UIView *)[tester waitForViewWithAccessibilityLabel:@"Attribution info"];

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
    
    UIView *logoBug = (UIView *)[tester waitForViewWithAccessibilityLabel:@"Mapbox logo"];
    UIView *attributionButton = (UIView *)[tester waitForViewWithAccessibilityLabel:@"Attribution info"];
    
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

- (void)testDelegateRegionDidChange {
    __block NSUInteger unanimatedCount;
    __block NSUInteger animatedCount;
    [[NSNotificationCenter defaultCenter] addObserverForName:@"regionDidChangeAnimated"
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

    NSNotification *notification = [system waitForNotificationName:@"regionDidChangeAnimated"
                                                            object:tester.mapView
                                               whileExecutingBlock:^{
                                                   tester.mapView.centerCoordinate = CLLocationCoordinate2DMake(0, 0);
                                               }];

    [tester waitForTimeInterval:1];

    XCTAssertEqual([notification.userInfo[@"animated"] boolValue],
                   NO,
                   @"regionDidChange delegate should not indicate animated change");
    XCTAssertEqual(unanimatedCount,
                   1,
                   @"regionDidChange delegate should indicate one unanimated change");

    notification = [system waitForNotificationName:@"regionDidChangeAnimated"
                                            object:tester.mapView
                               whileExecutingBlock:^{
                                   [tester.mapView setCenterCoordinate:CLLocationCoordinate2DMake(45, 100) animated:YES];
                               }];

    [tester waitForTimeInterval:1];

    XCTAssertEqual([notification.userInfo[@"animated"] boolValue],
                   YES,
                   @"regionDidChange delegate should indicate animated change");
    XCTAssertEqual(animatedCount,
                   1,
                   @"regionDidChange delegate should indicate one animated change");

    [[NSNotificationCenter defaultCenter] removeObserver:self
                                                    name:@"regionDidChangeAnimated"
                                                  object:tester.mapView];
}

- (void)mapView:(MGLMapView *)mapView regionDidChangeAnimated:(BOOL)animated {
    [[NSNotificationCenter defaultCenter] postNotificationName:@"regionDidChangeAnimated"
                                                        object:mapView
                                                      userInfo:@{ @"animated" : @(animated) }];
}

- (void)testDelegatesStartStopLocatingUser {
    NSNotification *notification = [system waitForNotificationName:@"mapViewWillStartLocatingUser"
                                                            object:tester.mapView
                                               whileExecutingBlock:^{
                                                   tester.mapView.showsUserLocation = YES;
                                               }];

    XCTAssertEqualObjects(notification.name,
                          @"mapViewWillStartLocatingUser",
                          @"mapViewWillStartLocatingUser delegate should receive message");
    XCTAssertNotNil(tester.mapView.locationManager,
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
    XCTAssertNil(tester.mapView.locationManager,
                 "map view location manager should be nil");
}

- (void)mapViewWillStartLocatingUser:(MGLMapView *)mapView {
    [[NSNotificationCenter defaultCenter] postNotificationName:@"mapViewWillStartLocatingUser" object:mapView];
}

- (void)mapViewDidStopLocatingUser:(MGLMapView *)mapView {
    [[NSNotificationCenter defaultCenter] postNotificationName:@"mapViewDidStopLocatingUser" object:mapView];
}

- (void)testUserTrackingModeFollow {
    tester.mapView.userTrackingMode = MGLUserTrackingModeFollow;

    [tester waitForTimeInterval:1];

    XCTAssertEqual(tester.mapView.userLocationVisible,
                   YES,
                   @"user location should be visible");
    XCTAssertEqual(tester.mapView.userLocation.coordinate.latitude,
                   kMockedLatitude,
                   @"user location latitude should match mocked latitude");
    XCTAssertEqual(tester.mapView.userLocation.coordinate.longitude,
                   kMockedLongitude,
                   @"user location longitude should match mocked longitude");

    [tester.mapView dragFromPoint:CGPointMake(10, 10) toPoint:CGPointMake(50, 100) steps:10];

    XCTAssertEqual(tester.mapView.userLocationVisible,
                   YES,
                   @"user location should still be visible after panning");
    XCTAssertEqual(tester.mapView.userTrackingMode,
                   MGLUserTrackingModeNone,
                   @"user tracking mode should reset to none");
}

// DOES NOT CURRENTLY PASS, bug with tracking mode not being set properly (or reset)
- (void)testUserTrackingModeFollowWithHeading {
    tester.mapView.userTrackingMode = MGLUserTrackingModeFollowWithHeading;
    
    [tester waitForTimeInterval:1];
    
    XCTAssertEqual(tester.mapView.userLocationVisible,
                   YES,
                   @"user location should be visible");
    XCTAssertEqual(tester.mapView.userLocation.coordinate.latitude,
                   kMockedLatitude,
                   @"user location latitude should match mocked latitude");
    XCTAssertEqual(tester.mapView.userLocation.coordinate.longitude,
                   kMockedLongitude,
                   @"user location longitude should match mocked longitude");
    
    XCTAssertEqual(tester.mapView.userTrackingMode,
                   MGLUserTrackingModeFollowWithHeading,
                   @"user tracking mode should be follow with heading");
    XCTAssertEqual(tester.mapView.userLocation.heading.trueHeading,
                   kMockedHeadingTrueHeading,
                   @"user true heading should match mocked true heading");
    XCTAssertEqual(tester.mapView.userLocation.heading.headingAccuracy,
                   kMockedHeadingAccuracy,
                   @"user heading accuracy should match mocked accuracy");
    
    [tester.compass tap];
    
    [tester waitForTimeInterval:1];
    
    XCTAssertEqual(tester.mapView.userLocationVisible,
                   YES,
                   @"user location should be visible");
    XCTAssertEqual(tester.mapView.userTrackingMode,
                   MGLUserTrackingModeFollow,
                   @"user tracking mode should be follow");
    XCTAssertEqual(tester.mapView.direction,
                   0,
                   @"user heading should be reset to zero/north");
}

@end
