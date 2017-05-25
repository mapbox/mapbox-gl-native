#import <Mapbox/Mapbox.h>
#import <XCTest/XCTest.h>

#import "../../darwin/src/MGLGeometry_Private.h"

@interface MGLGeometryTests : XCTestCase
@end

@implementation MGLGeometryTests

- (void)testCoordinateBoundsIsEmpty {
    MGLCoordinateBounds emptyBounds = MGLCoordinateBoundsMake(CLLocationCoordinate2DMake(0, 0), CLLocationCoordinate2DMake(10, 0));
    XCTAssertTrue(MGLCoordinateBoundsIsEmpty(emptyBounds));
    XCTAssertFalse(MGLCoordinateSpanEqualToCoordinateSpan(MGLCoordinateSpanZero, MGLCoordinateBoundsGetCoordinateSpan(emptyBounds)));
}

- (void)testAngleConversions {
    XCTAssertEqualWithAccuracy(-180, MGLDegreesFromRadians(-M_PI), 1e-5);
    XCTAssertEqual(0, MGLDegreesFromRadians(0));
    XCTAssertEqualWithAccuracy(45, MGLDegreesFromRadians(M_PI_4), 1e-5);
    XCTAssertEqualWithAccuracy(90, MGLDegreesFromRadians(M_PI_2), 1e-5);
    XCTAssertEqualWithAccuracy(180, MGLDegreesFromRadians(M_PI), 1e-5);
    XCTAssertEqualWithAccuracy(360, MGLDegreesFromRadians(2 * M_PI), 1e-5);
    XCTAssertEqualWithAccuracy(720, MGLDegreesFromRadians(4 * M_PI), 1e-5);
    
    XCTAssertEqualWithAccuracy(-360, MGLDegreesFromRadians(MGLRadiansFromDegrees(-360)), 1e-4);
    XCTAssertEqualWithAccuracy(-180, MGLDegreesFromRadians(MGLRadiansFromDegrees(-180)), 1e-5);
    XCTAssertEqualWithAccuracy(-90, MGLDegreesFromRadians(MGLRadiansFromDegrees(-90)), 1e-5);
    XCTAssertEqualWithAccuracy(-45, MGLDegreesFromRadians(MGLRadiansFromDegrees(-45)), 1e-5);
    XCTAssertEqualWithAccuracy(0, MGLDegreesFromRadians(MGLRadiansFromDegrees(0)), 1e-5);
    XCTAssertEqualWithAccuracy(45, MGLDegreesFromRadians(MGLRadiansFromDegrees(45)), 1e-5);
    XCTAssertEqualWithAccuracy(90, MGLDegreesFromRadians(MGLRadiansFromDegrees(90)), 1e-5);
    XCTAssertEqualWithAccuracy(180, MGLDegreesFromRadians(MGLRadiansFromDegrees(180)), 1e-5);
    XCTAssertEqualWithAccuracy(360, MGLDegreesFromRadians(MGLRadiansFromDegrees(360)), 1e-4);
}

- (void)testAltitudeConversions {
    CGSize tallSize = CGSizeMake(600, 1200);
    CGSize midSize = CGSizeMake(600, 800);
    CGSize shortSize = CGSizeMake(600, 400);
    
    XCTAssertEqualWithAccuracy(1800, MGLAltitudeForZoomLevel(MGLZoomLevelForAltitude(1800, 0, 0, midSize), 0, 0, midSize), 1e-8);
    XCTAssertLessThan(MGLZoomLevelForAltitude(1800, 0, 0, midSize), MGLZoomLevelForAltitude(1800, 0, 0, tallSize));
    XCTAssertGreaterThan(MGLZoomLevelForAltitude(1800, 0, 0, midSize), MGLZoomLevelForAltitude(1800, 0, 0, shortSize));
    
    XCTAssertEqualWithAccuracy(0, MGLZoomLevelForAltitude(MGLAltitudeForZoomLevel(0, 0, 0, midSize), 0, 0, midSize), 1e-8);
    XCTAssertEqualWithAccuracy(18, MGLZoomLevelForAltitude(MGLAltitudeForZoomLevel(18, 0, 0, midSize), 0, 0, midSize), 1e-8);
    
    XCTAssertEqualWithAccuracy(0, MGLZoomLevelForAltitude(MGLAltitudeForZoomLevel(0, 0, 40, midSize), 0, 40, midSize), 1e-8);
    XCTAssertEqualWithAccuracy(18, MGLZoomLevelForAltitude(MGLAltitudeForZoomLevel(18, 0, 40, midSize), 0, 40, midSize), 1e-8);
    
    XCTAssertEqualWithAccuracy(0, MGLZoomLevelForAltitude(MGLAltitudeForZoomLevel(0, 60, 40, midSize), 60, 40, midSize), 1e-8);
    XCTAssertEqualWithAccuracy(18, MGLZoomLevelForAltitude(MGLAltitudeForZoomLevel(18, 60, 40, midSize), 60, 40, midSize), 1e-8);
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

- (void)testCoordinateInCoordinateBounds {
    CLLocationCoordinate2D ne = CLLocationCoordinate2DMake(45, -104);
    CLLocationCoordinate2D sw = CLLocationCoordinate2DMake(41, -111);
    MGLCoordinateBounds wyoming = MGLCoordinateBoundsMake(sw, ne);

    CLLocationCoordinate2D centerOfWyoming = CLLocationCoordinate2DMake(43, -107.5);

    XCTAssertTrue(MGLCoordinateInCoordinateBounds(ne, wyoming));
    XCTAssertTrue(MGLCoordinateInCoordinateBounds(sw, wyoming));
    XCTAssertTrue(MGLCoordinateInCoordinateBounds(centerOfWyoming, wyoming));

    CLLocationCoordinate2D australia = CLLocationCoordinate2DMake(-25, 135);
    CLLocationCoordinate2D brazil = CLLocationCoordinate2DMake(-12, -50);
    CLLocationCoordinate2D china = CLLocationCoordinate2DMake(35, 100);

    XCTAssertFalse(MGLCoordinateInCoordinateBounds(australia, wyoming));
    XCTAssertFalse(MGLCoordinateInCoordinateBounds(brazil, wyoming));
    XCTAssertFalse(MGLCoordinateInCoordinateBounds(china, wyoming));
    XCTAssertFalse(MGLCoordinateInCoordinateBounds(kCLLocationCoordinate2DInvalid, wyoming));
}

- (void)testGeoJSONDeserialization {
    NSData *data = [@"{\"type\": \"Feature\", \"geometry\": {\"type\": \"Point\", \"coordinates\": [0, 0]}, \"properties\": {}}" dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error;
    MGLPointFeature *feature = (MGLPointFeature *)[MGLShape shapeWithData:data encoding:NSUTF8StringEncoding error:&error];
    XCTAssertNil(error, @"Valid GeoJSON data should produce no error on deserialization.");
    XCTAssertNotNil(feature, @"Valid GeoJSON data should produce an object on deserialization.");
    XCTAssertTrue([feature isKindOfClass:[MGLPointFeature class]], @"Valid GeoJSON point feature data should produce an MGLPointFeature.");
    XCTAssertEqual(feature.attributes.count, 0);
    XCTAssertEqual(feature.coordinate.latitude, 0);
    XCTAssertEqual(feature.coordinate.longitude, 0);

    data = [@"{\"type\": \"Feature\", \"feature\": {\"type\": \"Point\", \"coordinates\": [0, 0]}}" dataUsingEncoding:NSUTF8StringEncoding];
    error = nil;
    MGLShape *shape = [MGLShape shapeWithData:data encoding:NSUTF8StringEncoding error:&error];
    XCTAssertNotNil(error, @"Invalid GeoJSON data should produce an error on deserialization.");
    XCTAssertNil(shape, @"Invalid GeoJSON data should produce no object on deserialization.");
}

- (void)testGeoJSONSerialization {
    MGLPointFeature *feature = [[MGLPointFeature alloc] init];
    feature.identifier = @504;
    feature.coordinate = CLLocationCoordinate2DMake(29.95, -90.066667);

    NSData *data = [feature geoJSONDataUsingEncoding:NSUTF8StringEncoding];
    XCTAssertNotNil(data, @"MGLPointFeature should serialize as an UTF-8 string data object.");
    NSError *error;
    NSDictionary *serializedGeoJSON = [NSJSONSerialization JSONObjectWithData:data options:0 error:&error];
    XCTAssertNil(error, @"Serialized GeoJSON data should be deserializable JSON.");
    XCTAssertNotNil(serializedGeoJSON, @"Serialized GeoJSON data should be valid JSON.");
    XCTAssertTrue([serializedGeoJSON isKindOfClass:[NSDictionary class]], @"Serialized GeoJSON data should be a JSON object.");
    NSDictionary *geoJSON = @{
        @"type": @"Feature",
        @"id": @504,
        @"geometry": @{
            @"type": @"Point",
            @"coordinates": @[
                @(-90.066667),
                @29.95,
            ],
        },
        @"properties": @{},
    };
    XCTAssertEqualObjects(serializedGeoJSON, geoJSON, @"MGLPointFeature should serialize as a GeoJSON point feature.");
}

- (void)testMGLCoordinateBoundsToMGLCoordinateQuad {
    MGLCoordinateBounds bounds = MGLCoordinateBoundsMake(CLLocationCoordinate2DMake(37.936, -80.425),
                                                         CLLocationCoordinate2DMake(46.437, -71.516));

    MGLCoordinateQuad quad = MGLCoordinateQuadFromCoordinateBounds(bounds);
    XCTAssertEqualObjects([NSValue valueWithMGLCoordinate:bounds.sw],
                          [NSValue valueWithMGLCoordinate:quad.bottomLeft],
                          @"Bounds southwest should be bottom left of quad.");
    XCTAssertEqualObjects([NSValue valueWithMGLCoordinate:bounds.ne],
                          [NSValue valueWithMGLCoordinate:quad.topRight],
                          @"Bounds northeast should be top right of quad.");

    XCTAssertEqualObjects([NSValue valueWithMGLCoordinate:CLLocationCoordinate2DMake(46.437, -80.425)],
                          [NSValue valueWithMGLCoordinate:quad.topLeft],
                          @"Quad top left should be computed correctly.");
    XCTAssertEqualObjects([NSValue valueWithMGLCoordinate:CLLocationCoordinate2DMake(37.936, -71.516)],
                          [NSValue valueWithMGLCoordinate:quad.bottomRight],
                          @"Quad bottom right should be computed correctly.");
}
@end
