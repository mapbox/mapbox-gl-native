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

- (void)testDirectionAndRotation {
    mapView.zoomLevel = 0;
    mapView.direction = 30;
    XCTAssertEqual(mapView.direction, 0, @"Rotation is not allowed at world-scale zoom levels.");

    mapView.zoomLevel = 15;

    mapView.direction = 5;
    XCTAssertEqual(mapView.direction, 5);

    mapView.direction = -180;
    XCTAssertEqual(mapView.direction, 180, @"Negative (invalid) direction should be wrapped.");

    mapView.direction = 366;
    XCTAssertEqualWithAccuracy(mapView.direction, 6, 0.001, @">360° direction should be wrapped.");

    [mapView resetNorthAnimated:NO];
    XCTAssertEqual(mapView.direction, 0, @"Reset-to-north should set direction to 0°.");

    UIRotationGestureRecognizer *rotate = [[UIRotationGestureRecognizer alloc] initWithTarget:nil action:nil];
    rotate.state = UIGestureRecognizerStateBegan;
    rotate.rotation = 0;
    [mapView handleRotateGesture:rotate];
    XCTAssertEqual(mapView.direction, rotate.rotation);

    rotate.state = UIGestureRecognizerStateChanged;
    rotate.rotation = 0.4;
    [mapView handleRotateGesture:rotate];
    XCTAssertEqual(mapView.direction, mbgl::util::wrap(-MGLDegreesFromRadians(rotate.rotation), 0., 360.), @"Map direction should match gesture rotation.");
}

- (void)testCompassRotation {
    mapView.zoomLevel = 15;

    for (NSNumber *degrees in @[@-1, @0, @45, @90, @180, @240, @360, @999]) {
        mapView.direction = [degrees doubleValue];
        CGFloat wrappedDirection = mbgl::util::wrap(-mapView.direction, 0., 360.);
        CGAffineTransform rotation = CGAffineTransformMakeRotation(MGLRadiansFromDegrees(wrappedDirection));
        XCTAssertTrue(CGAffineTransformEqualToTransform(mapView.compassView.transform, rotation),
                      @"Compass transform %@ should equal wrapped transform %@.", NSStringFromCGAffineTransform(mapView.compassView.transform), NSStringFromCGAffineTransform(rotation));
    }
}

- (void)testResetPosition {
    [mapView resetPosition];
    MGLMapCamera *defaultCamera = [MGLMapCamera cameraLookingAtCenterCoordinate:CLLocationCoordinate2DMake(0, 0) altitude:mapView.camera.altitude pitch:0 heading:0];
    XCTAssertTrue([mapView.camera isEqualToMapCamera:defaultCamera], @"Map camera %@ should be equal to default camera %@.", mapView.camera, defaultCamera);
}

@end
