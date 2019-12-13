#import <Foundation/Foundation.h>
#import <mbgl/interface/native_apple_interface.h>

@implementation MGLNativeNetworkManager

static MGLNativeNetworkManager *instance = nil;

+ (MGLNativeNetworkManager *)sharedManager {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[MGLNativeNetworkManager alloc] init];
    });
    return instance;
}

- (NSURLSessionConfiguration *)sessionConfiguration {
    if (_delegate && [_delegate respondsToSelector:@selector(sessionConfiguration)]) {
        return [_delegate sessionConfiguration];
    }
    // For testing. Since we get a `nil` return when SDK is modualar, we use this for testing requests.
    // Same as `[MGLNetworkConfiguration defaultSessionConfiguration]` in `MGLNetworkConfiguration.m`.
    return [self testSessionConfiguration];
}

- (NSURLSessionConfiguration *)testSessionConfiguration {
    NSURLSessionConfiguration* sessionConfiguration = [NSURLSessionConfiguration defaultSessionConfiguration];
    
    sessionConfiguration.timeoutIntervalForResource = 30;
    sessionConfiguration.HTTPMaximumConnectionsPerHost = 8;
    sessionConfiguration.requestCachePolicy = NSURLRequestReloadIgnoringLocalCacheData;
    sessionConfiguration.URLCache = nil;
    
    return sessionConfiguration;
}

- (NSString *)skuToken {
    if(_delegate && [_delegate respondsToSelector:@selector(skuToken)]) {
        return [_delegate skuToken];
    }
    return nil;
}

- (void)startDownloadEvent:(NSString *)event type:(NSString *)type {
    if (_delegate && [_delegate respondsToSelector:@selector(startDownloadEvent:type:)]) {
        [_delegate startDownloadEvent:event type:type];
    }
}

- (void)cancelDownloadEventForResponse:(NSURLResponse *)response {
    if (_delegate && [_delegate respondsToSelector:@selector(cancelDownloadEventForResponse:)]) {
        return [_delegate cancelDownloadEventForResponse:response];
    }
}

- (void)stopDownloadEventForResponse:(NSURLResponse *)response {
    if (_delegate && [_delegate respondsToSelector:@selector(stopDownloadEventForResponse:)]) {
        return [_delegate stopDownloadEventForResponse:response];
    }
}

- (void)debugLog:(NSString *)format, ...{
    if (_delegate && [_delegate respondsToSelector:@selector(debugLog:)]) {
        return [_delegate debugLog:format];
    }
}

- (void)errorLog:(NSString *)format, ...{
    if (_delegate && [_delegate respondsToSelector:@selector(errorLog:)]) {
        return [_delegate errorLog:format];
    }
}

@end
