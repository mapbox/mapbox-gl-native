#import "MGLAccountManager.h"

NS_ASSUME_NONNULL_BEGIN

/// NSUserDefaults key that controls developer account type
FOUNDATION_EXTERN NSString * const MGLMapboxAccountTypeKey;

@interface MGLAccountManager (Private)

/// Returns the shared instance of the `MGLAccountManager` class.
@property (class, nonatomic, readonly) MGLAccountManager *sharedManager;

/// The current global access token.
@property (atomic, nullable) NSString *accessToken;

/// The API base URL that is used to access Mapbox resources. The default base URL is `https://api.mapbox.com`. If `nil`, the Mapbox default base API URL is in use.
@property (atomic, readwrite, nullable) NSURL *apiBaseURL;

#if TARGET_OS_IPHONE || TARGET_OS_SIMULATOR
/// The current global SKU.
@property (class, atomic, readonly) NSString *skuToken;
#endif

@end

NS_ASSUME_NONNULL_END
