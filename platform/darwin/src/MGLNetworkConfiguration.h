#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface MGLNetworkConfiguration : NSObject

/**
 Returns any custom API base URL in use by instances of MGLMapView and MGLOfflineStorage in the current application.
 */
+ (nullable NSURL *)apiBaseURL;

@end

NS_ASSUME_NONNULL_END
