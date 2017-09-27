#import "MGLUserLocation_Private.h"

#import "MGLMapView.h"
#import "NSBundle+MGLAdditions.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLUserLocation ()

@property (nonatomic, weak) MGLMapView *mapView;

@end

NS_ASSUME_NONNULL_END

@implementation MGLUserLocation

- (instancetype)initWithMapView:(MGLMapView *)mapView
{
    if (self = [super init])
    {
        _mapView = mapView;
    }

    return self;
}

+ (BOOL)supportsSecureCoding {
    return YES;
}

- (instancetype)initWithCoder:(NSCoder *)decoder {
    if (self = [super init]) {
        _location = [decoder decodeObjectOfClass:[CLLocation class] forKey:@"location"];
        _title = [decoder decodeObjectOfClass:[NSString class] forKey:@"title"];
        _subtitle = [decoder decodeObjectOfClass:[NSString class] forKey:@"subtitle"];
    }
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder {
    [coder encodeObject:_location forKey:@"location"];
    [coder encodeObject:_title forKey:@"title"];
    [coder encodeObject:_subtitle forKey:@"subtitle"];
}

- (BOOL)isEqual:(id)other {
    if (self == other) return YES;
    if (![other isKindOfClass:[MGLUserLocation class]]) return NO;

    MGLUserLocation *otherUserLocation = other;
    return ((!self.location && !otherUserLocation.location) || [self.location distanceFromLocation:otherUserLocation.location] == 0)
    && ((!self.title && !otherUserLocation.title) || [self.title isEqualToString:otherUserLocation.title])
    && ((!self.subtitle && !otherUserLocation.subtitle) || [self.subtitle isEqualToString:otherUserLocation.subtitle]);
}

- (NSUInteger)hash {
    NSUInteger hash = [super hash];
    hash += [_location hash];
    hash += [_heading hash];
    hash += [_title hash];
    hash += [_subtitle hash];
    return hash;
}

+ (BOOL)automaticallyNotifiesObserversForKey:(NSString *)key
{
    return ! [key isEqualToString:@"location"] && ! [key isEqualToString:@"heading"];
}

+ (NS_SET_OF(NSString *) *)keyPathsForValuesAffectingCoordinate
{
    return [NSSet setWithObject:@"location"];
}

- (void)setLocation:(CLLocation *)newLocation
{
    if ( ! newLocation || ! CLLocationCoordinate2DIsValid(newLocation.coordinate)) return;
    if ( _location && CLLocationCoordinate2DIsValid(_location.coordinate) && [newLocation distanceFromLocation:_location] == 0) return;
    if (newLocation.coordinate.latitude == 0 && newLocation.coordinate.longitude == 0) return;

    [self willChangeValueForKey:@"location"];
    _location = newLocation;
    [self didChangeValueForKey:@"location"];
}

- (BOOL)isUpdating
{
    return self.mapView.userTrackingMode != MGLUserTrackingModeNone;
}

- (void)setHeading:(CLHeading *)newHeading
{
    if (newHeading.trueHeading != _heading.trueHeading)
    {
        [self willChangeValueForKey:@"heading"];
        _heading = newHeading;
        [self didChangeValueForKey:@"heading"];
    }
}

- (CLLocationCoordinate2D)coordinate
{
    return _location ? _location.coordinate : kCLLocationCoordinate2DInvalid;
}

- (NSString *)title
{
    return _title ?: NSLocalizedStringWithDefaultValue(@"USER_DOT_TITLE", nil, nil, @"You Are Here", @"Default user location annotation title");
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"<%@: %p; location = %f, %f; updating = %@; altitude = %.0fm; heading = %.0f°; title = %@; subtitle = %@>",
            NSStringFromClass([self class]), (void *)self,
            self.location.coordinate.latitude, self.location.coordinate.longitude,
            self.updating ? @"yes" : @"no",
            self.location.altitude,
            self.heading.trueHeading,
            self.title ? [NSString stringWithFormat:@"\"%@\"", self.title] : self.title,
            self.subtitle ? [NSString stringWithFormat:@"\"%@\"", self.subtitle] : self.subtitle];
}

@end
