#import <Foundation/Foundation.h>

@interface MGLMetricsLocationManager : NSObject

+ (id)sharedManager;

- (BOOL) isAuthorizedStatusDetermined;
- (void) requestAlwaysAuthorization;

@end
