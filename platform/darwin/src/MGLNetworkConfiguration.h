#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

/**
 The MGLNetworkConfiguration object provides a global way to set a base API URL for
 retrieval of map data, styles, and other resources.

 Currently, MGLNetworkConfiguration is private API in code but is able to be used
 by any applications via the `MGLMapboxAPIBaseURL` dictionary key in the
 application's `Info.plist`.
 */
@interface MGLNetworkConfiguration : NSObject

/// Returns the shared instance of the `MGLNetworkConfiguration` class.
+ (instancetype)sharedManager;

/// The current API base URL. If `nil`, the Mapbox default base API URL is in use.
@property (atomic, nullable) NSURL *apiBaseURL;

@end

NS_ASSUME_NONNULL_END
