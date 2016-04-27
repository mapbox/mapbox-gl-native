#import "MGLUITestCase.h"

@interface MGLUserLocationTests : MGLUITestCase

@end

@implementation MGLUserLocationTests

- (void)testUserDot {
    XCUIApplication *app = [[XCUIApplication alloc] init];
    NSPredicate *exists = [NSPredicate predicateWithFormat:@"exists == YES"];
    NSPredicate *isAbsent = [NSPredicate predicateWithFormat:@"exists == NO"];
    NSPredicate *hittable = [NSPredicate predicateWithFormat:@"hittable == YES"];
    
    XCUIElement *userLocationAnnotationElement = app.buttons[@"MGLUserLocationAnnotationView"];
    XCTAssertFalse(userLocationAnnotationElement.exists, @"User dot should be hidden by default.");
    
    XCUIElement *userTrackingModeElement = app.navigationBars[@"MBXNavigationBar"].buttons[@"MBXUserTrackingModeButton"];
    XCTAssertEqualObjects(userTrackingModeElement.value, @"Off", @"User tracking mode should be off by default.");
    
    // Change to user location tracking mode.
    [self expectationForPredicate:hittable evaluatedWithObject:userLocationAnnotationElement handler:nil];
    __block CGRect userDotAnnotationFrame;
    [self expectationForPredicate:[NSPredicate predicateWithBlock:^BOOL(id  _Nonnull evaluatedObject, NSDictionary<NSString *,id> * _Nullable bindings) {
        // The user dot happens to lie underneath the Settings button until it is shown.
        userDotAnnotationFrame = userLocationAnnotationElement.frame;
        return CGRectContainsPoint(userDotAnnotationFrame, CGPointMake(190, 368));
    }] evaluatedWithObject:userLocationAnnotationElement handler:nil];
    [userTrackingModeElement tap];
    XCTAssertEqualObjects(userTrackingModeElement.value, @"Follow location", @"User location tracking mode should be on after tapping the user tracking mode button.");
    [self waitForExpectationsWithTimeout:3 handler:nil];
    
    // Open the user dot’s callout view.
    XCUIElement *titleText = app.staticTexts[@"You Are Here"];
    [self expectationForPredicate:exists evaluatedWithObject:titleText handler:nil];
    [userLocationAnnotationElement tap];
    [self waitForExpectationsWithTimeout:2 handler:nil];
    
    // Close the user dot’s callout view by tapping on the map proxy element. Note that the map proxy element has a gaping hole in the middle to accommodate the callout view.
    XCUIElement *mapProxyElement = app.buttons[@"MGLMapViewProxyAccessibilityElement"];
    [self expectationForPredicate:isAbsent evaluatedWithObject:mapProxyElement handler:nil];
    XCUICoordinate *coordinate = [[mapProxyElement coordinateWithNormalizedOffset:CGVectorMake(0, 0)] coordinateWithOffset:CGVectorMake(100, 100)];
    [coordinate tap];
    [self waitForExpectationsWithTimeout:2 handler:nil];
    
    // Change to user heading tracking mode.
    [userTrackingModeElement tap];
    XCTAssertEqualObjects(userTrackingModeElement.value, @"Follow location and heading", @"User heading tracking mode should be on after tapping the user tracking mode button.");
    
    // Change to user course tracking mode.
    [userTrackingModeElement tap];
    XCTAssertEqualObjects(userTrackingModeElement.value, @"Follow course", @"Course tracking mode should be on after tapping the user tracking mode button.");
    CGRect userPuckAnnotationFrame = userLocationAnnotationElement.frame;
    XCTAssertGreaterThan(userPuckAnnotationFrame.size.width, userDotAnnotationFrame.size.width, @"User puck should be wider than a user dot.");
    XCTAssertGreaterThan(userPuckAnnotationFrame.size.height, userDotAnnotationFrame.size.height, @"User puck should be taller than a user dot.");
    
    // Turn user tracking mode off.
    [userTrackingModeElement tap];
    XCTAssertEqualObjects(userTrackingModeElement.value, @"Off", @"Course tracking mode should be off after tapping the user tracking mode button.");
}

@end
