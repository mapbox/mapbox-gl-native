#import "MGLMultiPoint.h"

@implementation MGLMultiPoint
{
    CLLocationCoordinate2D *_coords;
}

- (CLLocationCoordinate2D)coordinate
{
    return CLLocationCoordinate2DMake(_coords[0].latitude, _coords[0].longitude);
}

- (void)getCoordinates:(CLLocationCoordinate2D *)coords range:(NSRange)range
{
    coords = _coords;
}

@end
