#import "MGLPolyline.h"

#import "MGLMultiPoint_Private.h"

@implementation MGLPolyline

@synthesize overlayBounds;

+ (instancetype)polylineWithCoordinates:(CLLocationCoordinate2D *)coords
                                  count:(NSUInteger)count
{
    return [[self alloc] initWithCoordinates:coords count:count];
}

@end
