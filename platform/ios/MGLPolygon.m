#import "MGLPolygon.h"

@interface MGLPolygon (MGLMultiPointDescendent)

- (instancetype)initWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count;

@end

@implementation MGLPolygon

+ (instancetype)polygonWithCoordinates:(CLLocationCoordinate2D *)coords
                                 count:(NSUInteger)count
{
    return [[self alloc] initWithCoordinates:coords count:count];
}

@end
