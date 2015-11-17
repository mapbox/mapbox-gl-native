#import "MGLMultiPoint.h"

#import <CoreLocation/CoreLocation.h>

@interface MGLMultiPoint (Private)

- (instancetype)initWithCoordinates:(CLLocationCoordinate2D *)coords count:(NSUInteger)count;
- (BOOL)intersectsOverlayBounds:(MGLCoordinateBounds)overlayBounds;

@end
