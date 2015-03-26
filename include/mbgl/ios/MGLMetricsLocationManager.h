#import <Foundation/Foundation.h>
#import <CoreLocation/CoreLocation.h>

@interface MGLMetricsLocationManager : NSObject <CLLocationManagerDelegate>

+ (id)sharedManager;

- (BOOL) isAuthorizedStatusDetermined;
- (void) requestAlwaysAuthorization;

- (void) startUpdatingLocation;
- (void) stopUpdatingLocation;

@end
