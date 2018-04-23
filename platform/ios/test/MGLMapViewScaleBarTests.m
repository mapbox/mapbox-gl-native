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
    XCTAssertFalse(CGRectIsEmpty(scaleBar.frame));
    XCTAssertEqual(scaleBar.alpha, 0);

    self.mapView.zoomLevel = 15;
    XCTAssertGreaterThan(scaleBar.alpha, 0);
}

- (void)testDirectlySettingScaleBarViewHiddenProperty {
    UIView *scaleBar = self.mapView.scaleBar;

    scaleBar.hidden = NO;
    XCTAssertFalse(scaleBar.hidden);

    // Directly setting `.hidden` after the map has finished initializing will not update the scale bar.
    XCTAssertTrue(CGRectIsEmpty(scaleBar.frame));

    // ... but triggering any camera event will update it.
    self.mapView.zoomLevel = 1;
    XCTAssertFalse(CGRectIsEmpty(scaleBar.frame));
    XCTAssertEqual(scaleBar.alpha, 0);

    self.mapView.zoomLevel = 15;
    XCTAssertGreaterThan(scaleBar.alpha, 0);
}
@end
