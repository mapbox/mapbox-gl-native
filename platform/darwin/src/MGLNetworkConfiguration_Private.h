#import "MGLNetworkConfiguration.h"

NS_ASSUME_NONNULL_BEGIN

extern NSString * const kMGLDownloadPerformanceEvent;

@protocol MGLNetworkEventDelegate <NSObject>

@optional
- (void)networkConfiguration:(MGLNetworkConfiguration *)networkConfiguration didReceiveNetworkEvent:(NSDictionary *)event;

@end

@interface MGLNetworkConfiguration (Private)

@property (nonatomic, strong) NSMutableDictionary<NSString*, NSDictionary*> *events;
@property (nonatomic, weak, nullable) id<MGLNetworkEventDelegate> delegate;

- (void)startDownloadEvent:(NSString *)urlString type:(NSString *)resourceType;
- (void)stopDownloadEvent:(NSString *)urlString;
- (void)cancelDownloadEvent:(NSString *)urlString;

@end

NS_ASSUME_NONNULL_END
