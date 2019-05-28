
#import "MBXTestMapView.h"

@interface MGLMapView ()
- (void)updateFromDisplayLink:(CADisplayLink*)displayLink;
@end

@implementation MBXTestMapView

- (void)updateFromDisplayLink:(CADisplayLink*)displayLink {
    [super updateFromDisplayLink:displayLink];
}

- (void)didMoveToWindow {
    [super didMoveToWindow];
    NSLog(@"MOVING TO WINDOW");
}


@end
