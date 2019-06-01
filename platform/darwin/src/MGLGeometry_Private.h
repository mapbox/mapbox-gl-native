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

#if TARGET_OS_IPHONE
NS_INLINE NSString *MGLStringFromSize(CGSize size) {
    return NSStringFromCGSize(size);
}
#else
NS_INLINE NSString *MGLStringFromSize(NSSize size) {
    return NSStringFromSize(size);
}
#endif

NS_INLINE NSString *MGLStringFromCLLocationCoordinate2D(CLLocationCoordinate2D coordinate) {
    return [NSString stringWithFormat:@"(lat: %f, lon: %f)", coordinate.latitude, coordinate.longitude];
}

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

/**
 YES if the coordinate is valid or NO if it is not.
 Considers extended coordinates.
 */
NS_INLINE BOOL MGLLocationCoordinate2DIsValid(CLLocationCoordinate2D coordinate) {
    return (coordinate.latitude  <= 90.0  &&
            coordinate.latitude  >= -90.0  &&
            coordinate.longitude <= 360.0 &&
            coordinate.longitude >= -360.0);
}

#if TARGET_OS_IPHONE
    #define MGLEdgeInsets UIEdgeInsets
    #define MGLEdgeInsetsMake UIEdgeInsetsMake
#else
    #define MGLEdgeInsets NSEdgeInsets
    #define MGLEdgeInsetsMake NSEdgeInsetsMake
#endif

NS_INLINE mbgl::EdgeInsets MGLEdgeInsetsFromNSEdgeInsets(MGLEdgeInsets insets) {
    return { insets.top, insets.left, insets.bottom, insets.right };
}

/// Returns the combination of two edge insets.
NS_INLINE MGLEdgeInsets MGLEdgeInsetsInsetEdgeInset(MGLEdgeInsets base, MGLEdgeInsets inset) {
    return MGLEdgeInsetsMake(base.top + inset.top,
                             base.left + inset.left,
                             base.bottom + inset.bottom,
                             base.right + inset.right);
}

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

/**
 Returns a point with coordinates rounded to the nearest logical pixel.
 */
CGPoint MGLPointRounded(CGPoint point);

MGLMatrix4 MGLMatrix4Make(std::array<double, 16> mat);
