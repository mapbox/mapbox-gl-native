#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

static MGLMapView *mapView;
static CGFloat mbx_tiltGestureYTranslation;
static NSUInteger mbx_numberOfFingersForGesture;

@interface MGLMapView (MGLMapViewPitchTests)
- (void)handleTwoFingerDragGesture:(UIPanGestureRecognizer *)rotate;
- (MGLMapCamera *)cameraByTiltingToPitch:(CGFloat)pitch;
@end

@interface UIPanGestureRecognizer (MGLMapViewPitchTests)
@property (nonatomic, readwrite) UIGestureRecognizerState state;
@end

@implementation UIPanGestureRecognizer (MGLMapViewPitchTests)
- (NSUInteger)numberOfTouches { return mbx_numberOfFingersForGesture; }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wobjc-protocol-method-implementation"
- (CGPoint)translationInView:(UIView *)view { return CGPointMake(0, mbx_tiltGestureYTranslation); }
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

    mbx_tiltGestureYTranslation = 0;
    mbx_numberOfFingersForGesture = 2;
}

- (void)tearDown {
    mapView = nil;
    [MGLAccountManager setAccessToken:nil];
    [super tearDown];
}

- (void)testPitchEnabled {
    mapView.pitchEnabled = NO;
    mapView.camera = [mapView cameraByTiltingToPitch:30];
    XCTAssertEqualWithAccuracy(mapView.camera.pitch, 30, 0.001, @"Tilt should not be set when pitchEnabled = NO.");

    mapView.pitchEnabled = YES;
    mapView.camera = [mapView cameraByTiltingToPitch:30];
    XCTAssertEqualWithAccuracy(mapView.camera.pitch, 30, 0.001, @"Tilt should be set when pitchEnabled = YES.");
}

- (void)testPitchInValidRange {
    for (NSNumber *degrees in @[@0, @5, @20, @40, @60]) {
        CGFloat inputDegrees = [degrees floatValue];
        mapView.camera = [mapView cameraByTiltingToPitch:inputDegrees];
        XCTAssertEqualWithAccuracy(mapView.camera.pitch, inputDegrees, 0.001, @"Tilt should be able to be set within range 0-60°.");
    }
}

- (void)testNegativePitchClamping {
    for (NSNumber *degrees in @[@CGFLOAT_MIN, @-999, @-60, @-30, @-0]) {
        CGFloat inputDegrees = [degrees floatValue];
        mapView.camera = [mapView cameraByTiltingToPitch:inputDegrees];
        XCTAssertEqualWithAccuracy(mapView.camera.pitch, 0, 0.001, @"Minimum tilt should be clamped to 0°.");
    }
}

- (void)testPositivePitchClamping {
    for (NSNumber *degrees in @[@61, @90, @999, @CGFLOAT_MAX]) {
        CGFloat inputDegrees = [degrees floatValue];
        mapView.camera = [mapView cameraByTiltingToPitch:inputDegrees];
        XCTAssertEqualWithAccuracy(mapView.camera.pitch, 60, 0.001, @"Maximum tilt should be clamped to 60°.");
    }
}

- (void)testPitchAtVariousZoomLevels {
    for (NSNumber *zoomLevel in @[@0, @5, @10, @15, @18, @21, @CGFLOAT_MAX]) {
        mapView.camera = [mapView cameraByTiltingToPitch:0];
        mapView.zoomLevel = 0;

        CGFloat inputZoomLevel = [zoomLevel floatValue];
        mapView.zoomLevel = inputZoomLevel;
        mapView.camera = [mapView cameraByTiltingToPitch:30];
        XCTAssertEqualWithAccuracy(mapView.camera.pitch, 30, 0.001, @"Tilting should be allowed at z%.f.", inputZoomLevel);
    }
}

- (void)testTiltGesture {
    UIPanGestureRecognizer *gesture = [[UIPanGestureRecognizer alloc] initWithTarget:mapView action:nil];
    gesture.state = UIGestureRecognizerStateBegan;
    [mapView handleTwoFingerDragGesture:gesture];
    XCTAssertEqual(mapView.camera.pitch, 0, @"Pitch should initially be set to 0°.");

    // Use a tilt gesture to tilt the map within its acceptable range (0-60°).
    for (NSNumber *degrees in @[@0, @5, @20, @40, @60]) {
        mapView.camera = [mapView cameraByTiltingToPitch:0];
        gesture.state = UIGestureRecognizerStateChanged;

        CGFloat inputDegrees = [degrees floatValue];
        [self setTiltGestureYTranslationForPitchDegrees:inputDegrees];
        [mapView handleTwoFingerDragGesture:gesture];
        XCTAssertEqualWithAccuracy(mapView.camera.pitch, inputDegrees, 0.001, @"Pitch should be set to %.f°.", inputDegrees);
    }
}

- (void)testTiltGestureFromInitialTilt {
    CGFloat initialTilt = 20;
    CGFloat additionalTilt = 30;

    // Set the map camera to a pitched state, perhaps from a previous gesture or camera movement.
    mapView.camera = [mapView cameraByTiltingToPitch:initialTilt];
    XCTAssertEqual(mapView.camera.pitch, initialTilt, @"Tilt should initially be set to %.f°.", initialTilt);

    // Initialize a tilt gesture.
    UIPanGestureRecognizer *gesture = [[UIPanGestureRecognizer alloc] initWithTarget:mapView action:nil];
    gesture.state = UIGestureRecognizerStateBegan;
    [mapView handleTwoFingerDragGesture:gesture];

    // Use the gesture to tilt the map even more.
    gesture.state = UIGestureRecognizerStateChanged;
    [self setTiltGestureYTranslationForPitchDegrees:additionalTilt];
    [mapView handleTwoFingerDragGesture:gesture];
    XCTAssertEqualWithAccuracy(mapView.camera.pitch, initialTilt + additionalTilt, 0.001, @"Tilt should be set to %.f°.", initialTilt + additionalTilt);
}

- (void)testTiltGestureNumberOfFingersRequired {
    // Initialize a tilt gesture with two fingers.
    UIPanGestureRecognizer *gesture = [[UIPanGestureRecognizer alloc] initWithTarget:mapView action:nil];
    gesture.state = UIGestureRecognizerStateBegan;
    mbx_numberOfFingersForGesture = 2;

    // Use the gesture to tilt to 30°.
    [self setTiltGestureYTranslationForPitchDegrees:30];
    [mapView handleTwoFingerDragGesture:gesture];
    XCTAssertEqualWithAccuracy(mapView.camera.pitch, 30, 0.001, @"Tilt gesture should work with two fingers down.");

    // Change the gesture's number of fingers to one and try to update the pitch.
    mbx_numberOfFingersForGesture = 1;
    gesture.state = UIGestureRecognizerStateChanged;
    [self setTiltGestureYTranslationForPitchDegrees:0];
    [mapView handleTwoFingerDragGesture:gesture];
    XCTAssertEqual(gesture.state, UIGestureRecognizerStateEnded, @"Gesture should end when the number of fingers is less than two.");
    XCTAssertEqualWithAccuracy(mapView.camera.pitch, 30, 0.001, @"Pitch should remain unchanged if gesture has ended.");
}

- (void)setTiltGestureYTranslationForPitchDegrees:(CGFloat)degrees {
    // The tilt gesture takes the number of screen points the fingers have moved and then divides them by a "slowdown" factor, which happens to be set to 2.0 in -[MGLMapView handleTwoFingerDragGesture:].
    mbx_tiltGestureYTranslation = -(degrees * 2.0);
}

@end
