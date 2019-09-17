#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>
#import "MGLMockGestureRecognizers.h"

#import <mbgl/math/wrap.hpp>

@interface MGLMapView (MGLMapViewDirectionTests)
- (void)handleRotateGesture:(UIRotationGestureRecognizer *)rotate;
- (void)resetNorthAnimated:(BOOL)animated;
@end


@interface MGLMapViewDirectionTests : XCTestCase
@property (nonatomic) MGLMapView *mapView;
@end

@implementation MGLMapViewDirectionTests

- (void)setUp {
    [super setUp];

    [MGLAccountManager setAccessToken:@"pk.feedcafedeadbeefbadebede"];
    NSURL *styleURL = [[NSBundle bundleForClass:[self class]] URLForResource:@"one-liner" withExtension:@"json"];
    self.mapView = [[MGLMapView alloc] initWithFrame:CGRectMake(0, 0, 64, 64) styleURL:styleURL];
}

- (void)tearDown {
    self.mapView = nil;
    [MGLAccountManager setAccessToken:nil];
    [super tearDown];
}

- (void)testDirection {
    self.mapView.zoomLevel = 0;
    self.mapView.direction = 30;
    XCTAssertEqual(self.mapView.direction, 0, @"Rotation is not allowed at world-scale zoom levels.");

    self.mapView.zoomLevel = 15;
    CLLocationCoordinate2D originalCenterCoordinate = self.mapView.centerCoordinate;

    for (NSNumber *degrees in @[@-999, @-359, @-240, @-180, @-90, @-45, @0, @45, @90, @180, @240, @360, @999]) {
        double inputDegrees = [degrees doubleValue];
        double wrappedDegrees = mbgl::util::wrap(inputDegrees, 0., 360.);
        self.mapView.direction = inputDegrees;
        XCTAssertEqualWithAccuracy(self.mapView.direction, wrappedDegrees, 0.001);

        XCTAssertEqual(originalCenterCoordinate.latitude, self.mapView.centerCoordinate.latitude, "@Map center coordinate latitude should remain constant when direction is set to %@°.", degrees);
        XCTAssertEqual(originalCenterCoordinate.longitude, self.mapView.centerCoordinate.longitude, @"Map center coordinate longitude should remain constant when direction is set to %@°.", degrees);
    }

    [self.mapView resetNorthAnimated:NO];
    XCTAssertEqual(self.mapView.direction, 0, @"Reset-to-north should set direction to 0°.");
    XCTAssertEqual(originalCenterCoordinate.latitude, self.mapView.centerCoordinate.latitude, "@Map center coordinate latitude should remain constant when direction is reset.");
    XCTAssertEqual(originalCenterCoordinate.longitude, self.mapView.centerCoordinate.longitude, @"Map center coordinate latitude should remain constant when direction is reset.");
}

- (void)testRotateEnabled {
    self.mapView.zoomLevel = 10;

    UIRotationGestureRecognizerMock *gesture = [[UIRotationGestureRecognizerMock alloc] initWithTarget:nil action:nil];
    gesture.state = UIGestureRecognizerStateBegan;
    gesture.rotation = MGLRadiansFromDegrees(30);
    CGFloat wrappedRotation = mbgl::util::wrap(-MGLDegreesFromRadians(gesture.rotation), 0., 360.);

    // Disabled
    {
        self.mapView.rotateEnabled = NO;
        XCTAssertEqual(self.mapView.allowsRotating, NO);

        [self.mapView handleRotateGesture:gesture];
        XCTAssertNotEqual(self.mapView.direction, wrappedRotation, @"Gestural rotation should not work when rotation is disabled.");

        self.mapView.direction = 45.f;
        XCTAssertEqualWithAccuracy(self.mapView.direction, 45, 0.001, @"Programmatic rotation is allowed when rotateEnabled = NO.");
    }

    // Enabled
    {
        [self.mapView resetNorthAnimated:NO];
        self.mapView.rotateEnabled = YES;
        XCTAssertEqual(self.mapView.allowsRotating, YES);

        gesture.state = UIGestureRecognizerStateChanged;
        gesture.rotation = MGLRadiansFromDegrees(30);
        [self.mapView handleRotateGesture:gesture];
        XCTAssertEqualWithAccuracy(self.mapView.direction, wrappedRotation, 0.001, @"Gestural rotation should work when rotation is enabled.");
    }
}

- (void)testRotationGesture {
    self.mapView.zoomLevel = 15;
    CLLocationCoordinate2D originalCenterCoordinate = self.mapView.centerCoordinate;

    UIRotationGestureRecognizerMock *gesture = [[UIRotationGestureRecognizerMock alloc] initWithTarget:self.mapView action:nil];
    gesture.state = UIGestureRecognizerStateBegan;
    gesture.rotation = 0;
    [self.mapView addGestureRecognizer:gesture];
    [self.mapView handleRotateGesture:gesture];
    XCTAssertEqual(self.mapView.direction, gesture.rotation);

    for (NSNumber *degrees in @[@-999, @-360, @-240, @-180, @-90, @-45, @0, @45, @90, @180, @240, @359, @999]) {
        gesture.state = UIGestureRecognizerStateChanged;
        gesture.rotation = MGLRadiansFromDegrees([degrees doubleValue]);
        [self.mapView handleRotateGesture:gesture];
        CGFloat wrappedRotation = mbgl::util::wrap(-MGLDegreesFromRadians(gesture.rotation), 0., 360.);
        XCTAssertEqualWithAccuracy(self.mapView.direction, wrappedRotation, 0.001, @"Map direction should match gesture rotation for input of %@°.", degrees);

        // Given a hypothetical rotation around the exact center of the map, the center coordinate should remain the same.
        // See above where we override -[UIRotationGestureRecognizer locationInView:] to always return the center of the target view.
        XCTAssertEqualWithAccuracy(originalCenterCoordinate.latitude, self.mapView.centerCoordinate.latitude, 0.0000001, "@Map center coordinate latitude should remain constant during rotation of %@°.", degrees);
        XCTAssertEqualWithAccuracy(originalCenterCoordinate.longitude, self.mapView.centerCoordinate.longitude, 0.0000001, @"Map center coordinate longitude should remain constant during rotation of %@°.", degrees);
    }
}

- (void)testResetPosition {
    [self.mapView resetPosition];
    MGLMapCamera *defaultCamera = [MGLMapCamera cameraLookingAtCenterCoordinate:CLLocationCoordinate2DMake(0, 0) altitude:self.mapView.camera.altitude pitch:0 heading:0];
    XCTAssertTrue([self.mapView.camera isEqualToMapCamera:defaultCamera], @"Map camera %@ should be equal to default camera %@.", self.mapView.camera, defaultCamera);
}

- (CGFloat)degreesFromAffineTransform:(CGAffineTransform)transform {
    CGFloat angle = atan2f(transform.b, transform.a);
    return MGLDegreesFromRadians(angle);
}

@end
