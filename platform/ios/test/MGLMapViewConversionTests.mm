#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>
#import "../../darwin/src/MGLGeometry_Private.h"


@interface MGLMapViewConversionTests : XCTestCase
@property (nonatomic) MGLMapView *mapView;
@end

@implementation MGLMapViewConversionTests

- (void)setUp {
    [super setUp];
    self.mapView = [[MGLMapView alloc] initWithFrame:UIScreen.mainScreen.bounds];
}

- (void)testConvertCoordinateBounds {
    // Test for https://github.com/mapbox/mapbox-gl-native/issues/11609

    CGSize viewSize = self.mapView.bounds.size;
    CGFloat epsilon = 0.00001;

    MGLCoordinateBounds testBounds[] = {
        {{ 35.68476,-220.24257 }, { 37.78428,-122.41310 }},   // Tokyo -> SF
        {{ -34.88593094075315, 123.04687499999999 }, { 65.94647177615738, 269.296875} },
        {{ -85.051128627918402,-212.45777702685075}, { 85.051128627917535,-10.058978406935694 }}
    };

    // CLLocationCoordinate2DIsValid states: a coordinate is invalid IF:
    // - Its latitude is greater than 90 degrees or less than -90 degrees.
    // - Its longitude is greater than 180 degrees or less than -180 degrees.
    //
    // So we use MGLLocationCoordinate2DIsValid in the conversion methods that
    // is a little more lenient.

    for (NSInteger test = 0; test < sizeof(testBounds)/sizeof(testBounds[0]); test++) {

        MGLCoordinateBounds bounds = testBounds[test];

#define ASSERT_REASON @"Unexpected result for test %ld",test

        // 0. Parameter check
        XCTAssert(MGLLocationCoordinate2DIsValid(bounds.sw), ASSERT_REASON);
        XCTAssert(MGLLocationCoordinate2DIsValid(bounds.ne), ASSERT_REASON);
        XCTAssert(bounds.sw.longitude < bounds.ne.longitude, ASSERT_REASON);
        XCTAssert(bounds.sw.latitude < bounds.ne.latitude, ASSERT_REASON);

        // Set visible bounds
        [self.mapView setVisibleCoordinateBounds:bounds animated:NO];
        MGLCoordinateBounds visibleBounds = self.mapView.visibleCoordinateBounds;


        // 1. Check the visible bounds
        // Given shape of device, visible bounds should be larger (extending latitude)
        // than the source.
        XCTAssert(visibleBounds.ne.latitude >= bounds.ne.latitude-epsilon, ASSERT_REASON);
        XCTAssert(visibleBounds.sw.latitude <= bounds.sw.latitude+epsilon, ASSERT_REASON);
        XCTAssertEqualWithAccuracy(visibleBounds.ne.longitude, visibleBounds.ne.longitude, 0.00001, ASSERT_REASON);
        XCTAssertEqualWithAccuracy(visibleBounds.sw.longitude, visibleBounds.sw.longitude, 0.00001, ASSERT_REASON);

        // 2. Check the individual points
        CGPoint west = [self.mapView convertCoordinate:bounds.sw toPointToView:self.mapView];
        XCTAssertEqualWithAccuracy(west.x, 0.0, epsilon, ASSERT_REASON);
        XCTAssert(west.y > -epsilon, ASSERT_REASON);
        XCTAssert(west.y < viewSize.height+epsilon, ASSERT_REASON);

        CGPoint east = [self.mapView convertCoordinate:bounds.ne toPointToView:self.mapView];
        XCTAssertEqualWithAccuracy(east.x, viewSize.width, epsilon, ASSERT_REASON);
        XCTAssert(east.y > -epsilon, ASSERT_REASON);
        XCTAssert(east.y < viewSize.height+epsilon, ASSERT_REASON);

        // 3. Now check the bounds
        CGRect rect = [self.mapView convertCoordinateBounds:bounds
                                               toRectToView:self.mapView];

        XCTAssertFalse(CGRectEqualToRect(rect, CGRectNull), ASSERT_REASON);

        // Check individual properties
        XCTAssertFalse(rect.origin.x == INFINITY, ASSERT_REASON);
        XCTAssertFalse(rect.origin.y == INFINITY, ASSERT_REASON);
        XCTAssertFalse(rect.size.width == 0, ASSERT_REASON);
        XCTAssertFalse(rect.size.height == 0, ASSERT_REASON);

        // Is the rect returned, roughly where we'd expect it? (left and right edges
        // aligned with the view's)
        XCTAssertEqualWithAccuracy(rect.origin.x, 0.0, epsilon, ASSERT_REASON);
        XCTAssert(rect.origin.y > -epsilon, ASSERT_REASON);
        XCTAssert(rect.origin.y < viewSize.height+epsilon, ASSERT_REASON);

        XCTAssertEqualWithAccuracy(rect.origin.x + rect.size.width, viewSize.width, epsilon, ASSERT_REASON);
        XCTAssert(rect.size.height > -epsilon, ASSERT_REASON);
        XCTAssert(rect.size.height < viewSize.height+epsilon, ASSERT_REASON);

#undef ASSERT_REASON
    }
}

@end
