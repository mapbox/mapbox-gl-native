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

@end
