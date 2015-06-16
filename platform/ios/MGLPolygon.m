#import "MGLPolygon.h"

#import "MGLMultiPoint_Private.h"

@implementation MGLPolygon

@dynamic overlayBounds;

+ (instancetype)polygonWithCoordinates:(CLLocationCoordinate2D *)coords
                                 count:(NSUInteger)count
{
    return [[self alloc] initWithCoordinates:coords count:count];
}

@end
