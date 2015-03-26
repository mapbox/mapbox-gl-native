#import "MGLUserLocation_Private.h"

@implementation MGLUserLocation
{
    CLLocationCoordinate2D _coordinate;
}

@synthesize coordinate = _coordinate;

- (instancetype)init
{
    if (self = [super init])
    {
        _coordinate = CLLocationCoordinate2DMake(MAXFLOAT, MAXFLOAT);
    }

    return self;
}

+ (BOOL)automaticallyNotifiesObserversForKey:(NSString *)key
{
    return ! [key isEqualToString:@"location"] && ! [key isEqualToString:@"heading"];
}

+ (NSSet *)keyPathsForValuesAffectingCoordinate
{
    return [NSSet setWithObject:@"location"];
}

- (void)setLocation:(CLLocation *)newLocation
{
    if ([newLocation distanceFromLocation:_location] && newLocation.coordinate.latitude != 0 &&
            newLocation.coordinate.longitude != 0)
    {
        [self willChangeValueForKey:@"location"];
        _location = newLocation;
        _coordinate = _location.coordinate;
        [self didChangeValueForKey:@"location"];
    }
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

- (NSString *)title
{
    if ( ! _title) return @"Current Location";
}

@end
