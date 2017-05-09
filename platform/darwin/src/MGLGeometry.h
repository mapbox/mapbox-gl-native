#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
#import <CoreGraphics/CGBase.h>

#import "MGLFoundation.h"

NS_ASSUME_NONNULL_BEGIN


typedef double MGLLocationRadians;
typedef double MGLRadianDistance;
typedef double MGLRadianDirection;

extern double const MGLMetersPerRadian;

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


/** Defines the area spanned by an `MGLCoordinateBounds`. */
typedef struct MGLCoordinateSpan {
    /** Latitudes spanned by an `MGLCoordinateBounds`. */
    CLLocationDegrees latitudeDelta;
    /** Longitudes spanned by an `MGLCoordinateBounds`. */
    CLLocationDegrees longitudeDelta;
} MGLCoordinateSpan;

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
 Returns `YES` if the two coordinate spans represent the same latitudinal change
 and the same longitudinal change.
 */
NS_INLINE BOOL MGLCoordinateSpanEqualToCoordinateSpan(MGLCoordinateSpan span1, MGLCoordinateSpan span2) {
    return (span1.latitudeDelta == span2.latitudeDelta &&
            span1.longitudeDelta == span2.longitudeDelta);
}

/** An area of zero width and zero height. */
extern MGL_EXPORT const MGLCoordinateSpan MGLCoordinateSpanZero;

/** A rectangular area as measured on a two-dimensional map projection. */
typedef struct MGLCoordinateBounds {
    /** Coordinate at the southwest corner. */
    CLLocationCoordinate2D sw;
    /** Coordinate at the northeast corner. */
    CLLocationCoordinate2D ne;
} MGLCoordinateBounds;

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

/** Returns `YES` if the coordinate is within the coordinate bounds. */
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

/** Returns radians, converted from degrees. */
NS_INLINE CGFloat MGLRadiansFromDegrees(CLLocationDegrees degrees) {
    return (CGFloat)(degrees * M_PI) / 180;
}

/** Returns degrees, converted from radians. */
NS_INLINE CLLocationDegrees MGLDegreesFromRadians(CGFloat radians) {
    return radians * 180 / M_PI;
}

/** Returns MGLRadianCoordinate2D, converted from CLLocationCoordinate2D. */
NS_INLINE MGLRadianCoordinate2D MGLRadianCoordinateFromLocationCoordinate(CLLocationCoordinate2D locationCoordinate) {
    return MGLRadianCoordinate2DMake(MGLRadiansFromDegrees(locationCoordinate.latitude),
                                     MGLRadiansFromDegrees(locationCoordinate.longitude));
}

/*
 Returns the distance in radians given two coordinates.
 */
NS_INLINE MGLRadianDistance MGLDistanceBetweenRadianCoordinates(MGLRadianCoordinate2D from, MGLRadianCoordinate2D to)
{
    double a = pow(sin((to.latitude - from.latitude) / 2), 2)
        + pow(sin((to.longitude - from.longitude) / 2), 2) * cos(from.latitude) * cos(to.latitude);
    
    return 2 * atan2(sqrt(a), sqrt(1 - a));
}

/*
 Returns direction in radians given two coordinates.
 */
NS_INLINE MGLRadianDirection MGLRadianCoordinatesDirection(MGLRadianCoordinate2D from, MGLRadianCoordinate2D to) {
    double a = sin(to.longitude - from.longitude) * cos(to.latitude);
    double b = cos(from.latitude) * sin(to.latitude)
                - sin(from.latitude) * cos(to.latitude) * cos(to.longitude - from.longitude);
    return atan2(a, b);
}

/*
 Returns coordinate at a given distance and direction away from coordinate.
 */
NS_INLINE MGLRadianCoordinate2D MGLRadianCoordinateAtDistanceFacingDirection(MGLRadianCoordinate2D coordinate,
                                                                               MGLRadianDistance distance,
                                                                               MGLRadianDirection direction) {
    double otherLatitude = asin(sin(coordinate.latitude) * cos(distance)
                             + cos(coordinate.latitude) * sin(distance) * cos(direction));
    double otherLongitude = coordinate.longitude + atan2(sin(direction) * sin(distance) * cos(coordinate.latitude),
                                           cos(distance) - sin(coordinate.latitude) * sin(otherLatitude));
    return MGLRadianCoordinate2DMake(otherLatitude, otherLongitude);
}
    

NS_ASSUME_NONNULL_END
