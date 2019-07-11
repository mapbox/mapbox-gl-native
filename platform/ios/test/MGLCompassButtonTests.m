#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

#import "../../ios/src/MGLCompassButton_Private.h"

@interface MGLMapView (MGLCompassButtonTests)

- (void)resetNorthAnimated:(BOOL)animated;

@end

@interface MGLCompassButtonTests : XCTestCase

@property (nonatomic) MGLMapView *mapView;

@end

@implementation MGLCompassButtonTests

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

- (void)testCompassButton {
    XCTAssertNotNil(self.mapView.compassView);
    XCTAssertTrue(self.mapView.compassView.userInteractionEnabled);
    XCTAssertEqual(self.mapView.compassView.gestureRecognizers.count, 1);
    XCTAssertEqual(self.mapView.compassView.accessibilityTraits, UIAccessibilityTraitButton);
    XCTAssertNotNil(self.mapView.compassView.accessibilityLabel);
    XCTAssertNotNil(self.mapView.compassView.accessibilityHint);
}

- (void)testVisibilityAdaptive {
    XCTAssertEqual(self.mapView.compassView.compassVisibility, MGLOrnamentVisibilityAdaptive, @"Adaptive should be the default visibility setting.");
    XCTAssertEqual(self.mapView.compassView.alpha, 0, @"Compass should not initially be visible.");

    self.mapView.zoomLevel = 15.f;
    [self.mapView setDirection:45.f animated:false];
    XCTAssertEqualWithAccuracy(self.mapView.direction, 45, 0.001);
    XCTAssertEqual(self.mapView.compassView.alpha, 1, @"Compass should become visible when direction changes.");

    [self.mapView resetNorthAnimated:NO];
    XCTAssertEqual(self.mapView.direction, 0);
    XCTAssertEqual(self.mapView.compassView.alpha, 0, @"Compass should hide itself when direction is north.");
}

- (void)testVisibilityHidden {
    self.mapView.compassView.compassVisibility = MGLOrnamentVisibilityHidden;
    XCTAssertEqual(self.mapView.compassView.compassVisibility, MGLOrnamentVisibilityHidden);
    XCTAssertEqual(self.mapView.compassView.alpha, 0, @"Compass should not initially be visible.");

    self.mapView.zoomLevel = 15.f;
    [self.mapView setDirection:45.f animated:false];
    XCTAssertEqualWithAccuracy(self.mapView.direction, 45, 0.001);
    XCTAssertEqual(self.mapView.compassView.alpha, 0, @"Compass should stay hidden when direction changes.");
}

- (void)testVisibilityVisible {
    self.mapView.compassView.compassVisibility = MGLOrnamentVisibilityVisible;
    XCTAssertEqual(self.mapView.compassView.compassVisibility, MGLOrnamentVisibilityVisible);
    XCTAssertEqual(self.mapView.compassView.alpha, 1, @"Compass should initially be visible.");

    self.mapView.zoomLevel = 15.f;
    [self.mapView setDirection:45.f animated:false];
    XCTAssertEqualWithAccuracy(self.mapView.direction, 45, 0.001);
    XCTAssertEqual(self.mapView.compassView.alpha, 1, @"Compass should continue to be visible when direction changes.");

    [self.mapView resetNorthAnimated:NO];
    XCTAssertEqual(self.mapView.direction, 0);
    XCTAssertEqual(self.mapView.compassView.alpha, 1, @"Compass should continue to be visible when direction is north.");
}

@end
