#import "MGLMetricsManager_Private.h"
#import "MGLNetworkConfiguration_Private.h"
#if TARGET_OS_IOS
#import "MGLMapboxEvents.h"
#endif

NSString* MGLStringFromMetricType(MGLMetricType metricType) {
    NSString *eventName;
    
    switch (metricType) {
        case MGLMetricTypePerformance:
            eventName = kMGLDownloadPerformanceEvent;
            break;
    }
    return eventName;
}

@interface MGLMetricsManager() <MGLNetworkConfigurationMetricsDelegate>

@end

@implementation MGLMetricsManager

+ (instancetype)sharedManager
{
    static dispatch_once_t once;
    static id sharedConfiguration;
    dispatch_once(&once, ^{
        sharedConfiguration = [[self alloc] init];
        [MGLNetworkConfiguration sharedManager].metricsDelegate = sharedConfiguration;
    });
    return sharedConfiguration;
}

- (void)handleMetricsEvent:(MGLMetricType)metricType withAttributes:(NSDictionary *)attributes {
    if ([self.delegate metricsManager:self shouldHandleMetric:metricType]) {
        [self.delegate metricsManager:self didCollectMetric:metricType withAttributes:attributes];
    }
}

#if TARGET_OS_IOS
- (void)pushMetric:(MGLMetricType)metricType withAttributes:(NSDictionary *)attributes {
    NSString *eventName = MGLStringFromMetricType(metricType);
    
    [MGLMapboxEvents pushEvent:eventName withAttributes:attributes];
}
#endif

- (void)networkConfiguration:(MGLNetworkConfiguration *)networkConfiguration didGenerateMetricEvent:(NSDictionary *)metricEvent {
    [self handleMetricsEvent:MGLMetricTypePerformance withAttributes:metricEvent];
}

@end
