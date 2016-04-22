#import "../../darwin/src/MGLGeometry_Private.h"

#import <XCTest/XCTest.h>

@interface MGLGeometryTests : XCTestCase
@end

@implementation MGLGeometryTests

- (void)testCoordinateBoundsIsEmpty {
    MGLCoordinateBounds emptyBounds = MGLCoordinateBoundsMake(CLLocationCoordinate2DMake(0, 0), CLLocationCoordinate2DMake(10, 0));
    XCTAssertTrue(MGLCoordinateBoundsIsEmpty(emptyBounds));
    XCTAssertFalse(MGLCoordinateSpanEqualToCoordinateSpan(MGLCoordinateSpanZero, MGLCoordinateBoundsGetCoordinateSpan(emptyBounds)));
}

- (void)testAngleConversions {
    XCTAssertEqualWithAccuracy(-180, MGLDegreesFromRadians(-M_PI), 5);
    XCTAssertEqual(0, MGLDegreesFromRadians(0));
    XCTAssertEqualWithAccuracy(45, MGLDegreesFromRadians(M_PI_4), 5);
    XCTAssertEqualWithAccuracy(90, MGLDegreesFromRadians(M_PI_2), 5);
    XCTAssertEqualWithAccuracy(180, MGLDegreesFromRadians(M_PI), 5);
    XCTAssertEqualWithAccuracy(360, MGLDegreesFromRadians(2 * M_PI), 5);
    XCTAssertEqualWithAccuracy(720, MGLDegreesFromRadians(4 * M_PI), 5);
    
    XCTAssertEqualWithAccuracy(-360, MGLDegreesFromRadians(MGLRadiansFromDegrees(-360)), 4);
    XCTAssertEqualWithAccuracy(-180, MGLDegreesFromRadians(MGLRadiansFromDegrees(-180)), 5);
    XCTAssertEqualWithAccuracy(-90, MGLDegreesFromRadians(MGLRadiansFromDegrees(-90)), 5);
    XCTAssertEqualWithAccuracy(-45, MGLDegreesFromRadians(MGLRadiansFromDegrees(-45)), 5);
    XCTAssertEqualWithAccuracy(0, MGLDegreesFromRadians(MGLRadiansFromDegrees(0)), 5);
    XCTAssertEqualWithAccuracy(45, MGLDegreesFromRadians(MGLRadiansFromDegrees(45)), 5);
    XCTAssertEqualWithAccuracy(90, MGLDegreesFromRadians(MGLRadiansFromDegrees(90)), 5);
    XCTAssertEqualWithAccuracy(180, MGLDegreesFromRadians(MGLRadiansFromDegrees(180)), 5);
    XCTAssertEqualWithAccuracy(360, MGLDegreesFromRadians(MGLRadiansFromDegrees(360)), 4);
}

- (void)testAltitudeConversions {
    CGSize tallSize = CGSizeMake(600, 1200);
    CGSize midSize = CGSizeMake(600, 800);
    CGSize shortSize = CGSizeMake(600, 400);
    
    XCTAssertEqualWithAccuracy(1800, MGLAltitudeForZoomLevel(MGLZoomLevelForAltitude(1800, 0, 0, midSize), 0, 0, midSize), 1);
    XCTAssertLessThan(MGLZoomLevelForAltitude(1800, 0, 0, midSize), MGLZoomLevelForAltitude(1800, 0, 0, tallSize));
    XCTAssertGreaterThan(MGLZoomLevelForAltitude(1800, 0, 0, midSize), MGLZoomLevelForAltitude(1800, 0, 0, shortSize));
    
    XCTAssertEqualWithAccuracy(0, MGLZoomLevelForAltitude(MGLAltitudeForZoomLevel(0, 0, 0, midSize), 0, 0, midSize), 3);
    XCTAssertEqualWithAccuracy(18, MGLZoomLevelForAltitude(MGLAltitudeForZoomLevel(18, 0, 0, midSize), 0, 0, midSize), 3);
    
    XCTAssertEqualWithAccuracy(0, MGLZoomLevelForAltitude(MGLAltitudeForZoomLevel(0, 0, 40, midSize), 0, 40, midSize), 3);
    XCTAssertEqualWithAccuracy(18, MGLZoomLevelForAltitude(MGLAltitudeForZoomLevel(18, 0, 40, midSize), 0, 40, midSize), 3);
    
    XCTAssertEqualWithAccuracy(0, MGLZoomLevelForAltitude(MGLAltitudeForZoomLevel(0, 60, 40, midSize), 60, 40, midSize), 3);
    XCTAssertEqualWithAccuracy(18, MGLZoomLevelForAltitude(MGLAltitudeForZoomLevel(18, 60, 40, midSize), 60, 40, midSize), 3);
}

- (void)testGeometryBoxing {
    CLLocationCoordinate2D coordinate = CLLocationCoordinate2DMake(38.9131982, -77.0325453144239);
    CLLocationCoordinate2D roundTrippedCoordinate = [NSValue valueWithMGLCoordinate:coordinate].MGLCoordinateValue;
    
    XCTAssertEqual(coordinate.latitude, roundTrippedCoordinate.latitude, @"Latitude should round-trip.");
    XCTAssertEqual(coordinate.longitude, roundTrippedCoordinate.longitude, @"Longitude should round-trip.");
    
    MGLCoordinateSpan span = MGLCoordinateSpanMake(4.383333333333335, -4.299999999999997);
    MGLCoordinateSpan roundTrippedSpan = [NSValue valueWithMGLCoordinateSpan:span].MGLCoordinateSpanValue;
    
    XCTAssertEqual(span.latitudeDelta, roundTrippedSpan.latitudeDelta, @"Latitude delta should round-trip.");
    XCTAssertEqual(span.longitudeDelta, roundTrippedSpan.longitudeDelta, @"Longitude delta should round-trip.");
    
    MGLCoordinateBounds bounds = MGLCoordinateBoundsMake(CLLocationCoordinate2DMake(38.9131982, -77.0325453144239),
                                                         CLLocationCoordinate2DMake(37.7757368, -122.4135302));
    MGLCoordinateBounds roundTrippedBounds = [NSValue valueWithMGLCoordinateBounds:bounds].MGLCoordinateBoundsValue;
    
    XCTAssertEqualObjects([NSValue valueWithMGLCoordinate:bounds.sw],
                          [NSValue valueWithMGLCoordinate:roundTrippedBounds.sw],
                          @"Southwest should round-trip.");
    XCTAssertEqualObjects([NSValue valueWithMGLCoordinate:bounds.ne],
                          [NSValue valueWithMGLCoordinate:roundTrippedBounds.ne],
                          @"Northeast should round-trip.");
}

@end
