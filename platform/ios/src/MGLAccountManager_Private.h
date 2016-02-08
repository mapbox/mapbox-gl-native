#import "MGLAccountManager.h"

@interface MGLAccountManager (Private)

/** Returns the shared instance of the `MGLAccountManager` class. */
+ (instancetype)sharedManager;

@property (atomic) NSString *accessToken;

@end
