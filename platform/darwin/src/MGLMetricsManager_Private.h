#import "MGLMetricsManager.h"

NS_ASSUME_NONNULL_BEGIN

FOUNDATION_EXTERN MGL_EXPORT NSString *const kMGLPerformanceMetric;

@interface MGLMetricsManager (Private)

- (void)handleMetricsEvent:(MGLMetricType)metricType withAttributes:(NSDictionary *)attributes;

@end

NS_ASSUME_NONNULL_END
