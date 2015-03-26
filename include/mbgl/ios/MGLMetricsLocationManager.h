#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

@interface MGLMetricsLocationManager : NSObject <CLLocationManagerDelegate>

+ (id)sharedManager;

- (BOOL) isAuthorizedStatusDetermined;
- (void) requestAlwaysAuthorization;

@end
