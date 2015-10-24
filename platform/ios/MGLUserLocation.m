#import "MGLUserLocation_Private.h"

#import "MGLMapView.h"

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
        _location = [[CLLocation alloc] initWithLatitude:MAXFLOAT longitude:MAXFLOAT];
        _mapView = mapView;
    }

    return self;
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

#if !TARGET_OS_TV
- (void)setHeading:(CLHeading *)newHeading
{
    if (newHeading.trueHeading != _heading.trueHeading)
    {
        [self willChangeValueForKey:@"heading"];
        _heading = newHeading;
        [self didChangeValueForKey:@"heading"];
    }
}
#endif

- (CLLocationCoordinate2D)coordinate
{
    return self.location.coordinate;
}

- (NSString *)title
{
    return (_title ? _title : @"You Are Here");
}

@end
