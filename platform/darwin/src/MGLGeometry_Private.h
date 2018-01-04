#import "MGLGeometry.h"

#import <TargetConditionals.h>
#if TARGET_OS_IPHONE
    #import <UIKit/UIKit.h>
#endif

#import <mbgl/util/geo.hpp>
#import <mbgl/util/geometry.hpp>

#import <array>
typedef double MGLLocationRadians;
typedef double MGLRadianDistance;
typedef double MGLRadianDirection;

/** Defines the coordinate by a `MGLRadianCoordinate2D`. */
typedef struct MGLRadianCoordinate2D {
    MGLLocationRadians latitude;
    MGLLocationRadians longitude;
} MGLRadianCoordinate2D;

/**
 Creates a new `MGLRadianCoordinate2D` from the given latitudinal and longitudinal.
 */
NS_INLINE MGLRadianCoordinate2D MGLRadianCoordinate2DMake(MGLLocationRadians latitude, MGLLocationRadians longitude) {
    MGLRadianCoordinate2D radianCoordinate;
    radianCoordinate.latitude = latitude;
    radianCoordinate.longitude = longitude;
    return radianCoordinate;
}

/// Returns the smallest rectangle that contains both the given rectangle and
/// the given point.
CGRect MGLExtendRect(CGRect rect, CGPoint point);

mbgl::LatLng MGLLatLngFromLocationCoordinate2D(CLLocationCoordinate2D coordinate);

NS_INLINE mbgl::Point<double> MGLPointFromLocationCoordinate2D(CLLocationCoordinate2D coordinate) {
    return mbgl::Point<double>(coordinate.longitude, coordinate.latitude);
}

NS_INLINE CLLocationCoordinate2D MGLLocationCoordinate2DFromPoint(mbgl::Point<double> point) {
    return CLLocationCoordinate2DMake(point.y, point.x);
}

NS_INLINE CLLocationCoordinate2D MGLLocationCoordinate2DFromLatLng(mbgl::LatLng latLng) {
    return CLLocationCoordinate2DMake(latLng.latitude(), latLng.longitude());
}

NS_INLINE MGLCoordinateBounds MGLCoordinateBoundsFromLatLngBounds(mbgl::LatLngBounds latLngBounds) {
    return MGLCoordinateBoundsMake(MGLLocationCoordinate2DFromLatLng(latLngBounds.southwest()),
                                   MGLLocationCoordinate2DFromLatLng(latLngBounds.northeast()));
}

NS_INLINE mbgl::LatLngBounds MGLLatLngBoundsFromCoordinateBounds(MGLCoordinateBounds coordinateBounds) {
    return mbgl::LatLngBounds::hull(MGLLatLngFromLocationCoordinate2D(coordinateBounds.sw),
                                    MGLLatLngFromLocationCoordinate2D(coordinateBounds.ne));
}

NS_INLINE std::array<mbgl::LatLng, 4> MGLLatLngArrayFromCoordinateQuad(MGLCoordinateQuad quad) {
    return { MGLLatLngFromLocationCoordinate2D(quad.topLeft),
    MGLLatLngFromLocationCoordinate2D(quad.topRight),
    MGLLatLngFromLocationCoordinate2D(quad.bottomRight),
    MGLLatLngFromLocationCoordinate2D(quad.bottomLeft) };
}

NS_INLINE MGLCoordinateQuad MGLCoordinateQuadFromLatLngArray(std::array<mbgl::LatLng, 4> quad) {
    return { MGLLocationCoordinate2DFromLatLng(quad[0]),
    MGLLocationCoordinate2DFromLatLng(quad[3]),
    MGLLocationCoordinate2DFromLatLng(quad[2]),
    MGLLocationCoordinate2DFromLatLng(quad[1]) };
}

#if TARGET_OS_IPHONE
NS_INLINE mbgl::EdgeInsets MGLEdgeInsetsFromNSEdgeInsets(UIEdgeInsets insets) {
    return { insets.top, insets.left, insets.bottom, insets.right };
}
#else
NS_INLINE mbgl::EdgeInsets MGLEdgeInsetsFromNSEdgeInsets(NSEdgeInsets insets) {
    return { insets.top, insets.left, insets.bottom, insets.right };
}
#endif

/** Converts a map zoom level to a camera altitude.

    @param zoomLevel The zoom level to convert.
    @param pitch The camera pitch, measured in degrees.
    @param latitude The latitude of the point at the center of the viewport.
    @param size The size of the viewport.
    @return An altitude measured in meters. */
CLLocationDistance MGLAltitudeForZoomLevel(double zoomLevel, CGFloat pitch, CLLocationDegrees latitude, CGSize size);

/** Converts a camera altitude to a map zoom level.

    @param altitude The altitude to convert, measured in meters.
    @param pitch The camera pitch, measured in degrees.
    @param latitude The latitude of the point at the center of the viewport.
    @param size The size of the viewport.
    @return A zero-based zoom level. */
double MGLZoomLevelForAltitude(CLLocationDistance altitude, CGFloat pitch, CLLocationDegrees latitude, CGSize size);

/** Returns MGLRadianCoordinate2D, converted from CLLocationCoordinate2D. */
NS_INLINE MGLRadianCoordinate2D MGLRadianCoordinateFromLocationCoordinate(CLLocationCoordinate2D locationCoordinate) {
    return MGLRadianCoordinate2DMake(MGLRadiansFromDegrees(locationCoordinate.latitude),
                                     MGLRadiansFromDegrees(locationCoordinate.longitude));
}

/**
 Returns the distance in radians given two coordinates.
 */
MGLRadianDistance MGLDistanceBetweenRadianCoordinates(MGLRadianCoordinate2D from, MGLRadianCoordinate2D to);

/**
 Returns direction in radians given two coordinates.
 */
MGLRadianDirection MGLRadianCoordinatesDirection(MGLRadianCoordinate2D from, MGLRadianCoordinate2D to);

/**
 Returns a coordinate at a given distance and direction away from coordinate.
 */
MGLRadianCoordinate2D MGLRadianCoordinateAtDistanceFacingDirection(MGLRadianCoordinate2D coordinate,
                                                                   MGLRadianDistance distance,
                                                                   MGLRadianDirection direction);

/**
 Returns the direction from one coordinate to another.
 */
CLLocationDirection MGLDirectionBetweenCoordinates(CLLocationCoordinate2D firstCoordinate, CLLocationCoordinate2D secondCoordinate);

CGPoint MGLPointRounded(CGPoint point);
