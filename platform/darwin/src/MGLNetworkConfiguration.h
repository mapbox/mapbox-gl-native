#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface MGLNetworkConfiguration : NSObject

+ (void)setAPIBaseURL:(nullable NSURL *)apiBaseURL;

+ (nullable NSURL *)apiBaseURL;

@end

NS_ASSUME_NONNULL_END
