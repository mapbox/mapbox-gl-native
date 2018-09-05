#import <Foundation/Foundation.h>

@interface MMEDispatchManager : NSObject

- (void)scheduleBlock:(void(^)(void))block afterDelay:(NSTimeInterval)delay;

@end

