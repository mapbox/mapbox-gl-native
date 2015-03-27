#import <Foundation/Foundation.h>

@interface MGLMetricsLocationManager : NSObject

// All methods are able to be called from any thread.
//
+ (instancetype)sharedManager;

- (BOOL) isAuthorizedStatusDetermined;
- (void) requestAlwaysAuthorization;

@end
