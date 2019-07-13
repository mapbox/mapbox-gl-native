#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

@interface MGLMapView (MGLMapViewZoomTests)
- (void)handlePinchGesture:(UIPinchGestureRecognizer *)pinch;
@end

@interface UIPinchGestureRecognizerMock : UIPinchGestureRecognizer
@property (nonatomic) CGPoint locationInViewOverride;
@end

@implementation UIPinchGestureRecognizerMock
- (CGPoint)locationInView:(nullable UIView *)view { return self.locationInViewOverride; }
@end

@interface MGLMapViewZoomTests : XCTestCase
@property (nonatomic) MGLMapView *mapView;
@end

@implementation MGLMapViewZoomTests

- (void)setUp {
    [super setUp];

    [MGLAccountManager setAccessToken:@"pk.feedcafedeadbeefbadebede"];
    NSURL *styleURL = [[NSBundle bundleForClass:[self class]] URLForResource:@"one-liner" withExtension:@"json"];
    self.mapView = [[MGLMapView alloc] initWithFrame:UIScreen.mainScreen.bounds styleURL:styleURL];
}

- (void)tearDown {
    self.mapView = nil;
    [MGLAccountManager setAccessToken:nil];
    [super tearDown];
}

- (void)testZoom {
    CLLocationCoordinate2D originalCenterCoordinate = self.mapView.centerCoordinate;

    for (NSNumber *zoom in @[@1, @5, @10, @15, @22]) {
        self.mapView.zoomLevel = [zoom doubleValue];
        XCTAssertEqual(self.mapView.zoomLevel, [zoom doubleValue], @"Map zoom should match z%@.", zoom);

        XCTAssertEqualWithAccuracy(originalCenterCoordinate.latitude, self.mapView.centerCoordinate.latitude, 0.0000001, "@Map center coordinate latitude should remain constant during zoom to z%@.", zoom);
        XCTAssertEqualWithAccuracy(originalCenterCoordinate.longitude, self.mapView.centerCoordinate.longitude, 0.0000001, @"Map center coordinate longitude should remain constant during zoom to z%@.", zoom);
    }
}

- (void)testZoomEnabled {
    UIPinchGestureRecognizerMock *gesture = [[UIPinchGestureRecognizerMock alloc] initWithTarget:nil action:nil];
    gesture.state = UIGestureRecognizerStateBegan;
    gesture.scale = 10.f;
    [self.mapView handlePinchGesture:gesture];
    gesture.state = UIGestureRecognizerStateChanged;

    // Disabled
    {
        self.mapView.zoomEnabled = NO;
        XCTAssertEqual(self.mapView.allowsZooming, NO);

        [self.mapView handlePinchGesture:gesture];
        XCTAssertNotEqual(self.mapView.zoomLevel, log2(gesture.scale), @"Gestural zoom should not work when zoom is disabled.");

        self.mapView.zoomLevel = 15.f;
        XCTAssertEqualWithAccuracy(self.mapView.zoomLevel, 15, 0.001, @"Programmatic zoom is allowed when zoomEnabled = NO.");
    }

    // Enabled
    {
        // No need to reset the map zoom or gesture scale, since gesture scale hasn't been applied yet and the map zoom will be overriden when the gesture is handled.
        self.mapView.zoomEnabled = YES;
        XCTAssertEqual(self.mapView.allowsZooming, YES);

        [self.mapView handlePinchGesture:gesture];
        XCTAssertEqualWithAccuracy(self.mapView.zoomLevel, log2(gesture.scale), 0.001, @"Gestural zoom should work when zoom is enabled.");
    }
}

- (void)testPinchGesture {
    CLLocationCoordinate2D originalCenterCoordinate = self.mapView.centerCoordinate;

    UIPinchGestureRecognizerMock *gesture = [[UIPinchGestureRecognizerMock alloc] initWithTarget:self.mapView action:nil];
    gesture.state = UIGestureRecognizerStateBegan;
    gesture.scale = 0;
    gesture.locationInViewOverride = self.mapView.center;
    [self.mapView handlePinchGesture:gesture];
    XCTAssertEqual(self.mapView.zoomLevel, 0);

    for (NSNumber *zoom in @[@1, @5, @10, @15, @22]) {
        gesture.state = UIGestureRecognizerStateChanged;
        gesture.scale = MGLScaleFromZoomLevel([zoom doubleValue]);
        [self.mapView handlePinchGesture:gesture];
        XCTAssertEqual(self.mapView.zoomLevel, [zoom doubleValue], @"Map zoom should match gesture to z%@.", zoom);

        // Given a hypothetical zoom into the exact center of the map, the center coordinate should remain the same.
        XCTAssertEqualWithAccuracy(originalCenterCoordinate.latitude, self.mapView.centerCoordinate.latitude, 0.0000001, "@Map center coordinate latitude should remain constant during zoom to z%@.", zoom);
        XCTAssertEqualWithAccuracy(originalCenterCoordinate.longitude, self.mapView.centerCoordinate.longitude, 0.0000001, @"Map center coordinate longitude should remain constant during zoom to z%@.", zoom);
    }
}

// Regression test for: https://github.com/mapbox/mapbox-gl-native/issues/14977
- (void)testPinchGestureOffCenter {
    self.mapView.zoomLevel = 15;

    // Set up pinch gesture at z15 with an origin of 0,0.
    UIPinchGestureRecognizerMock *gesture = [[UIPinchGestureRecognizerMock alloc] initWithTarget:self.mapView action:nil];
    gesture.state = UIGestureRecognizerStateBegan;
    gesture.scale = 0;
    gesture.locationInViewOverride = CGPointMake(0, 0);
    [self.mapView handlePinchGesture:gesture];
    XCTAssertEqual(self.mapView.zoomLevel, 15);

    // Set a map rotation so that we can later check if un-rotating happens around the correct center point.
    self.mapView.direction = 45;

    // Zoom to z18 at the off-center origin.
    gesture.state = UIGestureRecognizerStateChanged;
    gesture.scale = MGLScaleFromZoomLevel(3);
    [self.mapView handlePinchGesture:gesture];
    XCTAssertEqual(self.mapView.zoomLevel, 18, @"Map zoom should match original zoom plus gesture zoom.");

    // Check that the center coordinate remains the same when direction is reset to north.
    CLLocationCoordinate2D centerCoordinateBeforeReset = self.mapView.centerCoordinate;
    CLLocationCoordinate2D manuallyDerivedCenterCoordinate = [self.mapView convertPoint:self.mapView.center toCoordinateFromView:nil];
    XCTAssertEqualWithAccuracy(centerCoordinateBeforeReset.latitude, manuallyDerivedCenterCoordinate.latitude, 0.0000001, "@Map center latitude should be equal to manually derived latitude.");
    XCTAssertEqualWithAccuracy(centerCoordinateBeforeReset.longitude, manuallyDerivedCenterCoordinate.longitude, 0.0000001, @"Map center longitude should be equal to manually derived longitude.");

    self.mapView.direction = 0;
    XCTAssertEqualWithAccuracy(centerCoordinateBeforeReset.latitude, self.mapView.centerCoordinate.latitude, 0.0000001, "@Map center coordinate latitude should remain constant after resetting to north.");
    XCTAssertEqualWithAccuracy(centerCoordinateBeforeReset.longitude, self.mapView.centerCoordinate.longitude, 0.0000001, @"Map center coordinate longitude should remain constant after resetting to north.");
}

NS_INLINE CGFloat MGLScaleFromZoomLevel(double zoom) {
    return pow(2, zoom);
}

__unused NS_INLINE double MGLZoomLevelFromScale(CGFloat scale) {
    return log2(scale);
}

@end
