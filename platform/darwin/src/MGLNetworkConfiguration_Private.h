#import "MGLNetworkConfiguration.h"
@protocol MGLMapboxEventsDelegate;

NS_ASSUME_NONNULL_BEGIN

extern NSString * const kMGLDownloadPerformanceEvent;

@interface MGLNetworkConfiguration (Private)

@property (nonatomic, strong) NSMutableDictionary<NSString*, NSDictionary*> *events;

- (void)startDownloadEvent:(NSString *)urlString type:(NSString *)resourceType;
- (void)stopDownloadEvent:(NSString *)urlString;
- (void)cancelDownloadEvent:(NSString *)urlString;

@end

NS_ASSUME_NONNULL_END
