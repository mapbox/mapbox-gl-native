#import <Foundation/Foundation.h>

#import "MGLMapboxEvents.h"
#import "MGLTypes.h"

@interface MGLAPIClient : NSObject <NSURLSessionDelegate>

- (void)postEvents:(nonnull NS_ARRAY_OF(MGLMapboxEventAttributes *) *)events completionHandler:(nullable void (^)(NSError * _Nullable error))completionHandler;
- (void)postEvent:(nonnull MGLMapboxEventAttributes *)event completionHandler:(nullable void (^)(NSError * _Nullable error))completionHandler;
- (void)cancelAll;

@end
