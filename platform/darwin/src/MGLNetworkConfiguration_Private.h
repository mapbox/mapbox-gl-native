#import "MGLNetworkConfiguration.h"

NS_ASSUME_NONNULL_BEGIN

@class MGLNetworkConfiguration;
@protocol MGLNetworkConfigurationMetricsDelegate <NSObject>

- (void)networkConfiguration:(MGLNetworkConfiguration *)networkConfiguration didGenerateMetricEvent:(NSDictionary *)metricEvent;

@end

extern NSString * const kMGLDownloadPerformanceEvent;

@interface MGLNetworkConfiguration (Private)

@property (nonatomic, strong) NSMutableDictionary<NSString*, NSDictionary*> *events;
@property (nonatomic, weak) id<MGLNetworkConfigurationMetricsDelegate> metricsDelegate;

- (void)startDownloadEvent:(NSString *)urlString type:(NSString *)resourceType;
- (void)stopDownloadEventForResponse:(NSURLResponse *)response;
- (void)cancelDownloadEventForResponse:(NSURLResponse *)response;

@end

NS_ASSUME_NONNULL_END
