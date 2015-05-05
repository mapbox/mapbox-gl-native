#import <UIKit/UIKit.h>

@interface MGLMapView ()

#pragma mark - Debugging

/** Triggers another render pass even when it is not necessary. */
- (void)invalidate;

/** Returns whether the map view is currently loading or processing any assets required to render the map */
- (BOOL)isFullyLoaded;

@end

