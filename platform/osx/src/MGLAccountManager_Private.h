#import <MGLAccountManager.h>

@interface MGLAccountManager (Private)

/**
    Returns the shared instance of the `MGLAccountManager` class.
 
    This method can be called from any thread.
 */
+ (instancetype)sharedManager;

/// The current global access token.
@property (atomic) NSString *accessToken;

@end
