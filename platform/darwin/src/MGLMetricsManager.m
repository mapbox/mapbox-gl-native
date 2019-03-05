#import "MGLMetricsManager_Private.h"
#if TARGET_OS_IOS
#import "MGLMapboxEvents.h"
#endif

NSString * const kMGLPerformanceMetric = @"mobile.performance_trace";

@implementation MGLMetricsManager

+ (instancetype)sharedManager
{
    static dispatch_once_t once;
    static id sharedConfiguration;
    dispatch_once(&once, ^{
        sharedConfiguration = [[self alloc] init];
    });
    return sharedConfiguration;
}

- (void)handleMetricEvent:(MGLMetricType)metricType withAttributes:(NSDictionary *)attributes {
    if ([self.delegate shouldHandleMetric:metricType]) {
        [self.delegate didCollectMetric:metricType withAttributes:attributes];
    }
}

#if TARGET_OS_IOS
- (void)pushMetric:(MGLMetricType)metricType withAttributes:(NSDictionary *)attributes {
    
    NSString *eventName;
    
    switch (metricType) {
        case MGLMetricTypePerformance:
            eventName = kMGLPerformanceMetric;
            break;
            
        default:
            break;
    }
    
    [MGLMapboxEvents pushEvent:eventName withAttributes:attributes];

}
#endif

@end
