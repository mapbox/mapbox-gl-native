#import "MGLPolygon.h"

@implementation MGLPolygon
{
    CLLocationCoordinate2D *_coords;
}

+ (instancetype)polygonWithCoordinates:(CLLocationCoordinate2D *)coords
                                 count:(NSUInteger)count
{
    return [[self alloc] initWithCoordinates:coords count:count];
}

- (instancetype)initWithCoordinates:(CLLocationCoordinate2D *)coords
                              count:(NSUInteger)count
{
    self = [super init];

    if (self)
    {
        _coords = coords;
    }

    return self;
}

- (CLLocationCoordinate2D)coordinate
{
    return CLLocationCoordinate2DMake(_coords[0].latitude, _coords[0].longitude);
}

@end
