#import "MGLMultiPoint.h"

@interface MGLMultiPoint (Private)

- (instancetype)initWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count;
- (BOOL)intersectsOverlayBounds:(MGLCoordinateBounds)overlayBounds;

@end
