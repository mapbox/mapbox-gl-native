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

+ (instancetype)cameraLookingAtCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate
                              fromEyeCoordinate:(CLLocationCoordinate2D)eyeCoordinate
                                    eyeAltitude:(CLLocationDistance)eyeAltitude;

+ (instancetype)cameraLookingAtCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate
                                   fromDistance:(CLLocationDistance)distance
                                          pitch:(CGFloat)pitch
                                        heading:(CLLocationDirection)heading;

@end

NS_ASSUME_NONNULL_END
