#import "MGLNetworkConfiguration.h"

@interface MGLNetworkConfiguration ()

@property (strong) NSURLSessionConfiguration *sessionConfig;

@end

@implementation MGLNetworkConfiguration

+ (instancetype)sharedManager {
    static dispatch_once_t onceToken;
    static MGLNetworkConfiguration *_sharedManager;
    dispatch_once(&onceToken, ^{
        _sharedManager = [[self alloc] init];
        _sharedManager.sessionConfiguration = nil;
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

@end
