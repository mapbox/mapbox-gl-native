#import "MGLNetworkConfiguration.h"

@interface MGLNetworkConfiguration (Private)

/// Returns the shared instance of the `MGLNetworkConfiguration` class.
+ (instancetype)sharedManager;

/// The current API base URL.
@property (atomic) NSURL *apiBaseURL;

@end
