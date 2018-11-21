#import "MGLAccountManager.h"

@interface MGLAccountManager (Private)

/// Returns the shared instance of the `MGLAccountManager` class.
@property (class, nonatomic, readonly) MGLAccountManager *sharedManager;

/// The current global access token.
@property (atomic) NSString *accessToken;

@end
