#import "MGLMapCamera.h"

#include <mbgl/util/projection.hpp>

@implementation MGLMapCamera

+ (BOOL)supportsSecureCoding
{
    return YES;
}

+ (instancetype)camera
{
    return [[self alloc] init];
}

+ (instancetype)cameraLookingAtCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate
                              fromEyeCoordinate:(CLLocationCoordinate2D)eyeCoordinate
                                    eyeAltitude:(CLLocationDistance)eyeAltitude
{
    mbgl::LatLng centerLatLng = mbgl::LatLng(centerCoordinate.latitude, centerCoordinate.longitude);
    mbgl::LatLng eyeLatLng = mbgl::LatLng(eyeCoordinate.latitude, eyeCoordinate.longitude);
    
    mbgl::ProjectedMeters centerMeters = mbgl::Projection::projectedMetersForLatLng(centerLatLng);
    mbgl::ProjectedMeters eyeMeters = mbgl::Projection::projectedMetersForLatLng(eyeLatLng);
    CLLocationDirection heading = std::atan((centerMeters.northing - eyeMeters.northing) /
                                            (centerMeters.easting - eyeMeters.easting));
    
    double groundDistance = std::sqrt(std::pow(centerMeters.northing - eyeMeters.northing, 2) +
                                      std::pow(centerMeters.easting - eyeMeters.easting, 2));
    CGFloat pitch = std::atan(eyeAltitude / groundDistance);
    
    return [[self alloc] initWithCenterCoordinate:centerCoordinate
                                         altitude:eyeAltitude
                                            pitch:pitch
                                          heading:heading];
}

+ (instancetype)cameraLookingAtCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate
                                   fromDistance:(CLLocationDistance)distance
                                          pitch:(CGFloat)pitch
                                        heading:(CLLocationDirection)heading
{
    return [[self alloc] initWithCenterCoordinate:centerCoordinate
                                         altitude:distance
                                            pitch:(CGFloat)pitch
                                          heading:heading];
}

- (instancetype)initWithCenterCoordinate:(CLLocationCoordinate2D)centerCoordinate
                                altitude:(CLLocationDistance)altitude
                                   pitch:(CGFloat)pitch
                                 heading:(CLLocationDirection)heading
{
    if (self = [super init])
    {
        _centerCoordinate = centerCoordinate;
        _altitude = altitude;
        _pitch = pitch;
        _heading = heading;
    }
    return self;
}

- (nullable instancetype)initWithCoder:(NSCoder *)coder
{
    if (self = [super init])
    {
        _centerCoordinate = CLLocationCoordinate2DMake([[coder decodeObjectForKey:@"centerLatitude"] doubleValue],
                                                       [[coder decodeObjectForKey:@"centerLongitude"] doubleValue]);
        _altitude = [[coder decodeObjectForKey:@"altitude"] doubleValue];
        _pitch = [[coder decodeObjectForKey:@"pitch"] doubleValue];
        _heading = [[coder decodeObjectForKey:@"heading"] doubleValue];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder
{
    [coder encodeDouble:_centerCoordinate.latitude forKey:@"centerLatitude"];
    [coder encodeDouble:_centerCoordinate.longitude forKey:@"centerLongitude"];
    [coder encodeDouble:_altitude forKey:@"altitude"];
    [coder encodeDouble:_pitch forKey:@"pitch"];
    [coder encodeDouble:_heading forKey:@"heading"];
}

- (id)copyWithZone:(nullable NSZone *)zone
{
    return [[[self class] allocWithZone:zone] initWithCenterCoordinate:_centerCoordinate
                                                              altitude:_altitude
                                                                 pitch:_pitch
                                                               heading:_heading];
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<MKMapCamera %p centerCoordinate:%f, %f altitude:%.0fm heading:%.0f° pitch:%.0f°>",
            self, _centerCoordinate.latitude, _centerCoordinate.longitude, _altitude, _heading, _pitch];
}

@end
