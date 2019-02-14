#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
#import <CoreGraphics/CoreGraphics.h>

#import "MGLFoundation.h"

NS_ASSUME_NONNULL_BEGIN

/** Defines the area spanned by an `MGLCoordinateBounds`. */
typedef struct __attribute__((objc_boxable)) MGLCoordinateSpan {
    /** Latitudes spanned by an `MGLCoordinateBounds`. */
    CLLocationDegrees latitudeDelta;
    /** Longitudes spanned by an `MGLCoordinateBounds`. */
    CLLocationDegrees longitudeDelta;
} MGLCoordinateSpan;

/* Defines a point on the map in Mercator projection for a specific zoom level. */
typedef struct __attribute__((objc_boxable)) MGLMapPoint {
    /** X coordinate representing a longitude in Mercator projection. */
    CGFloat x;
    /** Y coordinate representing  a latitide in Mercator projection. */
    CGFloat y;
    /** Zoom level at which the X and Y coordinates are valid. */
    CGFloat zoomLevel;
} MGLMapPoint;

/* Defines a 4x4 matrix. */
typedef struct MGLMatrix4 {
    double m00, m01, m02, m03;
    double m10, m11, m12, m13;
    double m20, m21, m22, m23;
    double m30, m31, m32, m33;
} MGLMatrix4;

/**
 Creates a new `MGLCoordinateSpan` from the given latitudinal and longitudinal
 deltas.
 */
NS_INLINE MGLCoordinateSpan MGLCoordinateSpanMake(CLLocationDegrees latitudeDelta, CLLocationDegrees longitudeDelta) {
    MGLCoordinateSpan span;
    span.latitudeDelta = latitudeDelta;
    span.longitudeDelta = longitudeDelta;
    return span;
}

/**
 Creates a new `MGLMapPoint` from the given X and Y coordinates, and zoom level.
 */
NS_INLINE MGLMapPoint MGLMapPointMake(CGFloat x, CGFloat y, CGFloat zoomLevel) {
    MGLMapPoint point;
    point.x = x;
    point.y = y;
    point.zoomLevel = zoomLevel;
    return point;
}

/**
 Returns `YES` if the two coordinate spans represent the same latitudinal change
 and the same longitudinal change.
 */
NS_INLINE BOOL MGLCoordinateSpanEqualToCoordinateSpan(MGLCoordinateSpan span1, MGLCoordinateSpan span2) {
    return (span1.latitudeDelta == span2.latitudeDelta &&
            span1.longitudeDelta == span2.longitudeDelta);
}

/** An area of zero width and zero height. */
FOUNDATION_EXTERN MGL_EXPORT const MGLCoordinateSpan MGLCoordinateSpanZero;

/** A rectangular area as measured on a two-dimensional map projection. */
typedef struct __attribute__((objc_boxable)) MGLCoordinateBounds {
    /** Coordinate at the southwest corner. */
    CLLocationCoordinate2D sw;
    /** Coordinate at the northeast corner. */
    CLLocationCoordinate2D ne;
} MGLCoordinateBounds;

/** 
 A quadrilateral area as measured on a two-dimensional map projection.
 `MGLCoordinateQuad` differs from `MGLCoordinateBounds` in that it allows
 representation of non-axis aligned bounds and non-rectangular quadrilaterals.
 The coordinates are described in counter clockwise order from top left.
 */
typedef struct MGLCoordinateQuad {
    /** Coordinate at the top left corner. */
    CLLocationCoordinate2D topLeft;
    /** Coordinate at the bottom left corner. */
    CLLocationCoordinate2D bottomLeft;
    /** Coordinate at the bottom right corner. */
    CLLocationCoordinate2D bottomRight;
    /** Coordinate at the top right corner. */
    CLLocationCoordinate2D topRight;
} MGLCoordinateQuad;


/**
 Creates a new `MGLCoordinateBounds` structure from the given southwest and
 northeast coordinates.
 */
NS_INLINE MGLCoordinateBounds MGLCoordinateBoundsMake(CLLocationCoordinate2D sw, CLLocationCoordinate2D ne) {
    MGLCoordinateBounds bounds;
    bounds.sw = sw;
    bounds.ne = ne;
    return bounds;
}

/**
 Creates a new `MGLCoordinateQuad` structure from the given top left,
  bottom left, bottom right, and top right coordinates.
 */
NS_INLINE MGLCoordinateQuad MGLCoordinateQuadMake(CLLocationCoordinate2D topLeft, CLLocationCoordinate2D bottomLeft, CLLocationCoordinate2D bottomRight, CLLocationCoordinate2D topRight) {
    MGLCoordinateQuad quad;
    quad.topLeft = topLeft;
    quad.bottomLeft = bottomLeft;
    quad.bottomRight = bottomRight;
    quad.topRight = topRight;
    return quad;
}

/**
 Creates a new `MGLCoordinateQuad` structure from the given `MGLCoordinateBounds`.
 The returned quad uses the bounds' northeast coordinate as the top right, and the
  southwest coordinate at the bottom left.
 */
NS_INLINE MGLCoordinateQuad MGLCoordinateQuadFromCoordinateBounds(MGLCoordinateBounds bounds) {
    MGLCoordinateQuad quad;
    quad.topLeft = CLLocationCoordinate2DMake(bounds.ne.latitude, bounds.sw.longitude);
    quad.bottomLeft = bounds.sw;
    quad.bottomRight = CLLocationCoordinate2DMake(bounds.sw.latitude, bounds.ne.longitude);
    quad.topRight = bounds.ne;
    return quad;
}

/** Returns `YES` if the two coordinate bounds are equal to each other. */
NS_INLINE BOOL MGLCoordinateBoundsEqualToCoordinateBounds(MGLCoordinateBounds bounds1, MGLCoordinateBounds bounds2) {
    return (bounds1.sw.latitude == bounds2.sw.latitude &&
            bounds1.sw.longitude == bounds2.sw.longitude &&
            bounds1.ne.latitude == bounds2.ne.latitude &&
            bounds1.ne.longitude == bounds2.ne.longitude);
}

/** Returns `YES` if the two coordinate bounds intersect. */
NS_INLINE BOOL MGLCoordinateBoundsIntersectsCoordinateBounds(MGLCoordinateBounds bounds1, MGLCoordinateBounds bounds2) {
    return (bounds1.ne.latitude  > bounds2.sw.latitude  &&
            bounds1.sw.latitude  < bounds2.ne.latitude  &&
            bounds1.ne.longitude > bounds2.sw.longitude &&
            bounds1.sw.longitude < bounds2.ne.longitude);
}

/**
 Returns `YES` if the coordinate is within the coordinate bounds.
 
 #### Related examples
 See the <a href="https://docs.mapbox.com/ios/maps/examples/constraining-gestures/">
 Restrict map panning to an area</a> example to learn how to use
 `MGLCoordinateInCoordinateBounds` to determine if a point is within, or
 intersects, a given bounding box.
 */
NS_INLINE BOOL MGLCoordinateInCoordinateBounds(CLLocationCoordinate2D coordinate, MGLCoordinateBounds bounds) {
    return (coordinate.latitude  >= bounds.sw.latitude  &&
            coordinate.latitude  <= bounds.ne.latitude  &&
            coordinate.longitude >= bounds.sw.longitude &&
            coordinate.longitude <= bounds.ne.longitude);
}

/** Returns the area spanned by the coordinate bounds. */
NS_INLINE MGLCoordinateSpan MGLCoordinateBoundsGetCoordinateSpan(MGLCoordinateBounds bounds) {
    return MGLCoordinateSpanMake(bounds.ne.latitude - bounds.sw.latitude,
                                 bounds.ne.longitude - bounds.sw.longitude);
}

/**
 Returns a coordinate bounds with southwest and northeast coordinates that are
 offset from those of the source bounds.
 */
NS_INLINE MGLCoordinateBounds MGLCoordinateBoundsOffset(MGLCoordinateBounds bounds, MGLCoordinateSpan offset) {
    MGLCoordinateBounds offsetBounds = bounds;
    offsetBounds.sw.latitude += offset.latitudeDelta;
    offsetBounds.sw.longitude += offset.longitudeDelta;
    offsetBounds.ne.latitude += offset.latitudeDelta;
    offsetBounds.ne.longitude += offset.longitudeDelta;
    return offsetBounds;
}

/**
 Returns `YES` if the coordinate bounds covers no area.

 @note A bounds may be empty but have a non-zero coordinate span (e.g., when its
    northeast point lies due north of its southwest point).
 */
NS_INLINE BOOL MGLCoordinateBoundsIsEmpty(MGLCoordinateBounds bounds) {
    MGLCoordinateSpan span = MGLCoordinateBoundsGetCoordinateSpan(bounds);
    return span.latitudeDelta == 0 || span.longitudeDelta == 0;
}

/** Returns a formatted string for the given coordinate bounds. */
NS_INLINE NSString *MGLStringFromCoordinateBounds(MGLCoordinateBounds bounds) {
    return [NSString stringWithFormat:@"{ sw = {%.1f, %.1f}, ne = {%.1f, %.1f}}",
            bounds.sw.latitude, bounds.sw.longitude,
            bounds.ne.latitude, bounds.ne.longitude];
}

/** Returns a formatted string for the given coordinate quad. */
NS_INLINE NSString *MGLStringFromCoordinateQuad(MGLCoordinateQuad quad) {
    return [NSString stringWithFormat:@"{ topleft = {%.1f, %.1f}, bottomleft = {%.1f, %.1f}}, bottomright = {%.1f, %.1f}, topright = {%.1f, %.1f}",
            quad.topLeft.latitude, quad.topLeft.longitude,
            quad.bottomLeft.latitude, quad.bottomLeft.longitude,
            quad.bottomRight.latitude, quad.bottomRight.longitude,
            quad.topRight.latitude, quad.topRight.longitude];
}

/** Returns radians, converted from degrees. */
NS_INLINE CGFloat MGLRadiansFromDegrees(CLLocationDegrees degrees) {
    return (CGFloat)(degrees * M_PI) / 180;
}

/** Returns degrees, converted from radians. */
NS_INLINE CLLocationDegrees MGLDegreesFromRadians(CGFloat radians) {
    return radians * 180 / M_PI;
}

/** Returns Mercator projection of a WGS84 coordinate at the specified zoom level. */
FOUNDATION_EXTERN MGL_EXPORT MGLMapPoint MGLMapPointForCoordinate(CLLocationCoordinate2D coordinate, double zoomLevel);


/** Converts a map zoom level to a camera altitude.
 
 @param zoomLevel The zoom level to convert.
 @param pitch The camera pitch, measured in degrees.
 @param latitude The latitude of the point at the center of the viewport.
 @param size The size of the viewport.
 @return An altitude measured in meters. */
FOUNDATION_EXTERN MGL_EXPORT CLLocationDistance MGLAltitudeForZoomLevel(double zoomLevel, CGFloat pitch, CLLocationDegrees latitude, CGSize size);

/** Converts a camera altitude to a map zoom level.
 
 @param altitude The altitude to convert, measured in meters.
 @param pitch The camera pitch, measured in degrees.
 @param latitude The latitude of the point at the center of the viewport.
 @param size The size of the viewport.
 @return A zero-based zoom level. */
FOUNDATION_EXTERN MGL_EXPORT double MGLZoomLevelForAltitude(CLLocationDistance altitude, CGFloat pitch, CLLocationDegrees latitude, CGSize size);

NS_ASSUME_NONNULL_END
