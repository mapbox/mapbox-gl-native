#import <Foundation/Foundation.h>

#import "MGLMapboxEvents.h"
#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLAPIClient : NSObject <NSURLSessionDelegate>

- (void)postEvents:(NS_ARRAY_OF(MGLMapboxEventAttributes *) *)events completionHandler:(nullable void (^)(NSError * _Nullable error))completionHandler;
- (void)postEvent:(MGLMapboxEventAttributes *)event completionHandler:(nullable void (^)(NSError * _Nullable error))completionHandler;

@end

NS_ASSUME_NONNULL_END
