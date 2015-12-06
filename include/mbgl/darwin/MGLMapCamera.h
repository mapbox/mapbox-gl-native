#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <CoreLocation/CoreLocation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

/** An `MGLMapCamera` object represents a viewpoint from which the user observes some point on an `MGLMapView`. */
@interface MGLMapCamera : NSObject <NSSecureCoding, NSCopying>

/** Coordinate at the center of the map view. */
@property (nonatomic) CLLocationCoordinate2D centerCoordinate;

/** Heading measured in degrees clockwise from true north. */
@property (nonatomic) CLLocationDirection heading;

/** Pitch toward the horizon measured in degrees, with 0 degrees resulting in a two-dimensional map. */
@property (nonatomic) CGFloat pitch;

/** Meters above ground level. */
@property (nonatomic) CLLocationDistance altitude;

/** Returns a new camera with all properties set to 0. */
+ (instancetype)camera;

/**
 Returns a new camera using based on information about the camera’s viewpoint and focus point.
 
 @param centerCoordinate The geographic coordinate on which the map should be centered.
 @param eyeCoordinate The geometric coordinate at which the camera should be situated.
 @param eyeAltitude The altitude (measured in meters) above the map at which the camera should be situated. The altitude may be less than the distance from the camera’s viewpoint to the camera’s focus point.
 */
+ (instancetype)cameraLookingAtCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate
                              fromEyeCoordinate:(CLLocationCoordinate2D)eyeCoordinate
                                    eyeAltitude:(CLLocationDistance)eyeAltitude;

/**
 Returns a new camera with the given distance, pitch, and heading.
 
 @param centerCoordinate The geographic coordinate on which the map should be centered.
 @param distance The straight-line distance from the viewpoint to the `centerCoordinate`.
 @param pitch The viewing angle of the camera, measured in degrees. A value of `0` results in a camera pointed straight down at the map. Angles greater than `0` result in a camera angled toward the horizon.
 @param heading The camera’s heading, measured in degrees clockwise from true north. A value of `0` means that the top edge of the map view corresponds to true north. The value `90` means the top of the map is pointing due east. The value `180` means the top of the map points due south, and so on.
 */
+ (instancetype)cameraLookingAtCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate
                                   fromDistance:(CLLocationDistance)distance
                                          pitch:(CGFloat)pitch
                                        heading:(CLLocationDirection)heading;

@end

NS_ASSUME_NONNULL_END
