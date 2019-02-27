#import "MGLEventsManager.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLEventsManager (Private)

- (void)handleEvent:(NSString *)eventName withAttributes:(NSDictionary *)attributes;

@end

NS_ASSUME_NONNULL_END
