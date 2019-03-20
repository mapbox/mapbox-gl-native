#import <Foundation/Foundation.h>
#import "MGLFoundation.h"

NS_ASSUME_NONNULL_BEGIN

/**
 :nodoc:
 The metrics type used to handle metrics events.
 */
typedef NS_ENUM(NSUInteger, MGLMetricType) {
    /** :nodoc:
     Metric that measures performance.
     */
    MGLMetricTypePerformance = 0,
};

FOUNDATION_EXTERN MGL_EXPORT NSString* MGLStringFromMetricType(MGLMetricType metricType);

@class MGLMetricsManager;

/**
 :nodoc:
 The `MGLMetricsManagerDelegate` protocol defines a set of methods that you
 can use to receive metric events.
 */
@protocol MGLMetricsManagerDelegate <NSObject>

/**
 :nodoc:
 Asks the delegate whether the metrics manager should handle metric events.
 
 @param metricsManager The metrics manager object.
 @param metricType The metric type event.
 */
- (BOOL)metricsManager:(MGLMetricsManager *)metricsManager shouldHandleMetric:(MGLMetricType)metricType;

/**
 :nodoc:
 Asks the delegate to handle metric events.
 
 @param metricsManager The metrics manager object.
 @param metricType The metric type event.
 @param attributes The metric attributes.
 */
- (void)metricsManager:(MGLMetricsManager *)metricsManager didCollectMetric:(MGLMetricType)metricType withAttributes:(NSDictionary *)attributes;

@end

/**
 :nodoc:
 The `MGLMetricsManager` object provides a single poin to collect SDK metrics
 such as tile download latency.
 */
MGL_EXPORT
@interface MGLMetricsManager : NSObject

/**
 :nodoc:
 Returns the shared metrics manager object.
 */
@property (class, nonatomic, readonly) MGLMetricsManager *sharedManager;

/**
 :nodoc:
 The metrics manager delegate that will recieve metric events.
 */
@property (nonatomic, weak) id<MGLMetricsManagerDelegate> delegate;

#if TARGET_OS_IOS
/**
 :nodoc:
 Sends metric events to Mapbox.
 */
- (void)pushMetric:(MGLMetricType)metricType withAttributes:(NSDictionary *)attributes;
#endif

@end

NS_ASSUME_NONNULL_END
