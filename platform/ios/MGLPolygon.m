#import "MGLPolygon.h"

@interface MGLMultiPoint (MGLPolygon)

- (instancetype)initWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count;
- (BOOL)intersectsOverlayBounds:(MGLMapBounds)overlayBounds;

@end

@implementation MGLPolygon

@synthesize overlayBounds;

+ (instancetype)polygonWithCoordinates:(CLLocationCoordinate2D *)coords
                                 count:(NSUInteger)count
{
    // connect first & last if needed

    return [[self alloc] initWithCoordinates:coords count:count];
}

@end
