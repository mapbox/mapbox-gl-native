#import "MapViewTests.h"
#import <KIF/KIFTestStepValidation.h>
#import "KIFTestActor+MapboxGL.h"
#import "MGLMapView.h"

@interface MapViewTests () <MGLMapViewDelegate>

@end

@implementation MapViewTests

- (void)beforeEach {
    [system simulateDeviceRotationToOrientation:UIDeviceOrientationPortrait];
    tester.mapView.viewControllerForLayoutGuides = tester.viewController;
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
    __KIFAssertEqual(tester.mapView.direction,
                     270,
                     @"setting direction should take effect");

    [tester waitForTimeInterval:1];

    __KIFAssertEqual(tester.compass.alpha,
                     1,
                     @"compass should be visible when map is rotated");
    __KIFAssertEqualObjects([NSValue valueWithCGAffineTransform:tester.compass.transform],
                            [NSValue valueWithCGAffineTransform:CGAffineTransformMakeRotation(M_PI * 1.5)],
                            @"compass rotation should indicate map rotation");
}

- (void)testCompassTap {
    tester.mapView.direction = 180;
    __KIFAssertEqual(tester.mapView.direction,
                     180,
                     @"setting direction should take effect");

    [tester waitForTimeInterval:1];

    [tester.compass tap];

    [tester waitForTimeInterval:1];

    __KIFAssertEqual(tester.mapView.direction,
                     0,
                     @"tapping compass should reset map direction");
    __KIFAssertEqual(tester.compass.alpha,
                     0,
                     @"compass should not be visible when map is unrotated");
    __KIFAssertEqualObjects([NSValue valueWithCGAffineTransform:tester.compass.transform],
                            [NSValue valueWithCGAffineTransform:CGAffineTransformIdentity],
                            @"compass rotation should indicate map rotation");
}

- (void)testDirectionReset {
    tester.mapView.direction = 100;
    __KIFAssertEqual(tester.mapView.direction,
                     100,
                     @"setting direction should take effect");

    [tester.mapView resetNorth];

    [tester waitForTimeInterval:1];

    __KIFAssertEqual(tester.mapView.direction,
                     0,
                     @"resetting north should reset map direction");
    __KIFAssertEqual(tester.compass.alpha,
                     0,
                     @"compass should not be visible when map is unrotated");
    __KIFAssertEqualObjects([NSValue valueWithCGAffineTransform:tester.compass.transform],
                            [NSValue valueWithCGAffineTransform:CGAffineTransformIdentity],
                            @"compass rotation should indicate map rotation");
}

- (void)testZoom {
    double zoom = tester.mapView.zoomLevel;

    [tester.mapView zoomAtPoint:CGPointMake(tester.mapView.bounds.size.width / 2,
                                            tester.mapView.bounds.size.height / 2)
                       distance:50
                          steps:10];

    XCTAssertTrue(tester.mapView.zoomLevel > zoom,
                  @"zoom gesture should increase zoom level");

    zoom = tester.mapView.zoomLevel;
    [tester.mapView pinchAtPoint:CGPointMake(tester.mapView.bounds.size.width / 2,
                                             tester.mapView.bounds.size.height / 2)
                        distance:50
                           steps:10];

    XCTAssertTrue(tester.mapView.zoomLevel < zoom,
                  @"pinch gesture should decrease zoom level");
}

- (void)testZoomDisabled {
    tester.mapView.zoomEnabled = NO;
    double zoom = tester.mapView.zoomLevel;

    [tester.mapView zoomAtPoint:CGPointMake(tester.mapView.bounds.size.width / 2,
                                            tester.mapView.bounds.size.height / 2)
                       distance:50
                          steps:10];

    __KIFAssertEqual(tester.mapView.zoomLevel,
                     zoom,
                     @"disabling zoom gesture should disallow zooming");

    [tester.mapView pinchAtPoint:CGPointMake(tester.mapView.bounds.size.width / 2,
                                            tester.mapView.bounds.size.height / 2)
                        distance:50
                           steps:10];

    __KIFAssertEqual(tester.mapView.zoomLevel,
                     zoom,
                     @"disabling zoom gesture should disallow pinching");
}

- (void)testPan {
    CLLocationCoordinate2D centerCoordinate = tester.mapView.centerCoordinate;

    [tester.mapView dragFromPoint:CGPointMake(10, 10) toPoint:CGPointMake(300, 300) steps:10];

    XCTAssertTrue(tester.mapView.centerCoordinate.latitude > centerCoordinate.latitude,
                  @"panning map down should increase center latitude");
    XCTAssertTrue(tester.mapView.centerCoordinate.longitude < centerCoordinate.longitude,
                  @"panning map right should decrease center longitude");
}

- (void)testPanDisabled {
    tester.mapView.scrollEnabled = NO;
    CLLocationCoordinate2D centerCoordinate = tester.mapView.centerCoordinate;

    [tester.mapView dragFromPoint:CGPointMake(10, 10) toPoint:CGPointMake(300, 300) steps:10];

    __KIFAssertEqual(centerCoordinate.latitude,
                     tester.mapView.centerCoordinate.latitude,
                     @"disabling pan gesture should disallow vertical panning");
    __KIFAssertEqual(centerCoordinate.longitude,
                     tester.mapView.centerCoordinate.longitude,
                     @"disabling pan gesture should disallow horizontal panning");
}

- (void)testCenterSet {
    CLLocationCoordinate2D newCenterCoordinate = CLLocationCoordinate2DMake(45.23237263, -122.23287129);
    XCTAssertNotEqual(tester.mapView.centerCoordinate.latitude,
                      newCenterCoordinate.latitude,
                      @"initial setup should have differing center latitude");
    XCTAssertNotEqual(tester.mapView.centerCoordinate.longitude,
                      newCenterCoordinate.longitude,
                      @"initial setup should have differing center longitude");

    [tester.mapView setCenterCoordinate:newCenterCoordinate];

    XCTAssertTrue(tester.mapView.centerCoordinate.latitude == newCenterCoordinate.latitude,
                  @"setting center should change latitude");
    XCTAssertTrue(tester.mapView.centerCoordinate.longitude == newCenterCoordinate.longitude,
                  @"setting center should change longitude");
}

- (void)testZoomSet {
    double newZoom = 11.65;
    XCTAssertNotEqual(tester.mapView.zoomLevel,
                      newZoom,
                      @"initial setup should have differing zoom");

    tester.mapView.zoomLevel = newZoom;

    __KIFAssertEqual(tester.mapView.zoomLevel,
                     newZoom,
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
                   @"rotated device should not have logo buy under toolbar");

    attributionButtonFrame = [attributionButton.superview convertRect:attributionButton.frame toView:nil];
    XCTAssertFalse(CGRectIntersectsRect(attributionButtonFrame, toolbarFrame),
                   @"rotated device should not have attribution button under toolbar");
}

- (void)testDelegateRegionWillChange {
    __block NSUInteger unanimatedCount = 0;
    __block NSUInteger animatedCount = 0;
    [[NSNotificationCenter defaultCenter] addObserverForName:@"regionWillChangeAnimated"
                                                      object:tester.mapView
                                                       queue:[NSOperationQueue mainQueue]
                                                  usingBlock:^(NSNotification *note) {
                                                      if ([note.userInfo[@"animated"] boolValue]) {
                                                          animatedCount++;
                                                      } else {
                                                          unanimatedCount++;
                                                      }
                                                  }];

    NSNotification *notification = [system waitForNotificationName:@"regionWillChangeAnimated"
                                                            object:tester.mapView
                                               whileExecutingBlock:^{
                                                   tester.mapView.centerCoordinate = CLLocationCoordinate2DMake(0, 0);
                                               }];
    [tester waitForTimeInterval:1];
    XCTAssertNotNil(notification,
                    @"regionWillChange delegate should produce a notification");
    __KIFAssertEqual([notification.userInfo[@"animated"] boolValue],
                     NO,
                     @"regionWillChange delegate should not indicate animated change");
    __KIFAssertEqual(unanimatedCount,
                     1,
                     @"regionWillChange delegate should indicate one unanimated change");

    notification = [system waitForNotificationName:@"regionWillChangeAnimated"
                                            object:tester.mapView
                               whileExecutingBlock:^{
                                   [tester.mapView setCenterCoordinate:CLLocationCoordinate2DMake(45, 100) animated:YES];
                               }];
    [tester waitForTimeInterval:1];
    XCTAssertNotNil(notification,
                    @"regionWillChange delegate should produce a notification");
    __KIFAssertEqual([notification.userInfo[@"animated"] boolValue],
                     YES,
                     @"regionWillChange delegate should indicate an animated change");
    __KIFAssertEqual(animatedCount,
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
    __block NSUInteger unanimatedCount = 0;
    __block NSUInteger animatedCount = 0;
    [[NSNotificationCenter defaultCenter] addObserverForName:@"regionDidChangeAnimated"
                                                      object:tester.mapView
                                                       queue:[NSOperationQueue mainQueue]
                                                  usingBlock:^(NSNotification *note) {
                                                      if ([note.userInfo[@"animated"] boolValue]) {
                                                          animatedCount++;
                                                      } else {
                                                          unanimatedCount++;
                                                      }
                                                  }];

    NSNotification *notification = [system waitForNotificationName:@"regionDidChangeAnimated"
                                                            object:tester.mapView
                                               whileExecutingBlock:^{
                                                   tester.mapView.centerCoordinate = CLLocationCoordinate2DMake(0, 0);
                                               }];
    [tester waitForTimeInterval:1];
    XCTAssertNotNil(notification,
                    @"regionDidChange delegate should produce a notification");
    __KIFAssertEqual([notification.userInfo[@"animated"] boolValue],
                     NO,
                     @"regionDidChange delegate should not indicate animated change");
    __KIFAssertEqual(unanimatedCount,
                     1,
                     @"regionDidChange delegate should indicate one unanimated change");

    notification = [system waitForNotificationName:@"regionDidChangeAnimated"
                                            object:tester.mapView
                               whileExecutingBlock:^{
                                   [tester.mapView setCenterCoordinate:CLLocationCoordinate2DMake(45, 100) animated:YES];
                               }];
    [tester waitForTimeInterval:1];
    XCTAssertNotNil(notification,
                    @"regionDidChange delegate should produce a notification");
    __KIFAssertEqual([notification.userInfo[@"animated"] boolValue],
                     YES,
                     @"regionDidChange delegate should indicate animated change");
    __KIFAssertEqual(animatedCount,
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

@end
