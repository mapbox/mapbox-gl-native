#import "MGLNetworkConfiguration_Private.h"
#import "MGLEventsManager_Private.h"

NSString * const kMGLDownloadPerformanceEvent = @"mobile.performance_trace";
static NSString * const MGLStartTime = @"start_time";
static NSString * const MGLResourceType = @"resource_type";

@interface MGLNetworkConfiguration ()

@property (strong) NSURLSessionConfiguration *sessionConfig;
@property (nonatomic, strong) NSMutableDictionary<NSString *, NSDictionary*> *events;

@end

@implementation MGLNetworkConfiguration

+ (instancetype)sharedManager {
    static dispatch_once_t onceToken;
    static MGLNetworkConfiguration *_sharedManager;
    dispatch_once(&onceToken, ^{
        _sharedManager = [[self alloc] init];
        _sharedManager.sessionConfiguration = nil;
        _sharedManager.events = [NSMutableDictionary dictionary];
    });

    return _sharedManager;
}

- (void)setSessionConfiguration:(NSURLSessionConfiguration *)sessionConfiguration {
    @synchronized (self) {
        if (sessionConfiguration == nil) {
            _sessionConfig = [self defaultSessionConfiguration];
        } else {
            _sessionConfig = sessionConfiguration;
        }
    }
}

- (NSURLSessionConfiguration *)sessionConfiguration {
    NSURLSessionConfiguration *sessionConfig = nil;
    @synchronized (self) {
        sessionConfig = _sessionConfig;
    }
    return sessionConfig;
}

- (NSURLSessionConfiguration *)defaultSessionConfiguration {
    NSURLSessionConfiguration* sessionConfiguration = [NSURLSessionConfiguration defaultSessionConfiguration];
    
    sessionConfiguration.timeoutIntervalForResource = 30;
    sessionConfiguration.HTTPMaximumConnectionsPerHost = 8;
    sessionConfiguration.requestCachePolicy = NSURLRequestReloadIgnoringLocalCacheData;
    sessionConfiguration.URLCache = nil;
    
    return sessionConfiguration;
}

- (void)startDownloadEvent:(NSString *)urlString type:(NSString *)resourceType {
    if (urlString && ![self.events objectForKey:urlString]) {
        [self.events setObject:@{ MGLStartTime: [NSDate date], MGLResourceType: resourceType } forKey:urlString];
    }
}

- (void)stopDownloadEvent:(NSString *)urlString {
    [self sendEventForURL:urlString withAction:nil];
}

- (void)cancelDownloadEvent:(NSString *)urlString {
    [self sendEventForURL:urlString withAction:@"cancel"];
}

- (void)sendEventForURL:(NSString *)urlString withAction:(NSString *)action
{
    if (urlString && [self.events objectForKey:urlString]) {
        NSDictionary *eventAttributes = [self eventAttributesForURL:urlString withAction:action];
        [[MGLEventsManager sharedManager] handleEvent:kMGLDownloadPerformanceEvent withAttributes:eventAttributes];
        [self.events removeObjectForKey:urlString];
    }
}

- (NSDictionary *)eventAttributesForURL:(NSString *)urlString withAction:(NSString *)action
{
    NSDictionary *parameters = [self.events objectForKey:urlString];
    NSDate *startDate = [parameters objectForKey:MGLStartTime];
    NSDate *endDate = [NSDate date];
    NSTimeInterval elapsedTime = [endDate timeIntervalSinceDate:startDate];
    NSDateFormatter* iso8601Formatter = [[NSDateFormatter alloc] init];
    iso8601Formatter.dateFormat = @"yyyy-MM-dd'T'HH:mm:ssZ";
    NSString *createdDate = [iso8601Formatter stringFromDate:[NSDate date]];
    
    NSMutableArray *attributes = [NSMutableArray array];
    [attributes addObject:@{ @"name" : @"resource" , @"value" : urlString }];
    [attributes addObject:@{ @"name" : MGLResourceType , @"value" : [parameters objectForKey:MGLResourceType] }];
    if (action) {
        [attributes addObject:@{ @"name" : @"action" , @"value" : action }];
    }
    
    return @{
             @"event" : kMGLDownloadPerformanceEvent,
             @"created" : createdDate,
             @"sessionId" : [NSUUID UUID].UUIDString,
             @"counters" : @[ @{ @"name" : @"elapsed_time" , @"value" : @(elapsedTime) } ],
             @"attributes" : attributes
             };
}

@end
