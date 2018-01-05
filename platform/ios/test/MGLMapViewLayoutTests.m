#import <XCTest/XCTest.h>
#import "MGLMapView.h"
#import "MGLMapViewDelegate.h"
#import "MGLAccountManager.h"


@interface MGLMapViewLayoutTests : XCTestCase <MGLMapViewDelegate>

@property (nonatomic) MGLMapView *mapView;

@end

@implementation MGLMapViewLayoutTests {
    XCTestExpectation *_styleLoadingExpectation;
}

- (void)setUp {
    [super setUp];

    [MGLAccountManager setAccessToken:@"pk.feedcafedeadbeefbadebede"];
    NSURL *styleURL = [[NSBundle bundleForClass:[self class]] URLForResource:@"one-liner" withExtension:@"json"];
    self.mapView = [[MGLMapView alloc] initWithFrame:UIScreen.mainScreen.bounds styleURL:styleURL];
    self.mapView.delegate = self;
    if (!self.mapView.style) {
        _styleLoadingExpectation = [self expectationWithDescription:@"Map view should finish loading style."];
        [self waitForExpectationsWithTimeout:1 handler:nil];
    }
}

- (void)mapView:(MGLMapView *)mapView didFinishLoadingStyle:(MGLStyle *)style {
    XCTAssertNotNil(mapView.style);
    XCTAssertEqual(mapView.style, style);

    [_styleLoadingExpectation fulfill];
}

- (void)tearDown {
    _styleLoadingExpectation = nil;
    self.mapView = nil;

    [super tearDown];
}

- (void)testOrnamentPlacement {

    // TODO: check constraining to superview vs. constraining to safeAreaLayoutGuide.bottomAnchor
    // what about iOS 8??

    [self.mapView setZoomLevel:4.5 animated:NO];
    [self.mapView.camera setHeading:12.0];
    
    [self.mapView setNeedsLayout];
    [self.mapView layoutIfNeeded];

    //compass
    NSLog(@"================> %@", self.mapView.compassView);

    //scale bar
    NSLog(@"================> %@", self.mapView.scaleBar);

    //info button
    NSLog(@"================> %@", self.mapView.attributionButton);

    //mapbox logo
    NSLog(@"================> %@", self.mapView.logoView);
}

@end
