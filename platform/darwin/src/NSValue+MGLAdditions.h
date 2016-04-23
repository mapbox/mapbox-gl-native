#import <Foundation/Foundation.h>

#import "MGLGeometry.h"
#import "MGLOfflinePack.h"

NS_ASSUME_NONNULL_BEGIN

/**
 Methods for round-tripping values for Mapbox-defined types.
 */
@interface NSValue (MGLAdditions)

#pragma mark Working with Geographic Coordinate Values

/**
 Creates a new value object containing the specified Core Location geographic
 coordinate structure.
 
 @param coordinate The value for the new object.
 @return A new value object that contains the geographic coordinate information.
 */
+ (instancetype)valueWithMGLCoordinate:(CLLocationCoordinate2D)coordinate;

/**
 The Core Location geographic coordinate structure representation of the value.
 */
@property (readonly) CLLocationCoordinate2D MGLCoordinateValue;

/**
 Creates a new value object containing the specified Mapbox coordinate span
 structure.
 
 @param span The value for the new object.
 @return A new value object that contains the coordinate span information.
 */
+ (instancetype)valueWithMGLCoordinateSpan:(MGLCoordinateSpan)span;

/**
 The Mapbox coordinate span structure representation of the value.
 */
@property (readonly) MGLCoordinateSpan MGLCoordinateSpanValue;

/**
 Creates a new value object containing the specified Mapbox coordinate bounds
 structure.
 
 @param bounds The value for the new object.
 @return A new value object that contains the coordinate bounds information.
 */
+ (instancetype)valueWithMGLCoordinateBounds:(MGLCoordinateBounds)bounds;

/**
 The Mapbox coordinate bounds structure representation of the value.
 */
@property (readonly) MGLCoordinateBounds MGLCoordinateBoundsValue;

#pragma mark Working with Offline Map Values

/**
 Creates a new value object containing the given `MGLOfflinePackProgress`
 structure.
 
 @param progress The value for the new object.
 @return A new value object that contains the offline pack progress information.
 */
+ (NSValue *)valueWithMGLOfflinePackProgress:(MGLOfflinePackProgress)progress;

/**
 The `MGLOfflinePackProgress` structure representation of the value.
 */
@property (readonly) MGLOfflinePackProgress MGLOfflinePackProgressValue;

@end

NS_ASSUME_NONNULL_END
