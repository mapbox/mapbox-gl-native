#import "MMEDispatchManager.h"

@implementation MMEDispatchManager

- (void)scheduleBlock:(void(^)(void))block afterDelay:(NSTimeInterval)delay {
    dispatch_time_t dispatchTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delay * NSEC_PER_SEC));
    dispatch_after(dispatchTime, dispatch_get_main_queue(), block);
}

@end
