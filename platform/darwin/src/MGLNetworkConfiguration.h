#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 The MGLNetworkConfiguration object provides a global way to set a base API URL for
 retrieval of map data, styles, and other resources.

 Currently, MGLNetworkConfiguration is a private API.
 */
@interface MGLNetworkConfiguration : NSObject

/// Returns the shared instance of the `MGLNetworkConfiguration` class.
@property (class, nonatomic, readonly) MGLNetworkConfiguration *sharedManager;

@end

NS_ASSUME_NONNULL_END
