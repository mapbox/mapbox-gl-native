#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

@interface MGLMapViewScaleBarTests : XCTestCase

@property (nonatomic) MGLMapView *mapView;

@end

@implementation MGLMapViewScaleBarTests

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

- (void)testShowsScale {
    UIView *scaleBar = self.mapView.scaleBar;

    // Scale bar should not be enabled by default.
    XCTAssertFalse(self.mapView.showsScale);
    XCTAssertTrue(scaleBar.hidden);

    self.mapView.showsScale = YES;

    XCTAssertFalse(scaleBar.hidden);

    // Scale bar should not be visible at default zoom (~z0), but it should be ready.
    // Size is not a measure of readiness here though.
    XCTAssertTrue(CGSizeEqualToSize(scaleBar.intrinsicContentSize, CGSizeZero));
    XCTAssertEqual(scaleBar.alpha, 0);

    self.mapView.zoomLevel = 15;
    [self.mapView layoutIfNeeded];
    XCTAssertGreaterThan(scaleBar.alpha, 0);
    XCTAssertFalse(CGSizeEqualToSize(scaleBar.intrinsicContentSize, CGSizeZero));
}

- (void)testDirectlySettingScaleBarViewHiddenProperty {
    UIView *scaleBar = self.mapView.scaleBar;

    scaleBar.hidden = NO;
    XCTAssertFalse(scaleBar.hidden);

    // Directly setting `.hidden` after the map has finished initializing will not update the scale bar.
    XCTAssertTrue(CGSizeEqualToSize(scaleBar.intrinsicContentSize, CGSizeZero));

    // ... but triggering any camera event will update it.
    self.mapView.zoomLevel = 1;
    [self.mapView layoutIfNeeded];
    
    XCTAssertTrue(CGSizeEqualToSize(scaleBar.intrinsicContentSize, CGSizeZero));
    XCTAssertEqual(scaleBar.alpha, 0);

    self.mapView.zoomLevel = 15;
    [self.mapView layoutIfNeeded];

    XCTAssertGreaterThan(scaleBar.alpha, 0);
    XCTAssertFalse(CGSizeEqualToSize(scaleBar.intrinsicContentSize, CGSizeZero));
}@end
