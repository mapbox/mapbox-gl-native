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

+ (NSURLSessionConfiguration *)testSessionConfiguration {
    NSURLSessionConfiguration* sessionConfiguration = [NSURLSessionConfiguration defaultSessionConfiguration];

    sessionConfiguration.timeoutIntervalForResource = 30;
    sessionConfiguration.HTTPMaximumConnectionsPerHost = 8;
    sessionConfiguration.requestCachePolicy = NSURLRequestReloadIgnoringLocalCacheData;
    sessionConfiguration.URLCache = nil;

    return sessionConfiguration;
}

#pragma mark - Optionals

- (NSString *)skuToken {
    if([self.delegate respondsToSelector:@selector(skuToken)]) {
        return [self.delegate skuToken];
    }
    return nil;
}

#pragma mark - Required

- (NSURLSessionConfiguration *)sessionConfiguration {
    NSURLSessionConfiguration *configuration = [_delegate sessionConfiguration];

    if (!configuration) {
        // For testing. Since we get a `nil` return when SDK is modular, we use
        // this for testing requests.
        // Same as `[MGLNetworkConfiguration defaultSessionConfiguration]` in
        // `MGLNetworkConfiguration.m`.
        configuration = [MGLNativeNetworkManager testSessionConfiguration];
    }

    return configuration;
}

- (void)startDownloadEvent:(NSString *)event type:(NSString *)type {
    [self.delegate startDownloadEvent:event type:type];
}

- (void)cancelDownloadEventForResponse:(NSURLResponse *)response {
    [self.delegate cancelDownloadEventForResponse:response];
}

- (void)stopDownloadEventForResponse:(NSURLResponse *)response {
    [self.delegate stopDownloadEventForResponse:response];
}

- (void)debugLog:(NSString *)format, ... {
    // TODO: Replace with existing mbgl logging handling.
    [self.delegate debugLog:format];
}

- (void)errorLog:(NSString *)format, ... {
    // TODO: Replace with existing mbgl logging handling.
    [self.delegate errorLog:format];
}

@end
