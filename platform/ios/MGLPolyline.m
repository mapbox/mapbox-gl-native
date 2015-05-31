#import "MGLPolyline.h"

@interface MGLPolyline (MGLMultiPointDescendent)

- (instancetype)initWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count;

@end

@implementation MGLPolyline

+ (instancetype)polylineWithCoordinates:(CLLocationCoordinate2D *)coords
                                  count:(NSUInteger)count
{
    return [[self alloc] initWithCoordinates:coords count:count];
}

@end
