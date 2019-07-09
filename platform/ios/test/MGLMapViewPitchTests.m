#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

@interface MockUIPanGestureRecognizer : UIPanGestureRecognizer
@property CGFloat mbx_tiltGestureYTranslation;
@property NSUInteger mbx_numberOfFingersForGesture;
@end

@implementation MockUIPanGestureRecognizer
- (instancetype)initWithTarget:(id)target action:(SEL)action {
    if (self = [super initWithTarget:target action:action]) {
        self.mbx_tiltGestureYTranslation = 0;
        self.mbx_numberOfFingersForGesture = 2;
    }
    return self;
}
- (NSUInteger)numberOfTouches { return self.mbx_numberOfFingersForGesture; }
- (CGPoint)translationInView:(UIView *)view { return CGPointMake(0, self.mbx_tiltGestureYTranslation); }
- (void)setTiltGestureYTranslationForPitchDegrees:(CGFloat)degrees {
    // The tilt gesture takes the number of screen points the fingers have moved and then divides them by a "slowdown" factor, which happens to be set to 2.0 in -[MGLMapView handleTwoFingerDragGesture:].
    self.mbx_tiltGestureYTranslation = -(degrees * 2.0);
}
@end

// Forward declare relevant private methods
@interface MGLMapView (MGLMapViewPitchTests)
- (void)handleTwoFingerDragGesture:(UIPanGestureRecognizer *)rotate;
- (MGLMapCamera *)cameraByTiltingToPitch:(CGFloat)pitch;
@end

@interface MGLMapViewPitchTests : XCTestCase
@property (nonatomic) MGLMapView *mapView;
@end

@implementation MGLMapViewPitchTests

- (void)setUp {
    [super setUp];

    [MGLAccountManager setAccessToken:@"pk.feedcafedeadbeefbadebede"];
    NSURL *styleURL = [[NSBundle bundleForClass:[self class]] URLForResource:@"one-liner" withExtension:@"json"];
    self.mapView = [[MGLMapView alloc] initWithFrame:CGRectMake(0, 0, 400, 400) styleURL:styleURL];
}

- (void)tearDown {
    self.mapView = nil;
    [MGLAccountManager setAccessToken:nil];
    [super tearDown];
}

- (void)testPitchEnabled {
    self.mapView.pitchEnabled = NO;
    self.mapView.camera = [self.mapView cameraByTiltingToPitch:30];
    XCTAssertEqualWithAccuracy(self.mapView.camera.pitch, 30, 0.001, @"Tilt should not be set when pitchEnabled = NO.");

    self.mapView.pitchEnabled = YES;
    self.mapView.camera = [self.mapView cameraByTiltingToPitch:30];
    XCTAssertEqualWithAccuracy(self.mapView.camera.pitch, 30, 0.001, @"Tilt should be set when pitchEnabled = YES.");
}

- (void)testPitchInValidRange {
    for (NSNumber *degrees in @[@0, @5, @20, @40, @60]) {
        CGFloat inputDegrees = [degrees floatValue];
        self.mapView.camera = [self.mapView cameraByTiltingToPitch:inputDegrees];
        XCTAssertEqualWithAccuracy(self.mapView.camera.pitch, inputDegrees, 0.001, @"Tilt should be able to be set within range 0-60°.");
    }
}

- (void)testNegativePitchClamping {
    for (NSNumber *degrees in @[@CGFLOAT_MIN, @-999, @-60, @-30, @-0]) {
        CGFloat inputDegrees = [degrees floatValue];
        self.mapView.camera = [self.mapView cameraByTiltingToPitch:inputDegrees];
        XCTAssertEqualWithAccuracy(self.mapView.camera.pitch, 0, 0.001, @"Minimum tilt should be clamped to 0°.");
    }
}

- (void)testPositivePitchClamping {
    for (NSNumber *degrees in @[@61, @90, @999, @CGFLOAT_MAX]) {
        CGFloat inputDegrees = [degrees floatValue];
        self.mapView.camera = [self.mapView cameraByTiltingToPitch:inputDegrees];
        XCTAssertEqualWithAccuracy(self.mapView.camera.pitch, 60, 0.001, @"Maximum tilt should be clamped to 60°.");
    }
}

- (void)testPitchAtVariousZoomLevels {
    for (NSNumber *zoomLevel in @[@0, @5, @10, @15, @18, @21, @CGFLOAT_MAX]) {
        self.mapView.camera = [self.mapView cameraByTiltingToPitch:0];
        self.mapView.zoomLevel = 0;

        CGFloat inputZoomLevel = [zoomLevel floatValue];
        self.mapView.zoomLevel = inputZoomLevel;
        self.mapView.camera = [self.mapView cameraByTiltingToPitch:30];
        XCTAssertEqualWithAccuracy(self.mapView.camera.pitch, 30, 0.001, @"Tilting should be allowed at z%.f.", inputZoomLevel);
    }
}

- (void)testTiltGesture {
    MockUIPanGestureRecognizer *gesture = [[MockUIPanGestureRecognizer alloc] initWithTarget:self.mapView action:nil];
    gesture.state = UIGestureRecognizerStateBegan;
    [self.mapView handleTwoFingerDragGesture:gesture];
    XCTAssertEqual(self.mapView.camera.pitch, 0, @"Pitch should initially be set to 0°.");

    // Use a tilt gesture to tilt the map within its acceptable range (0-60°).
    for (NSNumber *degrees in @[@0, @5, @20, @40, @60]) {
        self.mapView.camera = [self.mapView cameraByTiltingToPitch:0];
        gesture.state = UIGestureRecognizerStateChanged;

        CGFloat inputDegrees = [degrees floatValue];
        [gesture setTiltGestureYTranslationForPitchDegrees:inputDegrees];
        [self.mapView handleTwoFingerDragGesture:gesture];
        XCTAssertEqualWithAccuracy(self.mapView.camera.pitch, inputDegrees, 0.001, @"Pitch should be set to %.f°.", inputDegrees);
    }
}

- (void)testTiltGestureFromInitialTilt {
    CGFloat initialTilt = 20;
    CGFloat additionalTilt = 30;

    // Set the map camera to a pitched state, perhaps from a previous gesture or camera movement.
    self.mapView.camera = [self.mapView cameraByTiltingToPitch:initialTilt];
    XCTAssertEqual(self.mapView.camera.pitch, initialTilt, @"Tilt should initially be set to %.f°.", initialTilt);

    // Initialize a tilt gesture.
    MockUIPanGestureRecognizer *gesture = [[MockUIPanGestureRecognizer alloc] initWithTarget:self.mapView action:nil];
    gesture.state = UIGestureRecognizerStateBegan;
    [self.mapView handleTwoFingerDragGesture:gesture];

    // Use the gesture to tilt the map even more.
    gesture.state = UIGestureRecognizerStateChanged;
    [gesture setTiltGestureYTranslationForPitchDegrees:additionalTilt];
    [self.mapView handleTwoFingerDragGesture:gesture];
    XCTAssertEqualWithAccuracy(self.mapView.camera.pitch, initialTilt + additionalTilt, 0.001, @"Tilt should be set to %.f°.", initialTilt + additionalTilt);
}

- (void)testTiltGestureNumberOfFingersRequired {
    // Initialize a tilt gesture with two fingers.
    MockUIPanGestureRecognizer *gesture = [[MockUIPanGestureRecognizer alloc] initWithTarget:self.mapView action:nil];
    gesture.state = UIGestureRecognizerStateBegan;
    gesture.mbx_numberOfFingersForGesture = 2;

    // Use the gesture to tilt to 30°.
    [gesture setTiltGestureYTranslationForPitchDegrees:30];
    [self.mapView handleTwoFingerDragGesture:gesture];
    XCTAssertEqualWithAccuracy(self.mapView.camera.pitch, 30, 0.001, @"Tilt gesture should work with two fingers down.");

    // Change the gesture's number of fingers to one and try to update the pitch.
    gesture.mbx_numberOfFingersForGesture = 1;
    gesture.state = UIGestureRecognizerStateChanged;
    [gesture setTiltGestureYTranslationForPitchDegrees:0];
    [self.mapView handleTwoFingerDragGesture:gesture];
    XCTAssertEqual(gesture.state, UIGestureRecognizerStateEnded, @"Gesture should end when the number of fingers is less than two.");
    XCTAssertEqualWithAccuracy(self.mapView.camera.pitch, 30, 0.001, @"Pitch should remain unchanged if gesture has ended.");
}

@end
