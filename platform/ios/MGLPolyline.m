#import "MGLPolyline.h"

@interface MGLMultiPoint (MGLPolyline)

- (instancetype)initWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count;
- (BOOL)intersectsOverlayBounds:(MGLMapBounds)overlayBounds;

@end

@implementation MGLPolyline

@synthesize overlayBounds;

+ (instancetype)polylineWithCoordinates:(CLLocationCoordinate2D *)coords
                                  count:(NSUInteger)count
{
    return [[self alloc] initWithCoordinates:coords count:count];
}

@end
