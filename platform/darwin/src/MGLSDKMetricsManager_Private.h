#import "MGLSDKMetricsManager.h"

NS_ASSUME_NONNULL_BEGIN

@interface MGLMetricsManager (Private)

- (void)handleMetricsEvent:(MGLMetricType)metricType withAttributes:(NSDictionary *)attributes;

@end

NS_ASSUME_NONNULL_END
