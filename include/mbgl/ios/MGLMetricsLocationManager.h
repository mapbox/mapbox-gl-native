#import <Foundation/Foundation.h>

@interface MGLMetricsLocationManager : NSObject

// These methods can be called from any thread.
//
+ (instancetype)sharedManager;
- (void) startUpdatingLocation;
- (void) stopUpdatingLocation;
- (void) startMonitoringVisits;
- (void) stopMonitoringVisits;

@end
