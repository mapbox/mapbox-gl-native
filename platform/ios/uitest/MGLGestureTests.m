#import "MGLUITestCase.h"

@interface MGLGestureTests : MGLUITestCase

@end

@implementation MGLGestureTests

- (void)testZoom {
    // Tap to zoom
    XCUIElement *mapElement = [[XCUIApplication alloc] init].otherElements[@"MGLMapView"];
    XCTAssertEqual([self accessibilityZoomLevel], 1, @"Map should start out at z0.");
    [mapElement doubleTap];
    [mapElement doubleTap];
    XCTAssertGreaterThan([self accessibilityZoomLevel], 1, @"Double-tapping should zoom in.");
    // Two-finger tapping has more friction than one-finger tapping.
    [mapElement twoFingerTap];
    [mapElement twoFingerTap];
    [mapElement twoFingerTap];
    XCTAssertEqual([self accessibilityZoomLevel], 1, @"Two-finger tapping should zoom back out.");
    
    // Pinch to zoom
    [mapElement pinchWithScale:2 velocity:2];
    XCTAssertGreaterThan([self accessibilityZoomLevel], 1, @"Pinching outward should zoom in.");
    [mapElement pinchWithScale:0.025 velocity:-2];
    [mapElement pinchWithScale:0.025 velocity:-2];
    XCTAssertEqual([self accessibilityZoomLevel], 1, @"Pinching inward should zoom out.");
}

- (NSInteger)accessibilityZoomLevel {
    XCUIElement *mapElement = [[XCUIApplication alloc] init].otherElements[@"MGLMapView"];
    XCTAssertNotNil(mapElement.value);
    NSString *prefixString = [mapElement.value componentsSeparatedByString:@"x"].firstObject;
    XCTAssertGreaterThan(prefixString.length, 0);
    XCTAssertTrue([prefixString hasPrefix:@"Zoom "]);
    NSString *zoomString = [prefixString substringFromIndex:@"Zoom ".length];
    return zoomString.integerValue;
}

- (void)testRotate {
    XCUIElement *mapElement = [[XCUIApplication alloc] init].otherElements[@"MGLMapView"];
    XCUIElement *compassElement = mapElement.buttons[@"MGLMapViewCompass"];
    NSPredicate *unhittable = [NSPredicate predicateWithFormat:@"hittable == NO"];
    
    XCTAssertFalse(compassElement.hittable, @"Compass should be absent when the map is unrotated.");
    
    // Attempt to rotate the map 90° counterclockwise. The map should unrotate itself.
    [mapElement rotate:M_PI_2 withVelocity:M_PI];
    XCTAssertTrue(compassElement.hittable, @"Compass should be present when the map is rotated.");
    XCTAssertNotEqualWithAccuracy([compassElement.value rangeOfString:@"west"].location, NSNotFound, 1, @"Map should point west or thereabouts after rotating 90° counterclockwise. Instead, it points %@.", compassElement.value);
    [self expectationForPredicate:unhittable evaluatedWithObject:compassElement handler:nil];
    [self waitForExpectationsWithTimeout:3 handler:nil];
    
    // Zoom in until rotation is allowed.
    [mapElement doubleTap];
    [mapElement doubleTap];
    [mapElement doubleTap];
    
    // Rotate the map 90° counterclockwise.
    [mapElement rotate:M_PI_2 withVelocity:M_PI];
    XCTAssertTrue(compassElement.hittable, @"Compass should be present when the map is rotated.");
    XCTAssertNotEqualWithAccuracy([compassElement.value rangeOfString:@"west"].location, NSNotFound, 1, @"Map should point west or thereabouts after rotating 90° counterclockwise. Instead, it points %@.", compassElement.value);
    
    // Keep interacting with the map to make sure it doesn’t unrotate.
    [mapElement swipeLeft];
    [mapElement swipeRight];
    
    // Unrotate the map.
    [compassElement tap];
    [self expectationForPredicate:unhittable evaluatedWithObject:compassElement handler:nil];
    [self waitForExpectationsWithTimeout:3 handler:nil];
}

@end
