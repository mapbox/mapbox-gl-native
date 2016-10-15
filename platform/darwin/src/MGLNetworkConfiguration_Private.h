#import <Foundation/Foundation.h>

@interface MGLNetworkConfiguration : NSObject

/// Returns the shared instance of the `MGLNetworkConfiguration` class.
+ (instancetype)sharedManager;

/// The current API base URL.
@property (atomic) NSURL *apiBaseURL;

@end
