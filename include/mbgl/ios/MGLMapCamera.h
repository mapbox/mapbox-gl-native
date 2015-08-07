#import "Mapbox.h"

#pragma once

NS_ASSUME_NONNULL_BEGIN

@interface MGLMapCamera : NSObject <NSSecureCoding, NSCopying>

/** Coordinate at the center of the map view. */
@property (nonatomic) CLLocationCoordinate2D centerCoordinate;

/** Heading measured in degrees clockwise from true north. */
@property (nonatomic) CLLocationDirection heading;

/** Meters above ground level. */
@property (nonatomic) CLLocationDistance altitude;

/** Returns a new camera with all properties set to 0. */
+ (instancetype)camera;

+ (instancetype)cameraLookingAtCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate
                              fromEyeCoordinate:(CLLocationCoordinate2D)eyeCoordinate
                                    eyeAltitude:(CLLocationDistance)eyeAltitude;

+ (instancetype)cameraLookingAtCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate
                                   fromDistance:(CLLocationDistance)distance
                                        heading:(CLLocationDirection)heading;

@end

NS_ASSUME_NONNULL_END
