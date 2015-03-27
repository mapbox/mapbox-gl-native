#import <Foundation/Foundation.h>

@interface MGLMetricsLocationManager : NSObject

// This method can be called from any thread.
//
+ (instancetype)sharedManager;

@end
