#import "MGLNetworkConfiguration_Private.h"

#include <mbgl/storage/reachability.h>

static NSString * const MGLStartTime = @"start_time";
static NSString * const MGLResourceType = @"resource_type";
NSString * const kMGLDownloadPerformanceEvent = @"mobile.performance_trace";

@interface MGLNetworkConfiguration ()

@property (strong) NSURLSessionConfiguration *sessionConfig;
@property (nonatomic, strong) NSMutableDictionary<NSString *, NSDictionary*> *events;
@property (nonatomic, weak) id<MGLNetworkConfigurationMetricsDelegate> metricsDelegate;

@end

@implementation MGLNetworkConfiguration

- (instancetype)init {
    if (self = [super init]) {
        self.sessionConfiguration = nil;
        _events = [NSMutableDictionary dictionary];
    }
    
    return self;
}

+ (instancetype)sharedManager {
    static dispatch_once_t onceToken;
    static MGLNetworkConfiguration *_sharedManager;
    dispatch_once(&onceToken, ^{
        _sharedManager = [[self alloc] init];
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
    NSString *retainedUrlString = urlString;
    NSString *retainedResourceType = resourceType;
    if (retainedUrlString && !self.events[retainedUrlString]) {
        self.events[retainedUrlString] = @{ MGLStartTime: [NSDate date], MGLResourceType: retainedResourceType };
    }
    retainedUrlString = nil;
    retainedResourceType = nil;
}

- (void)stopDownloadEventForResponse:(NSURLResponse *)response {
    [self sendEventForURLResponse:response withAction:nil];
}

- (void)cancelDownloadEventForResponse:(NSURLResponse *)response {
    [self sendEventForURLResponse:response withAction:@"cancel"];
}

- (void)sendEventForURLResponse:(NSURLResponse *)response withAction:(NSString *)action
{
    NSURLResponse *retainedResponse = response;
    if ([retainedResponse isKindOfClass:[NSURLResponse class]]) {
        NSString *urlString = retainedResponse.URL.relativePath;
        if (urlString && [self.events objectForKey:urlString]) {
            NSDictionary *eventAttributes = [self eventAttributesForURL:retainedResponse withAction:action];
            [self.metricsDelegate networkConfiguration:self didGenerateMetricEvent:eventAttributes];
            [self.events removeObjectForKey:urlString];
        }
    }
    retainedResponse = nil;
}

- (NSDictionary *)eventAttributesForURL:(NSURLResponse *)response withAction:(NSString *)action
{
    NSString *urlString = response.URL.relativePath;
    NSDictionary *parameters = [self.events objectForKey:urlString];
    NSDate *startDate = [parameters objectForKey:MGLStartTime];
    NSDate *endDate = [NSDate date];
    NSTimeInterval elapsedTime = [endDate timeIntervalSinceDate:startDate];
    NSDateFormatter* iso8601Formatter = [[NSDateFormatter alloc] init];
    iso8601Formatter.dateFormat = @"yyyy-MM-dd'T'HH:mm:ssZ";
    NSString *createdDate = [iso8601Formatter stringFromDate:[NSDate date]];
    
    NSMutableArray *attributes = [NSMutableArray array];
    [attributes addObject:@{ @"name" : @"requestUrl" , @"value" : urlString }];
    [attributes addObject:@{ @"name" : MGLResourceType , @"value" : [parameters objectForKey:MGLResourceType] }];
    
    if ([response isKindOfClass:[NSHTTPURLResponse class]]) {
        NSInteger responseCode = [(NSHTTPURLResponse *)response statusCode];
        [attributes addObject:@{ @"name" : @"responseCode", @"value" : @(responseCode)}];
    }
    
    BOOL isWIFIOn = [[MGLReachability reachabilityWithHostName:response.URL.host] isReachableViaWiFi];
    [attributes addObject:@{ @"name" : @"wifiOn", @"value" : @(isWIFIOn)}];
    
    if (action) {
        [attributes addObject:@{ @"name" : @"action" , @"value" : action }];
    }
    
    double elapsedTimeInMS = elapsedTime * 1000.0;
    
    return @{
             @"event" : kMGLDownloadPerformanceEvent,
             @"created" : createdDate,
             @"sessionId" : [NSUUID UUID].UUIDString,
             @"counters" : @[ @{ @"name" : @"elapsedMS" , @"value" : @(elapsedTimeInMS) } ],
             @"attributes" : attributes
             };
}

@end
