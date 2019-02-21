#import "MGLNetworkConfiguration_Private.h"


NSString * const kMGLDownloadPerformanceEvent = @"mobile.performance_trace";
static NSString * const MGLStartTime = @"start_time";
static NSString * const MGLResourceType = @"resource_type";

@interface MGLNetworkConfiguration ()

@property (strong) NSURLSessionConfiguration *sessionConfig;
@property (nonatomic, strong) NSMutableDictionary<NSString *, NSDictionary*> *events;
@property (nonatomic, weak, nullable) id<MGLNetworkEventDelegate> delegate;

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
    if (urlString && [self.events objectForKey:urlString]) {
        if ([self.delegate respondsToSelector:@selector(networkConfiguration:didReceiveNetworkEvent:)])
        {
            NSDictionary *parameters = [self.events objectForKey:urlString];
            NSDate *startDate = [parameters objectForKey:MGLStartTime];
            NSDate *endDate = [NSDate date];
            NSTimeInterval elapsedTime = [endDate timeIntervalSinceDate:startDate];
            NSDateFormatter* iso8601Formatter = [[NSDateFormatter alloc] init];
            iso8601Formatter.dateFormat = @"yyyy-MM-dd'T'HH:mm:ssZ";
            
            NSDictionary *eventAttributes = @{
                                              @"event" : kMGLDownloadPerformanceEvent,
                                              @"created" : [iso8601Formatter stringFromDate:[NSDate date]],
                                              @"sessionId" : [NSUUID UUID].UUIDString,
                                              @"counters" : @[ @{ @"name" : @"elapsed_time" }, @{ @"value" : @(elapsedTime) } ],
                                              @"attributes" : @[ @{ @"name" : @"resource" }, @{ @"value" : urlString },  ]
                                              };
            
            [self.delegate networkConfiguration:self didReceiveNetworkEvent:eventAttributes];
        }
        [self.events removeObjectForKey:urlString];
    }
}

- (void)cancelDownloadEvent:(NSString *)urlString {
    if (urlString && [self.events objectForKey:urlString]) {
        [self.events removeObjectForKey:urlString];
    }
}

@end
