#import "MGLMapSnapshotter.h"

@protocol MGLMapSnapshotProtocol <NSObject>

#if TARGET_OS_IPHONE
- (CGPoint)pointForCoordinate:(CLLocationCoordinate2D)coordinate;
- (CLLocationCoordinate2D)coordinateForPoint:(CGPoint)point;

#else
- (NSPoint)pointForCoordinate:(CLLocationCoordinate2D)coordinate;
- (CLLocationCoordinate2D)coordinateForPoint:(NSPoint)point;
#endif

@end
