#import <Foundation/Foundation.h>

@interface MGLMetricsLocationManager : NSObject

+ (instancetype)sharedManager;

- (BOOL) isAuthorizedStatusDetermined;
- (void) requestAlwaysAuthorization;

@end
