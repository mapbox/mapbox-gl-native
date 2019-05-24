
#import "MBXTestMapView.h"

@interface MGLMapView
- (void)updateFromDisplayLink:(CADisplayLink*)displayLink;
@end
@implementation MBXTestMapView

- (void)updateFromDisplayLink:(CADisplayLink*)displayLink {
    
    dispatch_block_t update = ^{
        [super updateFromDisplayLink:displayLink];
    };

    NSLog(@"UPDATING");
    update();

}

- (void)didMoveToWindow {
    [super didMoveToWindow];
    NSLog(@"MOVING TO WINDOW");
}


@end
