#import "MGLMetricsManager.h"

NS_ASSUME_NONNULL_BEGIN

extern NSString *const kMGLPerformanceMetric;

@interface MGLMetricsManager (Private)

- (void)handleMetricEvent:(MGLMetricType)metricType withAttributes:(NSDictionary *)attributes;

@end

NS_ASSUME_NONNULL_END
