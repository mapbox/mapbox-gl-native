#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

static MGLMapView *mapView;

@interface MGLMapViewTests : XCTestCase

@end

@implementation MGLMapViewTests

+ (void)setUp {
    [super setUp];
    
    [MGLAccountManager setAccessToken:@"pk.feedcafedeadbeefbadebede"];
    mapView = [[MGLMapView alloc] initWithFrame:CGRectMake(0, 0, 64, 64) styleURL:MGLStyle.streetsStyleURL];
    [mapView setCenterCoordinate:CLLocationCoordinate2DMake(33, 179)];
}

- (void)testCoordinateBoundsConversion {
    MGLCoordinateBounds leftAntimeridianBounds = MGLCoordinateBoundsMake(CLLocationCoordinate2DMake(-75, 175), CLLocationCoordinate2DMake(75, 180));
    CGRect leftAntimeridianBoundsRect = [mapView convertCoordinateBounds:leftAntimeridianBounds toRectToView:mapView];
    
    MGLCoordinateBounds rightAntimeridianBounds = MGLCoordinateBoundsMake(CLLocationCoordinate2DMake(-75, -180), CLLocationCoordinate2DMake(75, -175));
    CGRect rightAntimeridianBoundsRect = [mapView convertCoordinateBounds:rightAntimeridianBounds toRectToView:mapView];
    
    MGLCoordinateBounds spanningBounds = MGLCoordinateBoundsMake(CLLocationCoordinate2DMake(24, 140), CLLocationCoordinate2DMake(44, 240));
    CGRect spanningBoundsRect = [mapView convertCoordinateBounds:spanningBounds toRectToView:mapView];
    
    // If the resulting CGRect from -convertCoordinateBounds:toRectToView:
    // intersects the set of bounds to the left and right of the
    // antimeridian, then we know that the CGRect spans across the antimeridian
    XCTAssertTrue(CGRectIntersectsRect(spanningBoundsRect, leftAntimeridianBoundsRect), @"Resulting ");
    XCTAssertTrue(CGRectIntersectsRect(spanningBoundsRect, rightAntimeridianBoundsRect), @"Something");
}

+ (void)tearDown {
    mapView = nil;
    [super tearDown];
}

@end
