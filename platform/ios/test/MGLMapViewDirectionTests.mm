#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

#import <mbgl/math/wrap.hpp>

static MGLMapView *mapView;

@interface MGLMapView (MGLMapViewDirectionTests)
- (void)handleRotateGesture:(UIRotationGestureRecognizer *)rotate;
- (void)resetNorthAnimated:(BOOL)animated;
@end

@interface UIRotationGestureRecognizer (MGLMapViewDirectionTests)
@property (nonatomic, readwrite) UIGestureRecognizerState state;
@end

@interface MGLMapViewDirectionTests : XCTestCase
@end

@implementation MGLMapViewDirectionTests

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

- (void)testDirection {
    mapView.zoomLevel = 0;
    mapView.direction = 30;
    XCTAssertEqual(mapView.direction, 0, @"Rotation is not allowed at world-scale zoom levels.");

    mapView.zoomLevel = 15;

    for (NSNumber *degrees in @[@-999, @-359, @-240, @-180, @-90, @-45, @0, @45, @90, @180, @240, @360, @999]) {
        double inputDegrees = [degrees doubleValue];
        double wrappedDegrees = mbgl::util::wrap(inputDegrees, 0., 360.);
        mapView.direction = inputDegrees;
        XCTAssertEqualWithAccuracy(mapView.direction, wrappedDegrees, 0.001);
    }

    [mapView resetNorthAnimated:NO];
    XCTAssertEqual(mapView.direction, 0, @"Reset-to-north should set direction to 0°.");
}

- (void)testRotateEnabled {
    mapView.zoomLevel = 10;

    UIRotationGestureRecognizer *gesture = [[UIRotationGestureRecognizer alloc] initWithTarget:nil action:nil];
    gesture.state = UIGestureRecognizerStateBegan;
    gesture.rotation = MGLRadiansFromDegrees(30);
    CGFloat wrappedRotation = mbgl::util::wrap(-MGLDegreesFromRadians(gesture.rotation), 0., 360.);

    // Disabled
    {
        mapView.rotateEnabled = NO;
        XCTAssertEqual(mapView.allowsRotating, NO);

        [mapView handleRotateGesture:gesture];
        XCTAssertNotEqual(mapView.direction, wrappedRotation, @"Gestural rotation should not work when rotation is disabled.");

        mapView.direction = 45.f;
        XCTAssertEqualWithAccuracy(mapView.direction, 45, 0.001, @"Programmatic rotation is allowed when rotateEnabled = NO.");
    }

    // Enabled
    {
        [mapView resetNorthAnimated:NO];
        mapView.rotateEnabled = YES;
        XCTAssertEqual(mapView.allowsRotating, YES);

        gesture.state = UIGestureRecognizerStateChanged;
        gesture.rotation = MGLRadiansFromDegrees(30);
        [mapView handleRotateGesture:gesture];
        XCTAssertEqualWithAccuracy(mapView.direction, wrappedRotation, 0.001, @"Gestural rotation should work when rotation is enabled.");
    }
}

- (void)testRotateGesture {
    mapView.zoomLevel = 15;

    UIRotationGestureRecognizer *gesture = [[UIRotationGestureRecognizer alloc] initWithTarget:nil action:nil];
    gesture.state = UIGestureRecognizerStateBegan;
    gesture.rotation = 0;
    [mapView handleRotateGesture:gesture];
    XCTAssertEqual(mapView.direction, gesture.rotation);

    for (NSNumber *degrees in @[@-999, @-360, @-240, @-180, @-90, @-45, @0, @45, @90, @180, @240, @359, @999]) {
        gesture.state = UIGestureRecognizerStateChanged;
        gesture.rotation = MGLRadiansFromDegrees([degrees doubleValue]);
        [mapView handleRotateGesture:gesture];
        CGFloat wrappedRotation = mbgl::util::wrap(-MGLDegreesFromRadians(gesture.rotation), 0., 360.);
        XCTAssertEqualWithAccuracy(mapView.direction, wrappedRotation, 0.001, @"Map direction should match gesture rotation for input of %@°.", degrees);
    }
}

- (void)testCompassRotation {
    mapView.zoomLevel = 15;

    for (NSNumber *degrees in @[@-999, @-359, @-240, @-180, @-90, @-45, @0, @45, @90, @180, @240, @360, @999]) {
        mapView.direction = [degrees doubleValue];
        CGFloat wrappedDirection = mbgl::util::wrap(-mapView.direction, 0., 360.);
        CGAffineTransform rotation = CGAffineTransformMakeRotation(MGLRadiansFromDegrees(wrappedDirection));
        XCTAssertTrue(CGAffineTransformEqualToTransform(mapView.compassView.transform, rotation),
                      @"Compass transform direction %f° should equal wrapped transform direction %f° (~%.f°).", [self degreesFromAffineTransform:mapView.compassView.transform], [self degreesFromAffineTransform:rotation], wrappedDirection);
    }
}

- (void)testResetPosition {
    [mapView resetPosition];
    MGLMapCamera *defaultCamera = [MGLMapCamera cameraLookingAtCenterCoordinate:CLLocationCoordinate2DMake(0, 0) altitude:mapView.camera.altitude pitch:0 heading:0];
    XCTAssertTrue([mapView.camera isEqualToMapCamera:defaultCamera], @"Map camera %@ should be equal to default camera %@.", mapView.camera, defaultCamera);
}

- (CGFloat)degreesFromAffineTransform:(CGAffineTransform)transform {
    CGFloat angle = atan2f(transform.b, transform.a);
    return MGLDegreesFromRadians(angle);
}

@end
