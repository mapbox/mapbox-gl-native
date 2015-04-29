#import <UIKit/UIKit.h>

@interface MGLMapView ()

#pragma mark - Debugging

/** Triggers another render pass even when it is not necessary. */
- (void)invalidate;

@end

