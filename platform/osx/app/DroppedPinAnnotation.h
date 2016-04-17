#import <Mapbox/Mapbox.h>

@interface DroppedPinAnnotation : MGLPointAnnotation

@property (nonatomic, readonly) NSTimeInterval elapsedShownTime;

- (void)resume;
- (void)pause;

@end
