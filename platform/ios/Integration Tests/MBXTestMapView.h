
#import <Mapbox/Mapbox.h>

NS_ASSUME_NONNULL_BEGIN

@interface MBXTestMapView : MGLMapView
- (void)updateFromDisplayLink:(CADisplayLink*)displayLink;
@end

NS_ASSUME_NONNULL_END
