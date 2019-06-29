#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

static MGLMapView *mapView;
static CGFloat tiltGestureYTranslation;

@interface MGLMapView (MGLMapViewPitchTests)
- (void)handleTwoFingerDragGesture:(UIPanGestureRecognizer *)rotate;
- (MGLMapCamera *)cameraByTiltingToPitch:(CGFloat)pitch;
@end

@interface UIPanGestureRecognizer (MGLMapViewPitchTests)
@property (nonatomic, readwrite) UIGestureRecognizerState state;
@end

@implementation UIPanGestureRecognizer (MGLMapViewPitchTests)
- (NSUInteger)numberOfTouches { return 2; }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wobjc-protocol-method-implementation"
- (CGPoint)translationInView:(UIView *)view { return CGPointMake(0, tiltGestureYTranslation); }
#pragma clang diagnostic pop
@end

@interface MGLMapViewPitchTests : XCTestCase
@end

@implementation MGLMapViewPitchTests

- (void)setUp {
    [super setUp];

    [MGLAccountManager setAccessToken:@"pk.feedcafedeadbeefbadebede"];
    NSURL *styleURL = [[NSBundle bundleForClass:[self class]] URLForResource:@"one-liner" withExtension:@"json"];
    mapView = [[MGLMapView alloc] initWithFrame:CGRectMake(0, 0, 400, 400) styleURL:styleURL];

    tiltGestureYTranslation = 0;
}

- (void)tearDown {
    mapView = nil;
    [MGLAccountManager setAccessToken:nil];
    [super tearDown];
}

- (void)testPitch {
    mapView.zoomLevel = 0;
    mapView.camera = [mapView cameraByTiltingToPitch:30];
    XCTAssertEqualWithAccuracy(mapView.camera.pitch, 30, 0.001, @"Tilt should be allowed at world-scale zoom levels.");

    mapView.zoomLevel = 15;
    mapView.camera = [mapView cameraByTiltingToPitch:30];
    XCTAssertEqualWithAccuracy(mapView.camera.pitch, 30, 0.001, @"Tilt should work at z15.");

    for (NSNumber *degrees in @[@0, @5, @20, @40, @60]) {
        CGFloat inputDegrees = [degrees floatValue];
        mapView.camera = [mapView cameraByTiltingToPitch:inputDegrees];
        XCTAssertEqualWithAccuracy(mapView.camera.pitch, inputDegrees, 0.001, @"Tilt should be able to be set to range 0-60°.");
    }

    // Currently fails, which seems incorrect.
//    mapView.camera = [mapView cameraByTiltingToPitch:-10];
//    XCTAssertEqualWithAccuracy(mapView.camera.pitch, 0, 0.001, @"Minimum tilt should be clamped to 0°.");

    mapView.camera = [mapView cameraByTiltingToPitch:999];
    XCTAssertEqualWithAccuracy(mapView.camera.pitch, 60, 0.001, @"Maximum tilt should be clamped to 60°.");
}

- (void)testPitchEnabled {
    mapView.pitchEnabled = NO;
    mapView.camera = [mapView cameraByTiltingToPitch:30];
    XCTAssertEqualWithAccuracy(mapView.camera.pitch, 30, 0.001, @"Tilt should not be set when pitchEnabled = NO.");

    mapView.pitchEnabled = YES;
    mapView.camera = [mapView cameraByTiltingToPitch:30];
    XCTAssertEqualWithAccuracy(mapView.camera.pitch, 30, 0.001, @"Tilt should be set when pitchEnabled = YES.");
}

- (void)testTiltGesture {
    UIPanGestureRecognizer *gesture = [[UIPanGestureRecognizer alloc] initWithTarget:mapView action:nil];
    gesture.state = UIGestureRecognizerStateBegan;
    [mapView handleTwoFingerDragGesture:gesture];
    XCTAssertEqual(mapView.camera.pitch, 0, @"Tilt should initially be set to 0°.");

    for (NSNumber *degrees in @[@0, @5, @20, @40, @60]) {
        mapView.camera = [mapView cameraByTiltingToPitch:0];
        gesture.state = UIGestureRecognizerStateChanged;

        CGFloat inputDegrees = [degrees floatValue];
        [self setTiltGestureYTranslationForPitchDegrees:inputDegrees];
        [mapView handleTwoFingerDragGesture:gesture];
        XCTAssertEqualWithAccuracy(mapView.camera.pitch, inputDegrees, 0.001, @"Tilt should be set to %.f°.", inputDegrees);
    }
}

- (void)testTiltGestureFromInitialTilt {
    CGFloat initialTilt = 20;
    CGFloat additionalTilt = 30;

    mapView.camera = [mapView cameraByTiltingToPitch:initialTilt];
    XCTAssertEqual(mapView.camera.pitch, initialTilt, @"Tilt should initially be set to %.f°.", initialTilt);

    UIPanGestureRecognizer *gesture = [[UIPanGestureRecognizer alloc] initWithTarget:mapView action:nil];
    gesture.state = UIGestureRecognizerStateBegan;
    [mapView handleTwoFingerDragGesture:gesture];

    [self setTiltGestureYTranslationForPitchDegrees:additionalTilt];
    [mapView handleTwoFingerDragGesture:gesture];
    XCTAssertEqualWithAccuracy(mapView.camera.pitch, initialTilt + additionalTilt, 0.001, @"Tilt should be set to %.f°.", initialTilt + additionalTilt);
}

- (void)setTiltGestureYTranslationForPitchDegrees:(CGFloat)degrees {
    // The tilt gesture takes the number of screen points the fingers have moved and then divides them by a "slowdown" factor, which happens to be set to 2.0 in -[MGLMapView handleTwoFingerDragGesture:].
    tiltGestureYTranslation = -(degrees * 2.0);
}

@end
