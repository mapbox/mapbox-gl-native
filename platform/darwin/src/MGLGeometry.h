#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>
#import <CoreGraphics/CGBase.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

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
extern const MGLCoordinateSpan MGLCoordinateSpanZero;

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

/**
 Methods for round-tripping Mapbox geometry structure values.
 */
@interface NSValue (MGLGeometryAdditions)

@end

NS_ASSUME_NONNULL_END
