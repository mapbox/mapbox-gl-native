#import "MGLEventsManager_Private.h"
#if TARGET_OS_IOS
#import "MGLMapboxEvents.h"
#endif

@implementation MGLEventsManager

+ (instancetype)sharedManager
{
    static dispatch_once_t once;
    static id sharedConfiguration;
    dispatch_once(&once, ^{
        sharedConfiguration = [[self alloc] init];
    });
    return sharedConfiguration;
}

- (void)handleEvent:(NSString *)eventName withAttributes:(NSDictionary *)attributes {
#if TARGET_OS_IOS
    [MGLMapboxEvents pushEvent:eventName withAttributes:attributes];
#endif
    if (self.shouldHandleEvents && self.handler) {
        self.handler(eventName, attributes);
    }
}

@end
