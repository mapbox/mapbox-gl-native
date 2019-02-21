#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol MGLMapboxEventsDelegate <NSObject>

- (BOOL)shouldReceiveEvents;
- (void)didReceiveEvent:(NSString *)eventName withAttributes:(NSDictionary *)attributes;

@end

NS_ASSUME_NONNULL_END
