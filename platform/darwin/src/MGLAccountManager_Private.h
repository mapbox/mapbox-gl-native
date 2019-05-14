#import "MGLAccountManager.h"

@interface MGLAccountManager (Private)

/// Returns the shared instance of the `MGLAccountManager` class.
@property (class, nonatomic, readonly) MGLAccountManager *sharedManager;

/// The current global access token.
@property (atomic) NSString *accessToken;

/// The API base URL that is used to access Mapbox resources. The default base URL is `https://api.mapbox.com`. If `nil`, the Mapbox default base API URL is in use.
@property (atomic, readwrite) NSURL *apiBaseURL;

@end
